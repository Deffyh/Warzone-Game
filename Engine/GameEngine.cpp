//
// Created by Howard on 2025-11-03.
//
#include "GameEngine.h"
#include <iostream>
#include <limits>
#include <algorithm>
#include <random>
#include "../Logging-Observer/LoggingObserver.h"
#include "../CommandProcessor/CommandProcessing.h"
#include "../Orders/Orders.h"
#include "../Part-4-Deck/Cards.h"
#include "../part1-map/Map.h"
#include <sstream>
#include "../PlayerStrategy/PlayerStrategy.h"

using std::string;
using std::cout;
using std::endl;
using std::cin;

static inline std::string trim_copy(std::string s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch){ return !std::isspace(ch); }));
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch){ return !std::isspace(ch); }).base(), s.end());
    return s;
}
// returns true if 'line' starts with 'verb' (case-sensitive) followed by either space or end
static inline bool starts_with_verb(const std::string &line, const std::string &verb) {
    if (line.size() < verb.size()) return false;
    if (line.rfind(verb, 0) != 0) return false;
    if (line.size() == verb.size()) return true;            // exact match
    return std::isspace(static_cast<unsigned char>(line[verb.size()])); // followed by space
}
// returns the trimmed remainder after the verb, or empty string if none
static inline std::string arg_after(const std::string &line, const std::string &verb) {
    if (!starts_with_verb(line, verb)) return "";
    std::string rest = line.substr(verb.size());
    return trim_copy(rest);
}

GameEngine::GameEngine(Observer* observer) : state("start"), gameOver(false),map(nullptr), deck(nullptr), numGames(0), numTurns(0)
{
    Subject::addObserver(observer);
    GameEngine::notify(*this);
    counter = 0;
}

GameEngine::GameEngine(const GameEngine& other)  : Subject(other) {
    this->state = other.state;
    this->gameOver = other.gameOver;


    for (Player* player: other.players) {
        this->players.push_back(new Player(*player));
    }
    for (string map: other.MapsToUse) {
        this->MapsToUse.push_back(map);
    }
    for (string strategy: other.StrategiesToUse) {
        this->StrategiesToUse.push_back(strategy);
    }

    this->map = new Map(*other.map);
    this->deck = new Deck(*other.deck);
    this->numGames = other.numGames;
    this->numTurns = other.numTurns;

};
GameEngine& GameEngine::operator=(const GameEngine& other) {
    if (this != &other) {
        this->state = other.state;
        this->gameOver = other.gameOver;
        this->observer_ = other.observer_;
        this->counter = other.counter;

        for (Player* player: players) {
            delete player;
        }
        players.clear();
        MapsToUse.clear();
        StrategiesToUse.clear();
        delete map;
        for (const Player* player: other.players) {
            this->players.push_back(new Player(*player));
        }
        for (string map: other.MapsToUse) {
            this->MapsToUse.push_back(map);
        }
        for (string strategy: other.StrategiesToUse) {
            this->StrategiesToUse.push_back(strategy);
        }

        this->map = new Map(*other.map);
    }
    return *this;
}
GameEngine::~GameEngine() {
    delete map;
    //for (auto* player: players) {
    //    delete player;
    //}
    delete deck;
}

string GameEngine::stringToLog()
{
    string str="The state of the game has changed!\nState:\t"+state+"\n\n";
    return str;
}

void GameEngine::notify(ILoggable& subject)
{
    Subject::observer_->update(subject);
}

string GameEngine::getState() const {
    return state;
}
void GameEngine::changeState(const string& newState, const string& message) {
    state = newState;
    cout << message << endl;
    cout << "Now in state: " << state << endl<<endl;
    notify(*this);
}

