#ifndef REMOTEBACKUPSYSTEM_SERVER_HANDLE_H
#define REMOTEBACKUPSYSTEM_SERVER_HANDLE_H
#include <unistd.h>
#include <string.h>

#define USER_INFO_LENGTH 128

#define ONLINE 1
#define OFFLINE 0

struct user_info {
    char username[USER_INFO_LENGTH];
    char password[USER_INFO_LENGTH];
    char filepath[USER_INFO_LENGTH];
};

/**
 * Verify user login information. Return 1 if the login is successful. Otherwise return 0.
 */
int login_verify(int client_socket, struct user_info *users, int user_num);

/**
 * Respond to LS with a series of lines of text indicating filename, file size and date created.
 * Then respond with "OK".
 */
void server_ls(int client_socket);

/**
 * Creates a new directory called "dirName" and respond with "OK"
 */
void server_mkdir(int client_socket);


/**
 * Changes the current directory to dirName and respond with "OK" or "Failed".
 */
void server_cd(int client_socket);

/**
 * If the user doesn’t login, reject their commands (send back a message like “you aren’t logged in”).
 */
void reject_commands(int client_socket);


#endif //REMOTEBACKUPSYSTEM_SERVER_HANDLE_H
