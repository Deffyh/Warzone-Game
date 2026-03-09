//
// Created by Malak on 2025-09-27.
//

#include <iostream>
#include <set>
#include <string>
#include <vector>
#include "Map.h"
#include "../Player/Player.h"
#include <sstream>
using std::vector;
using std::string;
using std::ostream;
using std::cout;
using std::set;
using std::ifstream;


//forward declaration
class Territory;
class Continent;

//free functions needed for Map::validate()
void recursDFS(vector <vector<int>> &adjM,vector<bool> &visited,int n){
    visited[n] = true;

    for (int i=0;i<adjM[n].size();i++){
        if (!visited[i]&&adjM[n][i]==1){
            recursDFS(adjM, visited, i);
        }
    }
}

bool isConnectedGraph(vector <vector<int>> &adjM, string n, string type){
    vector<bool> visited(adjM.size(), false);
    recursDFS(adjM, visited, 0);
    for (bool b : visited){
        if (!b){
            cout<<type<<" "<<n<<" is not a connected graph"<<"\n";
            return false;
        }
    }
    cout<<type<<" "<<n<<" is a connected graph"<<"\n\n";
    return true;
}

//constructor with a string name parameter
Map::Map(const string& n):name(n){}

//copy constructor of another Map instance
Map::Map(const Map& otherMap){
    this->name = otherMap.name;

    for (Continent* c: otherMap.continents) {
        this->continents.push_back(new Continent(*c));
        this->continents.back()->clearTerritories();
    }
    for (Territory *t: otherMap.territories) {
        this->territories.push_back(new Territory(*t));
        this->territories.back()->setContinent(this->getContinent(t->getContinent()->getName()));
        this->getContinent(t->getContinent()->getName())->addTerritory(getTerritories().back());
    }


    this->adjMatrix = otherMap.adjMatrix;
}

//comparing method
bool Map::operator==(const Map& otherMap){
    if (this->name==otherMap.name&&this->adjMatrix==otherMap.adjMatrix){
        return true;
    }
    return false;
}

//copy assignment operator
Map& Map::operator = (const Map& otherMap){
    if (this==&otherMap){return *this;}
    this->name = (otherMap.name);

    //delete the current territories and continents to avoid memory leaks
    for(Territory* t : territories){
        delete t;
    }
    territories.clear();

    for(Continent* c : continents){
        delete c;
    }
    continents.clear();
    for (Continent* c: otherMap.continents) {
        this->continents.push_back(new Continent(*c));
        this->continents.back()->clearTerritories();
    }
    for (Territory *t: otherMap.territories) {
        this->territories.push_back(new Territory(*t));
        this->territories.back()->setContinent(this->getContinent(t->getContinent()->getName()));
        this->getContinent(t->getContinent()->getName())->addTerritory(getTerritories().back());
    }

    this->adjMatrix = otherMap.adjMatrix;

    return *this;
}

//destructor because Map has attribute that are of user-defined class type (territories and continents)
Map::~Map(){
    
    for(Territory* t : territories){
        delete t;
    }
    territories.clear();

    for(Continent* c : continents){
        delete c;
    }
    continents.clear();
    //clear vectors
}

//getter for the name attribute
string Map::getName(){
    return name;
}

//setter for the name attribute
void Map::setName(string n){
    name = n;
}

//getter for the collection of the territories attribute
vector<Territory*> Map::getTerritories(){
    return territories;
}


//getter for the collection of the continents attribute
vector <Continent*> Map::getContinents(){
    return continents;
}

vector<vector<int>> Map::getAdjMat(){
    return adjMatrix;
}

//adding Territory instances to the vector territories
//NOTE: in this method, a deep copy is used because there is a full aggregation association between the map and its territories
void Map::addTerritory(Territory* t){
    territories.push_back(new Territory(*t));
}

//adding Continent instances to the vector continents
//NOTE: in this method, a deep copy is used because there is a full aggregation association between the map and its continents
void Map::addContinent(Continent* c){
    continents.push_back(new Continent(*c));
}

