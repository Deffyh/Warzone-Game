#include "CommandProcessing.h"
#include "../Engine/GameEngine.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
using namespace std;

GameEngine* CommandProcessor::engine = nullptr;   // ← REQUIRED

/*command class implemetation, the command class represents a command entered by the player
each command has a name and an effect */
Command::Command(Observer *observer,const string& cm){
   command =cm;
   Subject::addObserver(observer);
    effect="";
}

//copy constructor shallow copy
Command::Command(const Command& other) : Subject(other) {
    command=other.command;
    effect=other.effect;
}
//releasing dynamically allocated memory
Command::~Command()=default;

void Command::notify(ILoggable& subject)
{
        Subject::observer_->update(subject);
}

string Command::stringToLog()
{
    string str="A command's effect has been saved!\nCommand : \n"+command+"\nEffect : \n"+effect+"\n\n";
    return str;
}

//assignment operator
Command& Command::operator=(const Command& other){
    if(this!=&other){ //avoids self-assignment
        command=other.command;
        effect=other.effect;
        Subject::addObserver(observer_);
    }
    return *this;
}
//storing the given string into effect attribute
void Command:: saveEffect(){
    size_t firstSpacePos = command.find(' ');
    std::string firstWord = command.substr(0, firstSpacePos);

    if (command == "exit"||command=="quit")//quick exit
    {
        effect = "exiting the game";
    }
    else if (command.find("loadmap") != string::npos)
    {//if the command is longer than 8 chars it means the name of the map is provided
        if (command.length() > 8)
        {
            string mName = command.substr(8); //skip "loadmap "
            effect = "map " + mName + " loaded successfully";
        }
        else
        {
            effect = "missing map name";
        }
    }
    else if (command == "validatemap")
    {
        effect = "map validated ";
    }
    else if (command.find("addplayer") != string::npos)
    {
        if (command.length() > 10)
        {
            string playerN = command.substr(10); //skip "addplayer "
            effect = "player " + playerN + " added ";
        }
        else
        {
            effect = "missing player name";
        }
    }
    else if (command == "gamestart")
    {
        effect = "game started";
    }
    else if (command == "replay")
    {
        effect = "replaying game";
    }
    else if (firstWord == "tournament"){
        effect = "Attempting tournament";
    }
    else
    {
        effect = "invalid command";
    }

    notify(*this);
}
//returns the command string
string Command::getCommand()const{
    return command;
}
void Command::setCommand(string cmd){
    this->command = cmd;
}

//returns the effect string
string Command::getEffect() const{
    return effect;
}
void Command::setEffect(const string& eff) {
    effect = eff;
}
//printing the command in a readable format
ostream& operator<<(ostream& os, const Command& cm){
os<<"Command: "<<cm.command<<", Effect: "<<cm.effect;
return os;
}
//command processor class implementation

CommandProcessor::CommandProcessor(Observer* observer){
    Subject::addObserver(observer);
}

//copy constructor, deep copying other command processor objs
CommandProcessor::CommandProcessor(const CommandProcessor& other) : Subject(other) {
    for(auto cm:other.commands){
        commands.push_back(new Command(*cm));
    }
}
//destructor, releasing dynamically allocated memory
CommandProcessor::~CommandProcessor(){
    for(auto cm:commands){
        delete cm; //deleting each command object in the vector
    }
    commands.clear(); //clear the vector
}

void CommandProcessor::notify(ILoggable& subject)
{
    Subject::observer_->update(*this);
}

string CommandProcessor::stringToLog()
{
    string str = "A new command has been saved!:\n"+commands.back()->getCommand()+"\n\n";
    return str;
}

//assignment operator
CommandProcessor& CommandProcessor::operator=(const CommandProcessor& other){
    if (this!=&other){ //avoiding self assignment
        for(auto cm:commands){ //deleting existing command objects
            delete cm;
        }
        commands.clear();
        //copy command objects from other
        for(auto cm:other.commands){
            commands.push_back(new Command(*cm));
        }
    }
    return *this;
}

//reads command from console
Command* CommandProcessor::readCommand(){
    std::string input;

    auto trim = [](string s) -> string {
        const char* ws = " \t\r\n";
        auto l = s.find_first_not_of(ws);
        if (l == string::npos) return "";
        auto r = s.find_last_not_of(ws);
        return s.substr(l, r - l + 1);
    };

        cout << "Enter command: ";
    while (true) {
        // read whole line; if stream closed/EOF, return nullptr so caller can handle it
        if (!getline(cin, input)) return nullptr;

        // remove surrounding whitespace
        input = trim(input);

        if (input.empty()) {
            // ignore empty/whitespace-only lines and prompt again
            continue;
        }

        // got a non-empty trimmed line -> save and return
        Command* com = saveCommand(input);
        return com;
    }
}

