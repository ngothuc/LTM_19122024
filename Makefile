# Biến Compiler và Flags
CC = gcc
CFLAGS = -Wall -g

# Tên file output
SERVER = server
CLIENT = client

# Các target mặc định
all: $(SERVER) $(CLIENT)

# Biên dịch server
$(SERVER): server.c
	$(CC) $(CFLAGS) -o $(SERVER) server.c

# Biên dịch client
$(CLIENT): client.c
	$(CC) $(CFLAGS) -o $(CLIENT) client.c

# Clean file biên dịch
clean:
	rm -f $(SERVER) $(CLIENT)

# Run server
run_server: $(SERVER)
	./$(SERVER)

# Run client
run_client: $(CLIENT)
	./$(CLIENT)
