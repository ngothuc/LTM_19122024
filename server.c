#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 2502
#define BUFFER_SIZE 1024

void handle_client(int client_socket);

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;

    // Tạo socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    // Cấu hình địa chỉ server
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Gán socket với địa chỉ
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(server_socket);
        exit(1);
    }

    // Lắng nghe kết nối
    if (listen(server_socket, 10) == -1) {
        perror("Listen failed");
        close(server_socket);
        exit(1);
    }

    printf("Server is listening on port %d...\n", PORT);

    while (1) {
        addr_size = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_size);
        if (client_socket == -1) {
            perror("Accept failed");
            continue;
        }

        printf("Connected to a client\n");
        handle_client(client_socket);
    }

    close(server_socket);
    return 0;
}

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    char login_name[50] = "guest"; // Mặc định là khách
    FILE *log_file;

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);

        if (bytes_received <= 0) {
            printf("Client disconnected\n");
            break;
        }

        // Tách header và body
        char header[50], body[BUFFER_SIZE];
        sscanf(buffer, "%s %[^\n]", header, body);

        // Xử lý lệnh từ client
        if (strcmp(header, "LOGIN") == 0) {
            if (strcmp(login_name, "guest") != 0) {
                printf("%s logged out\n", login_name);
            }

            // Đăng nhập
            strcpy(login_name, body);
            printf("User logged in: %s\n", login_name);

        } else if (strcmp(header, "MESSAGE") == 0) {
            // Lựa chọn file log phù hợp
            if (strcmp(login_name, "GUEST") == 0) {
                log_file = fopen("guest_log.txt", "a");
            } else {
                char file_name[100];
                snprintf(file_name, sizeof(file_name), "%s_log.txt", login_name);
                log_file = fopen(file_name, "a");
            }

            if (log_file == NULL) {
                perror("Failed to open log file");
            } else {
                fprintf(log_file, "%s: %s\n", login_name, body);
                fclose(log_file);
                printf("Message logged from %s: %s\n", login_name, body);
            }
        } else {
            printf("Unknown command from client: %s\n", buffer);
        }
    }

    close(client_socket);
}
