use std::net::TcpListener;
mod server; 
use server::handle_client; 

/// The entry point of the application.
///
/// This function creates a TCP listener on the address `127.0.0.1:8080` and listens 
/// for incoming connections. For each successful connection, it spawns a new thread 
/// to handle the client using the `handle_client` function.

fn main() {
    let listener = TcpListener::bind("127.0.0.1:8080").expect("Failed to bind to address");
    println!("Server listening on 127.0.0.1:8080");

    for stream in listener.incoming() {
        match stream {
            Ok(stream) => {
                std::thread::spawn(|| handle_client(stream));
            }
            Err(e) => {
                eprintln!("Failed to establish connection: {}", e);
            }
        }
    }
}
