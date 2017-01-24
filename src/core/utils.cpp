#include "utils.hpp"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <fstream>
#include <limits>
#include <memory>


void utils::getArgv(int argc, char **argv, std::vector<std::string> &options) {
    options.clear();

    for (size_t i = 1; i < argc; ++i) {
        options.push_back(argv[i]);
    }
}


void utils::split(const std::string &s, char delim, std::vector<std::string> &elems) {
    elems.clear();
    std::stringstream ss(s);
    std::string item;
    while(getline(ss, item, delim)) {
        elems.push_back(item);
    }
}


std::vector<std::string> utils::split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    utils::split(s, delim, elems);
    return elems;
}


void utils::dirlist(const std::string &path, std::vector<std::string> &files) {
    files.clear();
    std::string temp;
    std::string ignore(".");
    DIR *dir;
    struct dirent *file;

    dir = opendir(path.c_str());
    if (dir != NULL) {
        while ((file = readdir(dir)) != NULL) {
            temp = std::string(file->d_name);

            if (temp.at(0) != ignore.at(0)) {
                files.push_back(temp);
            }
        }
        closedir(dir);
    } else {
        std::cout << "ERROR: Cannot open directory " << path << std::endl;
    }
}


std::vector<std::string> utils::dirlist(const std::string &path) {
    std::vector<std::string> files;
    utils::dirlist(path, files);
    return files;
}


bool utils::compareStrings(std::string i, std::string j) {
    return (i==j);
}


bool utils::fileExists(const std::string &name) {
  struct stat buffer;
  return (stat(name.c_str(), &buffer) == 0);
}


std::string utils::baseName(const std::string &path) {
    std::string filename = path.substr(path.find_last_of("/\\") + 1);
    return filename.substr(0, filename.find_last_of('.'));
}

std::string utils::fileExtention(const std::string& path) {
    return path.substr(path.find_last_of(".") + 1);
}



std::string utils::getCWD() {
    char charCurDir[PATH_MAX];
    getcwd(charCurDir, PATH_MAX);

    return std::string(charCurDir);
}


void utils::createDirectory(std::string name, std::string dir) {
    std::string resultsDir = dir + "/" + name;
    struct stat s;
    if (stat(resultsDir.c_str(), &s) != 0) {
        mkdir(resultsDir.c_str(), 0755);
    }
}

void utils::createDirectory(std::string name) {
    utils::createDirectory(name, getCWD());
}
