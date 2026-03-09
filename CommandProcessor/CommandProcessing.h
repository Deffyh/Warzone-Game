#ifndef COMMANDPROCESSING_H
#define COMMANDPROCESSING_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "../Logging-Observer/LoggingObserver.h"

using namespace std;
class GameEngine;

class Command: public Subject, public ILoggable{
private:
    string command; //the command
    string effect; //the result of command
public:
    //constructors
    Command(Observer* observer, const string& cm);
    Command(const Command& other);
    ~Command() override;
    Command& operator=(const Command& other);
    void saveEffect();
    string getCommand()const;
    void setCommand(string cmd);
    string getEffect()const;
    void setEffect(const std::string& eff);
    string stringToLog() override;
    void notify(ILoggable& subject) override;
    friend ostream& operator<<(ostream& os, const Command& cm);
};
//this class is responsible for reading and storing commands
class CommandProcessor: public Subject, public ILoggable{
    protected:
        vector<Command*> commands;
    public:
        static GameEngine* engine;
        explicit CommandProcessor(Observer* observer);
        CommandProcessor(const CommandProcessor& other);
        CommandProcessor& operator=(const CommandProcessor& other);
        ~CommandProcessor() override;
        //methods
        virtual Command* readCommand(); //reading the command from console
        virtual Command* saveCommand(string cm);  //saving the command in the vector
        virtual Command* getCommand(string state); //getting the last command
        virtual bool validate(Command* cm, string state);
        bool validateTournamentParameters(const string& cmd);
        void printTournmanetHelpCmd();
        string returnTournamentParameters(const string& cmd, size_t nextDashPos);
        vector<Command*> getCommandList() const; //getter for commands vector
        //stream insertion operator
        friend ostream& operator<<(ostream& os, const CommandProcessor& cp);
        vector<Command*> getCommands() const ; //getter for the vector, it is needed for loops in the driver
        string stringToLog() override;
        void notify(ILoggable& subject) override;
};
//FileCommandProcessorAdapter extends CommandProcessor to read commands from a file
class FileCommandProcessorAdapter: public CommandProcessor{
private:
    ifstream file; 
    string fileN; 
    
public:
    explicit FileCommandProcessorAdapter(Observer* observer,const string& filename);
    FileCommandProcessorAdapter(const FileCommandProcessorAdapter& other);
    ~FileCommandProcessorAdapter() override;
    FileCommandProcessorAdapter& operator=(const FileCommandProcessorAdapter& other);
    Command* readCommand() override;
    Command* getCommand(string state) override;
    friend ostream& operator<<(ostream& os, const FileCommandProcessorAdapter& fcp);

    string stringToLog() override;
    void notify(ILoggable& subject) override;
};

#endif