void GameEngine::runTournament() {
    MapLoader mapLoader;
    //========
    mapCounter = 0;
    winnerList = vector<vector<string>>(getMapsToUse().size());
    for (int n = 0; n < getMapsToUse().size(); n++) {
            //dont forget to del
        for (int i = 0; i < getNumGames(); i++) {
            //set map
            counter = 0;
            this->map = new Map(mapLoader.loadMap(getMapsToUse()[n]));
            //debug purpose
            cout << endl;
            cout << "New Tournament game has begun!" << endl;
            cout << endl;

            //set players next line (ask Howard)

            for (const string& ps: getStrategiesToUse()) {
                if (ps == "Human") {
                    this->addPlayer(new Player("Human Player",observer_,StrategyType::Human));
                }
                else if (ps == "Aggressive") {
                    this->addPlayer(new Player("Aggressive Player",observer_,StrategyType::Aggressive));
                }
                else if (ps == "Benevolent") {
                    this->addPlayer(new Player("Benevolent Player",observer_,StrategyType::Benevolent));
                }
                else if (ps == "Neutral") {
                    this->addPlayer(new Player("Neutral Player",observer_,StrategyType::Neutral));
                }
                else if (ps == "Cheater") {
                    this->addPlayer(new Player("Cheater Player",observer_,StrategyType::Cheater));
                }
            }

            // deck + terr dist. + 50 uofa
            int numPlayer = static_cast<int>(this->players.size());
            int numTerr = static_cast<int>(this->map->getTerritories().size());
            int numCards = numPlayer * 5;
            int randomIndex;
            bool unowned;


            Deck* deckTemp = new Deck();
            this->deck = deckTemp;
            //cout<<this->deck->getDeckSize()<<endl;
            //delete deckTemp;

            //Add 5 * numberOfPlayer copies of each card type
            vector<string> cardNames = {"Bomb", "Airlift", "Negotiate", "Blockade"};
            for (const auto& name : cardNames) {
                for (int i = 0; i < numCards; ++i) {
                    deck->addCard(new Card(name));
                }
            }

            //this->addPlayer(Player("Neutral Player"));
            int territoryPerPlayer = numTerr / numPlayer;
            int terrLeft = numTerr % numPlayer;

            std::random_device rd;
            std::mt19937 g(rd());

            // Shuffle the vector of players to randomize the order
            shuffle(this->players.begin(), this->players.end(), g);

            for (Player* player : this->players)
            {
                //assign the correct number of territories per player
                for (int i = 0 ;i<territoryPerPlayer;i++)
                {
                    unowned=true;

                    //choose random number corresponds to index to territory
                    //continue to randomize the territory until an unowned territory is found
                    while (unowned){
                        randomIndex= rand() % numTerr;
                        if (map->getTerritories()[randomIndex]->getOwner()==nullptr)
                        {
                            map->getTerritories()[randomIndex]->setOwner(player);
                            player->addTerritory(map->getTerritories()[randomIndex]);
                            unowned=false;
                        }
                    }
                }





                //give every player 50 army unit
                player->addNumArmies(50);
                //make the player draw a card twice
                player->getHand()->draw(*this->deck);
                player->getHand()->draw(*this->deck);

            }

            if (terrLeft != 0)
            {
                for (Territory* territory : this->map->getTerritories())
                {
                    if (territory->getOwner() == nullptr)
                    {
                        //setting the owner as the last player added to the vector (neutral player)
                        territory->setOwner(this->players.back());
                    }
                }
            }



            mainGameLoop();

        //clear players and map for next game
            for (Player* player : this->players) {
                this->removePlayer(player);
            }
            delete map;
           // map = nullptr;

         // deck gets auto deleted
        }
        mapCounter++;
    }
   logWinners(winnerList, numGames, getMapsToUse().size());
}

