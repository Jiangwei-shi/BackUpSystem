#ifndef REMOTEBACKUPSYSTEM_FILE_HANDLE_H
#define REMOTEBACKUPSYSTEM_FILE_HANDLE_H

#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

#define DIRECTORY_LENGTH 128
#define RESULT_LENGTH (1024)


/**
 * init
 */
void file_handle_init();

/**
 * Reset current directory.
 */
void reset_current_dir();

/**
 * Initialize the folder, delete and recreate this folder if it exists
 */
void init_directory(const char *path);

/**
 * Change directory
 */
int change_directory(const char *path);

/**
 *  Just shows the contents of the current directory, indicating filename, file size and date created
 */
char *ls(const char *path);

/**
 * Create a new folder
 */
void make_dir(const char *path);

#endif //REMOTEBACKUPSYSTEM_FILE_HANDLE_H