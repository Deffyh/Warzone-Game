//
// Created by Howard on 2025-11-16.
//


#include "PlayerStrategy.h"
#include "../Orders/Orders.h"
#include "../Player/Player.h"
#include <algorithm>
#include <unordered_set>


PlayerStrategy::PlayerStrategy(const PlayerStrategy &other) {
    this->player = other.player;
    this->type = other.type;
}

PlayerStrategy &PlayerStrategy::operator=(const PlayerStrategy &other) {
    if (this != &other) {
        this->player = other.player;
        this->type = other.type;
    }
    return *this;
}

ostream & operator<<(ostream &out, const PlayerStrategy &ps) {
    out << "PlayerStrategy(Type=";

    switch (ps.getType()) {
        case StrategyType::Human:      out << "Human"; break;
        case StrategyType::Aggressive: out << "Aggressive"; break;
        case StrategyType::Benevolent: out << "Benevolent"; break;
        case StrategyType::Neutral:    out << "Neutral"; break;
        case StrategyType::Cheater:    out << "Cheater"; break;
    }

    out << ", Player="
        << (ps.getPlayer() ? ps.getPlayer()->getName() : "None")
        << ")";

    return out;
}

PlayerStrategy::PlayerStrategy() {
    this->player = nullptr;
}


StrategyType PlayerStrategy::getType() const {
    return type;
}

Player *PlayerStrategy::getPlayer() const {
    return this->player;
}

void PlayerStrategy::setType(StrategyType t) {
    type = t;
}


void HumanPlayerStrategy::issueOrder(Deck &deck, int mode, Territory *sourceTerritory, int numArmies, Territory *targetTerritory, Player &player2, Observer *obs) {
    switch (mode) {
        case 1: //Deploy
            this->getPlayer()->getOrderList()->add(new OrdersDeploy(this->getPlayer(),sourceTerritory,numArmies,obs));
            this->getPlayer()->removeNumFreeArmies(numArmies);
            break;
        case 2: //Advance
            this->getPlayer()->getOrderList()->add(
                new OrdersAdvance(this->getPlayer(), sourceTerritory, targetTerritory, numArmies, obs));
            //sourceTerritory->setNumOfArmies(sourceTerritory->getNumOfArmies() - numArmies);
            break;
        case 3:
        case 4:
        case 5:
        case 6:
            {
                Card* matchingCard = nullptr;
                for (Card* card : this->getPlayer()->getHand()->getCards())
                {
                    if (mode == 5 && card->getName() == "Airlift")
                    {
                        //sourceTerritory->setNumOfArmies(sourceTerritory->getNumOfArmies() - numArmies);
                    }
                    if ((mode == 3 && card->getName() == "Bomb") ||
                        (mode == 4 && card->getName() == "Blockade") ||
                        (mode == 5 && card->getName() == "Airlift") ||
                        (mode == 6 && card->getName() == "Diplomacy"))
                    {
                        matchingCard = card;
                        break;
                    }
            }
            if (!matchingCard) {
                cout << this->getPlayer()->getName() << " doesn't have the card required for this order." << endl;
                return;
            }
            OrdersList* olist = this->getPlayer()->getOrderList();

            matchingCard->play(deck, this->getPlayer()->getHand(), *olist, this->getPlayer(), sourceTerritory,mode,numArmies,targetTerritory,&player2, obs); //dereference problem
            cout << this->getPlayer()->getName() << " played a " << matchingCard->getName() << " card." << endl;
            this->getPlayer()->removeNumFreeArmies(numArmies);
            break;

        }
        default:
            cout << "Invalid order mode." << endl;

    }
}


vector<Territory*> HumanPlayerStrategy::toDefend(const std::vector<Territory *> &allTerritories) {
    return this->getPlayer()->getTerritories();
}

