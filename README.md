# Simple Time Server/Client Application

This project contains a simple TCP server and client application written in C. The server listens on a predefined port for incoming connections. Upon connection, the server sends the current time to the client and logs the connection. The client connects to a given server address and receives the current time from the server.

## Features

- TCP server that listens for incoming connections.
- Server sends current time in human-readable format to the client upon connection.
- Client establishes a connection with the server and receives the current time.
- Server logs the number of connections and the client's hostname.

## Compilation

To compile the server and client application, use the following command:

```bash
gcc -o time_server_client your_source_file.c
Replace your_source_file.c with the actual file name of your source code.
```

## Usage

To start the server, run the following command in the terminal:

```bash
Copy code
./time_server_client server
```
This will start the server on the predefined port (49999).


To connect to the server as a client, run the following command in the terminal:

```bash
Copy code
./time_server_client client <server_address>
```

Replace <server_address> with the actual IP address or hostname of the server.

## Configuration

The server port is defined as MY_PORT_NUMBER in the source code. Change this value to use a different port for the server.


## Notes

The server is configured to handle one connection at a time for simplicity.
Error handling is implemented for socket operations and child process creation.
This code is meant to be run on UNIX-like operating systems with support for the POSIX API.