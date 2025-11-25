#include "Player.h"
#include "../Orders/Orders.h"
#include "../part1-map/Map.h"
#include "../Part-4-Deck/Cards.h"

#include <iostream>
#include <algorithm>
#include <unordered_set>
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
Player::Player(const std:: string& playerName,Observer* obs): name(playerName),numArmies(0),numFreeArmies(0), truceList() {
    hand= new Hand(playerName);
    orderList=new OrdersList(obs);
    territories= vector<Territory*>();
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

    switch (mode) {
        case 1: //Deploy
            orderList->add(new OrdersDeploy(this,sourceTerritory,numArmies,obs));
            numFreeArmies -= numArmies;
            break;
        case 2: //Advance
            orderList->add(new OrdersAdvance(this,sourceTerritory,targetTerritory, numArmies,obs));
            numFreeArmies -= numArmies;
            break;
        case 3:
        case 4:
        case 5:
        case 6: {
            Card* matchingCard = nullptr;
            for (Card* card: hand->getCards()) {
                if ((mode == 3 && card->getName() == "Bomb") ||
                    (mode == 4 && card->getName() == "Blockade") ||
                    (mode == 5 && card->getName() == "Airlift") ||
                    (mode == 6 && card->getName() == "Diplomacy")) {
                    matchingCard = card;
                    break;
                    }
            }
            if (!matchingCard) {
                cout << name << " doesn't have the card required for this order." << endl;
                return;
            }
            OrdersList* olist = this->getOrderList();

            matchingCard->play(deck, hand, *olist, this, sourceTerritory,mode,numArmies,targetTerritory,&player2, obs);
            numFreeArmies -= numArmies;
            cout << name << " played a " << matchingCard->getName() << " card." << endl;
            break;

        }
        default:
            cout << "Invalid order mode." << endl;

    }
}

//returns a list of territories the player owns(to defend)
vector<Territory*> Player::toDefend(const std::vector<Territory*>& allTerritories) const{                                                                            //Require changes to work as intended
    /**
    //create a new empty vector defendList to store terr
    vector<Territory*> defendList;
    //loop through all the terr the player owns
    for(Territory* tr: territories){
        if(tr!=nullptr){
            //add it to the list
            defendList.push_back(tr);
        }
    }
    for (Territory* t: territories) {
        if (!t) continue;
        bool isBorder = false;
        for (const string& adjName: t->getAdjTerritoriesNames()) {
            Continent *cont = t->getContinent();
            if (cont) {
                for (Territory* cand: cont->getTerritories()) {
                    if (cand && cand->getName() == adjName) {
                        if (cand->getOwner() != this) {
                            isBorder = true;
                            break;
                        }
                    }
                }
            }
            if (isBorder) {
                break;
            }

        }
    }
    std::sort(defendList.begin(), defendList.end(), [](Territory* t1, Territory* t2){return t1->getNumOfArmies(), t2->getNumOfArmies();});
    return defendList;
    */
    std::vector<Territory*> defendList;
    struct Info { Territory* t; int threat; int armies; };

    std::vector<Info> infos;
    for (Territory* t : territories) {
        if (!t) continue;
        int threatCount = 0;
        for (const std::string& adjName : t->getAdjTerritoriesNames()) {
            auto it = std::find_if(allTerritories.begin(), allTerritories.end(),
                                   [&](Territory* cand) { return cand && cand->getName() == adjName; });
            if (it != allTerritories.end()) {
                Territory* cand = *it;
                if (cand->getOwner() != this) ++threatCount;
            }
        }
        infos.push_back({t, threatCount, t->getNumOfArmies()});
    }

    // Sort by descending threat (border territories first), then ascending armies (weaker first)
    std::sort(infos.begin(), infos.end(), [](const Info& a, const Info& b) {
        if (a.threat != b.threat) return a.threat > b.threat;
        return a.armies < b.armies;
    });

    for (auto &i : infos) defendList.push_back(i.t);
    return defendList;


}

//returns list of territories adjacent to player's territories but owned by other player(to attack)
vector<Territory*> Player::toAttack(const std::vector<Territory*>& allTerritories) const {                              //Require changes to work as intended

    /**
    std::vector<Territory*> attackList;


    cout << "TO ATTACK FUNCTION" << endl;

     // Loop through each territory the player owns
     for (Territory* myTerritory : territories) {
         if (!myTerritory) continue;

         // Loop through adjacent territory names
         for (const std::string& adjName : myTerritory->getAdjTerritoriesNames()) {
             // Find the Territory object by name in allTerritories
             auto it = std::find_if(allTerritories.begin(), allTerritories.end(),[&](Territory* t) {
                 return t && t->getName() == adjName && t->getOwner() != this;
             });

             // If found and not already in attackList, add it
             if (it != allTerritories.end() && std::find(attackList.begin(), attackList.end(), *it) == attackList.end()) {
                 attackList.push_back(*it);
             }
         }
     }*/


    std::vector<Territory*> attackList;

    // Build quick lookup of territories this player owns (by pointer and by name)
    std::unordered_set<Territory*> ownedPtrs;
    unordered_set<std::string> ownedNames;
    for (Territory* t : territories) {
        if (t) {
            ownedPtrs.insert(t);
            ownedNames.insert(t->getName());
        }
    }

    for (Territory* myT : territories) {
        if (!myT) continue;
        for (const std::string& adjName : myT->getAdjTerritoriesNames()) {
            auto it = std::find_if(allTerritories.begin(), allTerritories.end(),
                                   [&](Territory* cand) { return cand && cand->getName() == adjName; });
            if (it == allTerritories.end()) continue;

            Territory* target = *it;
            if (!target) continue;

            // Skip if target is one of this player's territories (by pointer or by matching name)
            if (ownedPtrs.count(target) || ownedNames.count(target->getName())) continue;

            // avoid duplicates
            if (std::find(attackList.begin(), attackList.end(), target) == attackList.end()) {
                attackList.push_back(target);
            }
        }
    }

    // sort by ascending defending armies (easier targets first)
    std::sort(attackList.begin(), attackList.end(),
              [](Territory* a, Territory* b) { return a->getNumOfArmies() < b->getNumOfArmies(); });

    return attackList;
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

Territory* Player::findTerritoryByName(const string& name) {
    for (auto t : territories)
        if (t->getName() == name)
            return t;
    return nullptr;
}
Hand* Player::getHand() {
    return hand;
}

void Player::setNumFreeArmies(int temp) {this->numFreeArmies = temp;}