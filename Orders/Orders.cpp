#include <iostream>
#include <list>
#include <algorithm>
#include <string>
#include <stdlib.h>
#include <time.h>
#include "Orders.h"
#include "../Player/Player.h"
#include "../part1-map/Map.h"
#include "../PlayerStrategy/PlayerStrategy.h"
using namespace std;

// For now the neutral player 
Player* OrdersBlockade::neutralPlayer = nullptr;

std::ostream& operator<<(std::ostream &strm, const Orders& order){
    order.printOrder(strm);
    return strm;
}
std::ostream& operator<<(std::ostream &strm, const OrdersList& orderList){
    strm << "Order List Contains the Below Orders:\n";
    for (Orders* order : orderList.getList()){
            strm << *order << endl;
       
    }
    return strm;
}
std::ostream& operator<<(std::ostream &strm, Territory& territory){
    strm << "Territory: " << territory.getName();
    return strm;
}
Orders::Orders() : issuingPlayer(nullptr) , sourceTerritory(nullptr), isActive(false), isValid(false){}
Orders::Orders(Player* issuingPlayer, Territory* sourceTerritory,Observer* obs) : issuingPlayer(issuingPlayer), sourceTerritory(sourceTerritory), isActive(false), isValid(false)
{
    Subject::addObserver(obs);
}
Orders::Orders(const Orders& order): Subject(order){

    if (order.getIssuingPlayer())
        // this->issuingPlayer = new Player(*(order.issuingPlayer)); // might be fine if player class has copy constructor and assignment operator
        this->issuingPlayer = order.getIssuingPlayer();
    else   
        this->issuingPlayer = nullptr;


    if (order.getSourceTerritory())
        // this->sourceTerritory = new Territory(*(order.sourceTerritory)); // might be fine if Territory class has copy constructor and assignment operator
        this->sourceTerritory = order.sourceTerritory;
    else
        this->setSourceTerritory(nullptr);

    this->setIsActive(order.getIsActive());
    this->setIsValid(order.getIsValid());
}
Orders::~Orders(){}

Orders& Orders::operator=(const Orders& order){
    this->observer_=order.observer_;

    if (this != &order){
        // delete this->getSourceTerritory(); // Free old memory before assignment
        // delete this->getIssuingPlayer();

        if (order.getIssuingPlayer())
            // this->issuingPlayer = new Player(*order.issuingPlayer); // might be fine if player class has copy constructor and assignment operator
            this->issuingPlayer = order.getIssuingPlayer();
        else   
            this->issuingPlayer = nullptr;

        if (order.getSourceTerritory())
            // this->sourceTerritory = new Territory(*(order.sourceTerritory)); // might be fine if Territory class has copy constructor and assignment operator
            this->sourceTerritory = order.sourceTerritory;
        else   
            this->sourceTerritory = nullptr;
        
        this->setIsActive(order.getIsActive());
        this->setIsValid(order.getIsValid());

    }
    return *this;
}
Player* Orders::getIssuingPlayer() const{
    return this->issuingPlayer;
};
void Orders::setIssuingPlayer(Player* issuingPlayer){
    this->issuingPlayer = issuingPlayer;
}
Territory* Orders::getSourceTerritory() const{
    return this->sourceTerritory;
}
void Orders::setSourceTerritory(Territory* sourceTerritory){
    this->sourceTerritory = sourceTerritory;
}
bool Orders::getIsActive() const{
    return this->isActive;
}
void Orders::setIsActive(bool isActive){
    this->isActive = isActive;
}
bool Orders::getIsValid() const{
    return this->isValid;
}
void Orders::setIsValid(bool isValid){
    this->isValid = isValid;
}
Orders* Orders::allocateClone() const{
    //return new Orders(*this);
    // Still requires allocate clone for Orders even though we cannot return a Order object since it is abstract
    // This is due to polymorphism and types at comilation different from types at runtime (dynmaic binding)
    return nullptr;
}
void Orders::printOrder(std::ostream& strm) const{
    strm << "An Order is the superclass of all the individual orders.\nIt has no affect since it is an abstract class.";
}


