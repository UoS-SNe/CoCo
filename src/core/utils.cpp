#include "utils.hpp"


void split(const string &s, char delim, vector<string> &elems) {
    elems.clear();
    stringstream ss(s);
    string item;
    while(getline(ss, item, delim)) {
        elems.push_back(item);
    }
}


vector<string> split(const string &s, char delim) {
    vector<string> elems;
    split(s, delim, elems);
    return elems;
}


void dirlist(const string &path, vector<string> &files) {
    files.clear();
    string temp;
    string ignore(".");
    DIR *dir;
    struct dirent *file;

    dir = opendir(path.c_str());
    if (dir != NULL) {
        while ((file = readdir(dir)) != NULL) {
            temp = string(file->d_name);

            if (temp.at(0) != ignore.at(0)) {
                files.push_back(temp);
            }
        }
        closedir (dir);
    } else {
        cout << "ERROR: Cannot open directory " << path << endl;
    }
}


vector<string> dirlist(const string &path) {
    vector<string> files;
    dirlist(path, files);
    return files;
}


bool compareStrings(string i, string j) {
    return (i==j);
}


bool fileExists(const string &name) {
  struct stat buffer;
  return (stat(name.c_str(), &buffer) == 0);
}


string baseName(const string &path) {
    string filename = path.substr(path.find_last_of("/\\") + 1);
    return filename.substr(0, filename.find_last_of('.'));
}


string getCWD() {
    char charCurDir[PATH_MAX];
    getcwd(charCurDir, PATH_MAX);

    return string(charCurDir);
}


void createDirectory(string name, string dir) {
    string resultsDir = dir + "/" + name;
    struct stat s;
    if (stat(resultsDir.c_str(), &s) != 0) {
        mkdir(resultsDir.c_str(), 0755);
    }
}

void createDirectory(string name) {
    createDirectory(name, getCWD());
}
