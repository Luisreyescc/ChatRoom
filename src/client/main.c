#include "client.h"
#include <stdio.h>
#include <unistd.h>

/**
 * @brief Main function that demonstrates the creation and connection of a `Client` 
 *        to a server at a specified IP address and port.
 *
 * The program creates a `Client` instance with the IP address "127.0.0.1" and 
 * port 8080. It then attempts to connect to the server using this client. If 
 * the client creation or connection fails, appropriate error messages are 
 * displayed, and the program exits with a failure status. If the connection 
 * is successful, a success message is printed and a new message appears in 
 * server, at the same time the client receive a new message from the server. 
 * Finally, the program closes the socket and frees the allocated memory 
 * before exiting (Client_destroy()).
 *
 * @return int Returns 0 on successful execution, or -1 if client creation or 
 *             connection fails.
 */
int main() {
    Client *client = Client_create("127.0.0.1", 8080);
    if (!client) {
        fprintf(stderr, "Failed to create client\n");
        return -1;
    }

    if (Client_connect(client) == 0) {
        printf("Successful connection to the server\n"); 
        
        Client_send(client, "Hi from the Client!");
       
        char buffer[1024] = {0};
        Client_receive(client, buffer, sizeof(buffer));
       
        printf("Response was: %s\n", buffer);
    } else 
        printf("Failed to connect to the server");

    Client_destroy(client);
    return 0;
}