OrdersDeploy::OrdersDeploy() : Orders(), numArmies(0) {}
OrdersDeploy::OrdersDeploy(Player* issuingPlayer, Territory* sourceTerritory, int numArmies,Observer* obs) : Orders(issuingPlayer, sourceTerritory,obs), numArmies(numArmies){}
OrdersDeploy::OrdersDeploy(const OrdersDeploy& order) : Orders(order) {
    this->setNumArmies(order.getNumArmies());
}
OrdersDeploy& OrdersDeploy::operator=(const OrdersDeploy& order){
        if (this != &order){
            Orders::operator=(order);

            this->setNumArmies(order.numArmies);
    }
    return *this;
}

int OrdersDeploy::getNumArmies() const{
    return this->numArmies;
}
void OrdersDeploy::setNumArmies(int numArmies) {
    this->numArmies = numArmies;
}

Orders* OrdersDeploy::allocateClone() const{
    return new OrdersDeploy(*this);
}
void OrdersDeploy::printOrder(std::ostream& strm) const{
    strm << "Player " << this->getIssuingPlayer()->getName() << " has deployed " <<  this->getNumArmies() << " to " << *this->getSourceTerritory();
}
bool OrdersDeploy::validate(){
    if (this->getSourceTerritory() == nullptr || this->getIssuingPlayer() == nullptr || this->getSourceTerritory()->getOwner() == nullptr)
        return false;
    // May have to add in equals functions
    if (this->getSourceTerritory()->getOwner()->equals(this->getIssuingPlayer())){
        //cout << "VALID DEPLOY" << endl;
        return true;
    }
    //cout << "Deploying to unowned territory is invalid" << endl;
    return false;
}
void OrdersDeploy::execute(){
    // Have to add checks for number of armies being deployed or will it be done elsewhere before calling execute?
    bool isValid = validate();
    if (!isValid){
        cout << "Deploying to unowned territory " << this->getSourceTerritory()->getName() << " is invalid" << endl;
        Orders* o = this->getIssuingPlayer()->getOrderList()->remove(0);
    notify(*this);
        delete o;
        return;
    }
    int currentNumArmies = this->getSourceTerritory()->getNumOfArmies();
    this->getSourceTerritory()->setNumOfArmies(currentNumArmies + this->getNumArmies());

    notify(*this);
    Orders* o = this->getIssuingPlayer()->getOrderList()->remove(0);
    delete o;
}

