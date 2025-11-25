#include "Orders.h"
#include "../Player/Player.h"
#include "../part1-map/Map.h"
#include "../Part-4-Deck/Cards.h"
#include <string>

void printPlayerTerrAndArmies(Player* p1, Player* p2, Player* neutralPlayer){
    cout << "List of territories owned by Player 1: ";
    for (auto terr : p1->getTerritories()){
        cout << terr->getName() << "(" << terr->getNumOfArmies() << ") ";
    }
    cout << endl;

    cout << "List of territories owned by Player 2: ";
    for (auto terr : p2->getTerritories()){
        cout << terr->getName() << "(" << terr->getNumOfArmies() << ") ";
    }
    cout << endl;

    cout << "List of territories owned by Neutral Player: ";
    for (auto terr : neutralPlayer->getTerritories()){
        cout << terr->getName() << "(" << terr->getNumOfArmies() << ") ";
    }
    std::cout << "\n\n";
}

void testOrderExecution(){
    cout << "-----------------------------------------------------------------------" << endl;

    LogObserver* observer = new LogObserver();

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

    Player* p1 = new Player("Shawn",observer);
    p1->addTerritory(Saskatchewan);

    Player* p2 = new Player("Howard",observer);
    p2->addTerritory(Quebec);

    // Define and initialize the neutral player (to be done in game engine)
    Player* neutralPlayer = new Player("neutral",observer);
    OrdersBlockade::neutralPlayer = neutralPlayer;
    neutralPlayer->addTerritory(Manitoba);
    neutralPlayer->addTerritory(Ontario);


    Orders* iorder1 = new OrdersDeploy(p2, Saskatchewan, 10,observer);
    Orders* iorder2 = new OrdersAdvance(p2, Saskatchewan, Quebec, 10,observer);
    Orders* iorder3 = new OrdersAdvance(p2, Quebec, Manitoba, 10,observer);
    Orders* iorder4 = new OrdersBomb(p2, Quebec,observer);
    Orders* iorder5 = new OrdersBomb(p2, Saskatchewan,observer);
    Orders* iorder6 = new OrdersBlockade(p2, Saskatchewan,observer);
    Orders* iorder7 = new OrdersAirlift(p2, Saskatchewan, Quebec, 10,observer);
    Orders* iorder8 = new OrdersAirlift(p2, Quebec, Saskatchewan, 10,observer);
    Orders* iorder9 = new OrdersNegotiate(p2, p2,observer);

    Orders* ilist[9] = {iorder1, iorder2, iorder3, iorder4, iorder5, iorder6, iorder7, iorder8, iorder9};

    for (int n = 0; n < 9; n++){
        p2->getOrderList()->add(ilist[n]);
    }

    cout << "BELOW IS THE LIST OF ORDERS FOR PLAYER 2 (INVALID)\n" << endl;
    cout << *(p2->getOrderList()) << endl;

    cout << "NOW ATTEMPTING TO EXECUTE THE ORDERS" << endl;
    cout << "\nPLAYER 2 ORDERS" << endl;
    int num = 1;
    while (!p2->getOrderList()->getList().empty()){
        cout << "Order Number " << num << ": ";
        p2->getOrderList()->getList().front()->execute();
        num++;
    }
    num = 0;

    // Perform Deploys
    cout << "\nDeploying armies" << endl;
    p1->getOrderList()->add(new OrdersDeploy(p1, Saskatchewan, 100,observer));
    p2->getOrderList()->add(new OrdersDeploy(p2, Quebec, 20,observer));
    p1->getOrderList()->getList().front()->execute();
    p2->getOrderList()->getList().front()->execute();

    printPlayerTerrAndArmies(p1,p2,neutralPlayer);

    // Perform advances
    cout << "Advancing armies" << endl;
    p1->getOrderList()->add(new OrdersAdvance(p1, Saskatchewan, Manitoba, 95,observer));
    p2->getOrderList()->add(new OrdersAdvance(p2, Quebec, Ontario, 2,observer));
    p1->getOrderList()->getList().front()->execute();
    p2->getOrderList()->getList().front()->execute();
    // ------------------------------------------------------------------------

    // DO PLAYERS GET A CARD WHEN CAPTURING A NEUTRAL TERRITORY?
    // MAYBE PRINT THE CARDS HELD BY THE PLAYER TOO
    printPlayerTerrAndArmies(p1,p2,neutralPlayer);

    // Attempt to conquer a territory that has defenses
    cout << "Player 1 capturing Player 2 Ontario" << endl;
    p1->getOrderList()->add(new OrdersAdvance(p1, Manitoba, Ontario, 20,observer));
    p1->getOrderList()->getList().front()->execute();

    printPlayerTerrAndArmies(p1, p2, neutralPlayer);

    // Perform Negotiate order
    cout << "Negotiation" << endl;
    p1->getOrderList()->add(new OrdersNegotiate(p1, p2,observer));
    p1->getOrderList()->getList().front()->execute();

    p1->getOrderList()->add(new OrdersAdvance(p1, Ontario, Quebec, 3,observer));
    p1->getOrderList()->add(new OrdersBomb(p1, Quebec,observer));
    p1->getOrderList()->getList().front()->execute();
    p1->getOrderList()->getList().front()->execute();
    p2->getOrderList()->add(new OrdersAdvance(p2, Quebec, Ontario, 7,observer));
    p2->getOrderList()->add(new OrdersBomb(p2, Ontario,observer));
    p2->getOrderList()->getList().front()->execute();
    p2->getOrderList()->getList().front()->execute();

    printPlayerTerrAndArmies(p1, p2, neutralPlayer);
    
    // Clear the players truce list (would be done in game loop/engine after a global turn)
    p1->getTruceList().clear();
    p2->getTruceList().clear();

    // Perform a blockade
    cout << "Blockade" << endl;
    p1->getOrderList()->add(new OrdersBlockade(p1, Ontario,observer));
    p1->getOrderList()->getList().front()->execute();

    printPlayerTerrAndArmies(p1, p2, neutralPlayer);

    // Perform Bombing
    cout << "Bomb" << endl;
    p1->getOrderList()->add(new OrdersBomb(p1, Ontario,observer));
    p1->getOrderList()->getList().front()->execute();

    printPlayerTerrAndArmies(p1, p2, neutralPlayer);

    // Advance (Try to capture)
    cout << "Advance armies" << endl;
    p1->getOrderList()->add(new OrdersAdvance(p1, Manitoba, Ontario, Manitoba->getNumOfArmies(),observer));
    p1->getOrderList()->getList().front()->execute();

    printPlayerTerrAndArmies(p1, p2, neutralPlayer);


    // Airlift (from start to a further owned territory)
    cout << "Airlift from Saskatchewan to Ontario" << endl;
    p1->getOrderList()->add(new OrdersAirlift(p1, Saskatchewan, Ontario, Saskatchewan->getNumOfArmies(),observer));
    p1->getOrderList()->getList().front()->execute();

    printPlayerTerrAndArmies(p1, p2, neutralPlayer);


    cout << "\n-----------------------------------------------------------------------" << endl;
}
