//
// Created by rielida on 13/11/17.
//

#include "../include/Environment.h"
#include "../include/GlobalVariables.h"

using namespace std;


Environment::Environment() : commandsHistory(), fs() ,currCommand(),work(true){

}

Environment::~Environment() { //destructor
    clear();
    if(verbose==1||verbose==3){
        cout <<"virtual ~Environment()\n";
    }
}

void Environment::copy(Environment &other) {
    if (!other.commandsHistory.empty())
    {
        for (auto iter = other.commandsHistory.begin(); iter != other.commandsHistory.end(); iter++) {
            BaseCommand *currCommand;
            string command = (*iter)->toString();

            if (command == "pwd")
                currCommand = new PwdCommand(*((PwdCommand *) (*iter)));
            else if(command == "cd")
                currCommand = new CdCommand(*((CdCommand *) (*iter)));
            else if(command == "ls")
                currCommand = new LsCommand(*((LsCommand *) (*iter)));
            else if(command == "mkdir")
                currCommand = new MkdirCommand(*((MkdirCommand *) (*iter)));
            else if(command == "mkfile")
                currCommand = new MkfileCommand(*((MkfileCommand *) (*iter)));
            else if(command == "cp")
                currCommand = new CpCommand(*((CpCommand *) (*iter)));
            else if(command == "mv")
                currCommand = new MvCommand(*((MvCommand *) (*iter)));
            else if(command == "rename")
                currCommand = new RenameCommand(*((RenameCommand *) (*iter)));
            else if(command == "rm")
                currCommand = new RmCommand(*((RmCommand *) (*iter)));
            else if(command == "history")
                currCommand = new HistoryCommand(*((HistoryCommand *) (*iter)));
            else if(command == "verbose")
                currCommand = new VerboseCommand(*((VerboseCommand *) (*iter)));
            else if(command == "exec")
                currCommand = new ExecCommand(*((ExecCommand *) (*iter)));
            else if(command == "error")
                currCommand = new ErrorCommand(*((ErrorCommand *) (*iter)));
            commandsHistory.push_back(currCommand);
        }
    }
    currCommand = other.currCommand;
    work = other.work;
}


void Environment::clear() {
    if (!commandsHistory.empty()) {
        for(auto iter=commandsHistory.begin(); iter<commandsHistory.end(); iter++) {
            delete (*iter);
            *iter = nullptr;
        }
        commandsHistory.clear(); //vector::clear();
    }
}

Environment::Environment(Environment &other): commandsHistory(), fs(), currCommand(), work() { //copy constructor
    copy(other);
    fs = other.fs;
    if(verbose==1||verbose==3){
        cout <<"Environment(Environment &other)\n";
    }
}


Environment::Environment(Environment &&other)  : commandsHistory(other.commandsHistory), fs(other.fs), currCommand(other.currCommand), work(other.work) { //Move Constructor
    other.commandsHistory.clear();

    if(verbose==1||verbose==3){
        cout <<"Environment(Environment &&other)\n";
    }
}


Environment &Environment::operator=(Environment &other) { //Copy Assignment
    if (this != &other)
    {
        clear();
        copy(other);
        fs = other.fs;
    }
    if(verbose==1||verbose==3){
        cout <<"operator=(Environment &other)\n";
    }


    return *this;
}


Environment &Environment::operator=(Environment &&other)  { //Move Assignment
    if (this != &other) {
        clear();
        commandsHistory = other.commandsHistory;
        fs = other.fs;
        other.commandsHistory.clear();

    }
    if(verbose==1||verbose==3){
        cout <<"operator=(Environment &&other)\n";
    }

    return *this;
}





void Environment::start() {
    while(work) {
        cout << fs.getWorkingDirectory().getAbsolutePath() + ">";
        string userIn;
        cin >> userIn;
        string com = userIn;
        getline(cin, userIn);
        createCommand(com, userIn);
        if(work) {
            if(verbose==2|| verbose==3){
                cout << com+""+userIn+"\n";
            }
            currCommand->execute(fs);
            addToHistory(currCommand);
            currCommand = nullptr;
        }

    }

}

FileSystem &Environment::getFileSystem(){
    return fs;
}

void Environment::addToHistory(BaseCommand *command) {
    commandsHistory.push_back(command);
}

const vector<BaseCommand *> &Environment::getHistory() const {
    return commandsHistory;
}

void Environment::createCommand(string command, string userIn) {
    string error = command + userIn;
    size_t i = 0;
    while (i<userIn.size() && userIn[i]!=' '){
        i++;
    }

    string commandName = command;
    string commandArgs = userIn;
    if(userIn!="")
        commandArgs = userIn.substr(1,userIn.size());

    if(commandName=="pwd")
        currCommand = new PwdCommand(commandArgs);
    else if(commandName=="cd")
        currCommand = new CdCommand(commandArgs);
    else if(commandName=="ls")
        currCommand = new LsCommand(commandArgs);
    else if(commandName=="mkdir")
        currCommand = new MkdirCommand(commandArgs);
    else if(commandName=="mkfile")
        currCommand= new MkfileCommand(commandArgs);
    else if(commandName=="cp")
        currCommand = new CpCommand(commandArgs);
    else if(commandName=="mv")
        currCommand =new MvCommand(commandArgs);
    else if(commandName=="rename")
        currCommand = new RenameCommand(commandArgs);
    else if(commandName=="rm")
        currCommand = new RmCommand(commandArgs);
    else if(commandName=="history")
        currCommand = new HistoryCommand(commandArgs,commandsHistory);
    else if(commandName=="verbose")
        currCommand = new VerboseCommand(commandArgs);
    else if(commandName=="exec")
        currCommand = new ExecCommand(commandArgs,commandsHistory);
    else if(commandName=="exit")
        this->work=false;
    else
        currCommand = new ErrorCommand(error);

}