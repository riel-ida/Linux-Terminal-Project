//
// Created by shiraev on 23/11/17.
//



#include "../include/Files.h"
#include "../include/GlobalVariables.h"
using namespace std;

BaseFile::BaseFile(string name) : name(name) {

}

string BaseFile::getName() const {

    return name;
}

void BaseFile::setName(string newName) {
    name = newName;//

}

BaseFile::~BaseFile() {}


//
// Created by shiraev on 12/11/17.
//
#include "../include/Files.h"
using namespace std;

File::File(string name, int size):BaseFile(name),size(size) {

}

File::~File() {

}

int File::getSize() {
    return size;
}

bool File::getType(){
    return false;
};

//
// Created by shiraev on 12/11/17.
//

#include "../include/Files.h"
#include <algorithm>
#include <iostream>
#include "../include/GlobalVariables.h"

using namespace std;


Directory::Directory(string name, Directory *parent) : BaseFile(name) , children() ,parent(parent){

}


Directory::Directory(Directory &other) : BaseFile(other.getName()),children(), parent(){
    CopyChildren(other.getChildren());
    if(verbose==1||verbose==3){
        cout <<"Directory(Directory &other) \n";
    }
}

Directory& Directory::operator=(Directory &other) {
    if (this==&other)
        return *this;
    clear();
    setName(other.getName());

    if(other.parent!= nullptr)
        setParent(other.getParent());
    CopyChildren(other.getChildren());

    if(verbose==1||verbose==3){
        cout <<"Directory& Directory::operator=(Directory &other)\n";
    }
    return *this;

}

void Directory::CopyChildren(vector<BaseFile *> otherChildren){
    children = vector<BaseFile*>();
    if(otherChildren.size()!=0) {
        for (size_t i = 0; i < otherChildren.size(); i++) {
            BaseFile *newChild;
            if (dynamic_cast<Directory *>(otherChildren[i]) != nullptr) {
                newChild = new Directory(*static_cast<Directory *>(otherChildren[i]));
                ((Directory *) newChild)->setParent(this);////
            } else {
                newChild = new File(otherChildren[i]->getName(), otherChildren[i]->getSize());
            }
            children.push_back(newChild);

        }
    }

}


Directory::~Directory(){
    clear();
    if(verbose==1||verbose==3){
        cout <<"Directory &Directory::~Directory()\n";
    }
}

void Directory::clear() {

    for (auto iter=children.begin();iter<children.end();iter++) {
        delete (*iter);
        *iter = nullptr;
    }
    if(parent!= nullptr){

        parent= nullptr;
    }
    children.clear();
}

Directory::Directory(Directory &&other) : BaseFile(other.getName()),children(other.children), parent(other.parent) {
    other.children.clear();
    other.parent = nullptr;
    if(other.parent!= nullptr)
        parent->addFile(this);
    other.setParent(nullptr);
    if(verbose==1||verbose==3){
        cout <<"Directory(Directory &&other)\n";
    }
}


Directory& Directory::operator=(Directory &&other) {
    if (this==&other)
        return *this;
    clear();
    setName(other.getName());
    setParent(other.getParent());
    children = other.getChildren();
    other.children.clear();
    other.parent = nullptr;

    if(parent!= nullptr)
        parent->addFile(this);

    if(verbose==1||verbose==3){
        cout <<"Directory& Directory::operator=(Directory &&other)\n";
    }

    return *this;


}



Directory *Directory::getParent() const {
    return parent;
}


void Directory::setParent(Directory *newParent) {

    parent = newParent;//

}

void Directory::addFile(BaseFile *file) {
    children.push_back(file);

}

void Directory::removeFile(string name) {

    for(auto iter=children.begin(); iter!=children.end(); iter++) {
        if((*iter)->getName() == name)
        {
            children.erase(iter);
            break;
        }
    }

}

void Directory::removeFile(BaseFile *file) {

    for(auto iter=children.begin(); iter!=children.end(); iter++) {
        if(*iter == file)
        {
            children.erase(iter);
            delete file;
            break;
        }
    }

}

void Directory::sortByName() {
    sort(children.begin(),children.end(),
         [](BaseFile* a, BaseFile* b)
         {
             return a->getName() < b->getName();
         });

}

void Directory::sortBySize() {
    sort(children.begin(),children.end(),
         [](BaseFile* a, BaseFile* b)
         {
             return a->getSize() < b->getSize();
         });

}

vector<BaseFile *> Directory::getChildren() {

    return children;
}

int Directory::getSize() {
    int sum=0;
    for (size_t i = 0; i<children.size(); i++)
    {
        sum = sum + children[i]->getSize(); // todo : check how it should work
    }

    return sum;
}

string Directory::getAbsolutePath() {



    if (this->parent== nullptr)
        return "/";
    if(this->parent->getName()=="/") return "/"+getName();

    return getParent()->getAbsolutePath() +"/"+getName();
}

bool Directory::getType() {
    return true;
}

Directory* Directory::findChild(string s) {
    for (size_t i = 0; i <getChildren().size() ; ++i) {
        if(children[i]->getName().compare(s)==0)
            return (Directory*)children[i];
    }
    Directory* notFound(nullptr);
    return notFound;
}

bool Directory::findChildb(string s) {
    for (size_t i = 0; i <getChildren().size() ; ++i) {
        if(children[i]->getName().compare(s)==0)
            return true;
    }
    return false;
}

Directory* Directory::getDirectoryFromPath(string path) {
    Directory* directoryFromPath = this;
    long sum = count(path.begin(), path.end(), '/');

    if (sum > 1)
    {
        size_t i = path.find_first_of('/', 1); // starts with '/'
        string name = path.substr(1,i-1);
        for(auto iter = children.begin(); iter != children.end(); iter++) {
            if ((*iter)->getName() == name)
            {
                if ((*iter)->getType())
                {
                    directoryFromPath = (Directory*)(*iter);
                    path = path.substr(i);
                    directoryFromPath->getDirectoryFromPath(path);
                }
                else{ //File
                    Directory* notFound(nullptr);
                    return notFound;;
                }

            }
        }
    }
    return directoryFromPath;
}
vector<string> Directory::splitPath(string path)
{
    string split{""};
    vector<string> vec;

    for(auto n:path)
    {
        if(n != '/') split+=n; else
        if(n == '/' && split != "") { vec.push_back(split); split = ""; }
    }
    if(split != "") vec.push_back(split);

    return vec;
}


Directory* Directory::getWdFromPath(vector<string> vec, long i, int j) {
    Directory* wdFromPath = this;
    while(i != 0) {
        for (auto iter = children.begin(); iter<children.end(); ++iter) {
            if (vec[j] == (*iter)->getName()) {
                wdFromPath = (Directory *) (*iter);
                return wdFromPath->getWdFromPath(vec, --i, ++j);
            }
        }
    }

    return wdFromPath;
}