Continent* Map::getContinent(const string& n) const{
    for (Continent* c:continents){
        if (c->getName()==n){
            return c;
        }
    }
    return nullptr;
}
Territory* Map::getTerritory(const string& n) const{
    for (Territory* t: territories){
        if(t->getName()==n){
            return t;
        }
    }
    return nullptr;
}

//getter for boolean value if there is vertice in the adjacency matrix between 2 territories
bool Map::isConnected(Territory* t1, Territory* t2){
    
    //the index of a vector cannot be -1 so if these variables are still -1 after the for loop, then t1 and t2 were not found in the territories vector
    int t1Index, t2Index;
    t1Index=-1;
    t2Index=-1;

    for (int i=0; i<territories.size(); i++)
    {
        if(*territories[i]==*t1){
            t1Index=i;
        } else if (*territories[i]==*t2){
            t2Index=i;
        }
    }

    if (t1Index==-1||t2Index==-1){
        cout<<"Either "<<(t1 ? t1->getName() : "null")<<" or "<<(t2 ? t2->getName() : "null")<<" is not in this map";
        return false;
    }

    if (adjMatrix[t1Index][t2Index]==1){
        return true;
    } else if (adjMatrix[t1Index][t2Index]==0){
        return false;
    } else {
        cout<<"Invalid input in the adjacency matrix of map "<<name;
        return false;
    }

}
//initialize the size of the matrix
void Map::initAdjMatrix(){
    int size=territories.size();
    adjMatrix.resize(size,vector<int> (size, 0));
}

//setter for vertices in adjMatrix
void Map::setVertice(Territory* t1, Territory* t2){
    //the index of a vector cannot be -1 so if these variables are still -1 after the for loop, then t1 and t2 were not found in the territories vector
    int t1Index, t2Index;
    t1Index=-1;
    t2Index=-1;

    for (int i=0; i<territories.size(); i++)
    {
        if(*territories[i]==*t1){
            t1Index=i;
        } else if (*territories[i]==*t2){
            t2Index=i;
        }
    }

    if (t1Index==-1||t2Index==-1){
        cout<<"Either "<<(t1 ? t1->getName() : "null")<<" or "<<(t2 ? t2->getName() : "null")<<" is not in this map";
        return;
    }

    adjMatrix[t1Index][t2Index]=1;
    adjMatrix[t2Index][t1Index]=1; //because the graph is undirected
}

bool Map::validate()
{
    bool check3=true;
    bool check2 = false;
    bool check1 = false;

    if (!adjMatrix.empty())
        //1. verify that the map is a connected graph through DFS function isConnectedGraph
        check1 = isConnectedGraph(adjMatrix, name, "Map");

    if (!adjMatrix.empty())
        //2. verify that the country is a connected graph through DFS function isConnectedGraph
        check2 = isConnectedGraph(adjMatrix, continents[0]->getName(), "Continent");

    //3.1 check if any territory is being repeated in any continent's territories list
    set <Territory*> setTerr;
    for (Continent* c: continents){
        for (Territory* t: c->getTerritories()){
            if (setTerr.count(t)){
                cout<<t->getName()<<" appears in more than one continent"<<std::endl;
                check3=false;
            }else{setTerr.insert(t);}
        }
    }
    //3.2 check if every territory has an association to a continent or nullptr
    for (Territory* t: territories){
        if (t->getContinent()==nullptr){
            cout<<t->getName()<<" is not associated to any continent"<<std::endl;
            check3=false;
        }
    }
    if (check1&&check2&&check3){return true;}
    return false;
}

ostream& operator <<(ostream& out, const Map& map){
    out<<"Map name: "<<map.name<<"\n";
    out<<"Here are the continents and their respective territories in the map : \n";
    for(Continent* c: map.continents){
        out<<"\t-\t"<<c->getName()<<"\n";
        for(Territory* t: c->getTerritories()){
            out<<"\t\t-\t"<<t->getName()<<" is connected to\n";
            for (const string& name: t->getAdjTerritoriesNames()){
                out<<"\t\t\t-\t"<<name<<"\n";
            }
        }
    }
    return out;
}

