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

char ** lines = NULL;

int populate_database() {
    return 0;
}

int connect_to_client() {

    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("[SERVER] Error opening server socket");
        return -EXIT_FAILURE;
    }

    printf("[SERVER] Created Socket\n");

    int status = 0;
    // configure the client address
    struct sockaddr_in client_addr;
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(PORT);

    // bind socket to the port:
    if (bind(sock_fd, (struct sockaddr*)&client_addr, sizeof(client_addr)) < 0) {
        perror("[SERVER] Server bind has failed");
        return -EXIT_FAILURE;
    }
    printf("[SERVER] Binded socket to port\n");


    // listen on the socket for new connections:
    if (listen(sock_fd, 3) < 0) {
        perror("[SERVER] Failure on listen()");
        return -EXIT_FAILURE;
    }
    printf("[SERVER] Listening to port for connection requests\n");

    int new_socket;
    socklen_t client_add_len = sizeof(client_addr);
    if ((new_socket = accept(sock_fd, (struct sockaddr*)&client_addr, &client_add_len)) < 0) {
        perror("[SERVER] Failure on accept()");
        return -EXIT_FAILURE;
    }
    printf("[SERVER] Accepted connection request and created a new socket\n");

    int val_read;
    char* buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    while (1) {
        if ((val_read = read(new_socket, buffer, BUFFER_SIZE)) == 0) {
            printf("[SERVER] EOF\n");
            break;
        }
        // printf("[SERVER] val_read: %d\n", val_read);
        printf("[SERVER] Received: \"%s\", from client\n", buffer);
    
        char* msg_to_client = "[SERVER] Hello from Server!";
        send(new_socket, msg_to_client, strlen(msg_to_client), 0);
        printf("[SERVER] Sent hello message\n");
    }


    close(new_socket);
    close(sock_fd);
    return 0;
}

int main(int argc, char** argv) {


    if (connect_to_client() < 0) {
        printf("[SERVER] Failed to connect and send message to client\n");
    }

}