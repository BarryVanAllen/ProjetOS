#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"



//fonction pour la gestions des semaphores ecriture et lecture
void gestion_semaphore(MemoirePartagee *mp, int is_writer) {
    if (is_writer) {
        sem_wait(&mp->mutex); // Protection des écrivains
    } else {
        sem_wait(&mp->mutLect);
        mp->nbrLect++;
        if (mp->nbrLect == 1) sem_wait(&mp->mutex); // Premier lecteur bloque les écrivains
        sem_post(&mp->mutLect);
    }
}
//idem qu'avant
void fin_gestion_semaphore(MemoirePartagee *mp, int is_writer) {
    if (is_writer) {
        sem_post(&mp->mutex); // Libération des écrivains
    } else {
        sem_wait(&mp->mutLect);
        mp->nbrLect--;
        if (mp->nbrLect == 0) sem_post(&mp->mutex); // Dernier lecteur débloque les écrivains
        sem_post(&mp->mutLect);
    }
}

/**
 * Reads the content of elim and returns an array of Pilote structures.
 * Assumes the file contains one Pilote per line.
 * @return A pointer to the array of Pilote structures, or NULL on failure.
 *         Caller must free the array after use.
 */
Pilote read_elim() {
    FILE *file = fopen("steps/elim", "r");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    // Initialize variables
    char line[MAX_LINE_LENGTH];
    size_t count = 0;
    Pilote pilotes = NULL;

    // Read lines and count the number of Pilote entries
    while (fgets(line, sizeof(line), file)) {
        Pilote p;
        // Parse the line into the Pilote structure
        if (sscanf(line, "%49s %d %f %f %f %f %f %f %d", 
                   p.nom, &p.num, &p.temps_meilleur_tour, 
                   &p.dernier_temps_tour, &p.temps_course_total, 
                   &p.secteur_1, &p.secteur_2, &p.secteur_3, &p.num_tour) == 9) {
            // Reallocate memory for the array of Pilote structures
            Pilote *temp = realloc(pilotes, (count + 1) * sizeof(Pilote));
            if (temp == NULL) {
                perror("Error reallocating memory");
                free(pilotes);
                fclose(file);
                return NULL;
            }
            pilotes = temp;
            pilotes[count++] = p;  // Add the new Pilote to the array
        }
    }

    fclose(file);

    // Set the number of pilotes found
    if (num_pilotes) {
        *num_pilotes = count;
    }

    return pilotes;
}

/**
 * Writes an array of Pilote structures to a file.
 * 
 * @param pilotes The array of Pilote structures to write to the file.
 * @param num_pilotes The number of Pilote structures in the array.
 * @param append If non-zero, data will be appended; otherwise, the file will be overwritten.
 * @return 0 on success, or -1 on failure.
 */
int write_pilotes_to_file(Pilote *pilotes, size_t num_pilotes, int append) {
    const char *mode = append ? "a" : "w";
    FILE *file = fopen("steps/elim", mode);
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }
    // Iterate over the array of Pilote structures and write them to the file
    for (size_t i = 0; i < num_pilotes; i++) {
        Pilote *p = &pilotes[i];
        // Format the Pilote data as a string and write it to the file
        int bytes_written = fprintf(file, "%s %d %.2f %.2f %.2f %.2f %.2f %.2f %d\n",
                                    p->nom, p->num, p->temps_meilleur_tour,
                                    p->dernier_temps_tour, p->temps_course_total,
                                    p->secteur_1, p->secteur_2, p->secteur_3, p->num_tour);

        if (bytes_written < 0) {
            fprintf(stderr, "Error writing to file\n");
            fclose(file);
            return -1;
        }
    }

    fclose(file);
    return 0;
}

/**
 * Parses all rows from a CSV file into an array of Pilote structs.
 * 
 * @param filename The name of the CSV file.
 * @param pilotes An output pointer to an array of Pilote structs.
 * @param count An output pointer to store the number of parsed rows.
 * @return 0 on success, or -1 on failure.
 */
