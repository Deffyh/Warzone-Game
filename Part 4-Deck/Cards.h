
#ifndef CLIONPROJECTS_DECK_H
#define CLIONPROJECTS_DECK_H

#include <list>
#include <string>
#include <vector>
#include "Map.h"
// #include "../Orders/Orders.h"


//4.2.6
class Deck;
class Hand;
class OrdersList;
class Player;
class Territory;

// class Order{
// private:
//     std::string* name;
// public:
//     Order(std::string& name);
//     ~Order();
// };

// class OrderList {
// private:
//     std::list<Order*> olist;
// public:
//     OrderList();
//     ~OrderList();
//     void addOrder(Order* order);
//     void resolveOrder();

// };

class Card {
    //The Card class is being used by both the Hand class and the Deck class
    private:
        std::string* name;  //Name of card 4.2.5


    public:
        Card();
        Card(const std::string& name);
        Card(const Card& other);
        Card& operator=(const Card& other); //4.2.7
        ~Card();

        std::string getName() const;
        void setName(const std::string& n);

        void play(Deck& deck, Hand* hand, OrdersList& olist, Player* player, Territory* territory,                //Mandatory parameters
                    int mode = 3, int numArmies = 0, Territory* target = nullptr, Player* otherPlayer = nullptr); //Optional Parameter

        friend std::ostream& operator<<(std::ostream& os, const Card& c);
};

class Deck {
    //The deck class owns all the card objects. (deep copy)
    private:
    std::vector<Card*> cards;  //Deck content   4.2.1 4.2.5

    public:
        Deck();
        Deck(const std::vector<Card *> &cards);
        Deck(const Deck& other);
        Deck& operator=(const Deck& other); //4.2.7
        ~Deck();

        int getDeckSize() const;

        void addCard(Card* card);


        Card* draw(); //4.2.2, 4.2.9 line 111

        bool isEmpty() const;
        friend std::ostream& operator<<(std::ostream& os, const Deck& d);


};

class Hand {
    //The hand class only borrows cards from the deck (shallow copy)
    private:
        std::string* player;    //Player name (currently a placeholder) 4.2.5

        std::vector<Card*> cards;  //Player hand  4.2.3, 4.2.5

    public:
        Hand();
        Hand(const std::string& name);
        Hand(const Hand& other); //It is here for the instructions, but a Hand should not be replicated since it is unique to every player.
        ~Hand(); //4.2.7
        Hand& operator=(const Hand& other);

        void addCard(Card* card);
        void draw(Deck& deck);
        void playCard(Deck& deck, const std::string& cardName, OrdersList& olist,Player* p, Territory* territory, Player* p2 = nullptr, Territory* territory2 = nullptr, int numArmies =1 ); //temporary for testing
        void removeCard(Card* card);
        void returnAll(Deck& deck);

        bool isEmpty() const;
        friend std::ostream& operator<<(std::ostream& os, const Hand& hand);
};

#endif //CLIONPROJECTS_DECK_H