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
#include "client_handle.h"
#include "file_handle.h"

#define MAX_SIZE 1024
#define FILE_NAME_SIZE 100
/**
 * assignment 3 Remote Backup System 1 for CS 333
 * Author: Shunjie Wan --client mode
 * Partner: Jiangwei Shi -- server mode
 */


/**
 * create socket for client to connect with server
 */
void connect_server(const char *ip, short port) {
    int client_socket;
    struct sockaddr_in serverAddr;
    //throw error when can not create socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(ip);
    serverAddr.sin_port = htons(port);
    //throw error when can not connect with server
    if (connect(client_socket, (struct sockaddr *) &serverAddr,
                sizeof(serverAddr)) < 0) {
        perror("connect");
        exit(1);
    } else {
        printf("Successfully connect to the server...\n");
    }
}

/**
 * stat() the file, get number of bytes 
 * send the file to server
 */
void push(int client_socket) {
    char file_name[FILE_NAME_SIZE];
    bzero(file_name, sizeof(file_name));
    printf("----------------------------------\n");
    printf("Here will create a test file send to the server.\n");
    printf("Please enter file name:\n");
    scanf("%s", file_name);
    FILE *fp;
    fp = fopen(file_name, "r");
    //write a sentence to the file as test
    if (fp == NULL) {
        printf("File:\t%s can not open.\n", file_name);
        return;
    }

    //stat() function to get the number of bytes
    struct stat file_stat;
    int ret = stat(file_name, &file_stat);
    int file_size = file_stat.st_size;

    char buffer[MAX_SIZE];
    printf("The file size is: %d bytes.\n", file_size);
//    char buffer[MAX_SIZE];
    bzero(buffer, sizeof(buffer));
    int offset = 5;
    // PUSH space
    strncpy(buffer, "PUSH ", offset);
    // filename
    strncpy(buffer + offset, file_name, strlen(file_name));
    offset += strlen(file_name);
    // space
    buffer[offset] = ' ';
    offset++;
    // file size
    int file_size_cp = file_size;
    for (int i = 3; i >= 0; --i) {
        buffer[offset + i] = file_size_cp & 0xff;
        file_size_cp = file_size_cp >> 8;
    }
    offset += 4;
    // space
    buffer[offset] = ' ';
    offset++;
    while (file_size > 0) {
        // read file.
        int n = fread(buffer + offset, sizeof(char), MAX_SIZE - offset, fp);
        // send
        write(client_socket, buffer, offset + n);
        file_size -= n;
        offset = 0;
    }

    printf("File has successfully sent to server.\n");
    // recv reply
    bzero(buffer, MAX_SIZE);
    read(client_socket, buffer, MAX_SIZE);
    printf("%s\n", buffer);
    fclose(fp);
}

/**
 * receive file from server 
 * store the file in disk
 */
void pull(int client_socket) {
    char file_name[FILE_NAME_SIZE];
    int length = 0;
    printf("Please enter file name:\n");
    scanf("%s", file_name);

    char buffer[MAX_SIZE];
    bzero(buffer, MAX_SIZE);

    // build request
    int offset = 5;
    // PUSH space
    strncpy(buffer, "PULL ", offset);
    // filename
    strncpy(buffer + offset, file_name, strlen(file_name));
    offset += strlen(file_name);
    write(client_socket, buffer, offset);

    // recv file size
    bzero(buffer, MAX_SIZE);
    int n = read(client_socket, buffer, 4);
    if (n != 4) {
        fprintf(stderr, "not 4 bytes.\n");
        exit(-1);
    }
    int file_size = 0;
    for (int i = 0; i < 3; ++i) {
        file_size = file_size | buffer[i++];
        file_size = file_size << 8;
    }
    file_size = file_size | buffer[3];

    printf("The file size is: %d bytes.\n", file_size);

    FILE *fp = fopen(file_name, "w");
//    int file_fp = open(file_name, O_CREAT | O_RDWR, 0777);
    // recv file data
    while (file_size > 0) {
        bzero(buffer, MAX_SIZE);
        int read_len = file_size > MAX_SIZE ? MAX_SIZE : file_size;
        n = read(client_socket, buffer, read_len);
        file_size -= n;
        // write to file
        for (int i = 0; i < n; ++i) {
            fputc(buffer[i], fp);
        }
    }
    // recv reply
    bzero(buffer, MAX_SIZE);
    read(client_socket, buffer, MAX_SIZE);
    printf("%s\n", buffer);
    fclose(fp);
}

/**
 * quit from the server
 * free all open resources
 */
void quit(int client_socket) {
    printf("Quit from the server...\n");
    close(client_socket);
//    exit(1);
}

void error(const char *msg) {
    perror(msg);
    exit(0);
}

int main(int argc, char const *argv[]) {
    char ip[20];//store server ip
    short port; // store server port
    const int command_len = 1024;
    char command[command_len];
    struct sockaddr_in serv_addr;
    struct hostent *server;
    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }
    int portno = atoi(argv[2]);
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        error("error opening socket");
    }
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "Error, no such host");
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(client_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("connect failed");
    }

    file_handle_init();
    //accept command from user 
    while (1) {
        printf("-------------------------------------------\n");
        printf("Please enter a command: PUSH | PULL | QUIT | LOGIN | LOCALCD | LOCALLS | MKDIR | LS | CD .\n");
        fflush(stdout);
        bzero(command, command_len);
        fgets(command, command_len, stdin);
        // Delete the carriage return at the end
        int len = strlen(command);
        command[len - 1] = '\0';
        if (strcmp(command, "PUSH") == 0) {
            push(client_socket);
        } else if (strcmp(command, "PULL") == 0) {
            pull(client_socket);
        } else if (strcmp(command, "QUIT") == 0) {
            quit(client_socket);
            return 0;
        } else if (strncmp(command, "LOGIN ", 5) == 0) {
            // LOGIN mphipps iloveICSI333
            client_login(client_socket, command);
        } else if (strncmp(command, "LOCALCD ", 8) == 0) {
            local_cd(command);
        } else if (strcmp(command, "LOCALLS") == 0) {
            local_ls(command);
        } else if (strncmp(command, "MKDIR ", 6) == 0) {
            remote_mkdir(client_socket, command);
        } else if (strcmp(command, "LS") == 0) {
            remote_ls(client_socket);
        } else if (strncmp(command, "CD ", 3) == 0) {
            remote_cd(client_socket, command);
        } else {
            printf("Please enter a correct command.\n");
            scanf("%s", command);
        }
    }
    return 0;
}
