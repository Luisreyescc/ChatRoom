# Define the compiler and compilation flags
CC = gcc  
CFLAGS = -Wall -Wextra -O2 -lpthread -Ilibs/cJSON  # Compilation flags, including pthread and cJSON path
LDFLAGS = -lm  # Linker flags, if cJSON requires math.h

# Paths for the client and server binaries
CLIENT_BIN = client
SERVER_BIN = server

# Source directories
CLIENT_SRC_DIR = src/client
SERVER_SRC_DIR = src/server
CJSON_SRC = src/libs/cJSON/cJSON.c  # Path to the cJSON source file

# Build both client and server
all: $(CLIENT_BIN) $(SERVER_BIN)

# Rule to compile the client
# It compiles the client's main and client source files along with cJSON
$(CLIENT_BIN): $(CLIENT_SRC_DIR)/main.c $(CLIENT_SRC_DIR)/client.c $(CLIENT_SRC_DIR)/client.h $(CJSON_SRC)
	$(CC) $(CFLAGS) $(CLIENT_SRC_DIR)/main.c $(CLIENT_SRC_DIR)/client.c $(CJSON_SRC) -o $(CLIENT_BIN) $(LDFLAGS)

# Rule to compile the server
# It compiles the server's main and server source files along with cJSON
$(SERVER_BIN): $(SERVER_SRC_DIR)/main.c $(SERVER_SRC_DIR)/server.c $(SERVER_SRC_DIR)/server.h $(CJSON_SRC)
	$(CC) $(CFLAGS) $(SERVER_SRC_DIR)/main.c $(SERVER_SRC_DIR)/server.c $(CJSON_SRC) -o $(SERVER_BIN) $(LDFLAGS)

# Rule to clean the generated binaries
# This removes both the client and server binaries
clean:
	rm -f $(CLIENT_BIN) $(SERVER_BIN)

