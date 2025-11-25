//
// Created by Howard on 2025-11-16.
//

#ifndef PLAYERSTRATEGY_H
#define PLAYERSTRATEGY_H

#include "../Player/Player.h"
#include <vector>

using std::vector;
using std::string;


class PlayerStrategy {
protected:
    Player* player;

public:
    explicit PlayerStrategy(Player* player);

    virtual ~PlayerStrategy() = default;

    virtual void issueOrder(Deck& deck, int mode, Territory* sourceTerritory, int numArmies, Territory* targetTerritory, Player& player2,Observer* obs) = 0;
    virtual vector<Territory*> toAttack(const std::vector<Territory*>& allTerritories) = 0;
    virtual vector<Territory*> toDefend(const std::vector<Territory*>& allTerritories) = 0;


};

class AggressivePlayerStrategy : public PlayerStrategy {

public:
    explicit AggressivePlayerStrategy(Player* player);

    ~AggressivePlayerStrategy() override = default;

    void issueOrder(Deck& deck, int mode, Territory* sourceTerritory, int numArmies, Territory* targetTerritory, Player& player2,Observer* obs) override;
    vector<Territory*> toAttack(const std::vector<Territory*>& allTerritories) override;
    vector<Territory*> toDefend(const std::vector<Territory*>& allTerritories) override;

};

class HumanPlayerStrategy : public PlayerStrategy {

public:
    explicit HumanPlayerStrategy(Player* player);

    ~HumanPlayerStrategy() override = default;

    void issueOrder(Deck& deck, int mode, Territory* sourceTerritory, int numArmies, Territory* targetTerritory, Player& player2,Observer* obs) override;
    vector<Territory*> toAttack(const std::vector<Territory*>& allTerritories) override;
    vector<Territory*> toDefend(const std::vector<Territory*>& allTerritories) override;

};

class BenevolentPlayerStrategy : public PlayerStrategy {

public:
    explicit BenevolentPlayerStrategy(Player* player);

    ~BenevolentPlayerStrategy() override = default;

    void issueOrder(Deck& deck, int mode, Territory* sourceTerritory, int numArmies, Territory* targetTerritory, Player& player2,Observer* obs) override;
    vector<Territory*> toAttack(const std::vector<Territory*>& allTerritories) override;
    vector<Territory*> toDefend(const std::vector<Territory*>& allTerritories) override;

};


#endif //PLAYERSTRATEGY_H