OrdersAdvance::OrdersAdvance() : Orders(), targetTerritory(nullptr), numArmies(0) {}
OrdersAdvance::OrdersAdvance(Player* issuingPlayer, Territory* sourceTerritory, Territory* targetTerritory, int numArmies,Observer* obs) : Orders(issuingPlayer, sourceTerritory,obs), targetTerritory(targetTerritory), numArmies(numArmies){}
OrdersAdvance::OrdersAdvance(const OrdersAdvance& order) : Orders(order) {
    if (order.getTargetTerritory()){
        // this->targetTerritory = new Territory(*order.targetTerritory); // might be fine if Territory class has copy constructor and assignment operator
        this->targetTerritory = order.targetTerritory;
    }
    else   
        this->setTargetTerritory(nullptr);
    this->setNumArmies(order.getNumArmies());
}
OrdersAdvance::~OrdersAdvance(){}
OrdersAdvance& OrdersAdvance::operator=(const OrdersAdvance& order){
    if (this != &order){
        Orders::operator=(order);
        // delete this->getTargetTerritory();

        if (order.getTargetTerritory())
            // this->targetTerritory = new Territory(*order.targetTerritory); // might be fine if Territory class has copy constructor and assignment operator
            this->targetTerritory = order.targetTerritory;
        else
            this->setTargetTerritory(nullptr);
        this->setNumArmies(order.getNumArmies());
    }
    return *this;
}
Territory* OrdersAdvance::getTargetTerritory() const{
    return this->targetTerritory;
}
void OrdersAdvance::setTargetTerritory(Territory* targetTerritory){
    this->targetTerritory = targetTerritory;
}
int OrdersAdvance::getNumArmies() const{
    return this->numArmies;
}
void OrdersAdvance::setNumArmies(int numArmies) {
    this->numArmies = numArmies;
}
Orders* OrdersAdvance::allocateClone() const{
    return new OrdersAdvance(*this);
}
void OrdersAdvance::printOrder(std::ostream& strm) const{
    strm << "Player " << this->getIssuingPlayer()->getName() << " is advancing " << this->getNumArmies() << " armies from " << *this->getSourceTerritory() << " to " << *this->getTargetTerritory();
}
bool OrdersAdvance::validate(){
    bool SourceTerritoryBelongsToPlayer = false;
    bool TargetTerritoryAdjacentToSource = false;

    if (this->getSourceTerritory() == nullptr || this->getIssuingPlayer() == nullptr || this->getTargetTerritory() == nullptr)
        return false;
    
    for (Player* enemyWithTrue : this->getIssuingPlayer()->getTruceList()){
        if (this->getTargetTerritory()->getOwner() == enemyWithTrue)
            return false;
    }

    if (this->getSourceTerritory()->getOwner()->equals(this->getIssuingPlayer())){
        SourceTerritoryBelongsToPlayer = true;
    }

    string targetTerritoryName = this->getTargetTerritory()->getName();
    // for (int index = 0; index < this->getSourceTerritory()->getAdjTerritoriesNames().size(); index++)
    for (string AdjacentTerrName : this->getSourceTerritory()->getAdjTerritoriesNames()){
        if (AdjacentTerrName == targetTerritoryName){
            TargetTerritoryAdjacentToSource = true;
            break;
        }
    }
    return (SourceTerritoryBelongsToPlayer && TargetTerritoryAdjacentToSource);
}
void OrdersAdvance::execute(){
    srand((unsigned) time(NULL));
    bool isValid = this->validate();
    int probAttackArmyKills = 60;
    int probDefendArmyKills = 70;
    if (!isValid){
        cout << "Invalid advance from territory " << this->getSourceTerritory()->getName() << " to territory " <<
            this->getTargetTerritory()->getName() << endl;
    notify(*this);
        Orders* o = this->getIssuingPlayer()->getOrderList()->remove(0);
        delete o;
        return;
    }

    if (targetTerritory->getOwner()->getPlayerStrategy()->getType() == StrategyType::Neutral) {
        if (auto* neutral = dynamic_cast<NeutralPlayerStrategy*>(targetTerritory->getOwner()->getPlayerStrategy())) {
            neutral->setAttacked();
        }
    }

    // We are advancing armies from one of our territories to another one of our territories
    int currentNumArmiesSource = this->getSourceTerritory()->getNumOfArmies();
    int currentNumArmiesTarget = this->getTargetTerritory()->getNumOfArmies();
    if (this->getTargetTerritory()->getOwner()->equals(this->getIssuingPlayer())){
        this->getSourceTerritory()->setNumOfArmies(currentNumArmiesSource - this->getNumArmies());
        this->getTargetTerritory()->setNumOfArmies(currentNumArmiesTarget + this->getNumArmies());

    notify(*this);
        Orders* o = this->getIssuingPlayer()->getOrderList()->remove(0);
        delete o;
        return;
    }

    // After validating the only other possible case is if were advancing to enemy territory
    // Neutral territory likely has no owner and 0 armies
    int randomNum;
    int attackingArmies = this->getNumArmies();
    int defendingArmies = this->getTargetTerritory()->getNumOfArmies();
    this->getSourceTerritory()->setNumOfArmies(currentNumArmiesSource - this->getNumArmies());
    while (true){
        if (attackingArmies <= 0 || defendingArmies <= 0)
            break;

        randomNum = rand() % 101;
        if (randomNum <= probAttackArmyKills)
            defendingArmies--;
        randomNum = rand() % 101;
        if (randomNum <= probDefendArmyKills)
            attackingArmies--;
    }
    // Defenders win
    if (attackingArmies <= 0 && defendingArmies >= 0)
        this->getTargetTerritory()->setNumOfArmies(defendingArmies);
    // Attackers win
    else if (attackingArmies > 0 && defendingArmies <= 0){
        // Remove territory from defending playerlist of territories
        // Change owner of territory to new territory
        // Add territory to attacking players list of territories
        // Set the number of armies in territory to surviving armies (attackingArmies)
        if (this->getTargetTerritory()->getOwner() != nullptr)
            this->getTargetTerritory()->getOwner()->removeTerritory(this->getTargetTerritory());
        this->getTargetTerritory()->setOwner(this->getIssuingPlayer());
        this->getIssuingPlayer()->addTerritory(this->getTargetTerritory());
        this->getTargetTerritory()->setNumOfArmies(attackingArmies);
        this->getIssuingPlayer()->setEarnedCard(true);
    }
    notify(*this);
    Orders* o = this->getIssuingPlayer()->getOrderList()->remove(0);
    delete o;
}