//returns the most recently added command
Command* CommandProcessor::getCommand(string state){
    Command* com =readCommand();
    if (validate(com, state))
    {
        return com;
    }

    cout<<"The command is invalid! Re-enter command!"<<std::endl;
    return nullptr;
}

//saves a new command object into the commands vector
Command* CommandProcessor ::saveCommand(string cm){
    Command *command = new Command(Subject::observer_, cm);
    commands.push_back(command); //creates and add new command object to the vector
    notify(*this);
    return command;
}

/* check if the given command is valid in the current game state
call saveEffect() if valid to store the message and if invalid store an error message
 returns true if valid, false otherwise */
bool CommandProcessor:: validate(Command* cm, string state){
    if (!cm) return false; // defensive: avoid crash if caller forgets to check

    bool valid=false;
    string command = cm->getCommand();
    size_t firstSpacePos = command.find(' ');
    std::string firstWord = command.substr(0, firstSpacePos);

    if (command == "exit") { //quick exit
        valid=true;
    }
    else if (command.find("loadmap") != string::npos && (state == "start" || state == "maploaded")) {
        valid=true;
    }
    else if (command == "validatemap" && state == "maploaded") {
        valid=true;
    }
    else if (command.find("addplayer") != string::npos && (state == "mapvalidated" || state == "playersadded")) {
        if (command.length()>10){
            valid=true;
        }else
            valid=false;
    }
    else if (command == "gamestart" && state == "playersadded") {
        valid=true;
    }
    else if ((command == "replay" || command == "quit") && state == "win") {
            valid=true;
    }
    else if (firstWord == "tournament" && state == "start"){
        size_t firstDashPos = command.find('-');
        valid = validateTournamentParameters(command);

        if (valid) cm->setCommand("tournament");
    }
    else {
        cout << "Invalid command for current state: " << state << endl;
    }

    return valid;


}

