#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

#define SERVER_PORT 12345
#define LOG_FILE "ping_log.txt"

void *handle_ping(void *client_socket) {
    int client_sock = *((int *)client_socket);
    char buffer[1024];
    ssize_t bytes_received;

    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    getpeername(client_sock, (struct sockaddr *)&client_addr, &client_addr_len);

    // Receive the ping request
    bytes_received = recv(client_sock, buffer, sizeof(buffer), 0);
    if (bytes_received < 0) {
        perror("Error receiving data");
    } else {
        buffer[bytes_received] = '\0';

        // Log the incoming ping request
        char log_entry[256];
        snprintf(log_entry, sizeof(log_entry), "Received ping from %s:%d - Payload: %s\n",
                 inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buffer);
        int log_fd = open(LOG_FILE, O_WRONLY | O_CREAT | O_APPEND, 0666);
        if (log_fd == -1) {
            perror("Error opening log file");
        } else {
            write(log_fd, log_entry, strlen(log_entry));
            close(log_fd);
        }

        // Respond with an acknowledgment containing the same payload
        send(client_sock, buffer, strlen(buffer), 0);
    }

    close(client_sock);
    free(client_socket);
    return NULL;
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // Create a socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating server socket");
        exit(1);
    }

    server_addr.sin_family = AF_INET; // sets the address family for the server_addr structure to AF_INET (IPv4)
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(SERVER_PORT);

    // Bind the socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding server socket");
        exit(1);
    }

    // Listen for incoming connections
    if (listen(server_socket, 5) == -1) {
        perror("Error listening for connections");
        exit(1);
    }

    printf("Server is listening on port %d\n", SERVER_PORT);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket == -1) {
            perror("Error accepting connection");
        } else {
            pthread_t tid;
            int *client_sock_ptr = malloc(sizeof(int));
            *client_sock_ptr = client_socket;
            pthread_create(&tid, NULL, handle_ping, client_sock_ptr);
        }
    }

    close(server_socket);
    return 0;
}

