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

// server config {{{
#define MAX_CLIENTS 4
#define PORT 8001
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

// handle incoming connections {{{
void handle_connection(int client_socket_fd) {
    bool connection_open = true;

    while (connection_open) {
        string request;
        int bytes_received = 0;
        tie(request, bytes_received) = read_socket(client_socket_fd, BUFFER_SIZE);

        if (bytes_received <= 0) {
            cerr << "Error reading client message\n";
            connection_open = false;
            break;
        }

        cout << "Message from client: " << request << "\n";

        if (request == "exit") {
            cout << "Exiting\n";
            connection_open = false;
            break;
        }

        string response = "Ack: " + request;

        int bytes_sent = write_socket(client_socket_fd, response);
        if (bytes_sent == -1) {
            cerr << "Error writing to client, socket may be closed.\n";
            connection_open = false;
            break;
        }
    }

    // close client socket fd
    close(client_socket_fd);
    cout << ANSI_RED << "Disconnected from client." << ANSI_RESET << "\n";
}
// }}}

int main(int argc, char* argv[]) {
    int welcoming_socket_fd, client_socket_fd;
    socklen_t clilen;

    struct sockaddr_in server_addr, client_addr;

    // create welcoming socket
    welcoming_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (welcoming_socket_fd < 0) {
        cerr << "Error creating welcoming socket\n";
        exit(-1);
    }

    // set server address config
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // bind socket to address
    if (bind(welcoming_socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "Error binding welcome socket\n";
        exit(-1);
    }

    // listen for incoming requests
    if (listen(welcoming_socket_fd, MAX_CLIENTS)) {
        cerr << "Error listening on welcoming socket\n";
        exit(-1);
    }
    cout << "Server listening on port " << PORT << ".\n";

    clilen = sizeof(client_addr);
    while (true) {
        // accept a new request and create a client socket
        client_socket_fd = accept(welcoming_socket_fd, (struct sockaddr*)&client_addr, &clilen);
        if (client_socket_fd < 0) {
            cerr << "Error accepting request\n";
            exit(-1);
        }
        cout << ANSI_GREEN << "Client " << inet_ntoa(client_addr.sin_addr) << ":"
             << ntohs(client_addr.sin_port) << ":"
             << " connected." << ANSI_RESET << "\n";

        handle_connection(client_socket_fd);
    }

    // close welcoming socket fd
    close(welcoming_socket_fd);

    return 0;
}