Orders* OrdersBomb::allocateClone() const{
    return new OrdersBomb(*this);
}
void OrdersBomb::printOrder(std::ostream& strm) const{
    strm << "Player " << this->getIssuingPlayer()->getName() << " has sent a bomb to " << *this->getSourceTerritory();
}
bool OrdersBomb::validate(){
    bool isBombingEnemy = false;
    bool isTerrAdjToOwnedTerr = false;
    if (this->getSourceTerritory() == nullptr || this->getSourceTerritory()->getOwner() == nullptr || this->getIssuingPlayer() == nullptr)
        return false;
    
    for (Player* enemyWithTrue : this->getIssuingPlayer()->getTruceList()){
        if (this->getSourceTerritory()->getOwner() == enemyWithTrue)
            return false;
    }

    if (!(this->getSourceTerritory()->getOwner()->equals(this->getIssuingPlayer()))){
        isBombingEnemy = true;
    }
    string terrToBomb = this->getSourceTerritory()->getName();
    // for (int index = 0; index < this->getSourceTerritory()->getAdjTerritoriesNames().size(); index++)
    for (Territory* ownedTerr : this->getIssuingPlayer()->getTerritories()){
        for (string adjacentTerr : ownedTerr->getAdjTerritoriesNames()){
            if (adjacentTerr == terrToBomb){
                isTerrAdjToOwnedTerr = true;
                break;
            }
        }
        if (isTerrAdjToOwnedTerr) break;
    } 

    return (isBombingEnemy && isTerrAdjToOwnedTerr);     
}
void OrdersBomb::execute(){
    bool isValid = this->validate();
    if (!isValid){
        cout << "Cannot perform bombing of territory " << this->getSourceTerritory()->getName() << endl;
    notify(*this);
        Orders* o = this->getIssuingPlayer()->getOrderList()->remove(0);
        delete o;
        return;
    }

    if (this->getSourceTerritory()->getOwner()->getPlayerStrategy()->getType() == StrategyType::Neutral) {
        if (auto* neutral = dynamic_cast<NeutralPlayerStrategy*>(this->getSourceTerritory()->getOwner()->getPlayerStrategy())) {
            neutral->setAttacked();
        }
    }


    int numArmiesBeforeBomb = this->getSourceTerritory()->getNumOfArmies();
    this->getSourceTerritory()->setNumOfArmies(numArmiesBeforeBomb / 2);

    notify(*this);
    Orders* o = this->getIssuingPlayer()->getOrderList()->remove(0);
    delete o;
}

Orders* OrdersBlockade::allocateClone() const{
    return new OrdersBlockade(*this);
}
void OrdersBlockade::printOrder(std::ostream& strm) const{
    strm << "Player " << this->getIssuingPlayer()->getName() << " is performing a blockade on " << *this->getSourceTerritory();
}
bool OrdersBlockade::validate(){
    if (this->getSourceTerritory() == nullptr || this->getIssuingPlayer() == nullptr)
        return false;

    if (this->getSourceTerritory()->getOwner()->equals(this->getIssuingPlayer())){
        // cout << "VALID BLOCKADE" << endl;
        return true;
    }
    return false;
}
void OrdersBlockade::execute(){
    bool isValid = this->validate();
    if (!isValid){
        cout << "Blockade on territory that is not yours is invalid" << endl;

    notify(*this);
        Orders* o = this->getIssuingPlayer()->getOrderList()->remove(0);
        delete o;
        return;
    }
    int currentNumArmies = this->getSourceTerritory()->getNumOfArmies();
    this->getSourceTerritory()->setNumOfArmies(currentNumArmies * 2);
    this->getIssuingPlayer()->removeTerritory(this->getSourceTerritory());
    this->getSourceTerritory()->setOwner(neutralPlayer);
    neutralPlayer->addTerritory(this->getSourceTerritory());

    notify(*this);
    Orders* o = this->getIssuingPlayer()->getOrderList()->remove(0);
    delete o;
}

