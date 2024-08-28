# Definition of the generated binaries
CLIENT_BIN = client                  # Path for the client binary
SERVER_BIN = src/server/target/debug/server  # Path for the server binary

# Build both client and server
all: client server

# Rule to compile the client
client:
	$(CC) src/client/main.c src/client/client.c -o $(CLIENT_BIN)  # Compile the client source files into the client binary

# Rule to compile the server
server:
	cargo build --manifest-path src/server/Cargo.toml  # Build the server using Cargo and the specified manifest path

# Rule to clean the generated binaries
clean:
	rm -f $(CLIENT_BIN)  # Remove the client binary
	cargo clean --manifest-path src/server/Cargo.toml  # Clean the server build artifacts using Cargo

# Rule to run the client
run-client: client
	./$(CLIENT_BIN)  # Execute the client binary

# Rule to run the server
run-server: server
	./$(SERVER_BIN)  # Execute the server binary
