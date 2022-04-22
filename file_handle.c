#include "file_handle.h"

static char current_directory[DIRECTORY_LENGTH];

// The directory where the program first runs.
static char root_dir[DIRECTORY_LENGTH];

static char dir_buffer[DIRECTORY_LENGTH];

static char result_buffer[RESULT_LENGTH];


#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pwd.h>


/**
 * Reset current directory.
 */
void reset_current_dir() {
    strcpy(current_directory, root_dir);
}

/**
 * init
 */
void file_handle_init() {
    getcwd(root_dir, DIRECTORY_LENGTH);
    getcwd(current_directory, DIRECTORY_LENGTH);
}

char *get_abs_path(const char *path) {
//    char abs_path[DIRECTORY_LENGTH] = {0};
    bzero(dir_buffer, DIRECTORY_LENGTH);
//    strcpy(dir_buffer, root_dir);
    strcpy(dir_buffer, current_directory);
    if (path[0] != '\\' && path[0] != '/') {
        strcpy(dir_buffer + strlen(dir_buffer), "/");
    }
    strcpy(dir_buffer + strlen(dir_buffer), path);
    return dir_buffer;
}

/**
 * Initialize the folder, delete and recreate this folder if it exists
 */
void init_directory(const char *path) {
    // Get current path.
    getcwd(root_dir, DIRECTORY_LENGTH);
    if (path == NULL) {
        return;
    }
    // Get absolute path
    char *abs_path = get_abs_path(path);
    // Delete first
    char cmd_buffer[1024] = {0};
    sprintf(cmd_buffer, "rm -rf %s", abs_path);
    system(cmd_buffer);

    // create
    make_dir(abs_path);
//    bzero(cmd_buffer, 1024);
//    sprintf(cmd_buffer, "mkdir -vp %s", abs_path);
//    system(cmd_buffer);
}


/**
 * Change directory. Return 0 on success, otherwise return non-zero.
 */
int change_directory(const char *path) {
    char *abs_path = get_abs_path(path);
    int ret = chdir(abs_path);
    if (ret != 0) {
        perror("Change directory ERROR");
    } else {
        // change
        bzero(current_directory, DIRECTORY_LENGTH);
        getcwd(current_directory, DIRECTORY_LENGTH);
    }
    return ret;
}

/**
 *  Just shows the contents of the current directory, indicating filename, file size and date created
 */
char *ls(const char *path) {
    FILE *fp;
    char *abs_path;
    if (path == NULL) {
        abs_path = current_directory;
    } else {
        abs_path = get_abs_path(path);
    }
    char command[1024] = {0};
    sprintf(command, "ls -l %s | awk '{print $9,$5,$6,$7,$8}'", abs_path);
    // Executing a command
    fp = popen(command, "r");
    // read result
    bzero(result_buffer, RESULT_LENGTH);
    fread(result_buffer, 1, RESULT_LENGTH, fp);
    pclose(fp);
    return result_buffer;
}

/**
 * Create a new folder
 */
void make_dir(const char *path) {
    char *abs_path = get_abs_path(path);
    char cmd_buffer[1024] = {0};
    // create
    bzero(cmd_buffer, 1024);
    sprintf(cmd_buffer, "mkdir -vp %s", abs_path);
    system(cmd_buffer);
}