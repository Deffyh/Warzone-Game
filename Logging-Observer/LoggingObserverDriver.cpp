//
// Created by Malak on 2025-11-10.
//

#include "LoggingObserver.h"
#include "../CommandProcessor/CommandProcessing.h"
#include <iostream>

#include "../Orders/Orders.h"
#include "../Player/Player.h"

using std::cout;

void  testLoggingObserver()
{
    LogObserver* logObserver = new LogObserver();
    Command* command = new Command(logObserver, "loadmap");
    CommandProcessor* command_processor= new CommandProcessor(logObserver);
    std::vector<std::string> adj(5);
    Continent* c1 = new Continent("NA", 5);
    std::string name1 = "Saskatchewan";
    std::string name2 = "Manitoba";
    std::string name3 = "Ontario";
    std::string name4 = "Quebec";

    std::vector<std::string> adjSaskatchewan = {"Manitoba"};
    std::vector<std::string> adjManitoba = {"Saskatchewan", "Ontario"};
    std::vector<std::string> adjOntario = {"Manitoba", "Quebec"};
    std::vector<std::string> adjQuebec = {"Ontario"};

    Territory* Saskatchewan = new Territory(name1, c1, 1, 3, adjSaskatchewan);
    Territory* Manitoba = new Territory(name2, c1, 2, 4, adjManitoba);
    Territory* Ontario = new Territory(name3, c1, 5, 6, adjOntario);
    Territory* Quebec = new Territory(name4, c1, 7 ,8, adjQuebec);

    Player* p1 = new Player("Shawn", logObserver);
    p1->addTerritory(Saskatchewan);


    cout<<"TESTING LOGGING OBSERVER"<<endl;
    cout<<"=============================="<<endl;
    cout<<"COMMAND NOTIFYING THE LOG OBSERVER AS A CLASS INHERITING WHEN SAVEEFFECT"<<endl;
    command->saveEffect();
    cout<<"=============================="<<endl;
    cout<<"ORDERLIST NOTIFYING THE LOG OBSERVER AS A CLASS INHERITING WHEN ADDING ORDER"<<endl;
    p1->getOrderList()->add(new OrdersDeploy(p1, Saskatchewan, 100, logObserver));
    cout<<"=============================="<<endl;
    cout<<"ORDER NOTIFYING THE LOG OBSERVER AS A CLASS INHERITING WHEN EXECUTE"<<endl;
    p1->getOrderList()->getList().front()->execute();
    cout<<"=============================="<<endl;
    cout<<"COMMAND PROCESSOR NOTIFYING THE LOG OBSERVER AS A CLASS INHERITING WHEN GET COMMAND"<<endl;
    command_processor->getCommand("start");
    cout<<"=============================="<<endl;
}
