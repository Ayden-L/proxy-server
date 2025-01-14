#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#define BUFFER_SIZE 1024
#define INITIAL_ARRAY_SIZE 10
#define PORT 9090
// #define DEBUG

char **database = NULL;                     // Array to store database
int db_size;                                // Size of database
int db_fd;                                  // File descriptor
ssize_t bytes_read;                         // Number of bytes read
char buffer[BUFFER_SIZE];                   // Buffer for reading file data
size_t line_count = 0;                      // Number of database
size_t array_size = INITIAL_ARRAY_SIZE;     // Current array capacity
char *line_buffer = NULL;                   // Temporary buffer to store a single line
size_t line_length = 0;                     // Length of the current line

int populate_db() {

    // Open the file
    db_fd = open("input.txt", O_RDONLY);
    if (db_fd == -1) {
        perror("[CLIENT] Error opening file");
        return -EXIT_FAILURE;
    }

    // Allocate memory for the initial array
    database = malloc(array_size * sizeof(char *));
    if (database == NULL) {
        perror("[CLIENT] Memory allocation failed");
        close(db_fd);
        return -EXIT_FAILURE;
    }

    // read the file in chunks
    while ((bytes_read = read(db_fd, buffer, BUFFER_SIZE)) > 0) {
        for (ssize_t i = 0; i < bytes_read; i++) {
            if (buffer[i] == '\n') {
                // Allocate memory for the current line and copy the data
                char *new_line = malloc(line_length + 1);
                if (!new_line) {
                    perror("[CLIENT] Memory allocation failed");
                    free(line_buffer);
                    return -EXIT_FAILURE;
                }
                // copy line_length amount of bytes from line_buffer to new_line
                memcpy(new_line, line_buffer, line_length);
                new_line[line_length] = '\0';           // null-terminate the byte sequence to create a string

                // Add the line to the array
                if (line_count >= array_size) {
                    array_size *= 2; // Double the array size
                    char **new_database = realloc(database, array_size * sizeof(char *));
                    if (!new_database) {
                        perror("[CLIENT] Memory allocation failed");
                        free(new_line);
                        free(line_buffer);
                        return -EXIT_FAILURE;
                    }
                    database = new_database;
                }
                database[line_count++] = new_line;

                // Reset line buffer for the next line
                free(line_buffer);
                line_buffer = NULL;
                line_length = 0;
                db_size += 1;
            } else {
                // Append character to the current line
                char *new_buffer = realloc(line_buffer, line_length + 1);
                if (!new_buffer) {
                    perror("[CLIENT] Memory allocation failed");
                    free(line_buffer);
                    return -EXIT_FAILURE;
                }
                line_buffer = new_buffer;
                line_buffer[line_length++] = buffer[i];
            }
        }
    }

    // Handle any remaining line (if the file does not end with a newline)
    if (line_length > 0) {
        char *new_line = malloc(line_length + 1);
        if (!new_line) {
            perror("[CLIENT] Memory allocation failed");
            free(line_buffer);
            return -EXIT_FAILURE;
        }
        memcpy(new_line, line_buffer, line_length);
        new_line[line_length] = '\0'; // Null-terminate the line
        database[line_count++] = new_line;
        free(line_buffer);
        db_size += 1;
    }

    if (bytes_read == -1) {
        perror("[CLIENT] Error reading file");
        return -EXIT_FAILURE;
    }

    // for (int i = 0; i < 10; i++) {
    //     printf("Line: %s\n", database[i]);
    // }

    return EXIT_SUCCESS;
}

int connect_to_server() {
    // open/create the socket
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("[CLIENT] Error opening socket");
        return -EXIT_FAILURE;
    }

    printf("[CLIENT] Created client socket\n");

    int status;

    // configure the server address structure
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    char* localhost = "127.0.0.1";
    // convert the IPv4 address from text to binary form:
    if (inet_pton(AF_INET, localhost, &serv_addr.sin_addr) <= 0) {
        perror("[CLIENT] Invalid server address");
        return -EXIT_FAILURE;
    }
    printf("[CLIENT] Converted IPv4 address from text to binary\n");

    // connect to the server
    if ((status = connect(sock_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0) {
        perror("[CLIENT] Connectin to server failed");
        return -EXIT_FAILURE;
    }
    printf("[CLIENT] Connected to server\n");

    // send a test message to server
    printf("[CLIENT] db_size: %d\n", db_size);
    for (int i = 0; i < db_size; i++) {
        // char* data = (char*) malloc(strlen(database[i]) + 1);
        // strcpy(data, database[i]);
        // data[strlen(database[i])] = '\0';
        char* data = "blah";
        send(sock_fd, data, strlen(data), 0);
        // printf("[CLIENT] strlen(database[i]): %d\n", strlen(database[i]));
        // printf("[CLIENT] Message was sent to server: %s\n", database[i]);
    }

    char* response_buffer = (char*) malloc(BUFFER_SIZE);
    if (!response_buffer) {
        perror("[CLIENT] Error allocating memeory");
        return -EXIT_FAILURE;
    }
    // read the response from the server
    read(sock_fd, response_buffer, BUFFER_SIZE);
    printf("[CLIENT] Received response from server: %s\n", response_buffer);

    close(sock_fd);

    return 0;
}

int main(int argc, char** argv) {

    printf("[CLIENT] Calling populate_db()\n");
    if (populate_db() != 0) {
        printf("[CLIENT] populate_db() failed\n");
        return -EXIT_FAILURE;
    }
    printf("[CLIENT] populate_db() SUCCESS\n");

    printf("[CLIENT] Connecting to server\n");
    if (connect_to_server() != 0) {
        printf("[CLIENT] Failed to connect to server\n");
        return -EXIT_FAILURE;
    }

    close(db_fd);
    for(ssize_t i = 0; i < line_count; i++) {
        free(database[i]);
    }
    free(database);

}