#ifndef DICTDB_H_
#define DICTDB_H_
#include <pthread.h>

#include <map>
#include <string>

// global dictionary
extern std::map<int, std::string> DICTDB;

// mutex lock for global dictionary
extern pthread_mutex_t dictdb_mutex;

// execute command on dictionary
std::string execute_query(std::string command);

// insert key-value pair into dictionary
std::string q_insert(int key, std::string value);

// delete key from dictionary
std::string q_delete(int key);

// update value of given key in dictionary
std::string q_update(int key, std::string value);

// fetch concatenation of values at specified keys in the dictionary
std::string q_concat(int key1, int key2);

// fetch value of given key from dictionary
std::string q_fetch(int key);

#endif