vector<Territory*> HumanPlayerStrategy::toAttack(const std::vector<Territory *> &allTerritories) {
    std::vector<Territory*> attackList;
    // Build quick lookup of territories this player owns (by pointer and by name)
    std::unordered_set<Territory*> ownedPtrs;
    unordered_set<std::string> ownedNames;
    for (Territory* t : this->getPlayer()->getTerritories()) {
        if (t) {
            ownedPtrs.insert(t);
            ownedNames.insert(t->getName());
        }
    }

    for (Territory* myT : this->getPlayer()->getTerritories()) {
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


//empty for now
void AggressivePlayerStrategy::issueOrder(Deck &deck, int mode, Territory *sourceTerritory, int numArmies, Territory *targetTerritory, Player &player2, Observer *obs) {
    switch (mode) {
    case 1: //Deploy
        this->getPlayer()->getOrderList()->add(new OrdersDeploy(this->getPlayer(),sourceTerritory,numArmies,obs));
        this->getPlayer()->removeNumFreeArmies(numArmies);
        if (sourceTerritory->getNumOfArmies()<0)
        {
            cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl<<"TRIED TO DEPLOY TO "<<targetTerritory->getName()<<" OWNER : "<<targetTerritory->getOwner()->getName()<<" FROM "<<sourceTerritory->getName()<<" OWNER : "<<targetTerritory->getOwner()->getName()<<" ARMIES IN SOURCE"<<sourceTerritory->getNumOfArmies()<<endl<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl<<endl;
        }
        break;
    case 2: //Advance
        this->getPlayer()->getOrderList()->add(new OrdersAdvance(this->getPlayer(),sourceTerritory,targetTerritory, numArmies,obs));
        // sourceTerritory->setNumOfArmies(sourceTerritory->getNumOfArmies()-numArmies);
        // if (sourceTerritory->getNumOfArmies()<0)
        // {
        //     cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl<<"TRIED TO ADVANCE TO "<<targetTerritory->getName()<<" OWNER : "<<targetTerritory->getOwner()->getName()<<" FROM "<<sourceTerritory->getName()<<" OWNER : "<<targetTerritory->getOwner()->getName()<<" ARMIES IN SOURCE"<<sourceTerritory->getNumOfArmies()<<endl<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl<<endl;
        // }
        break;
    case 3:
    case 4:
    case 5:
    case 6: {
            Card* matchingCard = nullptr;
            for (Card* card: this->getPlayer()->getHand()->getCards()) {
                if (mode == 5 && card->getName() == "Airlift")
                {
                    //sourceTerritory->setNumOfArmies(sourceTerritory->getNumOfArmies() - numArmies);
                }
                if ((mode == 3 && card->getName() == "Bomb") ||
                    (mode == 4 && card->getName() == "Blockade") ||
                    (mode == 5 && card->getName() == "Airlift") ||
                    (mode == 6 && card->getName() == "Diplomacy")) {
                    matchingCard = card;
                    break;
                    }
            }
            if (!matchingCard) {
                cout <<this->getPlayer()->getName() << " doesn't have the card required for this order." << endl;
                return;
            }
            OrdersList* olist = this->getPlayer()->getOrderList();

            matchingCard->play(deck, this->getPlayer()->getHand(), *olist, this->getPlayer(), sourceTerritory,mode,numArmies,targetTerritory,&player2, obs);
            this->getPlayer()->removeNumFreeArmies(this->getPlayer()->getNumFreeArmies()-numArmies);
            cout <<this->getPlayer()->getName() << " played a " << matchingCard->getName() << " card." << endl;
            break;

    }
    default:
        cout << "Invalid order mode." << endl;

    }
}


vector<Territory*> AggressivePlayerStrategy::toDefend(const std::vector<Territory *> &allTerritories) {

    vector<Territory*> territories = this->getPlayer()->getTerritories();
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
                if (cand->getOwner() !=this->getPlayer()) ++threatCount;
            }
        }
        infos.push_back({t, threatCount, t->getNumOfArmies()});
    }

    // Sort by descending threat (border territories first), then ascending armies (weaker first)
    std::sort(infos.begin(), infos.end(), [](const Info& a, const Info& b) {
        if (a.threat != b.threat) return a.threat > b.threat;
        return a.armies < b.armies;
    });
    //the strongest terr is returned
    defendList.push_back(infos.back().t);
    //
    // cout<<"+++++++++++++++++++++++++++++++++++++++"<<endl<<"DEFEND AGGRESSIVE"<<endl<<"+++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl;
    // cout<<*defendList.front()<<endl;
    // cout<<"+++++++++++++++++++++++++++++++++++++++"<<endl<<endl<<endl;

    return defendList;
}

