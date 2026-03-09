#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>
#include <iostream>
#include "../Part-4-Deck/Cards.h"
#include "../PlayerStrategy/PlayerStrategy.h"


// Forward declarations
class Territory;
class Card;
class Orders;
class OrdersList;
class Deck;
class Hand;
class PlayerStrategy;
enum class StrategyType;

using namespace std;

class Player {
  private:
    std::string name;
    std::vector<Territory*> territories; //dynamic array of territory pointers
    std::vector<Card*> handCards;
    OrdersList* orderList;
    Hand* hand;

    std::vector<Player*> truceList;
    bool earnedCard;

  //=====
    int numArmies; //Total of army units
    int numFreeArmies; //Number of free army units to deploy

    //=========
    PlayerStrategy* ps;



public:
   Player();
   Player(const std::string& playerName);
    Player(Observer* obs);
    Player(const std::string& playerName, Observer* obs, StrategyType type);
    Player(const Player& other); //copy constructor
   Player& operator =(const Player& other); //assignment operator
   ~Player(); //destructor, clears up resources when a player obj is destroyed
   std::string getName() const;
   OrdersList* getOrderList();
   std::vector<Territory*> getTerritories();
   std::vector<Player*> getTruceList();
   bool getEarnedCard();
   void setName(const std::string& newName);
   void setEarnedCard(bool hasEarned);
   void addTerritory(Territory* tr);
   void removeTerritory(Territory* tr);
   void addCard(Card* ca);
   void addOrder(Orders* ord);
   void addTruce(Player* enemyToTruce);
   void removeTruce(Player* enemy);
 //game actions
   std::vector<Territory*> toDefend(const std::vector<Territory*>& allTerritories) const;
   std::vector<Territory*> toAttack(const std::vector<Territory*>& allTerritories) const;
  //added item
   void issueOrder(Deck& deck, int mode, Territory* sourceTerritory, int numArmies, Territory* targetTerritory, Player& player2, Observer*
                   obs);//execute all orders and clean up memory

  void printStatus() const; //print player's name, territories
   bool equals(Player* player2);
  friend std::ostream& operator <<(std::ostream& out, const Player& p);

  //Part 2
  void addNumArmies(int newArmies);
  void removeNumFreeArmies(int newArmies);
  int getNumArmies();
  int getNumFreeArmies();

  Territory* findTerritoryByName(const string& name);
    Hand* getHand();

    //part 3
    void setPlayerStrategy(PlayerStrategy* ps);
    PlayerStrategy* getPlayerStrategy();
    void isAttacked();
    void setAttacked(bool newAttacked);

    bool getAttacked();

};



#endif