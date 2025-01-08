# Firewall Configuration Manager

This project implements a client-server system for managing firewall configurations. The system consists of two main components: a server program that maintains firewall rules and a client program that can send commands to the server.

## Features

- Maintain a collection of firewall rules
- Track IP addresses and ports that match each rule
- Keep history of all submitted requests
- Support for both interactive and network-based operation modes
- Concurrent request handling
- Memory leak-free implementation

## Prerequisites

- GCC compiler
- Make build system
- POSIX-compliant environment (Linux/WSL)
- Basic network connectivity for client-server mode

## Building the Project

The project can be built using the provided Makefile:

```bash
make
```

This will compile both the server and client programs.

## Running the Programs

### Server Program

The server can be run in two modes:

1. Interactive Mode:
```bash
./server -i
```
In this mode, the server reads commands from standard input and prints responses directly.

2. Network Mode:
```bash
./server <port>
```
In this mode, the server listens for incoming connections on the specified port.

### Client Program

To use the client program (when server is in network mode):
```bash
./client <serverHost> <serverPort> <command>
```

Example:
```bash
./client localhost 2200 A 147.188.193.15 22
```

## Supported Commands

- `R` - List all requests in chronological order
- `A <rule>` - Add a new firewall rule
- `C <IPAddress> <port>` - Check if an IP address and port are allowed
- `D <rule>` - Delete a specific rule
- `L` - List all rules and their matched queries

### Rule Format
Rules should be specified in the following format:
```
<IPAddresses> <ports>
```

Where:
- `<IPAddresses>` can be either:
  - A single IP address (xxx.xxx.xxx.xxx)
  - A range (IPAddress1-IPAddress2)
- `<ports>` can be either:
  - A single port number (0-65535)
  - A port range (port1-port2)

Example rules:
```
147.188.193.0-147.188.194.255 21-22
147.188.192.41 443
```

## WSL Compatibility

This project is fully compatible with Windows Subsystem for Linux (WSL). To run in WSL:

1. Ensure you have WSL installed and configured on your Windows system
2. Open your WSL terminal
3. Navigate to the project directory
4. Build and run the programs as described above

## Testing

A basic test script is provided to verify the core functionality:

```bash
./test.sh
```

The test script should be run from the directory containing the compiled binaries.

## Error Handling

The system includes comprehensive error handling for:
- Invalid IP addresses and ports
- Malformed rules
- Invalid commands
- Network communication errors

## Notes

- IP addresses in ranges must be specified with the smaller address first
- Port ranges must be specified with the smaller port first
- Each rule component should be separated by exactly one space
- The server maximizes concurrency for optimal performance
- Memory management is carefully implemented to prevent leaks