vector<Territory*> AggressivePlayerStrategy::toAttack(const std::vector<Territory *> &allTerritories) {

    vector<Territory*> territories = this->getPlayer()->getTerritories();
    std::vector<Territory*> defendList;
    std::vector<Territory*> attackList;

    // Build quick lookup of territories this player owns (by pointer and by name)
    unordered_set<Territory*> ownedPtrs;
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

    vector<Territory*> list;

    // sort by ascending defending armies (easier targets first)
    std::sort(attackList.begin(), attackList.end(),
    [](Territory* a, Territory* b) { return a->getNumOfArmies() < b->getNumOfArmies(); });

    //added for the aggressive player
    for (Territory* t: attackList)  //loops through every enemy terr the aggressivePlayer could attack
    {
        //checking every adjacent territory name of the player’s strongest owned terr
        for (const string& n: this->getPlayer()->toDefend(territories).front()->getAdjTerritoriesNames())
        {
            //if the enemy terr’s name matches one of those adjacent names, enemy terr could be attacked from the strongest terr
            if (t->getName()==n)
            {
                list.push_back(t);
            }
        }
    }

    return list;
}

void BenevolentPlayerStrategy::issueOrder(Deck &deck, int mode, Territory *sourceTerritory, int numArmies, Territory *targetTerritory, Player &player2, Observer *obs) {
         switch (mode) {
        case 1: //Deploy
            this->getPlayer()->getOrderList()->add(new OrdersDeploy(this->getPlayer(),sourceTerritory,numArmies,obs));
           this->getPlayer()->removeNumFreeArmies(numArmies);
            break;
        case 2: //Advance
            this->getPlayer()->getOrderList()->add(
                new OrdersAdvance(this->getPlayer(), sourceTerritory, targetTerritory, numArmies, obs));
            //sourceTerritory->setNumOfArmies(sourceTerritory->getNumOfArmies() - numArmies);

            break;
        case 3:
        case 4:
        case 5:
        case 6:
            {
                Card* matchingCard = nullptr;
                for (Card* card : this->getPlayer()->getHand()->getCards())
                {
                    if (mode == 5 && card->getName() == "Airlift")
                    {
                        //sourceTerritory->setNumOfArmies(sourceTerritory->getNumOfArmies() - numArmies);
                    }
                    if ((mode == 3 && card->getName() == "Bomb") ||
                        (mode == 4 && card->getName() == "Blockade") ||
                        (mode == 5 && card->getName() == "Airlift") ||
                        (mode == 6 && card->getName() == "Diplomacy"))
                    {
                        matchingCard = card;
                        break;
                    }
            }
            if (!matchingCard) {
                cout <<this->getPlayer()->getName() << " doesn't have the card required for this order." << endl;
                return;
            }
            OrdersList* olist = this->getPlayer()->getOrderList();

            matchingCard->play(deck, this->getPlayer()->getHand(), *olist, this->getPlayer(), sourceTerritory,mode,numArmies,targetTerritory,&player2, obs);
            this->getPlayer()->removeNumFreeArmies(this->getPlayer()->getNumFreeArmies()-numArmies);
            cout <<this->getPlayer()->getName() << " played a " << matchingCard->getName() << " card." << endl;
            break;

        }
        default:
            cout << "Invalid order mode." << endl;

    }
}