bool CommandProcessor::validateTournamentParameters(const string& cmd){
    int numParameters = 4;
    bool hasMaps = false;
    bool hasStrategies = false;
    bool hasNumGames = false;
    bool hasNumTurns = false;
    size_t nextDashPos = cmd.find('-');
    size_t nextLessThanPos = std::string::npos;
    size_t nextGreaterThanPos = std::string::npos;
    size_t nextSpacePos = std::string::npos;
    size_t secondNextSpacePos = std::string::npos;
    string parameters;
    string pathToMapFiles = "part1-map/mapFiles";
    std::vector<std::string> AvailableMapFiles;
    std::vector<std::string> AvailableStrategies = {"Aggressive", "Benevolent", "Neutral", "Cheater"};
    std::vector<std::string> MapsToUse;
    std::vector<std::string> StrategiesToUse;
    int numGames;
    int numTurns;

    for (const auto& entry : std::filesystem::directory_iterator(pathToMapFiles)) {
        if (entry.is_regular_file() && entry.path().extension() == ".map") {
            AvailableMapFiles.push_back(entry.path().filename().string());
        }
    }

    // Go over our command to endure the necessary number of parametes and make sure they are valid
    for (int paramNumber = 0; paramNumber < numParameters + 1; paramNumber++){
        // See if we can find another parameter having finding the necessary number of parameters
        if (paramNumber == numParameters && nextDashPos != std::string::npos){
            cout << "Too many parameters have been passes" << endl;
            printTournmanetHelpCmd();
            return false;
        }
        else if (paramNumber == numParameters)
            break;

        if (nextDashPos == std::string::npos){
            cout << "Missing tournament parameters" << endl;
            printTournmanetHelpCmd();
            return false;
        }
        
        char nextChar = '\0';  // default value
        if (nextDashPos != std::string::npos && nextDashPos + 1 < cmd.size()) {
            nextChar = cmd[nextDashPos + 1];
        }
        else{
            cout << "No argument after dash" << endl;
            printTournmanetHelpCmd();
            return false;
        }

        if (nextChar == 'M'){
            int givenNumMaps = 0;
            int maxNumMaps = 5;
            string mapName;
            bool isValidMap = false;

            parameters = returnTournamentParameters(cmd, nextDashPos);
            if (parameters.size() == 0) return false;
            
            for (int mapArgument = 0; mapArgument < maxNumMaps; mapArgument++){
                isValidMap = false;

                if (mapArgument == 0){
                    nextSpacePos = parameters.find(' ');
                    if (nextSpacePos != std::string::npos)
                        mapName = parameters.substr(0, nextSpacePos);
                    else{
                        //There is only 1 map file provided
                        mapName = parameters;
                        mapArgument = maxNumMaps;
                    }
                }
                else{
                    //nextSpacePos = parameters.find(' ', nextSpacePos + 1);
                    secondNextSpacePos = parameters.find(' ', nextSpacePos + 1);

                    if (secondNextSpacePos != std::string::npos)
                        mapName = parameters.substr(nextSpacePos + 1, secondNextSpacePos - nextSpacePos - 1);
                    else{
                        mapName = parameters.substr(nextSpacePos + 1);
                        mapArgument = maxNumMaps;
                    }
                    nextSpacePos = parameters.find(' ', nextSpacePos + 1);
                }

                for (string availableMapName : AvailableMapFiles){
                    if (mapName == availableMapName){
                        MapsToUse.push_back(mapName);
                        isValidMap = true;
                        givenNumMaps++;
                        break;
                    }
                }

                if (!isValidMap){
                    cout << "Invalid map provided" << endl;
                    printTournmanetHelpCmd();
                    return false;
                }
            }
            if (givenNumMaps < 1 || givenNumMaps > maxNumMaps || nextSpacePos != std::string::npos){
                cout << "Invalid number of maps provided" << endl;
                printTournmanetHelpCmd();
                return false;
            }
            nextDashPos = cmd.find('-', nextDashPos + 1);
            hasMaps = true;
        }
        else if (nextChar == 'P'){
            int givenNumStrategies = 0;
            int minNumStrategies = 2;
            int maxNumStrategies = 4;
            bool isValidStrategy;
            string strategyName;

            parameters = returnTournamentParameters(cmd, nextDashPos);
            if (parameters.size() == 0) return false;

            for (int strategyArgument = 0; strategyArgument < maxNumStrategies; strategyArgument++){
                isValidStrategy = false;

                if (strategyArgument == 0){
                    nextSpacePos = parameters.find(' ');
                    if (nextSpacePos != std::string::npos)
                        strategyName = parameters.substr(0, nextSpacePos);
                    else{
                        //There is only 1 map file provided
                        strategyName = parameters;
                        strategyArgument = maxNumStrategies;
                    }
                }
                else{
                    // nextSpacePos = parameters.find(' ', nextSpacePos + 1);
                    secondNextSpacePos = parameters.find(' ', nextSpacePos + 1);

                    if (secondNextSpacePos != std::string::npos)
                        strategyName = parameters.substr(nextSpacePos + 1, secondNextSpacePos - nextSpacePos - 1);
                    else{
                        strategyName = parameters.substr(nextSpacePos + 1);
                        strategyArgument = maxNumStrategies;
                    }
                    nextSpacePos = parameters.find(' ', nextSpacePos + 1);
                }

                for (string availableStrategy : AvailableStrategies){
                    if (strategyName == availableStrategy){
                        StrategiesToUse.push_back(strategyName);
                        isValidStrategy = true;
                        givenNumStrategies++;
                        break;
                    }
                }

                if (!isValidStrategy){
                    cout << "Invalid strategy provided" << endl;
                    printTournmanetHelpCmd();
                    return false;
                }

            }
            if (givenNumStrategies < minNumStrategies || givenNumStrategies > maxNumStrategies || nextSpacePos != std::string::npos){
                cout << "Invalid number of strategies provided" << endl;
                printTournmanetHelpCmd();
                return false;
            }
            nextDashPos = cmd.find('-', nextDashPos + 1);
            hasStrategies = true;
        }
        else if (nextChar == 'G'){
            int minNumGames = 1;
            int maxNumGames = 5;
            parameters = returnTournamentParameters(cmd, nextDashPos);
            if (parameters.size() == 0 || parameters.find(' ') != std::string::npos){
                cout << "Invalid number of games" << endl;
                printTournmanetHelpCmd();
                return false;
            }

            try{
                numGames = std::stoi(parameters);
            } catch (const std::invalid_argument& e) {
                cout << "Invalid argument: " << e.what() << std::endl;
                printTournmanetHelpCmd();
                return false;
            } catch (const std::out_of_range& e) {
                cout << "Out of range: " << e.what() << std::endl;
                printTournmanetHelpCmd();
                return false;
            }

            if (numGames < minNumGames || numGames > maxNumGames){
                cout << "Invalid number of games entered" << endl;
                printTournmanetHelpCmd();
                return false;
            }
            nextDashPos = cmd.find('-', nextDashPos + 1);
            hasNumGames = true;
        }
        else if (nextChar == 'D'){
            int minNumTurns = 10;
            int maxNumTurns = 50;
            parameters = returnTournamentParameters(cmd, nextDashPos);
            if (parameters.size() == 0 || parameters.find(' ') != std::string::npos){
                cout << "Invalid number of turns" << endl;
                printTournmanetHelpCmd();
                return false;
            }

            try{
                numTurns = std::stoi(parameters);
            } catch (const std::invalid_argument& e) {
                cout << "Invalid argument: " << e.what() << std::endl;
                printTournmanetHelpCmd();
                return false;
            } catch (const std::out_of_range& e) {
                cout << "Out of range: " << e.what() << std::endl;
                printTournmanetHelpCmd();
                return false;
            }

            if (numTurns < minNumTurns || numTurns > maxNumTurns){
                cout << "Invalid number of turns entered" << endl;
                printTournmanetHelpCmd();
                return false;
            }
            nextDashPos = cmd.find('-', nextDashPos + 1);
            hasNumTurns = true;
        }
        else{
            cout << "Invalid syntax" << endl;
            printTournmanetHelpCmd();
            return false;
        }
    }
    //return (hasMaps && hasStrategies && hasNumGames && hasNumTurns);
    if (hasMaps && hasStrategies && hasNumGames && hasNumTurns){
        CommandProcessor::engine->setMapsToUse(MapsToUse);
        CommandProcessor::engine->setStrategiesToUse(StrategiesToUse);
        CommandProcessor::engine->setNumGames(numGames);
        CommandProcessor::engine->setNumTurns(numTurns);
        cout << "VALID TOURNAMENT" << endl;
        return true;
    }
    return false;
}

