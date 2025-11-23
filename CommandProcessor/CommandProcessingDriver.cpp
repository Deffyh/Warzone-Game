
#include "CommandProcessing.h"
#include "../Engine/GameEngine.h"
#include <iostream>
using namespace std;
void testCommandProcessor(){
    //testing some manual commands
    cout<<"Manual command test:\n";
    LogObserver* log=new LogObserver();
    Command c1(log,"loadmap worldmap.txt");

    c1.saveEffect();
    cout<<c1.getCommand()<<" -> "<<c1.getEffect()<<endl;
    Command c2(log,"addplayer Alice");
    c2.saveEffect();
    cout<<c2.getCommand()<<" -> "<<c2.getEffect()<<endl;
    Command c3(log,"invalid");
    c3.saveEffect();
    cout<<c3.getCommand()<<" -> "<<c3.getEffect()<<endl;

    delete log;


    cout<<"Command Processor test\n";
    LogObserver* log2=new LogObserver();
    CommandProcessor cp(log2);
    string state="start";
    while(true){
        Command* cmd=cp.getCommand(state); //getting the last command entered
        if(!cmd) continue;
        bool valid=cp.validate(cmd,state);
        cout<<*cmd<<", Valid?"<<(valid?" Yes":" No")<<endl;
        //updating the state based on the rules
        string c=cmd->getCommand();
        /* using the find method to search for the substring inside a string and using"==0" to make sure the substring"loadmap" 
        appears at the very beginning of the command str. for example: "loadmap map.txt"*/

        if(state=="start"&& c.rfind("loadmap")==0) state="maploaded";
        else if(state=="maploaded"&&c=="validatemap") state="mapvalidated";
        else if((state=="mapvalidated"||state=="playersadded")&& c.rfind("addplayer",0)==0) state="playersadded";
        else if(state=="playersadded"&& c=="gamestart") state="win";
        else if((state=="win"||state=="assignreinforcement")&& c=="replay") state="start";
        else if(c=="quit"){
            cout<<"Exiting..."<<endl;
            break;
        }
        cout<<"Current state: "<<state<<endl;


    }
    cout<<"Testing File Command Processor Adapter\n";
    LogObserver* log3=new LogObserver();
    FileCommandProcessorAdapter fcp(log3,"../CommandProcessor/Commands.txt");
    string fState="start";
    while(true){
        Command* fCmnd = fcp.getCommand(fState);
        if (!fCmnd) break; // EOF or couldn't open file
        bool valid = fcp.validate(fCmnd, fState);
        cout << *fCmnd << ", Valid?" << (valid ? " Yes" : " No") << endl;
        string c = fCmnd->getCommand();
        if(fState=="start"&& c.rfind("loadmap",0)==0) fState="maploaded";
        else if(fState=="maploaded"&&c=="validatemap") fState="mapvalidated";
        else if(fState=="mapvalidated"&& c.rfind("addplayer",0)==0) fState="playersadded";
        else if(fState=="playersadded"&& c=="gamestart") fState="assignreinforcement";
        else if(fState=="win"&& c=="replay") fState="start";
        else if(c=="quit"){
            cout<<"Exiting..."<<endl;
            break;
        }
        cout<<"Current state: "<<fState<<endl;

    }
   
    delete log3;
}

int main(){
    LogObserver* log= new LogObserver();
    CommandProcessor cmdpc(log);

    GameEngine* gameEngine = new GameEngine(log);
    // CommandProcessor::engine = gameEngine;
    gameEngine->runGame();


    // Command* cmd = cmdpc.readCommand();
    // cmd->saveEffect();
    
    // cout << cmd->getCommand() << endl;
    // cout << cmd->getEffect() << endl << endl;

    // cout << "IS VALID: " << cmdpc.validate(cmd, "start") << endl;

    // cmdpc.getCommand("start");

    // cout << "MAPS" << endl;
    // for (string map : gameEngine->getMapsToUse()){
    //     cout << map << endl;
    // }

    // cout << "\nSTRATEGIES" << endl;
    // for (string strategy : gameEngine->getStrategiesToUse()){
    //     cout << strategy << endl;
    // }

    // cout << "\nNUM GAMES" << endl;
    // cout << gameEngine->getNumGames();

    // cout << "\nNUM TURNS" << endl;
    // cout << gameEngine->getNumTurns() << endl;

    return 0;
}