void GameEngine::startupPhase()
{
    cout << "Welcome to the Game Engine!" << endl << endl;
    cout << "---------------------" << endl;

    int choice;
    bool invalidInput = false;
    gameOver = false;

    while (!invalidInput){
        cout << "Start by choosing whether you want to start the game set up through (1) Console, or through a (2) Text file"<<endl;
        cin >> choice;

        switch(choice)
        {
        case 1:
            {
                auto* console= new CommandProcessor(this->observer_);
                CommandProcessor::engine = this;
                Command* command;
                string input;
                string filename;

                while (!gameOver)
                {
                    command = console->getCommand(this->state);
                    //if the command is invalid, the prompt will be run until a valid command is entered
                    while (command==nullptr){
                        command = console->getCommand(this->state);
                    }

                    input=command->getCommand();
                    //compares command input and gamestate
                    if (input == "exit") { //quick exit
                        gameOver = true;
                        delete console;
                    }
                    else if (starts_with_verb(input, "loadmap") && (state == "start" || state == "maploaded")) {

                        filename = arg_after(input, "loadmap");
                        if (filename.empty()) {
                            cout << "Missing filename for loadmap\n";
                        } else {
                            MapLoader maploader;
                            Map testCase = maploader.loadMap(filename);
                            this->map = new Map(testCase);
                            command->saveEffect();
                            string message = "Map "+ filename+" is now loaded!\n(Type 'validatemap' to proceed)\n\n";
                            changeState("maploaded", message);
                        }
                    }
                    else if (input == "validatemap" && state == "maploaded") {

                        if(this->map->validate())
                        {
                            command->saveEffect();
                            string message = "Map "+ filename+" is now validated!\n(Type 'addplayer' to proceed)\n\n";
                            changeState("mapvalidated", message);
                        }else cout<<"Map validation failed! Please load another map again!"<<endl;
                    }
                    else if (starts_with_verb(input,"addplayer") && (state == "mapvalidated" || state == "playersadded")) {
                        string name;
                        name = arg_after(input,"addplayer");
                        if (name.empty()) {
                            cout << "Missing player name for addplayer\n";
                        } else {
                            this->addPlayer(new Player(name, observer_, StrategyType::Human)); //For now testing
                            command->saveEffect();
                            string message = "Player "+ name+" is now added!\n(Type 'gamestart' to proceed)\n\n";
                            changeState("playersadded", message);
                        }
                    }
                    else if (input == "gamestart" && state == "playersadded") {

                        command->saveEffect();
                        changeState("maingameloop", "Game loop has begun (press 'enter' to proceed)\n\n");

                        int numPlayer = static_cast<int>(this->players.size());
                        int numTerr = static_cast<int>(this->map->getTerritories().size());
                        int numCards = numPlayer * 5;
                        int randomIndex;
                        bool unowned;

                        Deck* deckTemp = new Deck();
                        this->deck = deckTemp;
                        cout<<this->deck->getDeckSize()<<endl;
                        //delete deckTemp;

                        //Add 5 * numberOfPlayer copies of each card type
                        vector<string> cardNames = {"Bomb", "Airlift", "Negotiate", "Blockade"};
                        for (const auto& name : cardNames) {
                            for (int i = 0; i < numCards; ++i) {
                                deck->addCard(new Card(name));
                            }
                        }

                        //this->addPlayer(Player("Neutral Player"));
                        int territoryPerPlayer = numTerr / numPlayer;
                        int terrLeft = numTerr % numPlayer;

                        std::random_device rd;
                        std::mt19937 g(rd());

                        // Shuffle the vector of players to randomize the order
                        shuffle(this->players.begin(), this->players.end(), g);

                        for (Player* player : this->players)
                        {
                            //assign the correct number of territories per player
                            for (int i = 0 ;i<territoryPerPlayer;i++)
                            {
                                unowned=true;

                                //choose random number corresponds to index to territory
                                //continue to randomize the territory until an unowned territory is found
                                while (unowned){
                                    randomIndex= rand() % numTerr;
                                    if (map->getTerritories()[randomIndex]->getOwner()==nullptr)
                                    {
                                        map->getTerritories()[randomIndex]->setOwner(player);
                                        player->addTerritory(map->getTerritories()[randomIndex]);
                                        unowned=false;
                                    }
                                }
                            }
                            //give every player 50 army unit
                            player->addNumArmies(50);
                            //make the player draw a card twice
                            player->getHand()->draw(*this->deck);
                            player->getHand()->draw(*this->deck);

                        }

                        cout<<"All players have now each received 50 units of armies, 2 cards and "<<territoryPerPlayer<<" territories!"<<endl;

                        //PART TO ADD WHEN MERGING WITH SHAWN'S PART, PART4 ITERATION 2
                         //adding the neutral player
                         //OrdersBlockade::neutralPlayer=this->players.back();
                        //this->addPlayer(Player("Neutral Player"));

                         //if there are leftover territories then they are given to the neutral player
                         if (terrLeft != 0)
                         {
                             for (Territory* territory : this->map->getTerritories())
                             {
                                 if (territory->getOwner() == nullptr)
                                 {
                                     //setting the owner as the last player added to the vector (neutral player)
                                     territory->setOwner(this->players.back());
                                 }
                             }
                         }

                        mainGameLoop();
                    }
                    else if ((input == "replay" || input == "quit") && state == "win") {
                        if (input == "replay") {
                            state = "start";
                            this->players.clear();
                            //delete map;
                            //delete deck;
                            command->saveEffect();
                        }
                        else {
                            gameOver = true;
                            cout << "Exiting game." << endl;
                            command->saveEffect();
                            delete console;
                            break;
                        }
                    }
                    else if (input == "tournament" && state == "start"){
                        cout << "START TOURNAMENT" << endl;

                        runTournament();
                        // Enter tournament logic or tournament function call here

                    }
                }

                invalidInput = true;
                break;
            }
        case 2:
            {
                cout << "You have chosen to use a text file for the startup phase!\nEnter the file name in the following format filename.txt :\n"<<endl;
                string filename;
                cin >> filename;
                auto* textFile= new FileCommandProcessorAdapter(this->observer_, filename);
                Command* command;
                string input;
                while (!gameOver)
                {
                    command = textFile->getCommand(this->state);
                    //if the command is invalid, the prompt will be run until a valid command is entered
                    while (command==nullptr){
                        command = textFile->getCommand(this->state);
                    }
                    input=command->getCommand();
                    //compares command input and gamestate
                    if (input == "exit") { //quick exit
                        gameOver = true;
                        delete textFile;
                    }
                    else if (starts_with_verb(input, "loadmap") && (state == "start" || state == "maploaded")) {

                        filename = arg_after(input, "loadmap");
                        if (filename.empty()) {
                            cout << "Missing filename for loadmap\n";
                        } else {
                            MapLoader maploader;
                            Map testCase = maploader.loadMap(filename);
                            this->map = new Map(testCase);
                            command->saveEffect();
                            string message = "Map "+ filename+" is now loaded!\n(Type 'validatemap' to proceed)\n\n";
                            changeState("maploaded", message);
                        }

                    }
                    else if (input == "validatemap" && state == "maploaded") {

                        if(this->map->validate())
                        {
                            command->saveEffect();
                            string message = "Map "+ filename+" is now validated!\n(Type 'addplayer' to proceed)\n\n";
                            changeState("mapvalidated", message);
                        }else cout<<"Map validation failed! Please load another map again!"<<endl;
                    }
                    else if (starts_with_verb(input,"addplayer") && (state == "mapvalidated" || state == "playersadded")) {
                        string name;
                        name = arg_after(input,"addplayer");
                        if (name.empty()) {
                            cout << "Missing player name for addplayer\n";
                        } else {
                            if (this->players.size() == 1) {
                                this->addPlayer(new Player(name, observer_, StrategyType::Cheater));  //For now testing
                            }
                            else {
                                this->addPlayer(new Player(name, observer_, StrategyType::Neutral));  //For now testing
                            }

                            command->saveEffect();
                            string message = "Player "+ name+" is now added!\n(Type 'gamestart' to proceed)\n\n";
                            changeState("playersadded", message);
                        }
                    }
                    else if (input == "gamestart" && state == "playersadded") {

                        command->saveEffect();
                        changeState("maingameloop", "Game loop has begun (press 'enter' to proceed)\n\n");

                        int numPlayer = static_cast<int>(this->players.size());
                        int numTerr = static_cast<int>(this->map->getTerritories().size());
                        int numCards = numPlayer * 5;
                        int randomIndex;
                        bool unowned;

                        Deck* deckTemp = new Deck();
                        this->deck = deckTemp;

                        //Add 5 * numberOfPlayer copies of each card type
                        vector<string> cardNames = {"Bomb", "Airlift", "Negotiate", "Blockade"};
                        for (const auto& name : cardNames) {
                            for (int i = 0; i < numCards; ++i) {
                                deck->addCard(new Card(name));
                            }
                        }

                        //this->addPlayer(Player("Neutral Player"));
                        int territoryPerPlayer = numTerr / numPlayer;
                        int terrLeft = numTerr % numPlayer;

                        std::random_device rd;
                        std::mt19937 g(rd());

                        // Shuffle the vector of players to randomize the order
                        shuffle(this->players.begin(), this->players.end(), g);

                        for (Player* player : this->players)
                        {
                            //assign the correct number of territories per player
                            for (int i = 0;i<territoryPerPlayer;i++)
                            {
                                unowned=true;

                                //choose random number corresponds to index to territory
                                //continue to randomize the territory until an unowned territory is found
                                while (unowned){
                                    randomIndex= rand() % numTerr;
                                    if (map->getTerritories()[randomIndex]->getOwner()==nullptr)
                                    {
                                        map->getTerritories()[randomIndex]->setOwner(player);
                                        player->addTerritory(map->getTerritories()[randomIndex]);
                                        unowned=false;
                                    }
                                }
                            }
                            //give every player 50 army unit
                            player->addNumArmies(50);
                            //make the player draw a card twice
                            player->getHand()->draw(*this->deck);
                            player->getHand()->draw(*this->deck);

                        }

                        cout<<"\nAll players have now each received 50 units of armies, 2 cards and "<<territoryPerPlayer<<" territories!"<<endl<<endl;

                        //PART TO ADD WHEN MERGING WITH SHAWN'S PART, PART4 ITERATION 2
                         //adding the neutral player
                         //OrdersBlockade::neutralPlayer=this->players.back();

                         //if there are leftover territories then they are given to the neutral player
                        //this->addPlayer(Player("Neutral Player"));
                         if (terrLeft != 0)
                         {
                             for (Territory* territory : this->map->getTerritories())
                             {
                                 if (territory->getOwner() == nullptr)
                                 {
                                     //setting the owner as the last player added to the vector (neutral player)
                                     territory->setOwner(this->players.back());
                                 }
                             }
                         }

                        mainGameLoop();
                    }
                    else if ((input == "replay" || input == "quit") && state == "win") {
                        if (input == "replay") {
                            state = "start";
                            this->players.clear();
                            //delete map;
                            //delete deck;
                            command->saveEffect();
                        }
                        else {
                            gameOver = true;
                            cout << "Exiting game." << endl;
                            command->saveEffect();
                            delete textFile;
                            break;
                        }
                    }

                }
                invalidInput = true;
                break;
            }
        default:
            cout << "Invalid input!" << endl;
        }
    }
}

