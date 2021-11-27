#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdarg>
#include <cstring>
#include <iostream>
#include <tuple>

#include "colors.h"
#include "utils.h"
using namespace std;

// client config {{{
#define SERVER_PORT 8001
#define BUFFER_SIZE 1048576
// }}}

// request class {{{
class Request {
   public:
    int idx;
    int delay;
    string command;
    string response;

    Request(int p_idx, int p_delay, string p_command) {
        idx = p_idx;
        delay = p_delay;
        command = p_command;
        response = "";
    }
};
// }}}

// client routine {{{
void* client_routine(void* args) {
    Request request = *(Request*)args;

    // sleep away the delay time
    sleep(request.delay);

    struct sockaddr_in server_addr;

    // set server address config
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    int socket_fd = connect_to_server(&server_addr);

    int bytes_sent = write_socket(socket_fd, request.command);
    if (bytes_sent == -1) {
        cerr << "Error writing to client, socket may be closed.\n";
    }

    // read server response
    int bytes_received = 0;
    tie(request.response, bytes_received) = read_socket(socket_fd, BUFFER_SIZE);
    if (bytes_received <= 0) {
        cerr << "Error reading server message\n";
    }

    // print transaction info
    cout << request.idx << ":" << request.response << "\n";

    // close connection
    close(socket_fd);

    return NULL;
}
// }}}

int main(int argc, char* argv[]) {
    int request_count;
    cin >> request_count;

    // initialize request list
    cin.ignore();
    vector<Request> request_list;
    for (int i = 0; i < request_count; i++) {
        int delay;
        cin >> delay;
        string command;
        getline(cin, command);

        Request request = Request(i, delay, trim(command));
        request_list.push_back(request);
    }

    // initialize threads
    pthread_t user_threads[request_count];
    for (int i = 0; i < request_count; i++) {
        pthread_t* thread = &user_threads[i];
        Request* request = &request_list[i];
        if (pthread_create(thread, NULL, &client_routine, request)) {
            perror("Error creating client thread");
        }
    }

    // join threads
    for (int i = 0; i < request_count; i++) {
        if (pthread_join(user_threads[i], NULL)) {
            perror("Error joining client thread");
        }
    }

    return 0;
}
