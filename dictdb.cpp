#include "dictdb.h"

#include <pthread.h>

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "colors.h"
#include "utils.h"
using namespace std;

// global dictionary
map<int, std::string> DICTDB;

// mutex lock for global dictionary
pthread_mutex_t dictdb_mutex;

// execute command on dictionary
string execute_query(string command) {
    string response = "Invalid command!";
    vector<string> args = split(command, " ");
    if (args.size()) {
        if (args[0] == "insert") {
            if (args.size() < 3) {
                response = args[0] + ": insufficient number of arguments";
            } else {
                response = q_insert(stoi(args[1]), args[2]);
            }
        } else if (args[0] == "delete") {
            if (args.size() < 2) {
                response = args[0] + ": insufficient number of arguments";
            } else {
                response = q_delete(stoi(args[1]));
            }
        } else if (args[0] == "update") {
            if (args.size() < 3) {
                response = args[0] + ": insufficient number of arguments";
            } else {
                response = q_update(stoi(args[1]), args[2]);
            }
        } else if (args[0] == "concat") {
            if (args.size() < 3) {
                response = args[0] + ": insufficient number of arguments";
            } else {
                response = q_concat(stoi(args[1]), stoi(args[2]));
            }
        } else if (args[0] == "fetch") {
            if (args.size() < 2) {
                response = args[0] + ": insufficient number of arguments";
            } else {
                response = q_fetch(stoi(args[1]));
            }
        } else {
            response = args[0] + ": command not found";
        }
    }

    return response;
}

// insert key-value pair into dictionary
string q_insert(int key, string value) {
    /* ... */
    return "Insertion successful";
}

// delete key from dictionary
string q_delete(int key) {
    /* ... */
    return "Deletion successful";
}

// update value of given key in dictionary
string q_update(int key, string value) {
    /* ... */
    return "<updated value of key>";
}

// fetch concatenation of values at specified keys in the dictionary
string q_concat(int key1, int key2) {
    /* ... */
    return "Concatenation successful";
}

// fetch value of given key from dictionary
string q_fetch(int key) {
    /* ... */
    return "<fetched value>";
}

// print current state of dictionary
void print_dictdb() {
    cout << ANSI_CYAN;
    for (auto& t : DICTDB) cout << t.first << ": " << t.second << "\n";
    cout << ANSI_RESET;
}
