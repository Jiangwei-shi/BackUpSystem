#include <ctype.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include "server_handle.h"
#include "file_handle.h"

#define BUFFER_LENGTH 1024

void error(const char *msg) {
    perror(msg);
    exit(1);
}

//gcc server.c  -o server && cp server ~/clion
// gcc client.c -o client
void push(int clientfd) {
    /* code */
    const int max_length = 1024;
    char buffer[max_length];
    // read filename and fileSize
    int n = read(clientfd, &buffer, max_length);
    // extract filename
    char filename[100] = {0};// file_size_str[100] = {0};
    int index = 0;
    for (; index < n; ++index) {
        if (buffer[index] == ' ') {
            buffer[index] = '\0';
            break;
        }
    }
    // space
    index++;
    strcpy(filename, buffer);
    //  extract file_size, 4 bytes.
    int file_size = 0;
    for (int i = 0; i < 3; ++i) {
        file_size = file_size | buffer[index++];
        file_size = file_size << 8;
    }
    file_size = file_size | buffer[index++];
    // skip space
    index++;
    //Now the buffer pointed to by index is the beginning of the file content
    FILE *fp = fopen(filename, "w");
    while (1) {
        // Write to file
        while (index < n && file_size > 0) {
            fputc(buffer[index], fp);
            index++;
            file_size--;
        }
        // Finish all
        if (file_size <= 0) {
            break;
        }
        n = read(clientfd, &buffer, sizeof(max_length));
    }

    printf("The file has been received successfully\n");
    char reply[] = "OK";
    write(clientfd, reply, strlen(reply));
    fclose(fp);
}

void pull(int clientfd) {
    const int max_length = 1024;
    char buffer[max_length];
    bzero(buffer, max_length);
    // read file name
    int n = read(clientfd, buffer, max_length);
    struct stat file_stat;
    int ret = stat(buffer, &file_stat);
    int file_size = file_stat.st_size;
    FILE *fp = fopen(buffer, "r");
    // send file size
    bzero(buffer, max_length);
    int file_size_cp = file_size;
    for (int i = 3; i >= 0; --i) {
        buffer[i] = file_size_cp & 0xff;
        file_size_cp = file_size_cp >> 8;
    }
    write(clientfd, buffer, 4);

    // send file data
    while (file_size > 0) {
        // read file.
        n = fread(buffer, sizeof(char), max_length, fp);
        // send
        write(clientfd, buffer, n);
        file_size -= n;
    }

    printf("The file has been sent successfully.\n");
    fclose(fp);
    char reply[] = "OK";
    write(clientfd, reply, strlen(reply));
}

/**
 * Load the user's username, password and directory from the file. And initialize the user folder.
 * Returns the number of users loaded.
 * Can be optimized and dynamically allocated memory. In that case, the number of users
 * loaded can not be limited by parameters
 */
int load_user_info(const char *file_name, struct user_info users[], int max_user_num) {
    FILE *fp = fopen(file_name, "r");
    int user_num = 0;
    const int MAX_LEN = 1024;
    char buffer[MAX_LEN];
    // Read user information in the file
    while (fgets(buffer, MAX_LEN, fp) != NULL && user_num < max_user_num) {
        //Replace the carriage return and line feed in the string.
        for (int i = 0; i < strlen(buffer); ++i) {
            if (buffer[i] == '\r' || buffer[i] == '\n') {
                buffer[i] = '\0';
            }
        }
        // Use ":" to split string
        char *user_name = strtok(buffer, ":");
        char *passwd = strtok(NULL, ":");
        char *path = strtok(NULL, ":");
        // Stored in the structure
        strcpy(users[user_num].username, user_name);
        strcpy(users[user_num].password, passwd);
        strcpy(users[user_num].filepath, path);

        // Initialize the folder
        init_directory(path);
        user_num++;
    }
    return user_num;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "port not provided\n");
        exit(1);
    }
    // load user information
    const int max_user_num = 100;
    const char user_file_name[] = "user_info.txt";
    int user_num = 0;
    struct user_info users[max_user_num];
    user_num = load_user_info(user_file_name, users, max_user_num);

    // init socket
    struct sockaddr_in serv_addr, cli_addr;
    int sockfd, clientfd;
    socklen_t clilen;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("Error opening Socket");
    }
    printf("===socket successful===\n");
    bzero((char *) &serv_addr, sizeof(serv_addr));

    int portno = atoi(argv[1]);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("Binding failed\n");
    }
    printf("===bind successful===\n");

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    printf(" watting for the connection\n");
    file_handle_init();
    while (1) {
        clientfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (clientfd < 0) {
            error("error on accept");
        }
        printf("connection successful\n");
//    printf("wait for the PUSH from client. when finished,print 'ok'\n");
        char space[10];
        // 0 means not logged in. 1 means logged in.
        int login_state = 0;
        while (1) {
            char command[10] = {0};
            int n = read(clientfd, command, 4);
            if (n == 0) {
                reset_current_dir();
                break;
            }
            if (strcmp(command, "LOGI") == 0) {
                //  LOGIN and a space. Clear the remaining two characters.
                read(clientfd, space, 2);
                login_state = login_verify(clientfd, users, user_num);
                continue;
            }
            // If the user doesn’t login, reject their commands (send back a message like "you aren’t logged in").
            if (login_state != ONLINE) {
                reject_commands(clientfd);
                continue;
            }


            if (strcmp(command, "PULL") == 0) {
                // read a space
                read(clientfd, space, 1);
                pull(clientfd);
            } else if (strcmp(command, "PUSH") == 0) {
                // read a space
                read(clientfd, space, 1);
                push(clientfd);
            } else if (strcmp(command, "QUIT") == 0) {
                close(clientfd);
                break;
            } else if (strcmp(command, "LS") == 0) {
                server_ls(clientfd);
            } else if (strcmp(command, "MKDI") == 0) {
                //  MKDIR and a space. Clear the remaining two characters.
                read(clientfd, space, 2);
                server_mkdir(clientfd);
            } else if (strcmp(command, " CD ") == 0) {
                server_cd(clientfd);
            }
        }
    }
    close(sockfd);
    return 0;
}