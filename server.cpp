#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdarg>
#include <cstdlib>
#include <iostream>
#include <queue>
#include <string>
#include <tuple>
#include <utility>

#include "colors.h"
#include "dictdb.h"
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

// mutex lock and condition variable for request queue
pthread_mutex_t request_queue_mutex;
pthread_cond_t request_queue_cond;

// maintain worker thread pool
pthread_t* worker_pool;

// add incoming requests to the queue {{{
void submit_request(int client_socket_fd) {
    string request;
    int bytes_received = 0;
    tie(request, bytes_received) = read_socket(client_socket_fd, BUFFER_SIZE);

    // queue the request
    pthread_mutex_lock(&request_queue_mutex);
    request_queue.push(make_pair(client_socket_fd, request));
    pthread_mutex_unlock(&request_queue_mutex);
    pthread_cond_broadcast(&request_queue_cond);
}
// }}}

// worker routine {{{
void* worker_routine(void* args) {
    while (1) {
        // look for pending requests in the queue
        int socket_fd = -1;
        string command = "";

        pthread_mutex_lock(&request_queue_mutex);
        while (request_queue.empty()) {
            pthread_cond_wait(&request_queue_cond, &request_queue_mutex);
        }
        tie(socket_fd, command) = request_queue.front();
        request_queue.pop();
        pthread_mutex_unlock(&request_queue_mutex);

        // process command
        string response = to_string(pthread_self()) + ":" + execute_query(command);

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

        // TODO: remove
        print_dictdb();
    }
    return NULL;
}
// }}}

int main(int argc, char* argv[]) {
    int worker_pool_size = WORKER_POOL_SIZE;

    // determine number of workers in pool from argument
    if (argc > 1) worker_pool_size = atoi(argv[1]);

    // allocate memory for worker thread pool
    worker_pool = (pthread_t*)calloc(worker_pool_size, sizeof(pthread_t));

    // initialize worker threads
    for (int i = 0; i < worker_pool_size; i++) {
        if (pthread_create(&worker_pool[i], NULL, &worker_routine, NULL)) {
            perror("Error creating worker thread");
        }
    }

    // initialize request queue mutex
    pthread_mutex_init(&request_queue_mutex, NULL);

    // initialize request queue condition
    pthread_cond_init(&request_queue_cond, NULL);

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
    cout << "Server with " << worker_pool_size << " workers listening on port " << PORT << ".\n";

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

        submit_request(client_socket_fd);
    }

    // destroy mutexes
    pthread_mutex_destroy(&request_queue_mutex);

    // destroy condition variables
    pthread_cond_destroy(&request_queue_cond);

    // close welcoming socket fd
    close(welcoming_socket_fd);

    return 0;
}
