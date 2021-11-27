#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdarg>
#include <iostream>
#include <queue>
#include <string>
#include <tuple>
#include <utility>

#include "colors.h"
#include "utils.h"
using namespace std;

// server config {{{
#define MAX_CLIENTS 4
#define PORT 8001
#define BUFFER_SIZE 1048576
#define WORKER_POOL_SIZE 10
#define WORKER_DELAY 2
// }}}

// queue requests in the form <socket_fd, command>
queue<pair<int, string>> request_queue;

// mutex lock for request queue
pthread_mutex_t request_queue_mutex;

// maintain worker thread pool
pthread_t worker_pool[WORKER_POOL_SIZE];

// handle incoming connections {{{
void handle_connection(int client_socket_fd) {
    string request;
    int bytes_received = 0;
    tie(request, bytes_received) = read_socket(client_socket_fd, BUFFER_SIZE);

    // queue request
    request_queue.push(make_pair(client_socket_fd, request));
}
// }}}

// execute command on dictionary
string execute_query(string command) {
    /* ... */
    return "Successfully executed query: '" + command + "'";
}

void* worker_routine(void* args) {
    while (1) {
        // look for pending requests in the queue
        int socket_fd = -1;
        string command = "";
        pthread_mutex_lock(&request_queue_mutex);
        if (!request_queue.empty()) {
            tie(socket_fd, command) = request_queue.front();
            request_queue.pop();
        }
        pthread_mutex_unlock(&request_queue_mutex);

        // if request assigned, respond to it
        if (socket_fd >= 0) {
            // process command
            string response = execute_query(command);

            // add a slight delay before responding
            sleep(WORKER_DELAY);

            // send response to client
            int bytes_sent = write_socket(socket_fd, response);
            if (bytes_sent == -1) {
                cerr << "Error writing to client, socket may be closed.\n";
            }

            // close connection
            close(socket_fd);
            cout << ANSI_RED << "Disconnected from client." << ANSI_RESET << "\n";
        }
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    // initialize worker threads
    for (int i = 0; i < WORKER_POOL_SIZE; i++) {
        if (pthread_create(&worker_pool[i], NULL, &worker_routine, NULL)) {
            perror("Error creating worker thread");
        }
    }

    // initialize request queue mutex
    pthread_mutex_init(&request_queue_mutex, NULL);

    // initialize socket server
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
    cout << "Server with " << WORKER_POOL_SIZE << " workers listening on port " << PORT << ".\n";

    clilen = sizeof(client_addr);
    while (true) {
        // accept a new request and create a client socket
        client_socket_fd = accept(welcoming_socket_fd, (struct sockaddr*)&client_addr, &clilen);
        if (client_socket_fd < 0) {
            cerr << "Error accepting request\n";
            exit(-1);
        }
        cout << ANSI_GREEN << "Client " << inet_ntoa(client_addr.sin_addr) << ":"
             << ntohs(client_addr.sin_port) << " connected." << ANSI_RESET << "\n";

        handle_connection(client_socket_fd);
    }

    // close welcoming socket fd
    close(welcoming_socket_fd);

    return 0;
}
