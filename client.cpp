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
using namespace std;

// client config {{{
#define SERVER_PORT 8001
#define BUFFER_SIZE 1048576
// }}}

// ANSI color codes {{{
const string ANSI_RED = "\x1B[31m";
const string ANSI_GREEN = "\x1B[32m";
const string ANSI_YELLOW = "\x1B[33m";
const string ANSI_BLUE = "\x1B[34m";
const string ANSI_PURPLE = "\x1B[35m";
const string ANSI_CYAN = "\x1B[36m";
const string ANSI_WHITE = "\x1B[37m";
const string ANSI_RESET = "\x1B[0m";
// }}}

// utility function to read string from socket {{{
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

// utility function to send string to socket {{{
int write_socket(int fd, const string& s) {
    int bytes_sent = write(fd, s.c_str(), s.length());
    if (bytes_sent < 0) {
        cerr << "Failed to send data via socket.\n";
    }

    return bytes_sent;
}
// }}}

// utility function to connect to server and return file descriptor {{{
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
// }}}

int main(int argc, char* argv[]) {
    struct sockaddr_in server_addr;

    // set server address config
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    int socket_fd = connect_to_server(&server_addr);

    bool connection_open = true;
    while (true) {
        string request;
        cout << "> ";
        getline(cin, request);
        int bytes_sent = write_socket(socket_fd, request);
        if (bytes_sent == -1) {
            cerr << "Error writing to client, socket may be closed.\n";
            connection_open = false;
            break;
        }

        string response;
        int bytes_received = 0;
        tie(response, bytes_received) = read_socket(socket_fd, BUFFER_SIZE);

        if (bytes_received <= 0) {
            cerr << "Error reading client message\n";
            connection_open = false;
            break;
        }

        cout << "=> " << response << "\n";
    }

    return 0;
}
