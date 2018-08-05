//
// Created by rielida on 23/11/17.
//

#include "../include/Commands.h"
//
// Created by shiraev on 19/11/17.
//

#include <iostream>
#include <sstream>
#include "../include/Commands.h"
using namespace std;


BaseCommand::BaseCommand(string args) : args(args) {

}

BaseCommand::~BaseCommand() {

}

string BaseCommand::getArgs() {
    return args;
}

int BaseCommand::slashCheck(string s) {
    int memo=0;
    if(s.size()!=0) {
        if (s[0] == '/')
            memo++;
        else memo = memo + slashCheck(s.substr(1,s.size()));
    } else return memo;

    return memo;
}

vector<string> BaseCommand::separateString(char c, string s) {
    vector<string> op;
    if(s==""){
        op.push_back(s);
    } else {
        stringstream ss(s);
        string segment;
        while (std::getline(ss, segment, c)) {
            op.push_back(segment);
        }
        if (op[0] == "")
            op.erase(op.begin());
        return op;
    }
    return op;
}


using namespace std;

CdCommand::CdCommand(string args) : BaseCommand(args) {}

void CdCommand::execute(FileSystem &fs) {
    Directory* notChange = &fs.getWorkingDirectory();
    vector<string> newPath = separateString('/', getArgs());
    if(getArgs().compare("/")==0)
        fs.setWorkingDirectory(&fs.getRootDirectory());
    else if(getArgs().compare("..")!=0) {
        Directory *tmp;
        if (getArgs()[0]=='/')
            tmp = &fs.getRootDirectory();
        else
            tmp = &fs.getWorkingDirectory();
        for (size_t i = 0; i < newPath.size(); ++i) {
            if (D(tmp, newPath[i]) != nullptr)
                tmp = D(tmp, newPath[i]);
            else {
                cout << "The system cannot find the path specified\n";
                fs.setWorkingDirectory(notChange);
                return;
            }
        }
        fs.setWorkingDirectory(tmp);
    }
    else {
        if (fs.getWorkingDirectory().getAbsolutePath()=="/")
            return;
        fs.setWorkingDirectory(fs.getWorkingDirectory().getParent());
    }

}

string CdCommand::toString() {
    return "cd";
}

Directory* CdCommand:: D(Directory* goIn,string s){
    if(s.compare("..")==0)
        return goIn->getParent();
    Directory* newD = nullptr;
    if(goIn->findChild(s)!= nullptr)
        return goIn->findChild(s);
    return newD;
}


#include <iostream>
#include "../include/Commands.h"
using namespace std;

CpCommand::CpCommand(string args) : BaseCommand(args) {

}

void CpCommand::execute(FileSystem &fs) {
    vector<string> paths = separateString(' ',getArgs());
    vector<string> sourceP = separateString('/', paths[0]);
    vector<string> destinationP = separateString('/', paths[1]);
    ///check sourcePath
    BaseFile *source;
    BaseFile *destination;
    if(getArgs()[0]=='/') {
        source = &fs.getRootDirectory();
    } else{
        source = &fs.getWorkingDirectory();
    }
    if(paths[1][0]=='/'){
        destination = &fs.getRootDirectory();
    } else{
        destination = &fs.getWorkingDirectory();
    }

    for (size_t i = 0; i < sourceP.size(); ++i) {
        if (D(source, sourceP[i]) != nullptr)
            source = D(source, sourceP[i]);
        else {
            source = nullptr;
            cout << "No such file or directory\n";
            break;
        }
    }
    if(source!= nullptr) {
        for (size_t j = 0; j < destinationP.size(); ++j) {
            if (D(destination, destinationP[j]) != nullptr)
                destination = D(destination, destinationP[j]);
            else {
                destination = nullptr;
                cout << "No such file or directory\n";
                break;
            }
        }
        if (destination== nullptr) return;
    }
    else
        return;
    if(destination!= nullptr){
        //BaseFile* toAdd;
        if(source->getType()) {

            Directory* toAdd = new Directory(*(Directory *) source);
            (toAdd)->setParent((Directory*)destination);
            if(!((Directory*)destination)->findChildb(source->getName()))
                ((Directory*)destination)->addFile(toAdd);
            else {delete toAdd; return;}

        }
        else {
            File *toAdd = new File(*(File *) source);
            if(!((Directory*)destination)->findChildb(source->getName()))
                ((Directory*)destination)->addFile(toAdd);
            else {delete toAdd; return;}
        }

    }

}

