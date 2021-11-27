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
map<int, string> DICTDB;

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
    string response = "";

    pthread_mutex_lock(&dictdb_mutex);
    // check if key already exists
    if (DICTDB.find(key) != DICTDB.end()) {
        response = "Key already exists";
    } else {
        DICTDB.insert({key, value});
        response = "Insertion successful";
    }
    pthread_mutex_unlock(&dictdb_mutex);

    return response;
}

// delete key from dictionary
string q_delete(int key) {
    string response = "";

    pthread_mutex_lock(&dictdb_mutex);
    // check if key exists
    if (DICTDB.find(key) == DICTDB.end()) {
        response = "No such key exists";
    } else {
        DICTDB.erase(key);
        response = "Deletion successful";
    }
    pthread_mutex_unlock(&dictdb_mutex);

    return response;
}

// update value of given key in dictionary
string q_update(int key, string value) {
    string response = "";

    pthread_mutex_lock(&dictdb_mutex);
    // check if key exists
    auto itr = DICTDB.find(key);
    if (itr == DICTDB.end()) {
        response = "Key does not exist";
    } else {
        itr->second = value;
        response = value;
    }
    pthread_mutex_unlock(&dictdb_mutex);

    return response;
}

// fetch concatenation of values at specified keys in the dictionary
string q_concat(int key1, int key2) {
    string response = "";

    pthread_mutex_lock(&dictdb_mutex);
    // check if keys exist
    auto itr1 = DICTDB.find(key1);
    auto itr2 = DICTDB.find(key2);
    if ((itr1 == DICTDB.end()) || (itr2 == DICTDB.end())) {
        response = "Concat failed as at least one of the keys does not exist";
    } else {
        string temp = itr2->second;
        itr2->second += itr1->second;
        itr1->second += temp;
        response = itr2->second;
    }
    pthread_mutex_unlock(&dictdb_mutex);

    return response;
}

// fetch value of given key from dictionary
string q_fetch(int key) {
    string response = "";

    pthread_mutex_lock(&dictdb_mutex);
    // check if key exists
    auto itr = DICTDB.find(key);
    if (itr == DICTDB.end()) {
        response = "Key does not exist";
    } else {
        response = itr->second;
    }
    pthread_mutex_unlock(&dictdb_mutex);

    return response;
}

// print current state of dictionary
void print_dictdb() {
    cout << ANSI_CYAN;
    for (auto& t : DICTDB) cout << t.first << ": " << t.second << "\n";
    cout << ANSI_RESET;
}
