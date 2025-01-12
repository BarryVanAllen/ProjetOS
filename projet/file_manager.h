#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"



//fonction pour la gestions des semaphores ecriture et lecture
void gestion_semaphore(MemoirePartagee *mp, int is_writer);
//idem qu'avant
void fin_gestion_semaphore(MemoirePartagee *mp, int is_writer);

/**
 * Reads the content of elim and returns an array of Pilote structures.
 * Assumes the file contains one Pilote per line.
 * @return A pointer to the array of Pilote structures, or NULL on failure.
 *         Caller must free the array after use.
 */
Pilote read_elim();

/**
 * Writes an array of Pilote structures to a file.
 * 
 * @param pilotes The array of Pilote structures to write to the file.
 * @param num_pilotes The number of Pilote structures in the array.
 * @param append If non-zero, data will be appended; otherwise, the file will be overwritten.
 * @return 0 on success, or -1 on failure.
 */
int write_pilotes_to_file(Pilote *pilotes, size_t num_pilotes, int append)


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
* @param array of pilotes
* @param nmbr of pilotes
* @return nothing
* adds to ranking file
*/
void save_ranking(char *step, Pilote pilotes[], int nb_pilotes);

/**
 * Counts the number of lines in a given file.
 *
 * @param filename The name of the file to read.
 * @return The number of lines in the file, or -1 if there was an error opening the file.
 */
int count_lines(char *filename);

#endif // FILE_UTILS_H