void GameEngine::runGame() {


    startupPhase();

}

ostream& operator<<(ostream& out, const GameEngine& engine) {
    out << "GameEngine State: " << engine.state << "\n"
        << "Game Over: " << (engine.gameOver ? "Yes" : "No") << "\n"
        << "Players Count: " << engine.players.size() << "\n"
        << "Map: " << (engine.map ? "Loaded" : "None") << "\n";
    return out;
}
//==================================
//Part 2
void GameEngine::mainGameLoop() {
    bool roundOver = false; //gameloop check
    while (!roundOver) {
        reinforcementPhase();

        if (counter != numTurns) {
            issueOrdersPhase(players,map);
            printAllPlayerOrders(players);
            if (executeOrdersPhase()) {
                roundOver = true;

            }
            counter++;
            //add flag to draw one card if a territory was conquered
            for (Player* player : players) {
                if (player->getEarnedCard()) {
                    cout << endl;
                    player->addCard(deck->draw());
                    player->setEarnedCard(false);
                }
            }

        }
        else {
            //end game in a draw
            cout << "Turn limit reached, game ends in a draw." << endl;
            winnerList[mapCounter].push_back("Draw");
            roundOver = true;
        }
    }
}

void GameEngine::reinforcementPhase() {
    changeState("reinforcement", "Reinforcement Phase.");

    if (!map) { //if map is missing skip
        cout << "No map loaded, skipping reinforcements." << endl;
        return;
    }

    // take continents by value (avoid binding to a temporary reference)
    const auto& continents = map->getContinents();
    cout << "\n=== Reinforcement Phase Debug ===" << endl;

    for (Player* p : players) {
        if (!p||p->getName()=="Neutral Player") continue;//skip if missing

        int startArmies = p->getNumArmies(); //before reinforcement
        int territoryCount = static_cast<int>(p->getTerritories().size());

        // base reinforcement: floor(#territories / 3), minimum 3
        int baseReinforcements = territoryCount / 3;
        if (baseReinforcements < 3) baseReinforcements = 3;

        // continent control bonuses
        int continentBonus = 0;
        std::vector<std::string> ownedContinents;
        for (Continent* cont : continents) {
            if (!cont) continue;

            const auto& contTerritories = cont->getTerritories();
            if (contTerritories.empty()) continue;

            bool ownsAll = true;
            for (Territory* t : contTerritories) {
                string TerritoryName = t->getName();

                vector<Territory*> Territories = map->getTerritories();
                Territory* found;
                for (Territory* terr : Territories) {
                    if (terr->getName() == TerritoryName) {
                        found = terr;
                    }

                }
                if (!found->getOwner() || (found->getOwner() != p && found->getOwner()->getName() != p->getName())) {
                    ownsAll = false;
                    break;
                }
            }

            if (ownsAll) {
                int bonus = cont->getPointsToConquer();
                continentBonus += bonus;
                ownedContinents.push_back(cont->getName() + " (+" + std::to_string(bonus) + ")");
            }
        }
        int totalReinforcement = baseReinforcements + continentBonus;
        p->addNumArmies(totalReinforcement);

        // Debug output before applying
        cout << "\nPlayer: " << p->getName()
             << " | Territories: " << territoryCount
             << " | Start armies: " << startArmies << endl;
        cout << "  Base from territories: " << baseReinforcements << endl;
        cout << "  Continent bonus total: " << continentBonus << endl;
        cout << "  New army pool: " << p->getNumArmies() << endl;
    }


    cout << "=== End Reinforcement Debug ===\n" << endl;

}




