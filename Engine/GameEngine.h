//
// Created by Howard on 2025-11-03.
//

#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <iostream>
#include <string>
#include <vector>
#include "../Player/Player.h"
#include "../part1-map/Map.h"
#include "../Logging-Observer/LoggingObserver.h"
#include "../CommandProcessor/CommandProcessing.h"
#include "../part1-map/Map.h"

using std::string;
using std::cout;
using std::endl;
using std::cin;



class GameEngine : public Subject, public ILoggable{
    private:
        string state;   //state of the engine
        vector<Player*> players;
        bool gameOver;
        Map* map;
        Deck* deck;

        vector<string> MapsToUse;
        vector<string> StrategiesToUse;
        int numGames;
        int numTurns;
        int counter;

    public:
        explicit GameEngine(Observer* observer); //Default constructor
        GameEngine(const GameEngine& other);
        GameEngine &operator=(const GameEngine &other);
        ~GameEngine() override;

        string getState() const;
        void changeState(const string& newState, const string& message);

        void runGame();
        friend ostream& operator<<(ostream& out, const GameEngine& engine);

        void mainGameLoop();
        void reinforcementPhase();
        void issueOrdersPhase(vector<Player*>& allPlayers , Map* map);
        bool executeOrdersPhase();

        bool checkWinCondition(const std::vector<Player*>& players, Map* map);
        void waitForUser();

        void addPlayer( Player* player);
        void removePlayer(Player* player);
        void addMap(const Map& othermap);
        void giveDeck(Deck* deck);
        void printAllPlayerOrders(const std::vector<Player*>& players);


        string stringToLog() override;
        void notify(ILoggable& subject) override;

        void startupPhase();

        vector<string>& getMapsToUse();
        void setMapsToUse(const vector<string> maps);

        vector<string>& getStrategiesToUse();
        void setStrategiesToUse(const vector<string> strategies);

        int getNumGames();
        void setNumGames(int games);
        vector<vector<std::string>> winnerList;
        int getNumTurns();
        void setNumTurns(int turns);
        void runTournament();
};


#endif //COMP_345_PROJECT_FALL_2025_PART2_GAMEENGINE_H