string CpCommand::toString() {
    return "cp";
}

BaseFile* CpCommand:: D(BaseFile* goIn,string s){
    if(s.compare("..")==0)
        return ((Directory*)goIn)->getParent();
    BaseFile* op = nullptr;
    if (goIn->getType()) {
        if (((Directory *) goIn)->findChild(s) != nullptr)
            return ((Directory *) goIn)->findChild(s);
    } else return goIn;
    return op;
}




using namespace std;


ErrorCommand::ErrorCommand(string args) : BaseCommand(args) {

}

void ErrorCommand::execute(FileSystem &fs) {
    vector <string> h = separateString(' ', getArgs());
    cout << h[0]+": Unknown command\n";
}

string ErrorCommand::toString() {
    return "error";
}

//
// Created by shiraev on 19/11/17.
//

#include <iostream>
#include "../include/Commands.h"
using namespace std;

ExecCommand::ExecCommand(string args, const vector<BaseCommand *> &history) : BaseCommand(args),history(history) {

}

void ExecCommand::execute(FileSystem &fs) {
    int num = atoi(getArgs().c_str());
    if ((int)history.size()<=num) {
        cout << "Command not found\n";
        return;
    }
    BaseCommand* toExec = history[num];
    toExec->execute(fs);
}

string ExecCommand::toString() {
    return "exec";
}

//
// Created by shiraev on 19/11/17.
//

#include <iostream>
#include "../include/Commands.h"
using namespace std;

HistoryCommand::HistoryCommand(string args, const vector<BaseCommand *> &history):BaseCommand(args),history(history) {

}

void HistoryCommand::execute(FileSystem &fs) {
    for (size_t i = 0; i <history.size() ; ++i) {
        if(dynamic_cast<ErrorCommand*>(history[i]))
            cout << to_string(i)+"\t"+history[i]->getArgs()+"\n";
        else
            cout << to_string(i)+"\t"+history[i]->toString()+" "+history[i]->getArgs()+"\n";
    }
}

string HistoryCommand::toString() {
    return "history";
}

//
// Created by shiraev on 19/11/17.
//

#include <iostream>
#include "../include/Commands.h"
using namespace std;


LsCommand::LsCommand(string args) : BaseCommand(args) {

}

void LsCommand::execute(FileSystem &fs) {
    vector<string> commandSep = separateString(' ', getArgs());
    if (commandSep[0].compare("-s") == 0) {///-s!!
        if (commandSep.size() == 1) {///[-s] working D
            print(true, &fs.getWorkingDirectory());
        }
        else if (commandSep.size() == 2) {///ls -s in path
            vector<string> path = separateString('/',commandSep[1]);
            Directory* tmp = &fs.getRootDirectory();
            for (size_t i = 0; i < path.size() ; ++i) {
                if(D(tmp,path[i])!= nullptr)
                    tmp = D(tmp,path[i]);
                else {
                    cout << "The system cannot find the path specified\n";
                    return;
                }
                print(true, tmp);
            }
        }
    }

    else if(commandSep[0]==""){/// just ls
        print(false, &fs.getWorkingDirectory());
    }
    else {///ls in path
        vector<string> path = separateString('/',commandSep[0]);
        Directory* tmp;
        if (commandSep[0][0]=='/')
            tmp = &fs.getRootDirectory();
        else
            tmp = &fs.getWorkingDirectory();
        for (size_t i = 0; i < path.size() ; ++i) {
            if(D(tmp,path[i])!= nullptr)
                tmp = D(tmp,path[i]);
            else {
                cout << "The system cannot find the path specified\n";
                return;
            }

        }
        print(false, tmp);
    }
}



string LsCommand::toString() {
    return "ls";
}

