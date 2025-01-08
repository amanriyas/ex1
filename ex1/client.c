#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 2200

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    // Creating socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IP address from text to binary
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/Address not supported");
        return -1;
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        return -1;
    }

    printf("Connected to the server.\n");

    // Interaction loop with the server
    while (1) {
        printf("Client: ");
        fgets(buffer, 1024, stdin);  // Read message from client user

        send(sock, buffer, strlen(buffer), 0); // Send message to server
        memset(buffer, 0, sizeof(buffer));     // Clear the buffer

        int valread = read(sock, buffer, 1024); // Read server's response
        if (valread <= 0) break;  // Exit if server disconnects
        printf("Server: %s", buffer);  // Print server's message
    }

    close(sock); // Close the client socket
    return 0;
}
