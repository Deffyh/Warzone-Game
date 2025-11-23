//
// Created by Howard on 2025-11-16.
//

#ifndef COMP_345_PROJECT_FALL_2025_2_PLAYERSTRATEGY_H
#define COMP_345_PROJECT_FALL_2025_2_PLAYERSTRATEGY_H

#include "../Player/Player.h"
#include <vector>
class Player;
class Territory;


enum class StrategyType {
    Human,
    Aggressive,
    Benevolent,
    Neutral,
    Cheater
};

class PlayerStrategy {
protected:
    StrategyType type;
    Player* getPlayer() const;

private:
    Player* player;

public:
    PlayerStrategy();
    PlayerStrategy(Player* player, StrategyType t): player(player), type(t) {};

    PlayerStrategy(const PlayerStrategy& other);
    PlayerStrategy& operator=(const PlayerStrategy& other);
    friend std::ostream& operator<<(std::ostream& out, const PlayerStrategy& ps);



    StrategyType getType() const;
    void setType(StrategyType t);

    virtual ~PlayerStrategy() = default;

    virtual void issueOrder(Deck& deck, int mode, Territory* sourceTerritory, int numArmies, Territory* targetTerritory, Player& player2,Observer* obs) = 0;
    virtual std::vector<Territory*> toAttack(const std::vector<Territory*>& allTerritories) = 0;
    virtual std::vector<Territory*> toDefend(const std::vector<Territory*>& allTerritories) = 0;

};

class HumanPlayerStrategy: public PlayerStrategy {

public:
    HumanPlayerStrategy();
    HumanPlayerStrategy(Player* player): PlayerStrategy(player, StrategyType::Human) {};
    ~HumanPlayerStrategy() = default;

    vector<Territory *> toAttack(const std::vector<Territory *> &allTerritories) override;
    vector<Territory *> toDefend(const std::vector<Territory *> &allTerritories) override;
    void issueOrder(Deck &deck, int mode, Territory *sourceTerritory, int numArmies, Territory *targetTerritory, Player &player2, Observer *obs) override;
};


class AggressivePlayerStrategy: public PlayerStrategy {
public:
    AggressivePlayerStrategy();
    AggressivePlayerStrategy(Player* player): PlayerStrategy(player, StrategyType::Aggressive) {};
    ~AggressivePlayerStrategy() = default;

    vector<Territory *> toAttack(const std::vector<Territory *> &allTerritories) override;
    vector<Territory *> toDefend(const std::vector<Territory *> &allTerritories) override;
    void issueOrder(Deck &deck, int mode, Territory *sourceTerritory, int numArmies, Territory *targetTerritory, Player &player2, Observer *obs) override;
};

class BenevolentPlayerStrategy: public PlayerStrategy {
public:
    BenevolentPlayerStrategy();
    BenevolentPlayerStrategy(Player* player): PlayerStrategy(player, StrategyType::Benevolent) {};
    ~BenevolentPlayerStrategy() = default;

    vector<Territory *> toAttack(const std::vector<Territory *> &allTerritories) override;
    vector<Territory *> toDefend(const std::vector<Territory *> &allTerritories) override;
    void issueOrder(Deck &deck, int mode, Territory *sourceTerritory, int numArmies, Territory *targetTerritory, Player &player2, Observer *obs) override;
};

class NeutralPlayerStrategy: public PlayerStrategy {
private:
    bool isAttacked;

public:
    NeutralPlayerStrategy();
    NeutralPlayerStrategy(Player* player): PlayerStrategy(player, StrategyType::Neutral), isAttacked(false) {};

    ~NeutralPlayerStrategy() = default;

    std::vector<Territory*> toAttack(const std::vector<Territory*>& allTerritories) override;
    std::vector<Territory*> toDefend(const std::vector<Territory*>& allTerritories) override;
    void issueOrder(Deck& deck, int mode, Territory* sourceTerritory, int numArmies, Territory* targetTerritory, Player& player2,Observer* obs) override;

    void attacked();
    void setAttacked();

};

class CheaterPlayerStrategy: public PlayerStrategy {
private:
public:
    CheaterPlayerStrategy();
    CheaterPlayerStrategy(Player* player): PlayerStrategy(player, StrategyType::Cheater) {};
    ~CheaterPlayerStrategy() = default ;


    vector<Territory*> toAttack(const std::vector<Territory*>& allTerritories) override;
    vector<Territory*> toDefend(const std::vector<Territory*>& allTerritories) override;
    void issueOrder(Deck& deck, int mode, Territory* sourceTerritory, int numArmies, Territory* targetTerritory, Player& player2,Observer* obs) override;
};


#endif //COMP_345_PROJECT_FALL_2025_2_PLAYERSTRATEGY_H