void GameEngine::issueOrdersPhase(vector<Player*>& players , Map* map) {

    changeState("issueorders", "Issue Orders Phase.");

    bool allDone = false;
    vector<bool> playerDone(players.size(),false); //needed for player skips

    while (!allDone) {
        allDone = true; //assumes no action taken by player until proven otherwise

        for (size_t i = 0; i < players.size(); i++) {
            //current turn player
            Player* player = players[i];
            if (player && player->getName() == "Neutral Player") {
                playerDone[i] = true;
                continue;
            }
            if (playerDone[i]) { //player skips check
                continue;
            }

            //Part 3
            if (player->getPlayerStrategy()->getType() != StrategyType::Human) {
                //Will need more specific implementation for computer players
                switch (player->getPlayerStrategy()->getType()) {
                    case StrategyType::Aggressive: {
                        //logic
                        cout << "Aggressive Player acted." << endl;
                        //flow statement to stop the computer from doing nothing if nothing else is left to do
                        playerDone[i] = true;
                        break;
                    }
                    case StrategyType::Benevolent: {
                        //logic
                        cout << "Benevolent Player acted." << endl;
                        //flow statement to stop the computer from doing nothing if nothing else is left to do
                        playerDone[i] = true;
                        break;
                    }
                    case StrategyType::Neutral: {
                        cout << "\n" << player->getName() << " territories to defend: " << endl;
                        for (auto* t : player->toDefend(map->getTerritories())) cout << "  - " << t->getName() << endl;

                        cout << "Skipping the Neutral Player" << endl;
                        playerDone[i] = true;
                        break;
                    }
                    case StrategyType::Cheater: {

                        player->toAttack(map->getTerritories());

                        cout << "\n" << player->getName() << " territories to defend: " << endl;
                        for (auto* t : player->toDefend(map->getTerritories())) cout << "  - " << t->getName() << endl;
                        playerDone[i] = true;
                        cout << "\nPlayer (Cheater): " << player->getName() << " does not issue any orders." << endl;
                        break;
                    }
                }
                continue;
            }

            //Might need to change this part so it reflects the proper functions.
            cout << "\n" << player->getName() << " territories to defend: " << endl;
            for (auto* t : player->toDefend(map->getTerritories())) cout << "  - " << t->getName() << endl;

            cout << player->getName() << " territories to attack:" << endl;
            for (auto* t : player->toAttack(map->getTerritories())) cout << "  - " << t->getName() << endl;


            //Prompts user for a choice
            int choice;

            //Done issuing Orders.int choice;
            while (true) {
                cout << "\nPlayer " << player->getName() << " has " << player->getNumFreeArmies() << " free army units (" <<  player->getNumArmies() <<" total army units). Please issue an order:\n";
                cout << "1. Deploy\n2. Advance\n3. Bomb\n4. Blockade\n5. Airlift\n6. Negotiate\n7. Finish issuing\nChoice: ";

                if (!(cin >> choice)) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid input. Please enter a number." << endl;
                    cout << "Skipping your turn." << endl;
                    playerDone[i] = true;
                    continue;
                }

                if (choice == 8) {
                    cout << "Exiting order issuance." << endl;
                    state = "win";
                    return;
                }

                if (player->getNumFreeArmies() > 0 && choice != 1 && choice != 7) {
                    cout << "You still have " << player->getNumArmies()
                         << " undeployed armies. You must deploy them before issuing other orders." << endl;
                    continue;
                }

                if (choice < 1 || choice > 8) {
                    cout << "Invalid choice. Please try again." << endl;
                    continue;
                }
                break;
            }


            if (choice == 7) {
                playerDone[i] = true;
                cout << player->getName() << " is done issuing orders." << endl;
                continue;
            }

            //Pick territories
            Territory* source = nullptr;
            Territory* target = nullptr;
            Player* targetPlayer = nullptr;

            //Checks for the necessary parameters
            if (choice != 1 && choice != 4 && choice != 6) { //if requires two location
                string sourceName, targetName;
                cout << "Enter source territory name: ";
                cin >> sourceName;
                cout << "Enter target territory name: ";
                cin >> targetName;

                source = player->findTerritoryByName(sourceName);
                target = map->getTerritory(targetName);

                if (!source || !target) {
                    cout << "Invalid territories." << endl;
                    continue;
                }
            } else if (choice == 1 || choice == 4) { //if requires one location
                string terrName;
                cout << "Enter territory name: ";
                cin >> terrName;
                source = player->findTerritoryByName(terrName);
                if (!source) {
                    cout << "Invalid territory." << endl;
                    continue;
                }
            } else if (choice == 6) { //if requires a 2nd player name
                string otherPlayerName;
                cout << "Enter other player name: ";
                cin >> otherPlayerName;
                for (auto* p : players)
                    if (p->getName() == otherPlayerName)
                        targetPlayer = p;
            }
            //Insert a number of army to send out
            int armies = 0;
            if (choice == 1 || choice == 2 || choice == 5) {
                cout << "Enter number of armies: ";
                cin >> armies;
            }

            //creation of the order
            player->issueOrder(*deck,choice, source, armies, target, *targetPlayer, observer_);
            cout << "Order issued successfully!" << endl;

            allDone = false; // player can make another action after this current one + turn order wait
            this->counter++;

        }
        //checks if all players are done issuing orders, else continue loop
        allDone = std::all_of(playerDone.begin(), playerDone.end(), [](bool done){ return done; });
    }

    cout << "All players have finished issuing orders." << endl;
}

