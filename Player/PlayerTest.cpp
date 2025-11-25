#include "Player.h"
#include "../Orders/Orders.h"
#include "../part1-map/Map.h"
#include "../Part-4-Deck/Cards.h"

#include <iostream>
#include <algorithm>
#include <unordered_set>

#include "../PlayerStrategy/PlayerStrategy.h"

using namespace std;




Player::Player(Observer* obs) : name("hi") {
    orderList=new OrdersList(obs);
}
Player::Player(const std:: string& playerName): name(playerName),numArmies(0),numFreeArmies(0), truceList() {
    hand= new Hand(playerName);
    orderList=new OrdersList();
    territories= vector<Territory*>();

}
//cons with name
Player::Player(const std:: string& playerName,Observer* obs, StrategyType type): name(playerName),numArmies(0),numFreeArmies(0), truceList() {
    hand= new Hand(playerName);
    orderList=new OrdersList(obs);
    territories= vector<Territory*>();

    switch (type) {
        case StrategyType::Human: {
            ps = new HumanPlayerStrategy(this);
            break;
        }
        case StrategyType::Aggressive: {
            ps = new AggressivePlayerStrategy(this);
            break;
        }
        case StrategyType::Benevolent: {
            ps = new BenevolentPlayerStrategy(this);
            break;
        }
        case StrategyType::Neutral: {
            ps = new NeutralPlayerStrategy(this);
            break;
        }
        case StrategyType::Cheater: {
            ps = new CheaterPlayerStrategy(this);
            break;
        }
    }
}

Player::Player(const Player& other)
{
    name = other.name;
    hand = new Hand(this->name);

    // Deep copy territories
    for (auto t : other.territories)
    {
        territories.push_back(t);
    }
    numArmies = other.numArmies;
    numFreeArmies = other.numFreeArmies;

    // Deep copy hand cards
    for (auto c : other.hand->getCards()) hand->addCard(c);
    // Copy list of players with truce
    for (auto p : other.truceList) truceList.push_back(p);
    // Deep copy order list
    orderList = new OrdersList(*other.orderList);


    switch (other.ps->getType()) {
        case StrategyType::Human: {
            ps = new HumanPlayerStrategy(this);
            break;
        }
        case StrategyType::Aggressive: {
            ps = new AggressivePlayerStrategy(this);
            break;
        }
        case StrategyType::Benevolent: {
            ps = new BenevolentPlayerStrategy(this);
            break;
        }
        case StrategyType::Neutral: {
            ps = new NeutralPlayerStrategy(this);
            break;
        }
        case StrategyType::Cheater: {
            ps = new CheaterPlayerStrategy(this);
            break;
        }
    }
}

//asignment operator
Player::~Player() {
    delete hand;
    delete orderList;
}

Player& Player::operator=(const Player& other) {
    if (this == &other) return *this;

    // Clean up current
    territories.clear();
    delete hand;
    delete orderList;

    // WITH SHARED POINTER IDEA WE WOULD NOT BE DELETING THE PLAYERS
    truceList.clear();

    name = other.name;

    // Deep copy again
    for (auto t : other.territories) territories.push_back(t);
    for (auto c : other.hand->getCards()) hand->addCard(c);
    for (auto p: other.truceList) truceList.push_back(p);
    orderList = new OrdersList(*other.orderList);

    return *this;
}

//getters and setters
 string Player::getName() const{
  return name;
}
bool Player::getEarnedCard(){
    return earnedCard;
}
void Player::setName(const string& newName){
   name=newName;
}

void Player::setEarnedCard(bool hasEarned){
    earnedCard = hasEarned;
}

std::vector<Territory*> Player::getTerritories() {
    return this->territories;
}

std::vector<Player*> Player::getTruceList(){
    return this->truceList;
}

OrdersList* Player::getOrderList(){
    return this->orderList;
}

//adds a terr to the player's list
void Player:: addTerritory(Territory* tr){
    if(tr!=nullptr){
        territories.push_back(tr);
        tr->setOwner(this); //sets this player as its owner
    }
}
// removes a territory from a player's list
void Player::removeTerritory(Territory* tr){
    /**
    if (tr == nullptr) return;
    int indexOfRemovedTerr = -1;
    for (int index = 0; index < territories.size(); index++){
        if (territories[index] == tr){
            indexOfRemovedTerr = index;
            break;
        }
    }
    if (indexOfRemovedTerr != -1){
        Territory* temp = territories[territories.size()-1];
        territories[territories.size()-1] = tr;
        territories[indexOfRemovedTerr] = temp;
        territories.pop_back();
    }
    */
    if (!tr) return;

    auto it = std::find_if(territories.begin(), territories.end(),
        [tr](Territory* t) {
            // Compare by pointer OR by unique name
            return t == tr || (t && tr && t->getName() == tr->getName());
        });

    if (it != territories.end()) {
        territories.erase(it);
        cout << "Removed territory " << tr->getName() << " from player " << name << endl;
    } else {
        cout << "Warning: territory " << tr->getName() << " not found in player " << name << "'s list" << endl;
    }
}

