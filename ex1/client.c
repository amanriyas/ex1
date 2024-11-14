#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 256

// Function to connect to the server
int connect_to_server(const char *server_ip, int server_port) {
    int client_sock;
    struct sockaddr_in server_addr;

    // Create socket
    if ((client_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid server IP address");
        close(client_sock);
        exit(EXIT_FAILURE);
    }

    // Connect to server
    if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection to server failed");
        close(client_sock);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server at %s:%d\n", server_ip, server_port);
    return client_sock;
}

// Function to send a command to the server and print the response
void send_command(int client_sock, const char *command) {
    char buffer[BUFFER_SIZE];
    int bytes_received;

    // Send command to server
    if (send(client_sock, command, strlen(command), 0) < 0) {
        perror("Failed to send command");
        return;
    }

    // Receive response from server
    bytes_received = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        printf("Server response:\n%s\n", buffer);
    } else {
        printf("No response from server or connection closed\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <server_ip> <server_port> <command> [arguments]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *server_ip = argv[1];
    int server_port = atoi(argv[2]);

    // Connect to the server
    int client_sock = connect_to_server(server_ip, server_port);

    // Build the command string from arguments
    char command[BUFFER_SIZE] = {0};
    for (int i = 3; i < argc; i++) {
        strcat(command, argv[i]);
        if (i < argc - 1) strcat(command, " ");
    }

    // Send command and receive response
    send_command(client_sock, command);

    // Close the socket
    close(client_sock);
    return 0;
}
