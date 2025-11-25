//
// Created by Malak on 2025-09-27.
//

#ifndef MAP_H 
#define MAP_H

#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <vector>

using std::vector;
using std::string;
using std::ostream;
using std::cout;
using std::set;
using std::ifstream;

//forward declaration
class Territory;
class Continent;
class Player;

class Map {
private:
    string name;
    vector<Territory*> territories; //collection of all the territories that are in the map, according to requirements, a map has a maximum of 255 territories
    vector<Continent*> continents; //collection of all the continents that are in the map, according to requirements, a map has a maximum of 32 continents
    vector<vector <int>> adjMatrix; //this represents a matrix of all vertices connected to all nodes in the map (the index in the adjMatrix represents the the index of the corresponding Territory instance in the vector of territories)
public:
    //constructor with a string name parameter
    Map(const string& n);
    //copy constructor of another Map instance
    Map(const Map& otherMap);
    bool operator==(const Map& otherMap);
    //copy assignment operator
    Map& operator = (const Map& otherMap);
    //destructor because Map has attribute that are of user-defined class type (territories and continents)
    ~Map();
    //getter for the name attribute
    string getName();
    vector<vector<int>> getAdjMat();
    //setter for the name attribute
    void setName(string n);
    //getter for the collection of the territories attribute
    vector <Territory*> getTerritories();
    //getter for the collection of the continents attribute
    vector <Continent*> getContinents();
    //adding Territory instances to the vector territories
    //NOTE: in this method, a deep copy is used because there is a full aggregation association between the map and its territories
    void addTerritory(Territory* t);
    //adding Continent instances to the vector continents
    //NOTE: in this method, a deep copy is used because there is a full aggregation association between the map and its continents
    void addContinent(Continent* c);
    //getter for boolean value if there is vertice in the adjacency matrix between 2 territories
    bool isConnected(Territory* t1, Territory* t2);
    //initialize the size of the matrix
    void initAdjMatrix();
    //setter for vertices in adjMatrix
    void setVertice(Territory* t1, Territory* t2);
    bool validate();
    friend ostream& operator << (ostream& out, const Map& map);
    Continent* getContinent(const string& n) const;
    Territory* getTerritory(const string& n) const;
};

class Continent{
private:
    string name;
    vector<Territory*> territories;
    int pointsToConquer;
    vector<vector<int>> adjMatrix;
public:
    Continent(const string& n, int p);
    //copy constructor, NOTE: because only MAP will be accessed (and has deep copies of both continents and territories),
    //there is no need to make a deep copy of territories (the map object can access it directly through its deep copy)
    Continent(const Continent& otherContinent);
    //copy assignment operator
    Continent& operator = (const Continent& otherContinent);
    //destructor
    ~Continent();
    //accessors
    string getName();
    int getPointsToConquer() const;
    vector<Territory*> getTerritories();
    bool isConnected(Territory* t1, Territory* t2);
    //mutators
    void setName(const string& n);
    void clearTerritories();
    void setPointsToConquer(int p);
    void addTerritory(Territory* t);
    //initialize the size of the matrix
    void initAdjMatrix();
    //setter for vertices in adjMatrix
    void setVertice(Territory* t1, Territory* t2);;
    friend ostream& operator << (ostream& out, const Continent& continent);
};

class Territory{
private:
    string name;
    Continent* continent;
    Player* owner;
    int numOfArmies;
    int x;
    int y;
    vector <string> adjTerritoriesNames;
public:
    //constructor
    Territory(const string& n, Continent* c, int x_coord, int y_coord, const vector <string>& adjT);
    //copy constructor
    Territory(const Territory& otherTerr);
    //copy assignment operator
    Territory& operator = (const Territory& otherTerr);
    //destructor
    ~Territory();
    //comparing operator to make comparison through ==
    bool operator ==(const Territory& otherTerr) const;
    //accessors
    string getName();
    Continent* getContinent() const;
    Player* getOwner() const;
    int getNumOfArmies() const;
    vector<string> getAdjTerritoriesNames();
    //mutators
    void setName(const string& n);
    void setContinent(Continent* c);
    void setOwner(Player* p);
    void setNumOfArmies(const int& num);
    void setAdjTerritoriesNames(const vector <string>& list);
    void addAdjTerritoriesNames(const string& name);;
    friend ostream& operator << (ostream& out, const Territory& terr);
};

class MapLoader{
private:
    ifstream inputFileStream;
public:
    MapLoader();
    ~MapLoader();
    MapLoader(const MapLoader& otherMapLoader);
    MapLoader& operator = (const MapLoader& otherMapLoader);
    Map loadMap(const string& filename);
    friend ostream& operator << (ostream& out, const MapLoader& mapLoader);
};

#endif