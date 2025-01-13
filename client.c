#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>

#define SERVER_PORT 12345

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Usage: %s <server_ip> <num_requests> <interval_sec> <server_port>\n", argv[0]);
        return 1;
    }

    const char *server_ip = argv[1];
    int num_requests = atoi(argv[2]); // atoi converts string to integer
    int interval_sec = atoi(argv[3]);
    int server_port = atoi(argv[4]);

    struct sockaddr_in server_addr; // the struct is defined in the header file <netinet/in.h>
    server_addr.sin_family = AF_INET; // sets the address family for the server_addr structure to AF_INET (IPv4)
    server_addr.sin_port = htons(server_port); // Converts the port number to network byte order.
    if (inet_pton(AF_INET, server_ip, &(server_addr.sin_addr)) <= 0) // Converts the server IP string to binary format.
    {
        perror("Invalid server IP address");
        return 1;
    }

    int client_socket = socket(AF_INET, SOCK_STREAM, 0); // Creates a TCP socket (SOCK_STREAM)
    if (client_socket == -1) {
        perror("Error creating client socket");
        return 1;
    }
    
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error connecting to the server");
        return 1;
    }

    for (int i = 0; i < num_requests; i++) {
        // Get the current time before sending the ping
        struct timeval start_time, end_time;
        gettimeofday(&start_time, NULL);

        // Send the ping request
        char ping_message[] = "Ping!";
        send(client_socket, ping_message, sizeof(ping_message), 0);

        // Receive the acknowledgment
        char ack_buffer[1024];
        recv(client_socket, ack_buffer, sizeof(ack_buffer), 0);

        // Get the current time after receiving the acknowledgment
        gettimeofday(&end_time, NULL);

        // Calculate and display the RTT
        double rtt = (double)(end_time.tv_usec - start_time.tv_usec) / 1000.0;
        printf("Received acknowledgment: %s, RTT: %.2f ms\n", ack_buffer, rtt);

        sleep(interval_sec);  // Wait for the specified interval before sending the next ping
    }

    close(client_socket);
    return 0;
}

