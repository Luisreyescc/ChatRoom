# ChatRoom Application in C

Welcome to the **ChatRoom Application in C**, a simple client-server chat application built using C with support for multiple users, private messaging, and status management. The project uses **pthread** for handling multiple client connections and **cJSON** for managing data in JSON format.

## Features
- Public and private messaging between users.
- Status management: Users can set their status to active, away, or busy.
- Simple text-based interface for interaction.

## Requirements
Before building the project, ensure you have the following installed:
- **GCC**: The GNU Compiler Collection.
- **make**: To handle the build process.
- **pthread**: For handling concurrency.
- **cJSON**: A lightweight JSON parser.

## Getting Started

### Cloning the repository
First, clone the repository from GitHub:

```bash
git clone https://github.com/Luisreyescc/ChatRoom.git
cd ChatRoom
```

### Installing cJSON library
The project depends on the cJSON library, which you need to clone before compiling the code. Run the following command to download cJSON into the correct directory:
```
git clone https://github.com/DaveGamble/cJSON.git src/libs/cJSON
```

### Building the Project

To build the client and server binaries, simply run:

```bash
make
```

This will generate two binaries:
- `client`: The client application.
- `server`: The server application.

### Running the Server
To start the server, run the following command:

```bash
./server <server_ip> <port>
```

For example:

```bash
./server 127.0.0.1 8080
 ```

### Running the Client
To connect a client to the server, run the following command:

```bash
./client <server_ip> <port>
```

For example:

```bash
./client 127.0.0.1 8080
```

The client will prompt you for a username and then connect to the server.

### Commands in the Chat Application

Once connected to the chat, you can use the following commands to interact:

- `/public [message]`: Send a public message to all users.
- `/private [username] [message]`: Send a private message to a specific user.
- `/status [ACTIVE|AWAT|BUSy]`: Change your status to one of the following: **active**, **away**, or **busy**.
- `/users`: Show a list of all connected users.
- `/exit`: Leave the chat.

By default, any message without a command (slash `/`) is sent as a public message.

## Documentation

To generate the project documentation using **Doxygen**, run the following command:

```bash
make docs
```

This will create the documentation in the `docs/` directory.

To clean up the generated binaries and documentation, you can run:

```bash
make clean
```

This will remove the `client`, `server`, and all intermediate files, including the documentation.

## Project Structure
Here's an overview of the project structure:

```
.
├── Makefile              # The build instructions
├── README.md             # Project documentation
├── src/                  # Source code directory
│   ├── client/           # Client-side source code
│   ├── server/           # Server-side source code
│   └── libs/             # External libraries (e