void Player::addTruce(Player* enemyToTruce){
    if (enemyToTruce != nullptr){
        truceList.push_back(enemyToTruce);
    }
}
void Player::removeTruce(Player* enemy){
    if (enemy == nullptr) return;
    int indexOfRemovedTruce = -1;
    for (int index = 0; index < truceList.size(); index++){
        if (truceList[index] == enemy){
            indexOfRemovedTruce = index;
            break;
        }
    }
    if (indexOfRemovedTruce != -1){
        Player* temp = truceList[truceList.size()-1];
        truceList[truceList.size()-1] = enemy;
        truceList[indexOfRemovedTruce] = temp;
        truceList.pop_back();
    }
}

//add a card to the player's hand and prints a string
void Player::addCard(Card* ca){
    if (ca!=nullptr){
        hand->addCard(ca);
        //cout<<name<<" has card: "<<ca->getName()<<endl;
   }
}

//add an order to the player's order list. Player owns the order list and it will delete it in the destructor
void Player::addOrder(Orders* ord) {
    orderList->add(ord);
}

void Player::issueOrder(Deck& deck, int mode, Territory* sourceTerritory, int numArmies, Territory* targetTerritory, Player& player2,Observer* obs) {

    this->ps->issueOrder(deck, mode, sourceTerritory, numArmies, targetTerritory, player2, obs);
}

//returns a list of territories the player owns(to defend)
vector<Territory*> Player::toDefend(const std::vector<Territory*>& allTerritories) const{                                                                            //Require changes to work as intended
    return this->ps->toDefend(allTerritories);


}

//returns list of territories adjacent to player's territories but owned by other player(to attack)
vector<Territory*> Player::toAttack(const std::vector<Territory*>& allTerritories) const {                              //Require changes to work as intended

    return this->ps->toAttack(allTerritories);
}

 //print the status of the player
 void Player::printStatus() const{
    cout << "PLAYER PRINT STATUS" << endl;


  cout<<"Player: "<<name<<endl;
  cout<<"Territories: ";
  if(territories.empty()) cout<<"none";
  else for(Territory* t: territories) cout<<t->getName()<<" ";
  cout<<"\n Hand cards: ";
  if(hand->isEmpty()) cout<<"none";
  else for(Card* c: hand->getCards()) cout<<c->getName()<<" ";
  cout<<endl;
  cout<<"Orders: ";
  const auto list=orderList->getList();
  if(list.empty()) cout<<"none";
  else for (Orders* o:list) cout<< *o;
  cout<<endl;
 }

bool Player::equals(Player* player2){
    if (this->name != player2->name)
        return false;
    
    if (this->territories.size() != player2->territories.size())
        return false;

    for (int n = 0; n < this->territories.size(); n++){
        if (!(this->territories[n] == player2->territories[n]))
            return false;
    }
    return true;
}

//stream insertion operator
ostream& operator<<(std::ostream& out, const Player& p){
    out<<"Player: "<<p.name<<"\n";
    out<<"Territories: ";
    if(!p.territories.empty()){
        for(auto t:(p.territories))
            out<< t->getName()<<" ";
    } 
    else out<<"none";
    out<<"\n";

    out<<"Hand: ";
    if(!p.hand->isEmpty()){
        for(auto c: p.hand->getCards())
            out<<c->getName()<<" ";
    } 
    else{
            out<<"none";
    }
    out<<"\n";

    out<<"Orders: ";
    out<<"none\n";
    return out;
}


//=====================================
void Player::addNumArmies(int newArmies) {
    numArmies += newArmies;
    numFreeArmies += newArmies;
}
int Player::getNumArmies() {
    return numArmies;
}
int Player::getNumFreeArmies() {
    return numFreeArmies;
}
void Player::removeNumFreeArmies(int newArmies) {
    if (this->numFreeArmies < newArmies) {
        cout << "Not enough army units, attributing " << numFreeArmies << " armies." << endl;
        this->numFreeArmies -= numFreeArmies;
    }
    else {
        numFreeArmies -= newArmies;
    }
}

Territory* Player::findTerritoryByName(const string& name) {
    for (auto t : territories)
        if (t->getName() == name)
            return t;
    return nullptr;
}
Hand* Player::getHand() {
    return hand;
}

//===========================================

void Player::setPlayerStrategy(PlayerStrategy* ps) {
    if (this->ps) {
        delete this->ps;
    }
    this->ps = ps;
}
PlayerStrategy* Player::getPlayerStrategy() {
    return ps;
}