string CommandProcessor::returnTournamentParameters(const string& cmd, size_t nextDashPos){
    size_t nextLessThanPos = cmd.find('<', nextDashPos + 1);
    size_t nextGreaterThanPos = cmd.find('>', nextDashPos + 1);

    if (nextLessThanPos == std::string::npos || nextGreaterThanPos == std::string::npos || nextLessThanPos + 1 == cmd.size() || nextLessThanPos + 1 == nextGreaterThanPos){
        cout << "Missing parameter arguments or invlaid syntax" << endl;
        // printTournmanetHelpCmd();
        return "";
    }

    return cmd.substr(nextLessThanPos + 1, nextGreaterThanPos - nextLessThanPos - 1);
}

void CommandProcessor::printTournmanetHelpCmd(){
    cout << "To start a tournament please follow the syntax bellow\n\n";
    cout << "tournament -M <listofmapfiles> -P <listofplayerstrategies> -G <numberofgames> -D <maxnumberofturns>" << endl;
    cout << "1. listofmapfiles: are a series of available map files (1-5) separated with a space (' ') and with their name followed by .map (e.g. Antartica.map Andorra.map)" << endl;
    cout << "2. listofplayerstrategies: are a series (between 2-4) available strategies (Aggressive, Benevolent, Neutral, Cheater) separated with a space (' ')" << endl;
    cout << "3. numberofgames: is a number indicating how many games are to be played on each map (between 1-5)" << endl;
    cout << "4. maxnumberofturns: is a number indication the maximum number of turns per game (between 10-50)" << endl;
}

vector<Command*> CommandProcessor::getCommandList() const
{
    return commands;
}

//file command processor adapter class implementation
//constructor that opens the given file to reads commands from it
FileCommandProcessorAdapter::FileCommandProcessorAdapter(Observer* observer, const string& filename) :
    CommandProcessor(observer)
{
    file.open(filename);
    if (!file.is_open())
    {
        cerr << "Cannot open the file " << filename << endl; //error message if failed to open
    }
}

//copy constructor
FileCommandProcessorAdapter::FileCommandProcessorAdapter(const FileCommandProcessorAdapter& other):CommandProcessor(other){

}
//using the base class assignment operator
FileCommandProcessorAdapter& FileCommandProcessorAdapter::operator=(const FileCommandProcessorAdapter& other){
   CommandProcessor::operator=(other);
    return *this;

}
//closes file when object is destroyed
FileCommandProcessorAdapter::~FileCommandProcessorAdapter(){
    if(file.is_open()){
        file.close();
    }

    cout<<"DELETING FILECOMMANDPROCESSOR"<<endl;
}



//reads command from the file
Command* FileCommandProcessorAdapter::readCommand(){
    string line;
    //if the file is open and has a next line, read from it
    if(file.is_open() && getline(file, line)){
        Command* comm = saveCommand(line); //store the read command
        return comm;
    }
    return nullptr;
}

Command* FileCommandProcessorAdapter::getCommand(string state)
{
    Command* com = readCommand();
    if (!com) {
        // end of file OR file couldn't be read -> signal caller
        return nullptr;
    }

    if (validate(com, state)) {
        return com;
    }

    cout << "The command is invalid! Re-enter command!" << std::endl;
    return nullptr;
}


void FileCommandProcessorAdapter ::notify(ILoggable& subject)
{
    Subject::observer_->update(*this);
}

string FileCommandProcessorAdapter ::stringToLog()
{
    string str = "A new command has been saved!:\n"+commands.back()->getCommand()+"\n\n";
    return str;
}