OrdersAirlift::OrdersAirlift() : Orders(), targetTerritory(nullptr), numArmies(0) {}
OrdersAirlift::OrdersAirlift(Player* issuingPlayer, Territory* sourceTerritory, Territory* targetTerritory, int numArmies,Observer* obs) : Orders(issuingPlayer, sourceTerritory,obs), targetTerritory(targetTerritory), numArmies(numArmies) {}
OrdersAirlift:: OrdersAirlift(const OrdersAirlift& order) : Orders(order) {
    if (order.getTargetTerritory())
        // this->targetTerritory = new Territory(*order.targetTerritory); // might be fine if Territory class has copy constructor and assignment operator
        this->targetTerritory = order.targetTerritory;
    else   
        this->setTargetTerritory(nullptr);
    this->setNumArmies(order.getNumArmies());

}
OrdersAirlift::~OrdersAirlift(){}
OrdersAirlift& OrdersAirlift::operator=(const OrdersAirlift& order){
    if (this != &order){
        Orders::operator=(order);
        // delete this->getTargetTerritory();

        if (order.getTargetTerritory())
            // this->targetTerritory = new Territory(*order.targetTerritory); // might be fine if Territory class has copy constructor and assignment operator
            this->targetTerritory = order.targetTerritory;
        else
            this->setTargetTerritory(nullptr);
        this->setNumArmies(order.getNumArmies());
    }
    return *this;
}
Territory* OrdersAirlift::getTargetTerritory() const{
    return this->targetTerritory;
}
void OrdersAirlift::setTargetTerritory(Territory* targetTerritory){
    this->targetTerritory = targetTerritory;
}
int OrdersAirlift::getNumArmies() const{
    return this->numArmies;
}
void OrdersAirlift::setNumArmies(int numArmies) {
    this->numArmies = numArmies;
}
Orders* OrdersAirlift::allocateClone() const{
    return new OrdersAirlift(*this);
}
void OrdersAirlift::printOrder(std::ostream& strm) const{
    strm << "Player " << this->getIssuingPlayer()->getName() << " is airlifting " << this->getNumArmies() << " from " << *this->getSourceTerritory() << "to " << *this->getTargetTerritory();
}
bool OrdersAirlift::validate(){
    if (this->getSourceTerritory() == nullptr || this->getIssuingPlayer() == nullptr || this->getTargetTerritory() == nullptr)
        return false;

    if (this->getSourceTerritory()->getOwner()->equals(this->getIssuingPlayer()) && this->getTargetTerritory()->getOwner()->equals(this->getIssuingPlayer())){
        // cout << "VALID AIRLIFT" << endl;
        return true;
    }
    // cout << "INVALID AIRLIFT" << endl;
    return false;
}
void OrdersAirlift::execute(){
    bool isValid = this->validate();
    if (!isValid){
        cout << "Invalid airlift, cannot airlift from " << this->getSourceTerritory()->getName() << 
            " to " << this->getTargetTerritory()->getName() << endl;

    notify(*this);
        Orders* o = this->getIssuingPlayer()->getOrderList()->remove(0);
        delete o;
        return;
    }

    int sourceNumArmies = this->getSourceTerritory()->getNumOfArmies();
    int targetNumArmies = this->getTargetTerritory()->getNumOfArmies();

    this->getSourceTerritory()->setNumOfArmies(sourceNumArmies - this->getNumArmies());
    this->getTargetTerritory()->setNumOfArmies(targetNumArmies + this->getNumArmies());

    notify(*this);
    Orders* o = this->getIssuingPlayer()->getOrderList()->remove(0);
    delete o;
}

