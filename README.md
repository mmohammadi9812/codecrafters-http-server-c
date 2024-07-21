# HTTP Server Challenge

This repository contains my attempt to solve the ["Build Your Own HTTP server" Challenge](https://app.codecrafters.io/courses/http-server/overview) using the C programming language.

## Project Overview

In this challenge, I successfully built an HTTP/1.1 server capable of serving multiple clients simultaneously. This project provided an excellent opportunity to deepen my understanding of network programming and web protocols.

## Key Learning Points

Throughout the development process, I gained valuable insights into:

1. TCP server implementation
2. [HTTP request syntax](https://www.w3.org/Protocols/rfc2616/rfc2616-sec5.html) and structure
3. Concurrent client handling
4. File I/O operations in C
5. Command-line argument parsing

## Features

- HTTP/1.1 protocol support
- Multi-client handling
- Basic file read and write operations
- Directory-based file serving

## Running the Server

To run this HTTP server on your local machine, follow these steps:

1. Ensure you have `gcc` (GNU Compiler Collection) installed on your system.
2. Clone this repository to your local machine.
3. Navigate to the project directory in your terminal.
4. Run the server using the following command: `./your_server.sh`
5. To enable file read and write operations, use the `--directory` flag followed by the path to the directory you want to serve: `./your_server.sh --directory /path/to/your/directory`

## Testing

You can test the server using tools like `curl` or by accessing it through a web browser. The server should respond to various HTTP methods and serve files from the specified directory when applicable.

## Future Improvements

Potential areas for enhancement include:
- Implementing more HTTP methods
- Adding support for HTTPS
- Improving error handling and logging
- Optimizing performance for high concurrency

## Contributions

Feedback and contributions are welcome! Feel free to open issues or submit pull requests to help improve this project.
