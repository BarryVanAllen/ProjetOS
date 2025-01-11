#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
/**
 * Reads the content of a file into a dynamically allocated buffer.
 * 
 * @param filename The name of the file to read.
 * @param buffer_size The maximum size of the buffer to allocate.
 * @return A pointer to the buffer containing file contents, or NULL on failure.
 *         Caller must free the buffer after use.
 */
char *read_file(const char *filename, size_t *buffer_size) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    // Get the file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    if (file_size < 0) {
        perror("Error getting file size");
        fclose(file);
        return NULL;
    }

    // Allocate buffer to hold file contents
    char *buffer = malloc(file_size + 1);
    if (buffer == NULL) {
        perror("Error allocating memory");
        fclose(file);
        return NULL;
    }

    // Read the file into the buffer
    size_t bytes_read = fread(buffer, 1, file_size, file);
    buffer[bytes_read] = '\0';  // Null-terminate the buffer

    fclose(file);

    if (buffer_size) {
        *buffer_size = bytes_read;
    }

    return buffer;
}
/**
 * Writes data to a file.
 * 
 * @param filename The name of the file to write to.
 * @param data The data to write to the file.
 * @param append If non-zero, data will be appended; otherwise, the file will be overwritten.
 * @return 0 on success, or -1 on failure.
 */
int write_file(const char *filename, char *data, int append) {
    const char *mode = append ? "a" : "w";
    FILE *file = fopen(filename, mode);
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }

    size_t data_length = strlen(data);
    size_t bytes_written = fwrite(data, 1, data_length, file);

    fclose(file);

    if (bytes_written < data_length) {
        fprintf(stderr, "Error writing to file\n");
        return -1;
    }

    return 0;
}

/**
 * Copies the contents of one file to another.
 * 
 * @param source_file The source file to copy from.
 * @param destination_file The destination file to copy to.
 * @return 0 on success, or -1 on failure.
 */
int copy_file(const char *source_file, char *destination_file) {
    size_t buffer_size;
    char *buffer = read_file(source_file, &buffer_size);
    if (buffer == NULL) {
        return -1;
    }

    if (write_file(destination_file, buffer, 0) != 0) {
        free(buffer);
        return -1;
    }

    free(buffer);
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
 * Prints the entire content of a CSV file.
 * 
 * @param filename The name of the CSV file.
 */
void print_csv(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening CSV file");
        return;
    }

    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }

    fclose(file);
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
* @return nothing
* adds to ranking file
*/
void save_ranking(char *step,MemoirePartagee *mp) {
    FILE *file = fopen(step, "w");

    if (file == NULL) {
        perror("Open failed!");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < c; i++) {
        char best_lap_str[20];
        snprintf(best_lap_str, sizeof(best_lap_str), "%.2f", mp->pilotes[i].temps_meilleur_tour);
        fprintf(file, "%d --> %s\n", mp->pilotes[i].mum, best_lap_str);
    }

    if (fclose(file) != 0) {
        perror("fclose failed!");
        exit(EXIT_FAILURE);
    }
}
