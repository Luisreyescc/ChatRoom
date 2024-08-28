use std::io::{Read, Write};
use std::net::{TcpStream};

/// Manages communication with a client over a TCP stream.
/// 
/// # Arguments
///
/// * `stream` - A `TcpStream` representing the connection with the client.
/// 
/// # Description
///
/// This function continuously reads data from the provided TCP stream into a buffer,
/// converts the received data to a UTF-8 string, and prints the 
/// request. It then sends a "Hello from the Server!" response back to the client.
/// 
/// The loop will continue until the client disconnects, at which point the function
/// will exit. 
///
/// # Panics
///
/// This function will panic if:
/// * Reading from the stream fails.
/// * Writing to the stream fails.
pub fn handle_client(mut stream: TcpStream) {
    let mut buffer = [0; 1024];
    while let Ok(bytes_read) = stream.read(&mut buffer) {
       
        if bytes_read == 0 {
            println!("Client disconnected");
            break;
        }
     
        let request = String::from_utf8_lossy(&buffer[..bytes_read]);
        println!("Received request: {}", request);
       
        let response = "Hello from the Server!".as_bytes();
        stream.write(response).expect("Failed to write response!");
    
    }
}

