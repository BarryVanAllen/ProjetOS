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

/** la fonction save_eliminated_cars sauvegarde les voitures élimninés dans un fichier.
* @param char filetosave le fichier qui va etre sauver
*@param Pilote array[] le tableau qui contient les pilotes éliminés.
*/
void save_eliminated_cars(const char *filetosave, MemoirePartagee *mp);

/** la fonction read_eliminated_cars lit les pilotes élimninés depuis un fichier
 *  vers un tableau qui va contenir le classement 
 * @param const char filetoread
*@param Pilote array[] le tableau qui contient le classmeent des Qualifs
*/
void read_eliminated_cars(const char *filetoread, int array[]);

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
