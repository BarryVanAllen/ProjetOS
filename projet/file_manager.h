#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"

/**
 * Retrieves the path to a resource file.
 * @param file_name Name of the resource file.
 * @return Full path to the resource file.
 */
char *get_resources_file(char *file_name);

/**
 * Reads the content of a file into a dynamically allocated buffer.
 * 
 * @param filename The name of the file to read.
 * @param buffer_size The maximum size of the buffer to allocate.
 * @return A pointer to the buffer containing file contents, or NULL on failure.
 *         Caller must free the buffer after use.
 */
char *read_file(const char *filename, size_t *buffer_size);

/**
 * Writes data to a file.
 * 
 * @param filename The name of the file to write to.
 * @param data The data to write to the file.
 * @param append If non-zero, data will be appended; otherwise, the file will be overwritten.
 * @return 0 on success, or -1 on failure.
 */
int write_file(const char *filename, char *data, int append);

/**
 * Copies the contents of one file to another.
 * 
 * @param source_file The source file to copy from.
 * @param destination_file The destination file to copy to.
 * @return 0 on success, or -1 on failure.
 */
int copy_file(const char *source_file, char *destination_file);

/**
 * Parses all rows from a CSV file into an array of Pilote structs.
 * 
 * @param filename The name of the CSV file.
 * @param pilotes An output pointer to an array of Pilote structs.
 * @param count An output pointer to store the number of parsed rows.
 * @return 0 on success, or -1 on failure.
 */
int parse_csv_to_pilotes(const char *filename, Pilote **pilotes, int *count) ;

/**
 * Prints the entire content of a CSV file.
 * 
 * @param filename The name of the CSV file.
 */
void print_csv(char *filename);

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
int write_to_csv(const char *filename, char **data, size_t num_rows, int append);

/**
* @param name of step so that the file saves the rankings
* @return nothing
* adds to ranking file
*/
void save_ranking(char *step);

#endif // FILE_UTILS_H
