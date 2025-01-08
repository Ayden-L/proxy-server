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
// #define DEBUG

int port = 55521;
char** domains;
int domains_index = 0;

int add_to_db(char* line) {
    printf("line to add: %s\n", line);
    // add the domain name that is in the db to the domains array
    domains[domains_index] = line;
    printf("added: %s to domains[%d]\n", domains[domains_index], domains_index);
    domains_index += 1;
    return 0;
}

int main(int argc, char** argv) {

    #ifdef DEBUG
        printf("start of main\n");
    #endif

    domains = (char**) malloc(sizeof(BUFFER_SIZE));

    // open input file:
    int fd = open("input.txt", O_RDONLY);
    if (fd == -1) {
        perror("Error opening input file");
        free(domains);
        return -EXIT_FAILURE;
    }

    #ifdef DEBUG
        printf("opened input file successfully\n");
    #endif 

    // read database into the domains array
    unsigned int bytes_read = 0;
    char buffer[BUFFER_SIZE];
    char line[BUFFER_SIZE];
    unsigned int line_length = 0;
    int num_lines = 0;
    while ((bytes_read = read(fd, buffer, BUFFER_SIZE)) > 0) {
        for (int i = 0; i < bytes_read; i++) {
            if (buffer[i] == '\n' || buffer[i] == '\r') {
                // handle end of line
                line[line_length] = '\0';
                add_to_db(line);
                line_length = 0;
                num_lines += 1;
            } else if (line_length < BUFFER_SIZE - 1) {
                line[line_length++] = buffer[i];
            }
        }
    }

    if (line_length > 0) add_to_db(line);
    if (num_lines == 0) printf("Database input file is empty, pleae provide at least ONE valid entry\n");
    if (bytes_read == -1) {
        perror("Error reading file");
        close(fd);
        free(domains);
        return -EXIT_FAILURE;
    }

    /* 
        testing the population of the domains array
    */
    for (int i = 0; i < num_lines; i++) {
        printf("printing domain: %s\n", domains[i]);
    }

    // open socket
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        free(domains);
        perror("Error opening socket");
        return -EXIT_FAILURE;
    }

    close(fd);
    free(domains);


}