void LsCommand::print(bool b, Directory* tmp) {
    if (b) {///with -s
        (*tmp).sortByName();
        (*tmp).sortBySize();
        vector<BaseFile *> tp = (*tmp).getChildren();
        for (size_t i = 0; i < tp.size(); i++) {
            if (tp[i]->getType()) {///DIR
                int size = tp[i]->getSize();
                cout << "DIR\t" + tp[i]->getName() + "\t" + to_string(size)+"\n";
            } else {///FILE
                int size = tp[i]->getSize();
                cout << "FILE\t"+ tp[i]->getName() + "\t" + to_string(size)+"\n";
            }
        }
    } else {
        (*tmp).sortByName();
        vector<BaseFile *> tp = (*tmp).getChildren();
        for (size_t i = 0; i < tp.size(); i++) {
            if (tp[i]->getType()) {
                int size = tp[i]->getSize();
                cout << "DIR\t" + tp[i]->getName() + "\t" + to_string(size)+"\n";
            } else {
                int size = tp[i]->getSize();
                cout << "FILE\t" + tp[i]->getName() + "\t" + to_string(size)+"\n";
            }
        }

    }
}

Directory* LsCommand:: D(Directory* goIn,string s){
    if (s.compare("..")==0)
        return goIn->getParent();
    if(goIn->findChild(s)!= nullptr)
        return goIn->findChild(s);

    return nullptr;
}

//
// Created by shiraev on 19/11/17.
//


#include <iostream>
#include "../include/Commands.h"
using namespace std;


MkdirCommand::MkdirCommand(string args) : BaseCommand(args) , allExist(true){

}

void MkdirCommand::execute(FileSystem &fs) {
    vector <string> path = separateString('/',getArgs());
    for (size_t j = 0; j <getArgs().size() ; ++j) {//if there are spaces in the name
        if(getArgs()[j]==' ')
            return;
    }

    if(getArgs()[0]=='/') {
        Directory* tmp = &fs.getRootDirectory();
        for (size_t i = 0; i < path.size(); ++i) {
            if (tmp== nullptr) {
                cout << "The directory already exists\n";
                return;
            }
            tmp = D(tmp, path[i]);
            if(!tmp->getType()){
                cout << "The directory already exists\n";
                return;
            }
        }
        if(allExist) {
            cout << "The directory already exists\n";
            return;
        }
    } else{
        Directory* tmp = &fs.getWorkingDirectory();
        for (size_t i = 0; i < path.size(); ++i) {
            if (tmp== nullptr) {
                cout << "The directory already exists\n";
                return;
            }
            tmp = D(tmp, path[i]);
            if(!tmp->getType()){
                cout << "The directory already exists\n";
                return;
            }
        }
        if(allExist){
            cout << "The directory already exists\n";
            return;
        }
    }
}



string MkdirCommand::toString() {
    return "mkdir";
}


Directory* MkdirCommand:: D(Directory* goIn,string s){
    if(s.compare("..")==0)
        return goIn->getParent();
    Directory* newD = nullptr;
    if(goIn->findChild(s)!= nullptr)
        return goIn->findChild(s);
    else if(!goIn->findChildb(s)){
        newD = new Directory(s, goIn);
        goIn->addFile(newD);
        allExist = false;
    }
    return newD;
}

//
// Created by shiraev on 19/11/17.
//


#include <iostream>
#include "../include/Commands.h"
using namespace std;

MkfileCommand::MkfileCommand(string args) : BaseCommand(args) {

}

void MkfileCommand::execute(FileSystem &fs) {
    vector<string> com = separateString(' ',getArgs());
    vector<string> path = separateString('/',com[0]);
    string file = path[path.size()-1];
    int size = atoi(com[com.size()-1].c_str());
    path.erase(path.begin()+path.size()-1);
    Directory* tmp;
    if(getArgs()[0]=='/')
        tmp = &fs.getRootDirectory();
    else
        tmp = &fs.getWorkingDirectory();
    for (size_t i = 0; i < path.size() ; ++i) {
        if(D(tmp,path[i])!= nullptr)
            tmp = D(tmp, path[i]);
        else return;
    }
    if(tmp->findChildb(file))
        cout << "File already exists\n";
    else {
        File* toaddFile = new File(file, size);
        tmp->addFile(toaddFile);

    }
}

string MkfileCommand::toString() {
    return "mkfile";
}

