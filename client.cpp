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
#include "utils.h"
using namespace std;

// client config {{{
#define SERVER_PORT 8001
#define BUFFER_SIZE 1048576
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