Continent::Continent(const string& n, const int p):name(n),pointsToConquer(p){}

//copy constructor, NOTE: because only MAP will be accessed (and has deep copies of both continents and territories), 
//there is no need to make a deep copy of territories (the map object can access it directly through its deep copy)
Continent::Continent(const Continent& otherContinent){
    this->name=otherContinent.name;
    this->pointsToConquer=otherContinent.pointsToConquer;
    this->adjMatrix=otherContinent.adjMatrix;
    this->territories=otherContinent.territories;
}

Continent& Continent::operator =(const Continent& otherContinent){
    this->name=otherContinent.name;
    this->pointsToConquer=otherContinent.pointsToConquer;
    this->adjMatrix=otherContinent.adjMatrix;
    this->territories=otherContinent.territories;

    return *this;
}

Continent::~Continent()=default;//no deep copy, therefore the destructor contains nothing

string Continent::getName(){
    return name;
}

void Continent::setName(const string& n){
    name=n;
}

int Continent::getPointsToConquer() const{
    return pointsToConquer;
}

void Continent::clearTerritories(){
    this->territories.clear();
}


void Continent::setPointsToConquer(const int p){
    pointsToConquer=p;
}

void Continent::addTerritory(Territory* t){
    territories.push_back(t);
}

vector<Territory*> Continent::getTerritories(){
    return territories;
}

bool Continent::isConnected(Territory* t1, Territory* t2) {
    int t1Index = -1;
        int t2Index = -1;

        for (int i=0; i<territories.size(); i++)
        {
            if(*territories[i]==*t1){
                t1Index=i;
            } else if (*territories[i]==*t2){
                t2Index=i;
            }
        }

        if (t1Index==-1||t2Index==-1){
            cout<<"Either "<<(t1 ? t1->getName() : "null")<<" or "<<(t2 ? t2->getName() : "null")<<" is not in this continent";
            return false;
        }

        if (adjMatrix[t1Index][t2Index]==1){
            return true;
        } else if (adjMatrix[t1Index][t2Index]==0){
            return false;
        } else {
            cout<<"Invalid input in the adjacency matrix of continent "<<name;
            return false;
        }
}

//setter for vertices in adjMatrix
void Continent::setVertice(Territory* t1, Territory* t2){
    int t1Index = -1;
    int t2Index = -1;

    for (int i=0; i<territories.size(); i++)
    {
        if(*territories[i]==*t1){
            t1Index=i;
        } else if (*territories[i]==*t2){
            t2Index=i;
        }
    }

    if (t1Index==-1||t2Index==-1){
        cout<<"Either "<<(t1 ? t1->getName() : "null")<<" or "<<(t2 ? t2->getName() : "null")<<" is not in this continent"<<"\n";
        return;
    }

    adjMatrix[t1Index][t2Index]=1;
    adjMatrix[t2Index][t1Index]=1; //because the graph is undirected
}
//initialize the size of the matrix
void Continent::initAdjMatrix(){
    const int size=territories.size();
    adjMatrix.resize(size,vector<int> (size, 0));
}

ostream& operator << (ostream& out, const Continent& continent){
    out<<"Continent name: "<<continent.name<<std::endl;
    out<<"Points to conquer: "<<continent.pointsToConquer<<std::endl;
    out<<"Territories is continent: ";
    for (Territory* t :continent.territories){
        out<<"\t-"<<t->getName()<<"\n";
    }
    return out;
}

//constructor
Territory::Territory(const string& n, Continent* c, const int x_coord, const int y_coord, const vector <string>& adjT):
    name(n),
    continent(c),
    owner(nullptr),
    numOfArmies(0),
    x(x_coord),
    y(y_coord),
    adjTerritoriesNames(adjT)
{}

