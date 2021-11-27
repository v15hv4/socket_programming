#ifndef UTILS_H_
#define UTILS_H_

#include <string>
#include <vector>

// utility function to read string from socket
std::pair<std::string, int> read_socket(const int& fd, int bytes);

// utility function to send string to socket
int write_socket(int fd, const std::string& s);

// utility function to connect to server and return file descriptor
int connect_to_server(struct sockaddr_in* ptr);

// utility function to trim whitespace from a string
std::string trim(const std::string& line);

// utility function to split string given delimiter
std::vector<std::string> split(std::string str, std::string delim);

#endif
