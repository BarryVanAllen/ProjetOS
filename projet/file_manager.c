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

/** la fonction save_eliminated_cars sauvegarde les voitures élimninés dans un fichier.
* @param char filetosave le fichier qui va etre sauver
*@param Pilote array[] le tableau qui contient les pilotes éliminés.
*/

void save_eliminated_cars(const char *filetosave, MemoirePartagee *mp){

    FILE *file = fopen(filetosave, "w");

    if (file == NULL)
        perror("fopen failed !"), exit(EXIT_FAILURE);

    for (int i = 0; i < 5; i++) {
        fprintf(file, "%d\n", mp->pilotes[i].num);
    }

    if (fclose(file) != 0)
        perror("fclose failed !"), exit(EXIT_FAILURE);
}

/** la fonction read_eliminated_cars lit les pilotes élimninés depuis un fichier
 *  vers un tableau qui va contenir le classement 
 * @param const char filetoread
*@param Pilote array[] le tableau qui contient le classmeent des Qualifs
*/
void read_eliminated_cars(const char *filetoread, int array[]) {
    char results[5];

    // Open the file for reading
    FILE *file = fopen(filetosave, "r");
    if (file == NULL) {
        perror("fopen failed !");
        exit(EXIT_FAILURE);
    }

    int i = 15, j = 10;

    // Read each line from the file
    while (fgets(results, sizeof(results), file)) {
        // Check if the filename matches "steps/elim"
        if (strcmp(filetoread, "steps/elim") == 0) {
            // Convert the string read into an integer and store in array[i] and array[j]
            array[i] = atoi(results);
            array[j] = atoi(results);
            // Increment the indices for the next values
            i++;
            j++;
        }
    }

    // Close the file
    if (fclose(file) != 0) {
        perror("fclose failed !");
        exit(EXIT_FAILURE);
    }
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
