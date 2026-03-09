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

    Player* p1 = new Player("Shawn",observer, StrategyType::Neutral);
    p1->addTerritory(Saskatchewan);

    Player* p2 = new Player("Howard",observer,StrategyType::Neutral);
    p2->addTerritory(Quebec);

    // Define and initialize the neutral player (to be done in game engine)
    Player* neutralPlayer = new Player("neutral",observer, StrategyType::Neutral);
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

// void testOrdersLists(){
//     cout << "-----------------------------------------------------------------------" << endl;
//     cout << "ORDERS LIST TEST" << endl;

//     std::vector<std::string> adj(5);
//     Continent* c1 = new Continent("NA", 5);
//     std::string name1 = "Montreal";
//     std::string name2 = "Toronto";
//     std::string name3 = "Miami";
//     std::string name4 = "Talahasee";

//     Territory* t1 = new Territory(name1, c1, 1, 3, adj);
//     Territory* t2 = new Territory(name2, c1, 2, 4, adj);
//     Territory* t3 = new Territory(name3, c1, 5, 6, adj);
//     Territory* t4 = new Territory(name4, c1, 7 ,8, adj);

//     Player* p1 = new Player("Shawn");
//     p1->addTerritory(t1);
//     p1->addTerritory(t2);

//     Player* p2 = new Player("Howard");
//     p2->addTerritory(t3);
//     p2->addTerritory(t4);


//     cout << "VALID ORDERS" << endl;

//     Orders* vorder1 = new OrdersDeploy(p1, p1->getTerritories()[0], 3);
//     Orders* vorder2 = new OrdersAdvance(p1, p1->getTerritories()[0], p1->getTerritories()[1], 1);
//     Orders* vorder3 = new OrdersBomb(p1, p2->getTerritories()[0]);
//     Orders* vorder4 = new OrdersBlockade(p1, p1->getTerritories()[1]);
//     Orders* vorder5 = new OrdersAirlift(p1, p1->getTerritories()[0], p1->getTerritories()[1], 2);
//     Orders* vorder6 = new OrdersNegotiate(p1, p2);
//     Orders* vlist[6] = {vorder1, vorder2, vorder3, vorder4, vorder5, vorder6};

//     cout << "INVALID ORDERS" << endl;
//     Orders* iorder1 = new OrdersDeploy(p2, p1->getTerritories()[0], 3);
//     Orders* iorder2 = new OrdersAdvance(p2, p1->getTerritories()[1], p2->getTerritories()[0], 10);
//     Orders* iorder3 = new OrdersBomb(p2, p2->getTerritories()[0]);
//     Orders* iorder4 = new OrdersBlockade(p2, p1->getTerritories()[1]);
//     Orders* iorder5 = new OrdersAirlift(p2, p1->getTerritories()[0], p2->getTerritories()[0], 10);
//     Orders* iorder6 = new OrdersNegotiate(p2, p2);

//     Orders* ilist[6] = {iorder1, iorder2, iorder3, iorder4, iorder5, iorder6};



//     for (int n = 0; n < 6; n++){
//         p1->getOrderList()->add(vlist[n]);
//         p2->getOrderList()->add(ilist[n]);
//     }

//     cout << "BELOW IS THE LIST OF ORDERS FOR PLAYER ONE (VALID)\n" << endl;
//     cout << *(p1->getOrderList()) << endl;
//     cout << "\nBELOW IS THE LIST OF ORDERS FOR PLAYER 1 (INVALID)\n" << endl;
//     cout << *(p2->getOrderList()) << endl;

//     cout << "NOW PRINTING THE VALIDITY OF THE ORDERS\n" << endl;
//     cout << "PLAYER 1 ORDERS" << endl;
//     for (auto order : p1->getOrderList()->getList()){
//         cout << "IS VALID?: ";
//         order->validate();
//         cout << endl;
//     }

