#include "client_handle.h"
#include "file_handle.h"

/**
 * Accept the Login command : “LOGIN username password” and print the response.
 */
void client_login(int client_socket, const char *command) {
    char buffer[BUFFER_MAX_LENGTH];
    bzero(buffer, BUFFER_MAX_LENGTH);
    write(client_socket, command, strlen(command));
    read(client_socket, buffer, BUFFER_MAX_LENGTH);
    printf("%s\n", buffer);
}

/**
 * Accept the “LS” command and print the responses. “OK” will be the last response.
 */
void client_ls(int client_socket, const char *command) {
    char buffer[BUFFER_MAX_LENGTH];
    bzero(buffer, BUFFER_MAX_LENGTH);
    write(client_socket, command, strlen(command));
    read(client_socket, buffer, BUFFER_MAX_LENGTH);
    printf("%s\n", buffer);
}


/**
 * Accept the local cd command: “LOCALCD dirName” – doesn’t send anything to the server.
 * Just changes my local directory.
 */
void local_cd(char *command) {
    char *LOCAL_CD = strtok(command, " ");
    char *dir_name = strtok(NULL, " ");
    change_directory(dir_name);
}

/**
 * Accept the local ls command: “LOCALLS” – doesn’t send anything to the server.
 * Just shows the contents of the current directory, indicating filename, file size and date created
 */
void local_ls() {
//    char *LOCAL_CD = strtok(command, " ");
//    char *dir_name = strtok(NULL, " ");
    printf("%s\n", ls(NULL));
}


/**
 * Accept the remote LS command as described in the server section.
 */
void remote_ls(int client_socket) {
    char msg[] = "LS";
    write(client_socket, msg, strlen(msg));
    char buffer[BUFFER_MAX_LENGTH];
    bzero(buffer, BUFFER_MAX_LENGTH);
    read(client_socket, buffer, BUFFER_MAX_LENGTH);
    printf("%s\n", buffer);
}

/**
 * Accept the remote CD command as described in the server section.
 */
void remote_cd(int client_socket, char *command) {
    char buffer[BUFFER_MAX_LENGTH];
    bzero(buffer, BUFFER_MAX_LENGTH);
    //In order to adapt to the receiving method of the server.
    // In fact, it can be optimized here, but the server must also have corresponding optimization.
    buffer[0] = ' ';
    strcpy(buffer+1, command);
    write(client_socket, buffer, strlen(buffer));
    // recv result
    bzero(buffer, BUFFER_MAX_LENGTH);
    read(client_socket, buffer, BUFFER_MAX_LENGTH);
    printf("%s\n", buffer);
}

/**
 * Accept the remote MKDIR command as described in the server section.
 */
void remote_mkdir(int client_socket, char *command) {
    write(client_socket, command, strlen(command));
    char buffer[BUFFER_MAX_LENGTH];
    bzero(buffer, BUFFER_MAX_LENGTH);
    read(client_socket, buffer, BUFFER_MAX_LENGTH);
    printf("%s\n", buffer);
}