//copy constructor
Territory::Territory(const Territory& otherTerr){
    this->name=otherTerr.name;
    this->continent=otherTerr.continent;
    this->owner=otherTerr.owner;
    this->numOfArmies=otherTerr.numOfArmies;
    this->x=otherTerr.x;
    this->y=otherTerr.y;
    this->adjTerritoriesNames=otherTerr.adjTerritoriesNames;
}

//copy assignment operator
Territory& Territory::operator = (const Territory& otherTerr)= default;

//destructor
Territory::~Territory()=default;//nothing in destructor because there is no deep copy in the construction of a Territory

//comparing operator to make comparison through ==
bool Territory::operator ==(const Territory& otherTerr) const{
    return this->name==otherTerr.name;
}

//accessors & mutators
string Territory::getName(){
    return name;
}

void Territory::setName(const string& n){
    name=n;
}

Continent* Territory::getContinent() const{
    return continent;
}

void Territory::setContinent(Continent* c){
    continent=c;
}

Player* Territory::getOwner() const{
    return owner;
}

void Territory::setOwner(Player* p){
    owner=p;
}

int Territory::getNumOfArmies() const {
    return numOfArmies;
}

void Territory::setNumOfArmies(const int& num){
    numOfArmies=num;
}

vector<string> Territory::getAdjTerritoriesNames(){
    return adjTerritoriesNames;
}
//a method to replace the entire list
void Territory::setAdjTerritoriesNames(const vector <string>& list){
    adjTerritoriesNames=list;
}
//a method to add a single name
void Territory::addAdjTerritoriesNames(const string& n){
    adjTerritoriesNames.push_back(n);
}

ostream& operator << (ostream& out, const Territory& territory){
    out<<"Territory Name: "<< territory.name<<std::endl;
    out<<"Belongs in continent: "<<territory.continent->getName()<<std::endl;
    out<<"Owned by: ";
        if(territory.owner==nullptr){out<<"None"<<std::endl;}
        else {out<<territory.owner->getName()<<std::endl;}
    out<<"Connected to: "<<std::endl;
    for (const string& name:territory.adjTerritoriesNames){
        out<<"\t-"<<name<<std::endl;
    }
    out<<"X: "<<territory.x<<"\tY: "<<territory.y<<std::endl;
    out<<"Number of armies: "<<territory.numOfArmies<<std::endl;
    return out;
};

MapLoader::MapLoader()= default;                                                                                        //no private member to initialize
MapLoader::MapLoader(const MapLoader& otherMapLoader){};
MapLoader::~MapLoader()= default;
MapLoader& MapLoader::operator = (const MapLoader& otherMapLoader){return *this;}                                       //there is no difference between a MapLoader to another

