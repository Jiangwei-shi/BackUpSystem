#include "server_handle.h"
#include "file_handle.h"

/**
 * Verify user login information. Return 1 if the login is successful. Otherwise return 0.
 */
int login_verify(int client_socket, struct user_info *users, int user_num) {
    const int max_len = 1024;
    char buffer[max_len];
    int n = read(client_socket, buffer, max_len);
    // Separate username and password
    char *username = strtok(buffer, " ");
    char *passwd = strtok(NULL, " ");
    for (int i = 0; i < user_num; ++i) {
        // Match username
        if (strcmp(users[i].username, username) != 0) {
            continue;
        }
        // Match password
        if (strcmp(users[i].password, passwd) != 0) {
            break;
        }
        // Landed successfully
        // If their password is correct, change the current directory to their “filepath”.
        change_directory(users[i].filepath);
        char msg[] = "OK";
        write(client_socket, msg, strlen(msg));
        return ONLINE;
    }
    // not logged in
    char msg[] = "not logged in";
    write(client_socket, msg, strlen(msg));
    return OFFLINE;
}

/**
 * Respond to LS with a series of lines of text indicating filename, file size and date created.
 * Then respond with "OK".
 */
void server_ls(int client_socket) {
    const int max_len = 1024;
    char *result = ls(NULL);
    char buffer[max_len];
    strcpy(buffer, result);
    // Then respond with “OK”.
    strcpy(buffer+strlen(buffer), "\nOK");
    write(client_socket, buffer, strlen(buffer));
}

/**
 * Creates a new directory called "dirName" and respond with "OK"
 * MKDIR dirName
 */
void server_mkdir(int client_socket) {
    char dir[DIRECTORY_LENGTH] = {0};
    read(client_socket, dir, DIRECTORY_LENGTH);
    make_dir(dir);
    char msg[] = "OK";
    write(client_socket, msg, strlen(msg));
}


/**
 * Changes the current directory to dirName and respond with "OK" or "Failed".
 */
void server_cd(int client_socket) {
    char dir[DIRECTORY_LENGTH] = {0};
    read(client_socket, dir, DIRECTORY_LENGTH);
    char *msg;
    if(change_directory(dir) == 0){
        msg = "OK";
    } else{
        msg = "Failed";
    }
    write(client_socket, msg, strlen(msg));
}

/**
 * If the user doesn’t login, reject their commands (send back a message like “you aren’t logged in”).
 */
void reject_commands(int client_socket) {
    char string[] = "you aren’t logged in";
    write(client_socket, string, strlen(string));
}