bool GameEngine::executeOrdersPhase() {
    changeState("executeorders", "Execute Orders Phase.");
    bool ordersRemaining = true;
    while (ordersRemaining) {
        ordersRemaining = false;
        //round-robin style
        for (auto player : players) {
            OrdersList* ol = player->getOrderList();
            std::list<Orders*> &orders = ol->getList(); // reference to the actual list

            if (!orders.empty()) {
                Orders* nextOrder = orders.front();

                std::cout << player->getName() << " executing order: " << *nextOrder << std::endl;
                nextOrder->execute();

                ordersRemaining = true; // we executed at least one order
            }
        }
    }
    //removes eliminated players
    std::vector<Player*> eliminated;
    for (auto* player : players) {

        //Checked every turn if a neutral player has been attacked or not.
        if (player->getPlayerStrategy()->getType() == StrategyType::Neutral) {
            if (auto* neutral = dynamic_cast<NeutralPlayerStrategy*>(player->getPlayerStrategy())) {
                neutral->attacked();
            }
        }
        if (player->getName() == "Neutral Player") {
            continue;
        }
        if (player->getTerritories().empty()) {
            eliminated.push_back(player);
        }
    }

    for (auto* player : eliminated) {
        player->getHand()->returnAll(*deck);
        removePlayer(player);
    }
    cout << endl;

    if (checkWinCondition(players, map)) {
        return true;
    }


    int exitCode = 0;
    //for testing and faster exits
    cout << "If you want to exit the game loop, enter 1. To continue, enter 0: ";
    cin >> exitCode;
    if (exitCode == 1) {
        state = "win";
        return true;
    }

    return false;
}
//Might need to change this method, but it works for now :|
bool GameEngine::checkWinCondition(const std::vector<Player*>& players, Map* map) { //checks to see if any player has won
    cout << endl;
    cout << "Checking win condition..." << endl;

    //checks if players or map is missing
    if (players.empty() || !map) {
        cout << "Invalid players or map." << endl;
        return false;
    }

    //checks if map is empty
    const vector<Territory*>& allTerritories = map->getTerritories();
    if (allTerritories.empty()) {
        cout << "No territories in map." << endl;
        return false;
    }
    //total number of territory
    int totalTerritories = static_cast<int>(allTerritories.size());
    cout << "Total territories: " << totalTerritories << endl;

    //Loop through players and count how many territories each owns
    for (auto* player : players) {
        if (!player) continue;

        int ownedCount = static_cast<int>(player->getTerritories().size());
        cout <<  player->getName() << " owns " << ownedCount << " territories." << endl;

        // Win condition: owns all territories
        if (ownedCount == totalTerritories) {
            cout << "Player " << player->getName() << " controls the entire map!" << endl;
            winnerList[mapCounter].push_back(player->getName());
            state = "win";
            return true;
        }
    }
    if (players.size() == 1) {
        cout << "Player " << players[0]->getName() << " is the only player left!" << endl;
        winnerList[mapCounter].push_back(players[0]->getName());
        state = "win";
        return true;
    }

    cout << "No winner yet." << endl;
    return false;
}

