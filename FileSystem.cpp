//
// Created by shiraev on 13/11/17.
//

#include "../include/FileSystem.h"
#include "../include/GlobalVariables.h"
#include <iostream>

using namespace std;

FileSystem::FileSystem():rootDirectory(new Directory("/", nullptr)) ,workingDirectory(){
    workingDirectory=rootDirectory;
}





void FileSystem::copy(Directory *other_rootDirectory, Directory *other_workingDirectory){
    rootDirectory = new Directory(*other_rootDirectory);
    string path = other_workingDirectory->getAbsolutePath();
    vector<string> vec = other_workingDirectory->splitPath(path);
    workingDirectory = rootDirectory->getWdFromPath(vec, vec.size(), 0);
}

void FileSystem::clear() {
    if (rootDirectory) {
        delete rootDirectory;
        rootDirectory = nullptr;
    }
}





FileSystem::~FileSystem() { //destructor
    clear();
    if(verbose==1||verbose==3){
        cout <<"virtual ~FileSystem()\n";
    }
}


FileSystem::FileSystem(const FileSystem &other): rootDirectory(), workingDirectory(){ //copy constructor
    copy(other.rootDirectory, other.workingDirectory);
    if(verbose==1||verbose==3){
        cout <<"FileSystem(const FileSystem &other)\n";
    }
}


FileSystem::FileSystem(FileSystem &&other)  : rootDirectory(other.rootDirectory), workingDirectory(other.workingDirectory){ //Move Constructor
    other.rootDirectory = nullptr;
    other.workingDirectory = nullptr;
    if(verbose==1||verbose==3){
        cout <<"FileSystem(const FileSystem &&other)\n";
    }
}


FileSystem &FileSystem::operator=(const FileSystem &other) { //Copy Assignment
    if (this != &other) {
        clear();
        copy(other.rootDirectory, other.workingDirectory);
    }
    if(verbose==1||verbose==3){
        cout <<"operator=(const FileSystem &other)\n";
    }

    return *this;
}


FileSystem &FileSystem::operator=(FileSystem &&other)  { //Move Assignment
    if (this != &other) {
        clear();
        rootDirectory = other.rootDirectory;
        workingDirectory = other.workingDirectory;
        other.rootDirectory = nullptr;
        other.workingDirectory = nullptr;
    }
    if(verbose==1||verbose==3){
        cout <<"operator=(const FileSystem &&other)\n";
    }


    return *this;
}


Directory &FileSystem::getRootDirectory() const {
    return *rootDirectory;
}


Directory &FileSystem::getWorkingDirectory() const {
    return *workingDirectory;
}


void FileSystem::setWorkingDirectory(Directory* newWorkingDirectory) {
    workingDirectory=newWorkingDirectory;

}