//     cout << "\nPLAYER 2 ORDERS" << endl;
//     for (auto order : p2->getOrderList()->getList()){
//         cout << "IS VALID?: ";
//         order->validate();
//         cout << endl;
//     }


//     int firstInex, secondIndex;
//     int indexCount = 0;
//     cout << "PLEASE ENTER WHICH INDEX YOU WANT TO MOVE AND WHERE YOU WANT TO MOVE IT:" << endl;
//     for (auto order : p1->getOrderList()->getList()){
//         cout << indexCount << ": " << *order << endl;
//         indexCount++;
//     }
//     cin >> firstInex >> secondIndex;

//     cout << "LIST BEFORE MOVE\n" << *(p1->getOrderList()) << endl;
//     p1->getOrderList()->move(firstInex, secondIndex);
//     cout << "LIST AFTER MOVE\n" << *(p1->getOrderList());



//     indexCount = 0;
//     cout << "\nPLEASE ENTER WHICH INDEX YOU WANT TO REMOVE:" << endl;
//     for (auto order : p1->getOrderList()->getList()){
//         cout << indexCount << ": " << *order << endl;
//         indexCount++;
//     }
//     cin >> firstInex;

//     cout << "LIST BEFORE REMOVE\n" << *(p1->getOrderList()) << endl;
//     p1->getOrderList()->remove(firstInex);
//     cout << "LIST AFTER REMOVE\n" << *(p1->getOrderList());



//     cout << "\nTESTING COPY CONSTRUCTORS, AND ASSIGNMENT OPERATOR OVERLOAD\n" << endl;
//     Orders* o1 = new Orders();
//     Orders* o2 = new Orders(*o1);
//     Orders* o3 = new Orders();
//     *o3 = *o1;

//     cout << "ORDERS MEMORY ADDRESSES AND DATA:" << endl;
//     cout << o1 << ", DATA: " << *o1 << "\n\n";
//     cout << o2 << ", DATA: " << *o2 << "\n\n";
//     cout << o3 << ", DATA: " << *o3 << "\n\n";

//     OrdersAdvance* oa1 = new OrdersAdvance(p1, p1->getTerritories()[0], p1->getTerritories()[1], 1);
//     cout << "BEFORE COPY" << endl;
//     OrdersAdvance* oa2 = new OrdersAdvance(*oa1);
//     OrdersAdvance* oa3 = new OrdersAdvance();
//     *oa3 = *oa1;

//     cout << "ORDERS ADVANCE MEMORY ADDRESSES AND DATA:" << endl;
//     cout << oa1 << ", DATA: " << *oa1 << "\n\n";
//     cout << oa2 << ", DATA: " << *oa2 << "\n\n";
//     cout << oa3 << ", DATA: " << *oa3 << "\n\n";



//     OrdersList* olist1 = new OrdersList(*(p1->getOrderList()));
//     OrdersList* olist2 = new OrdersList();
//     *olist2 = *olist1;

//     cout << "\nORDER LIST MEMORY ADDESES AND DATA:" << endl;
//     cout << p1->getOrderList() << ", DATA: " << *(p1->getOrderList()) << "\n\n";
//     cout << olist1 << ", DATA: " << *olist1 << "\n\n";
//     cout << olist2 << ", DATA: " << *olist2 << "\n\n";

//     Player* p3 = new Player("Negin");
//     cout << "\nTESTING IHERITED CONSTRUCTOR" << endl;
//     OrdersBomb* b1 = new OrdersBomb(p3, new Territory("Hogwarts", c1, 5, 8, adj));
//     OrdersBomb* b2 = new OrdersBomb(*b1);
//     OrdersBomb* b3 = new OrdersBomb();
//     *b3 = *b1;

//     cout << "BOMB ADDRESSES" << endl;
//     cout << b1 << ": " << *b1 << endl;
//     cout << b2 << ": " << *b2 << endl;
//     cout << b3 << ": " << *b3 << endl;

//     cout << "-----------------------------------------------------------------------" << endl;
// }