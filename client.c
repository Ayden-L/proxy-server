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
#define PORT 8000
// #define DEBUG

int main(int argc, char** argv) {

    int fd;                       // File descriptor
    ssize_t bytes_read;           // Number of bytes read
    char buffer[BUFFER_SIZE];     // Buffer for reading file data
    char **lines = NULL;          // Array to store lines
    size_t line_count = 0;        // Number of lines
    size_t array_size = INITIAL_ARRAY_SIZE; // Current array capacity
    char *line_buffer = NULL;     // Temporary buffer to store a single line
    size_t line_length = 0;       // Length of the current line

    // Open the file
    fd = open("input.txt", O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    // Allocate memory for the initial array
    lines = malloc(array_size * sizeof(char *));
    if (lines == NULL) {
        perror("Memory allocation failed");
        close(fd);
        return EXIT_FAILURE;
    }

    // Read the file in chunks
    while ((bytes_read = read(fd, buffer, BUFFER_SIZE)) > 0) {
        for (ssize_t i = 0; i < bytes_read; i++) {
            if (buffer[i] == '\n') {
                // Allocate memory for the current line and copy the data
                char *new_line = malloc(line_length + 1);
                if (!new_line) {
                    perror("Memory allocation failed");
                    free(line_buffer);
                    return -EXIT_FAILURE;
                }
                // copy line_length amount of bytes from line_buffer to new_line
                memcpy(new_line, line_buffer, line_length);
                new_line[line_length] = '\0';           // null-terminate the byte sequence to create a string

                // Add the line to the array
                if (line_count >= array_size) {
                    array_size *= 2; // Double the array size
                    char **new_lines = realloc(lines, array_size * sizeof(char *));
                    if (!new_lines) {
                        perror("Memory allocation failed");
                        free(new_line);
                        free(line_buffer);
                        return -EXIT_FAILURE;
                    }
                    lines = new_lines;
                }
                lines[line_count++] = new_line;

                // Reset line buffer for the next line
                free(line_buffer);
                line_buffer = NULL;
                line_length = 0;
            } else {
                // Append character to the current line
                char *new_buffer = realloc(line_buffer, line_length + 1);
                if (!new_buffer) {
                    perror("Memory allocation failed");
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
            perror("Memory allocation failed");
            free(line_buffer);
            return -EXIT_FAILURE;
        }
        memcpy(new_line, line_buffer, line_length);
        new_line[line_length] = '\0'; // Null-terminate the line
        lines[line_count++] = new_line;
        free(line_buffer);
    }

    if (bytes_read == -1) {
        perror("Error reading file");
        return -EXIT_FAILURE;
    }

    // open/create the socket
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("Error opening socket");
        return -EXIT_FAILURE;
    }

    int status;

    // configure the server address structure
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // convert the IPv4 address from tet to binary form:
    if (inet_pton(AF_INET, 'localhost', &serv_addr.sin_addr) <= 0) {
        perror("Invalid address");
        return -EXIT_FAILURE;
    }

    // connect to the server
    if ((status = connect(sock_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0) {
        perror("Connectin to server failed");
        return -EXIT_FAILURE;
    }   

    // send a test message to server
    char* test_message = "Hello server, this is the client speaking";
    send(sock_fd, test_message, strlen(test_message), 0);
    printf("Message was sent to server: %s\n", test_message);


    char* response_buffer = (char*) malloc(BUFFER_SIZE);
    if (!response_buffer) {
        perror("Error allocating memeory");
        return -EXIT_FAILURE;
    }
    // read the response from the server
    read(sock_fd, response_buffer, BUFFER_SIZE);
    printf("Received response from server: %s\n", response_buffer);




    // end with
    close(fd);
    close(sock_fd);
    for(ssize_t i = 0; i < line_count; i++) {
        free(lines[i]);
    }
    free(lines);

}