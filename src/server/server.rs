use std::io::{Read, Write};
use std::net::{TcpStream};

/// Handles the communication with a client through a TCP stream.
/// 
/// # Arguments
///
/// * `stream` - A TcpStream representing the connection with the client.
/// 
/// # Description
///
/// This function reads data from the provided TCP stream into a buffer,
/// converts the received data to a UTF-8 string, and then prints the 
/// request. Finally, it sends a "Hello, Client!" response back to the client.
///
/// # Panics
///
/// This function will panic if:
/// * Reading from the stream fails.
/// * Writing to the stream fails.
pub fn handle_client(mut stream: TcpStream){
    let mut buffer = [0; 1024];
    stream.read(&mut buffer).expect("Failed to read from client!");
    
    let request = String::from_utf8_lossy(&buffer[..]);
    println!("Received request: {}", request);
    let response = "Hello, Client!".as_bytes();
    stream.write(response).expect("Failed to write response!");
}

