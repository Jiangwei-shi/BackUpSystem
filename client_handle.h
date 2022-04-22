#ifndef REMOTEBACKUPSYSTEM_CLIENT_HANDLE_H
#define REMOTEBACKUPSYSTEM_CLIENT_HANDLE_H
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFER_MAX_LENGTH (1024)

/**
 * Accept the Login command : “LOGIN username password” and print the response.
 */
void client_login(int client_socket, const char * command);

/**
 * Accept the “LS” command and print the responses. “OK” will be the last response.
 */
void client_ls(int client_socket, const char * command);

/**
 * Accept the local cd command: “LOCALCD dirName” – doesn’t send anything to the server.
 * Just changes my local directory.
 */
void local_cd(char *command);

/**
 * Accept the local ls command: “LOCALLS” – doesn’t send anything to the server.
 * Just shows the contents of the current directory, indicating filename, file size and date created
 */
void local_ls();

/**
 * Accept the remote LS command as described in the server section.
 */
void remote_ls(int client_socket);

/**
 * Accept the remote CD command as described in the server section.
 */
void remote_cd(int client_socket, char*command);

/**
 * Accept the remote MKDIR command as described in the server section.
 */
void remote_mkdir(int client_socket, char*command);


#endif //REMOTEBACKUPSYSTEM_CLIENT_HANDLE_H
