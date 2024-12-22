#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <stdlib.h> // For size_t

/**
 * Reads the content of a file into a dynamically allocated buffer.
 * 
 * @param filename The name of the file to read.
 * @param buffer_size Pointer to store the size of the file's content (optional, can be NULL).
 * @return A pointer to the buffer containing file contents, or NULL on failure.
 *         The caller is responsible for freeing the allocated buffer.
 */
char *read_file(const char *filename, size_t *buffer_size);

/**
 * Writes data to a file.
 * 
 * @param filename The name of the file to write to.
 * @param data The data to write to the file.
 * @param append If non-zero, data will be appended to the file; otherwise, the file will be overwritten.
 * @return 0 on success, or -1 on failure.
 */
int write_file(const char *filename, const char *data, int append);

/**
 * Copies the contents of one file to another.
 * 
 * @param source_file The source file to copy from.
 * @param destination_file The destination file to copy to.
 * @return 0 on success, or -1 on failure.
 */
int copy_file(const char *source_file, const char *destination_file);

#endif // FILE_UTILS_H