vector<Territory*> BenevolentPlayerStrategy::toDefend(const std::vector<Territory *> &allTerritories) {

    vector<Territory*> defendList;
    struct Info { Territory* t; int threat; int armies; };

    std::vector<Info> infos;
    for (Territory* t : this->getPlayer()->getTerritories()) {
        if (!t) continue;
        int threatCount = 0;
        for (const std::string& adjName : t->getAdjTerritoriesNames()) {
            auto it = std::find_if(allTerritories.begin(), allTerritories.end(),
                                   [&](Territory* cand) { return cand && cand->getName() == adjName; });
            if (it != allTerritories.end()) {
                Territory* cand = *it;
                if (cand->getOwner() != this->getPlayer()) ++threatCount;
            }
        }
        infos.push_back({t, threatCount, t->getNumOfArmies()});
    }

    // Sort by descending threat (border territories first), then ascending armies (weaker first)
    std::sort(infos.begin(), infos.end(), [](const Info& a, const Info& b) {
        if (a.threat != b.threat) return a.threat > b.threat;
        return a.armies < b.armies;
    });

    if (!infos.empty()) {
        defendList.push_back(infos.front().t);  //adding the weakest/highest threat territory to the vector
    }
    return defendList;
}

vector<Territory*> BenevolentPlayerStrategy::toAttack(const std::vector<Territory *> &allTerritories) {
    return {};
}

NeutralPlayerStrategy::NeutralPlayerStrategy() {
    isAttacked = false;
}

vector<Territory *> NeutralPlayerStrategy::toAttack(const std::vector<Territory *> &allTerritories) {
    //Do nothing since it wont attack anything
    return {};
}
vector<Territory *> NeutralPlayerStrategy::toDefend(const std::vector<Territory *> &allTerritories) {

    return this->getPlayer()->getTerritories();
}
void NeutralPlayerStrategy::issueOrder(Deck &deck, int mode, Territory *sourceTerritory, int numArmies, Territory *targetTerritory, Player &player2, Observer *obs) {
    //Skipping since not issuing orders
}

void NeutralPlayerStrategy::attacked() {
    if (isAttacked){
        cout << this->getPlayer()->getName() << " is now Aggressive." << endl;

        Player* player = this->getPlayer();
        player->setPlayerStrategy(new AggressivePlayerStrategy(player));
    }
}

void NeutralPlayerStrategy::setAttacked() {
    isAttacked = true;
}

vector<Territory*> CheaterPlayerStrategy::toDefend(const std::vector<Territory*>& allTerritories) {                                                                            //Require changes to work as intended

    return this->getPlayer()->getTerritories();

}

vector<Territory *> CheaterPlayerStrategy::toAttack(const std::vector<Territory *> &allTerritories) {
    //Auto Capture
    for (Territory* myTerr : this->getPlayer()->getTerritories()) {
        for (const std::string& adjName : myTerr->getAdjTerritoriesNames()) {
            Territory* adj = nullptr;

            for (Territory* t: allTerritories) {
                if (t->getName() == adjName && t->getOwner() != this->getPlayer()) {
                    adj = t;
                    t->getOwner()->removeTerritory(t);

                }

            }
            if (adj && adj->getOwner() != this->getPlayer()) {
                adj->setOwner(this->getPlayer());
                this->getPlayer()->addTerritory(adj);
                cout << this->getPlayer()->getName() << " has captured " << adj->getName() << " through cheats!" << endl;
            }
        }
    }



    return {};


}

void CheaterPlayerStrategy::issueOrder(Deck &deck, int mode, Territory *sourceTerritory, int numArmies, Territory *targetTerritory, Player &player2, Observer *obs) {
    //Does not actually issue orders
}