Map MapLoader::loadMap(const string& filename){
    Map map("holder");

    inputFileStream.open(filename);
    if (inputFileStream.is_open()){                                                                                     //if true, the file was opened successfully and will be read
        Territory* terrHolder=nullptr;
        Continent* conHolder=nullptr;

        string line,mapName,conName,terrName;
        int step=0,x,y,p;
        string::size_type firstPos, secondPos;
        vector <string> adjTerrName,terrInfo;

       while(getline(inputFileStream,line)){                                                                      //reading all the lines until the end of the file
           if (step==0&&line!="[Map]"){
               cout<<endl<<"The given file to load does not have the correct configuration to create a valid map object. "<<endl<<
                           "The returned map object from function loadMap is a placeholder map with no information."<<endl;
               break;
           }
           if (line=="[Map]"){                                                                                         //if true, the next line will have the name of the map in the image name(i.e. "image=Aldawin.png", Aldawin is the name of the map)
                getline(inputFileStream, line);
                firstPos=line.find('=');                                                                              //truncating the string to find the name by using '=' and '.' as delimiters
                secondPos=line.find('.');
                mapName=line.substr(firstPos+1,secondPos-firstPos-1);
                map=Map(mapName);step++;                                                                                       //object map is reinitialized to the right name using a copy constructor
                continue;
            }
            if (line=="[Continents]"){                                                                                  //unless the ifstream reaches the continent section marked by the string in if cond
                step++;continue;                                                                                        //the code will never initialize any territory or continent, because step (int 0) needs to be incremented to proceed to the next step
            }
           if (step==2){                                                                                                //step is only going to get incremented if it reaches the territories' section
               if (line.empty()){continue;}
               if (line=="[Territories]"){step++;continue;}
                firstPos=line.find('=');                                                                             //each line in the continent section shows 2 things separated by '=': 1. name 2. points to conquer
                conName=line.substr(0,firstPos);
                p=stoi(line.substr(firstPos+1,line.size()-firstPos-1));
                conHolder= new Continent(conName,p);                                                                    //a continent is initialized using the conHolder continent obj placeholder
                map.addContinent(conHolder);                                                                            //the newly initialized continent is then added to the map's vector of the continents
           }
           if (step==3){
               if (line.empty()){continue;}
               std::stringstream terrInfoStream(line);
               while (getline(terrInfoStream,terrName,',')){                                                 //Once in the territories section, each line is considered as csv in the following order: name,x,y,continent,adjterr1,adjterr2,....ajterrN
                   terrInfo.push_back(terrName);                                                                        //each string is put in vector terrInfo, then each string in the vector is assigned to a local variable that will be used for object initialization
               }
               for (int i=0;i<terrInfo.size();i++){
                   if (i==0){
                       terrName=terrInfo[0];
                   }else if (i==1){
                       x=stoi(terrInfo[1]);
                   }else if (i==2){
                       y=stoi(terrInfo[2]);
                   }else if (i==3){
                       conName=terrInfo[3];
                       conHolder= map.getContinent(conName);                                                            //because Continent is a pointer data member of territories we go through the map's collection of continents to find the pointer
                   }else if (i>3){
                       adjTerrName.push_back(terrInfo[i]);
                   }
               }
               terrInfo.clear();                                                                                        //after terrInfo was used, it is cleared to make sure there is no value from a previous territory added to the following one
               terrHolder=new Territory(terrName,conHolder,x,y,adjTerrName);
               adjTerrName.clear();
               map.addTerritory(terrHolder);                                                                            //add to map object
               map.getContinent(conName)->addTerritory(map.getTerritory(terrHolder->getName()));                    //add to continent referencing to the map because it has a deep copy of continent
               delete terrHolder;                                                                                       //to avoid memory leaks
           }
       }
    }else{                                                                                                              //if the file was not opened successfully
        cout<<"Error: file not found"<<std::endl;
    }
    inputFileStream.close();

    //populating the map graph data structure
    //to populate the graph, we must start with each continent's subgraph
    map.initAdjMatrix();
    vector<string>adjTerrName;
    for (Continent* c:map.getContinents()){
        c->initAdjMatrix();
        for (Territory* t:c->getTerritories()){
            adjTerrName.push_back(t->getName());
            for (const string& name:t->getAdjTerritoriesNames()){
                Territory* terr=map.getTerritory(name);
                if (terr!=nullptr&&terr->getContinent()==c&&!c->isConnected(t,map.getTerritory(name))){                 //3 checks: 1. is the destination territory pointer null 2. is the destination territory in the same continent as its source
                    c->setVertice(t,map.getTerritory(name));                                                            //3. was the vertice between the 2 territories already created
                }
            }
        }
    }
    adjTerrName.clear();
    //populate the adjacency matrix for the map
    for (Territory* t:map.getTerritories()){
        adjTerrName.push_back(t->getName());
        for (const string& name:t->getAdjTerritoriesNames()){
            Territory* terr=map.getTerritory(name);
            if (terr!=nullptr&&!map.isConnected(t,map.getTerritory(name))){
                map.setVertice(t,map.getTerritory(name));
            }
        }
    }
    return map;
};

ostream& operator << (ostream& out, const MapLoader& mapLoader){
    out<<"Map loader has a ifstream named inputFileStream"<<std::endl;
    return out;
};