OrdersNegotiate::OrdersNegotiate() : Orders(), enemyToTruce(nullptr) {}
OrdersNegotiate::OrdersNegotiate(Player* issuingPlayer, Player* enemyToTruce,Observer* obs) : Orders(issuingPlayer, nullptr, obs), enemyToTruce(enemyToTruce) {}
OrdersNegotiate::OrdersNegotiate(const OrdersNegotiate& order) : Orders(order){
    if (order.getEnemyToTruce())
        // this->enemyToTruce = new Player(*order.enemyToTruce); // might be fine if player class has copy constructor and assignment operator
        this->enemyToTruce = order.enemyToTruce;
}
OrdersNegotiate::~OrdersNegotiate(){}
OrdersNegotiate& OrdersNegotiate::operator=(const OrdersNegotiate& order){
    if (this != &order){
        Orders::operator=(order);
        // delete this->getEnemyToTruce();

        if (order.getEnemyToTruce())
            // this->enemyToTruce = new Player(*order.enemyToTruce); // might be fine if player class has copy constructor and assignment operator
            this->enemyToTruce = order.enemyToTruce;
        else
            this->setEnemyToTruce(nullptr);
    }
    return *this;
}
Player* OrdersNegotiate::getEnemyToTruce() const{
    return this->enemyToTruce;
}
void OrdersNegotiate::setEnemyToTruce(Player* enemyToTruce){
    this->enemyToTruce = enemyToTruce;
}
Orders* OrdersNegotiate::allocateClone() const{
    return new OrdersNegotiate(*this);
}
void OrdersNegotiate::printOrder(std::ostream& strm) const{
    strm << "Player " << this->getIssuingPlayer()->getName() << " is negotiating with Player " << this->getEnemyToTruce()->getName();
}
bool OrdersNegotiate::validate(){
    if (this->getEnemyToTruce() == nullptr || this->getIssuingPlayer() == nullptr)
        return false;

    if (this->getEnemyToTruce()->equals(this->getIssuingPlayer())){
        // cout << "INVALID NEGOTIATION" << endl;
        return false;
    }
    // cout << "VALID NEGOTIATE" << endl;
    return true;
        
}
void OrdersNegotiate::execute(){
    bool isValid = this->validate();
    if (!isValid)
    {
        cout << "Cannot perform a negotiation with self or null player" << endl;

        notify(*this);
        Orders* o = this->getIssuingPlayer()->getOrderList()->remove(0);
        delete o;
        return;
    }
    this->getIssuingPlayer()->addTruce(this->getEnemyToTruce());
    this->getEnemyToTruce()->addTruce(this->getIssuingPlayer());

    notify(*this);
    Orders* o = this->getIssuingPlayer()->getOrderList()->remove(0);
    delete o;
}

OrdersList::OrdersList() {}
OrdersList::OrdersList(Observer* obs)
{
    Subject::addObserver(obs);
}
OrdersList::OrdersList(OrdersList& orderList,Observer* obs){
    for (auto iterator : orderList.list){
        this->list.push_back(iterator->allocateClone());
    }
}
OrdersList& OrdersList::operator=(const OrdersList& orderList){

    this->observer_=orderList.observer_;
    if (this != &orderList){
        for (auto order : this->list)
            delete order;

        this->list.clear();
        
        if (orderList.list.front()){
            for (auto order : orderList.list)
                this->list.push_back(order->allocateClone());
        }
    }  
    return *this;
}
OrdersList::~OrdersList(){
    for (auto order : this->list)
        delete order;
    
    this->list.clear();
}
std::list<Orders*> OrdersList::getList() const{
    return this->list;
}
std::list<Orders*>& OrdersList::getList(){ //non-const version for modification of the orderlist
    return this->list;
}
void OrdersList::setList(std::list<Orders*> list){
    for (auto order : this->list)
        delete order;

    this->list.clear();

    for (auto order : list)
        this->list.push_back(order->allocateClone());
}
Orders* OrdersList::add(Orders* order){
    if (order == nullptr)
        return nullptr;
    this->list.push_back(order);
    notify(*this);
    return order;
}
Orders* OrdersList::remove(const Orders& order){
    auto it = std::find(this->list.begin(), this->list.end(), &order);

    if (it != this->list.end()){
        Orders* temp = *it;   // save pointer first
        this->list.erase(it); // erase iterator
        return temp; 
    }
    cout << "Element not found" << endl;
    return nullptr;
}
Orders* OrdersList::remove(const int index){
    auto it = this->list.begin();
    std::advance(it, index);

    if (it != this->list.end()){
        Orders* temp = *it;   // save pointer first
        this->list.erase(it); // erase iterator
        return temp; 
    }
    cout << "Element not found" << endl;
    return nullptr;

}
Orders* OrdersList::move(const int sourceIndex, const int targetIndex){
    auto firstIterator = this->list.begin();
    std::advance(firstIterator, sourceIndex);

    Orders* movedOrder = *firstIterator;  // save pointer before moving

    auto secondIterator = this->list.begin();
    std::advance(secondIterator, targetIndex);

    this->list.splice(secondIterator, this->list, firstIterator);

    return movedOrder;

}
void Orders::notify(ILoggable& subject)
{
    observer_->update(*this);
};


