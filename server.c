#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUFFER_SIZE 1024

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    char login_name[50] = "";
    FILE *log_file = NULL;

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            printf("Client disconnected.\n");
            break;
        }

        if (strncmp(buffer, "LOGIN", 5) == 0) {
            sscanf(buffer + 6, "%s", login_name);
            printf("Client logged in as: %s\n", login_name);
            char log_file_name[60];
            sprintf(log_file_name, "%s_log.txt", login_name);
            log_file = fopen(log_file_name, "a");
        } else if (strncmp(buffer, "MESSAGE", 7) == 0 && strlen(login_name) > 0) {
            char message[BUFFER_SIZE];
            strcpy(message, buffer + 8);
            printf("Message from %s: %s\n", login_name, message);
            if (log_file) {
                fprintf(log_file, "%s\n", message);
                fflush(log_file);
            }
        } else {
            printf("Invalid command or not logged in.\n");
        }
    }

    if (log_file) fclose(log_file);
    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 5) < 0) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    while (1) {
        addr_size = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);
        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }
        printf("Client connected.\n");
        if (fork() == 0) {
            handle_client(client_socket);
            exit(0);
        }
        close(client_socket);
    }

    close(server_socket);
    return 0;
}
