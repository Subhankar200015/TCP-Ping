### Project Description:  
This project implements a Ping Server and Client Application using socket programming in C. The application allows a client to send "Ping" requests to a server, which then logs the request and responds with an acknowledgment. It is designed to simulate a basic client-server interaction with round-trip time (RTT) measurement.

### Key Features  
**Server (server.c):**  
Listens on a specified port (default: 12345).
Handles multiple clients concurrently using multithreading (pthread library).
Logs incoming ping requests (IP address, port, and payload) to a file (ping_log.txt).
Responds to the client with the same payload.

**Client (client.c):**  
Sends a configurable number of ping requests to the server.
Calculates and displays the Round-Trip Time (RTT) for each ping.
Allows users to specify the server IP, number of requests, interval between pings, and server port via command-line arguments.


### Setup and Execution
**Server:**  
Compile: gcc -pthread server.c -o server
Run: ./server
The server listens on 127.0.0.1:12345 by default.

**Client:**  
Compile: gcc client.c -o client
Run: ./client <server_ip> <num_requests> <interval_sec> <server_port>  
*Example:*
./client 127.0.0.1 10 1 12345  
Sends 10 ping requests to 127.0.0.1 on port 12345 with a 1-second interval between pings.
