#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024
#define MAX_COLUMNS 100

// Function to split a line into tokens using a given delimiter
void split_line(const char *line, char tokens[MAX_COLUMNS][MAX_LINE_LENGTH], const char *delimiter, int *num_tokens) {
    char temp[MAX_LINE_LENGTH];
    strcpy(temp, line);

    char *token = strtok(temp, delimiter);
    *num_tokens = 0;

    while (token != NULL && *num_tokens < MAX_COLUMNS) {
        strcpy(tokens[*num_tokens], token);
        (*num_tokens)++;
        token = strtok(NULL, delimiter);
    }
}

// Function to read a CSV file and print its content
void read_csv(const char *filename, const char *delimiter) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        char tokens[MAX_COLUMNS][MAX_LINE_LENGTH];
        int num_tokens;
        split_line(line, tokens, delimiter, &num_tokens);

        for (int i = 0; i < num_tokens; i++) {
            printf("%s", tokens[i]);
            if (i < num_tokens - 1) {
                printf(", ");
            }
        }
        printf("\n");
    }

    fclose(file);
}

// Function to write data to a CSV file
void write_csv(const char *filename, const char *data[][MAX_COLUMNS], int rows, int columns) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Error opening file for writing");
        return;
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            fprintf(file, "%s", data[i][j]);
            if (j < columns - 1) {
                fprintf(file, ";");
            }
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

int main() {
    const char *courses_file = "Liste courses F1 2024.csv";
    const char *pilots_file = "Liste pilotes.csv";
    const char *output_file = "result.csv";

    printf("Reading courses file:\n");
    read_csv(courses_file, ";");

    printf("\nReading pilots file:\n");
    read_csv(pilots_file, ";");

    // Example: Write sample data to a new CSV file
    const char *sample_data[][MAX_COLUMNS] = {
        {"ID", "Name", "Country"},
        {"1", "Max Verstappen", "Netherlands"},
        {"2", "Lewis Hamilton", "United Kingdom"}
    };

    write_csv(output_file, sample_data, 3, 3);
    printf("\nData written to %s\n", output_file);

    return 0;
}