Directory* MkfileCommand:: D(Directory* goIn,string s){
    if(s.compare("..")==0)
        return goIn->getParent();
    Directory* nullD = nullptr;
    if(goIn->findChild(s)!= nullptr) {
        return goIn->findChild(s);
    }
    else {
        cout<<"The system cannot find the path specified\n";
    }
    return nullD;
}

//
// Created by shiraev on 19/11/17.
//

#include <iostream>
#include "../include/Commands.h"
using namespace std;

MvCommand::MvCommand(string args) : BaseCommand(args) {

}

void MvCommand::execute(FileSystem &fs) {
    vector<string> paths = separateString(' ',getArgs());
    vector<string> sourceP = separateString('/', paths[0]);
    vector<string> destinationP = separateString('/', paths[1]);

    ///check sourcePath
    BaseFile *source ;
    BaseFile *destination ;
    if(getArgs()[0]=='/') {
        source = &fs.getRootDirectory();
    } else{
        source = &fs.getWorkingDirectory();
    }
    if(paths[1][0]=='/'){
        destination = &fs.getRootDirectory();
    } else{
        destination = &fs.getWorkingDirectory();
    }
    Directory * sourceParent = nullptr;
    for (size_t i = 0; i < sourceP.size(); ++i) {
        if (D(source, sourceP[i]) != nullptr) {
            sourceParent = (Directory *) source;
            source = D(source, sourceP[i]);
        }
        else {
            source = nullptr;
            cout << "No such file or directory\n";
            break;
        }

    }
    if(source!= nullptr) {///source to remove not found
        for (size_t j = 0; j < destinationP.size(); ++j) {
            if (D(destination, destinationP[j]) != nullptr)
                destination = D(destination, destinationP[j]);
            else {
                destination = nullptr;
                cout << "No such file or directory\n";
                break;
            }
        }
        if (destination== nullptr) return;
    }
    else return;
    ///source cant be remove
    if (source->getType()) {
        if (source == &fs.getRootDirectory()
            || source == &fs.getWorkingDirectory()
            || sourceParent == fs.getWorkingDirectory().getParent()) {
            cout << "Can't move directory\n";
            return;
        }

        for (size_t k = 0; k < parentsCheck(&fs.getWorkingDirectory()).size(); ++k) {
            if(parentsCheck(&fs.getWorkingDirectory())[k]==source) {
                cout << "Can't move directory\n";
                return;
            }
        }
    }

    if(destination!= nullptr){
        ///adding to destination
        BaseFile* toAdd;
        if(source->getType())
            toAdd = (Directory*)source;
        else
            toAdd = (File*)source;
        if(!((Directory*)destination)->findChildb(toAdd->getName()))
            ((Directory*)destination)->addFile(toAdd);
        else{
            cout << "Can’t move directory\n";
            return;
        }
        ///delete from source
        sourceParent->removeFile(source->getName());
        if(toAdd->getType())
            ((Directory*)toAdd)->setParent((Directory*)destination);
    }
}

string MvCommand::toString() {
    return "mv";
}

BaseFile* MvCommand:: D(BaseFile* goIn,string s){
    if(s.compare("..")==0)
        return ((Directory*)goIn)->getParent();
    BaseFile* op = nullptr;
    if (goIn->getType()) {
        if (((Directory *) goIn)->findChild(s) != nullptr)
            return ((Directory *) goIn)->findChild(s);
    } else return goIn;///if go in is file
    return op;
}

vector<Directory *> MvCommand::parentsCheck(Directory *dir) {
    Directory * tmp = dir;
    vector<Directory *> op;
    while (tmp->getParent()!= nullptr){
        op.push_back(tmp->getParent());
        tmp = tmp->getParent();
    }
    return op;
}

//
// Created by shiraev on 19/11/17.
//

#include <iostream>
#include "../include/Commands.h"
using namespace std;

PwdCommand::PwdCommand(string args) : BaseCommand(args) {

}

void PwdCommand::execute(FileSystem &fs) {
    cout << fs.getWorkingDirectory().getAbsolutePath()+"\n";
}

string PwdCommand::toString() {
    return "pwd";
}