string OrdersDeploy::stringToLog()
{
    string str="Order Deploy is executed:\n\nPlayer "+this->getIssuingPlayer()->getName()+" is deploying armies to territory "+this->getSourceTerritory()->getName()+"\n\n";
    return str;
};


string OrdersAdvance::stringToLog()
{
    string str="Order Advance is executed:\n\nPlayer "+this->getIssuingPlayer()->getName()+" is advancing armies from territory "+this->getSourceTerritory()->getName()+" to "+ targetTerritory->getName()+"\n\n";
    return str;
};

string OrdersBomb::stringToLog()
{
    string str="Order Bombing is executed:\n\nPlayer "+this->getIssuingPlayer()->getName()+" is bombing territory "+this->getSourceTerritory()->getName()+"\n\n";
    return str;
};

string OrdersBlockade::stringToLog()
{
    string str="Order Blockade is executed:\n\nPlayer "+this->getIssuingPlayer()->getName()+" is blockading  territory "+this->getSourceTerritory()->getName()+"\n\n";
    return str;
};

string OrdersAirlift::stringToLog()
{
    string str="Order Airlift is executed:\n\nPlayer "+this->getIssuingPlayer()->getName()+" is airlifting armies from territory "+this->getSourceTerritory()->getName()+" to "+ targetTerritory->getName()+"\n\n";
    return str;
};
string OrdersNegotiate::stringToLog()
{
    string str="Order Negotiating is executed:\n\nPlayer "+this->getIssuingPlayer()->getName()+" is negotiating with player "+enemyToTruce->getName()+"\n\n";
    return str;
};

string OrdersDeploy::stringTo()
{
    string str="Order Deploy :\n\nPlayer "+this->getIssuingPlayer()->getName()+" is deploying armies to territory "+this->getSourceTerritory()->getName()+"\n\n";
    return str;
};


string OrdersAdvance::stringTo()
{
    string str="Order Advance :\n\nPlayer "+this->getIssuingPlayer()->getName()+" is advancing armies from territory "+this->getSourceTerritory()->getName()+" to "+ targetTerritory->getName()+"\n\n";
    return str;
};

string OrdersBomb::stringTo()
{
    string str="Order Bombing :\n\nPlayer "+this->getIssuingPlayer()->getName()+" is bombing territory "+this->getSourceTerritory()->getName()+"\n\n";
    return str;
};

string OrdersBlockade::stringTo()
{
    string str="Order Blockade :\n\nPlayer "+this->getIssuingPlayer()->getName()+" is blockading  territory "+this->getSourceTerritory()->getName()+"\n\n";
    return str;
};

string OrdersAirlift::stringTo()
{
    string str="Order Airlift :\n\nPlayer "+this->getIssuingPlayer()->getName()+" is airlifting armies from territory "+this->getSourceTerritory()->getName()+" to "+ targetTerritory->getName()+"\n\n";
    return str;
};
string OrdersNegotiate::stringTo()
{
    string str="Order Negotiating :\n\nPlayer "+this->getIssuingPlayer()->getName()+" is negotiating with player "+enemyToTruce->getName()+"\n\n";
    return str;
};

void OrdersList::notify(ILoggable& subject)
{
    observer_->update(*this);
};
string OrdersList::stringToLog()
{
    string str="An order has been added to a player's order list:\n"+list.back()->stringTo()+"\n\n";
    return str;
};