void GameEngine::addMap(const Map& map) { //adds a map to the engine
    this->map = new Map(map);
}
void GameEngine::addPlayer(Player* player) { //adds a player to the engine
    this->players.push_back(player);
}

void GameEngine::removePlayer(Player* player) { //to be implemented
    if (!player) return;

    auto it = std::find(players.begin(), players.end(), player);
    if (it != this->players.end()) {
        cout << "Player " << player->getName() << " removed." << endl;
        players.erase(it);
    }
}

void GameEngine::giveDeck(Deck* deck) {
    this->deck = deck;
}

void GameEngine::printAllPlayerOrders(const std::vector<Player*>& players) { //shows all the orders of all players
    cout << "======= PLAYER ORDER LISTS =======" << endl;
    for (const auto& player : players) {
        if (!player ) continue;
        cout << endl;
        cout << "Player: " << player->getName() << endl;

        OrdersList* ol = player->getOrderList();
        if (!ol) {
            cout << "  No OrdersList found." << endl;
            continue;
        }

        const std::list<Orders*>& orders = ol->getList();
        if (orders.empty()) {
            cout << "  No orders in list." << endl;
            continue;
        }

        int i = 1;
        for (auto o : orders) {
            if (o)
                cout << "  [" << i++ << "] " << *o << endl;
            else
                cout << "  [" << i++ << "] (null order)" << endl;
        }
    }
    cout << "===================================" << endl;
}

vector<string>& GameEngine::getMapsToUse(){
    return MapsToUse;
}
void GameEngine::setMapsToUse(const vector<string> maps){
    MapsToUse = maps;
}
vector<string>& GameEngine::getStrategiesToUse(){
    return StrategiesToUse;
}
void GameEngine::setStrategiesToUse(const vector<string> strategies){
    StrategiesToUse = strategies;
}
int GameEngine::getNumGames(){
    return numGames;
}
void GameEngine::setNumGames(int games){
    numGames = games;
}
int GameEngine::getNumTurns(){
    return numTurns;
}
void GameEngine::setNumTurns(int turns){
    numTurns = turns;
}