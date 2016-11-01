#ifndef COCO_UTILS_HPP_
#define COCO_UTILS_HPP_

#include <vector>
#include <memory>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <dirent.h>
#include <iterator>
#include <algorithm>
#include <iomanip>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>

using namespace std;


void split(const string&, char, vector<string>&);
vector<string> split(const string&, char);

void dirlist(const string&, vector<string>&);
vector<string> dirlist(const string&);

bool compareStrings(string, string);
bool fileExists(const string&);
string baseName(const string&);

string getCWD();
void createDirectory(string);
void createDirectory(string,string);


template<typename T>
string joinStrings(const vector<T> &vec, char delim) {
    ostringstream oss;

    if (vec.size() > 1) {
        copy(vec.begin(), vec.end() - 1, ostream_iterator<T>(oss, &delim));
        oss << vec.back();

    } else {
        oss << vec[0];
    }

    return oss.str();
}


template <typename T>
void removeDuplicates(vector<T> &vec) {
    std::sort(vec.begin(), vec.end());
    vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
}

#endif
