# Testing the binary communications

The code in the client folder takes modbus commands from the command line. The server file is a file for the arduinos that has a register of size 2. To put on multiple arduinos simply change the if (server_address == 1) to numbers other than 1 for multiple servers.