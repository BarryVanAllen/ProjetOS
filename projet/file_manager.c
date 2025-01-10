#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Retrieves the path to a resource file.
 * @param file_name Name of the resource file.
 * @return Full path to the resource file.
 */
char *get_resources_file(char *file_name) {
    static char resource_files[256];
    strcpy(resource_files, "raceResources/");
    strcat(resource_files, file_name);
    return resource_files;
}

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
 * Searches for a value in a CSV file.
 * 
 * @param filename The name of the CSV file.
 * @param value The value to search for.
 * @return 1 if the value is found, 0 otherwise.
 */
int search_csv(const char *filename, char *value) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening CSV file");
        return 0;
    }

    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, value)) {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

/**
 * Retrieves rows from a CSV file where a specific column matches a value.
 * 
 * @param filename The name of the CSV file.
 * @param column The column index to search in (0-based).
 * @param value The value to match.
 * @return 0 on success, or -1 on failure.
 */
int retrieve_rows_by_column(const char *filename, int column, char *value) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening CSV file");
        return -1;
    }

    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        char *token;
        char *line_copy = strdup(line);
        int current_column = 0;

        token = strtok(line_copy, ",");
        while (token) {
            if (current_column == column && strcmp(token, value) == 0) {
                printf("%s", line);  // Print the matching row
                break;
            }
            token = strtok(NULL, ",");
            current_column++;
        }

        free(line_copy);
    }

    fclose(file);
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
        if (fprintf(file, "%s\n", data[i]) < 0) {
            perror("Error writing to CSV file");
            fclose(file);
            return -1;
        }
    }

    fclose(file);
    return 0;
}
