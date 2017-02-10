// CoCo - Supernova templates and simulations package
// Copyright (C) 2017  Szymon Prajs
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// Contact author: S.Prajs@soton.ac.uk

#ifndef COCO_CORE_UTILS_HPP_
#define COCO_CORE_UTILS_HPP_

#include <vector>
#include <string>
#include <sstream>


namespace utils {

/* Split options into a vector of arguments */
void getArgv(int, char**, std::vector<std::string>&);

void split(const std::string&, char, std::vector<std::string>&);
std::vector<std::string> split(const std::string&, char);

void dirlist(const std::string&, std::vector<std::string>&);
std::vector<std::string> dirlist(const std::string&);

bool compareStrings(std::string, std::string);
bool fileExists(const std::string&);
std::string baseName(const std::string&);
std::string fileExtention(const std::string&);

std::string getCWD();
void createDirectory(std::string);
void createDirectory(std::string,std::string);


template<typename T>
std::string joinStrings(const std::vector<T> &vec, char delim) {
    std::ostringstream oss;

    if (vec.size() > 1) {
        copy(vec.begin(), vec.end() - 1, std::ostream_iterator<T>(oss, &delim));
        oss << vec.back();

    } else {
        oss << vec[0];
    }

    return oss.str();
}


template <typename T>
void removeDuplicates(std::vector<T> &vec) {
    std::sort(vec.begin(), vec.end());
    vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
}

}  // namespace utils

#endif  // COCO_CORE_UTILS_HPP_
