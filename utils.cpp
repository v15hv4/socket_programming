#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdarg>
#include <iostream>
#include <tuple>

#include "colors.h"
using namespace std;

// utility function to read string from socket
pair<string, int> read_socket(const int& fd, int bytes) {
    string message;
    message.resize(bytes);

    int bytes_received = read(fd, &message[0], bytes - 1);
    if (bytes_received <= 0) {
        cerr << "Failed to read data from socket.\n";
    }

    message[bytes_received] = 0;
    message.resize(bytes_received);

    return {message, bytes_received};
}
// }}}

// utility function to send string to socket
int write_socket(int fd, const string& s) {
    int bytes_sent = write(fd, s.c_str(), s.length());
    if (bytes_sent < 0) {
        cerr << "Failed to send data via socket.\n";
    }

    return bytes_sent;
}

// utility function to connect to server and return file descriptor
int connect_to_server(struct sockaddr_in* ptr) {
    struct sockaddr_in server_addr = *ptr;
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        cerr << "Error creating client socket.\n";
        exit(-1);
    }

    // establish connection
    if (connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "Error connecting to server.\n";
        exit(-1);
    }

    cout << ANSI_GREEN << "Successfully connected to server." << ANSI_RESET << "\n";

    return socket_fd;
}
