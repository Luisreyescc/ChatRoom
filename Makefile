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

# Source files for the client
CLIENT_SRC_FILES = $(CLIENT_SRC_DIR)/main.c \
                   $(CLIENT_SRC_DIR)/connection.c \
                   $(CLIENT_SRC_DIR)/messaging.c \
                   $(CLIENT_SRC_DIR)/tui.c

# Source files for the server
SERVER_SRC_FILES = $(SERVER_SRC_DIR)/main.c \
                   $(SERVER_SRC_DIR)/connection.c \
                   $(SERVER_SRC_DIR)/client_manager.c\
					$(SERVER_SRC_DIR)/messaging.c

# Build both client and server
all: $(CLIENT_BIN) $(SERVER_BIN)

# Rule to compile the client
$(CLIENT_BIN): $(CLIENT_SRC_FILES) $(CJSON_SRC)
	$(CC) $(CFLAGS) $(CLIENT_SRC_FILES) $(CJSON_SRC) -o $(CLIENT_BIN) $(LDFLAGS)

# Rule to compile the server
$(SERVER_BIN): $(SERVER_SRC_FILES) $(CJSON_SRC)
	$(CC) $(CFLAGS) $(SERVER_SRC_FILES) $(CJSON_SRC) -o $(SERVER_BIN) $(LDFLAGS)

# Rule to clean the generated binaries
clean:
	rm -f $(CLIENT_BIN) $(SERVER_BIN)