//
// Created by shiraev on 19/11/17.
//
#include <iostream>
#include "../include/Commands.h"
using namespace std;

RenameCommand::RenameCommand(string args) : BaseCommand(args) {

}

void RenameCommand::execute(FileSystem &fs) {
    vector<string> command = separateString(' ',getArgs());
    vector<string> path = separateString('/',command[0]);
    BaseFile *baseFileToRename;
    BaseFile* fileP;
    if(getArgs()[0]=='/')
        baseFileToRename = &fs.getRootDirectory();
    else
        baseFileToRename = &fs.getWorkingDirectory();
    for (size_t i = 0; i < path.size(); ++i) {
        fileP = baseFileToRename;
        baseFileToRename = D(baseFileToRename, path[i]);
    }
    if (baseFileToRename == nullptr) {
        cout << "No such file or directory\n";
        return;
    } else if (baseFileToRename == &fs.getWorkingDirectory()) {
        cout << "Can't rename the working directory\n";
        return;
    }
    if(((Directory*)fileP)->findChildb(command[1]))
        return;
    else {
        baseFileToRename->setName(command[1]);
    }
}

string RenameCommand::toString() {
    return "rename";
}

BaseFile* RenameCommand:: D(BaseFile* goIn,string s){
    if(s.compare("..")==0)
        return ((Directory*)goIn)->getParent();
    BaseFile* op = nullptr;
    if (goIn->getType()) {
        if (((Directory *) goIn)->findChild(s) != nullptr)
            return ((Directory *) goIn)->findChild(s);
    } else return goIn;///if go in is file
    return op;///if not found
}

//
// Created by shiraev on 19/11/17.
//

#include <iostream>
#include "../include/Commands.h"
using namespace std;


RmCommand::RmCommand(string args) : BaseCommand(args) {

}

void RmCommand::execute(FileSystem &fs) {
    vector <string> path = separateString('/',getArgs());
    BaseFile* fileToRemove;
    Directory* parent = nullptr;
    if(getArgs()[0]=='/')
        fileToRemove = &fs.getRootDirectory();
    else
        fileToRemove = &fs.getWorkingDirectory();
    for (size_t i = 0; i <path.size() ; ++i) {
        parent = (Directory*)fileToRemove;
        if(D(fileToRemove,path[i])!= nullptr)
            fileToRemove = D(fileToRemove,path[i]);
        else{
            cout << "No such file or directory\n";
            return;
        }
    }
    if (fileToRemove==&fs.getRootDirectory()
        || fileToRemove ==&fs.getWorkingDirectory()
        ||fileToRemove == fs.getWorkingDirectory().getParent())
    {
        cout << "Can't remove directory\n";
        return;
    }
    remove(fileToRemove, parent, fs);
}

string RmCommand::toString() {
    return "rm";
}

void RmCommand:: remove(BaseFile* b, Directory* parnet, FileSystem &fs){
    if (b->getType()&& ((Directory*)b)->getChildren().size()>0) {
        for (size_t i = 0; i < ((Directory *) b)->getChildren().size(); ++i) {
            remove(((Directory *) b)->getChildren()[i], (Directory *) b, fs);

        }
        parnet->removeFile(b);
    }
    else {
        parnet->removeFile(b);
    }
}

BaseFile* RmCommand:: D(BaseFile* goIn,string s){
    if(s.compare("..")==0)
        return ((Directory*)goIn)->getParent();
    BaseFile* op = nullptr;
    if (goIn->getType()) {
        if (((Directory *) goIn)->findChild(s) != nullptr)
            return ((Directory *) goIn)->findChild(s);
    } else return goIn;///if go in is file
    return op;///if not found
}

//
// Created by shiraev on 20/11/17.
//

#include <iostream>
#include "../include/Commands.h"
#include "../include/GlobalVariables.h"
using namespace std;

VerboseCommand::VerboseCommand(string args) : BaseCommand(args) {

}

void VerboseCommand::execute(FileSystem &fs) {
    if (getArgs()!="0" && getArgs()!="1" && getArgs()!="2" && getArgs()!="3")
        cout << "Wrong verbose input\n";
    verbose = atoi(getArgs().c_str());
}

string VerboseCommand::toString() {
    return "verbose";
}