int parse_csv_to_pilotes(const char *filename, Pilote **pilotes, int *count) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening CSV file");
        return -1;
    }

    char line[1024];
    int capacity = 20;  // Initial capacity for the dynamic array
    int size = 0;

    *pilotes = malloc(capacity * sizeof(Pilote));
    if (*pilotes == NULL) {
        perror("Memory allocation failed");
        fclose(file);
        return -1;
    }

    while (fgets(line, sizeof(line), file)) {
        Pilote pilote;
        char *token;

        // Parse the "nom" field
        token = strtok(line, ",");
        if (token == NULL) continue;
        strncpy(pilote.nom, token, sizeof(pilote.nom) - 1);
        pilote.nom[sizeof(pilote.nom) - 1] = '\0';

        // Parse the "num" field
        token = strtok(NULL, ",");
        if (token == NULL) continue;
        pilote.num = atoi(token);

        // Parse the "temps_meilleur_tour" field
        token = strtok(NULL, ",");
        if (token == NULL) continue;
        pilote.temps_meilleur_tour = atof(token);

        // Parse the "dernier_temps_tour" field
        token = strtok(NULL, ",");
        if (token == NULL) continue;
        pilote.dernier_temps_tour = atof(token);

        // Add the Pilote to the array
        if (size >= capacity) {
            capacity *= 2;
            Pilote *new_array = realloc(*pilotes, capacity * sizeof(Pilote));
            if (new_array == NULL) {
                perror("Memory reallocation failed");
                free(*pilotes);
                fclose(file);
                return -1;
            }
            *pilotes = new_array;
        }

        (*pilotes)[size++] = pilote;
    }

    fclose(file);

    *count = size;  // Store the total number of rows parsed
    return 0;
}

/**
 * Writes data to a CSV file.
 * 
 * @param filename The name of the CSV file to write to.
 * @param data Array of strings representing rows to write to the CSV file.
 *             Each string should be a row formatted with commas.
 * @param num_rows The number of rows in the data array.
 * @param append If non-zero, data will be appended; otherwise, the file will be overwritten.
 * @return 0 on success, or -1 on failure.
 */
int write_to_csv(const char *filename, char **data, size_t num_rows, int append) {
    const char *mode = append ? "a" : "w";
    FILE *file = fopen(filename, mode);
    if (file == NULL) {
        perror("Error opening CSV file");
        return -1;
    }

    for (size_t i = 0; i < num_rows; i++) {
        if (fputs(data[i], file) == EOF || fputc('\n', file) == EOF) {
            perror("Error writing to CSV file");
            fclose(file);
            return -1;
        }
    }

    fclose(file);
    return 0;
}

/**
* @param name of step so that the file saves the rankings
* @param array of pilotes
* @param nmbr of pilotes
* @return nothing
* adds to ranking file
*/
void save_ranking(char *step, Pilote pilotes[], int nb_pilotes) {
    char filelocation[20] = "steps/";
    strcat(filelocation, step);
    FILE *file = fopen(filelocation, "w");

    if (file == NULL) {
        perror("Open failed!");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < nb_pilotes; i++) {
        char best_lap_str[20];
        snprintf(best_lap_str, sizeof(best_lap_str), "%.2f", pilotes[i].temps_meilleur_tour);
        fprintf(file, "%d --> %s\n", pilotes[i].num, best_lap_str);
    }

    if (fclose(file) != 0) {
        perror("fclose failed!");
        exit(EXIT_FAILURE);
    }
}

/**
 * Counts the number of lines in elim.
 * @return The number of lines in the file, or -1 if there was an error opening the file.
 */
int count_lines() {
    // Open the file in read mode ("r")
    char filelocation[20] = "steps/elim";
    FILE *file = fopen(filelocation, "r");

    if (file == NULL) {
        perror("Error opening file");
        return -1; // Return -1 to indicate an error occurred while opening the file
    }

    int count = 0; // Initialize a counter to store the number of lines
    char ch; // Variable to store each character read from the file

    while ((ch = fgetc(file)) != EOF) {
        // If a newline character is found, increment the line count
        if (ch == '\n') {
            count++;
        }
    }

    // Close the file after reading is complete
    fclose(file);
    return count;
}

