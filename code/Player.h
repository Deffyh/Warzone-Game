//
// Created by ngyna on 2025-09-26.
//

#ifndef COMP_335_PROJECT_FALL_2025_PLAYER_H
#define COMP_335_PROJECT_FALL_2025_PLAYER_H
#include <string>
#include <vector>
#include <iostream>
using namespace std;

//forward declarations
class Player;
class Territory;
class Cards;
class Orders;
class OrdersList;
//placeholder classes
class OrderList;

class Player {
private:
   std::string name;
   std::vector<Territory*> territories; //dynamic array of territory pointers
   std::vector<Cards*> handCards;
   std::vector<Orders*> orders;
    OrdersList* orderList;


public:
   Player();
   Player(const std::string& playerName);
   Player(const Player& other); //copy constructor
   Player& operator =(const Player& other); //assignemnt operator
   ~Player(); //destructor, clears up resources when a player obj is destroyed
   std::string getName() const;
    const std::vector<Orders*> getOrderList() const;
   void setName(const std::string& newName);
   void addTerritory(Territory* tr);
   void addCard(Cards* ca);
   void addOrder(Orders* ord);
 //game actions
   std::vector<Territory*> toDefend() const;
   std::vector<Territory*> toAttack(const std::vector<Territory*>& allTerritories) const;
   void issueOrder(); //execute all orders and clean up memory
   void printStatus() const; //print player's name, territories
  friend std::ostream& operator <<(std::ostream& out, const Player& p);


};


#endif //COMP_335_PROJECT_FALL_2025_PLAYER_H