//
// Created by ngyna on 2025-09-28.
//testing file
#include "Player.h"

#include <iostream>
#include <vector>
using namespace std;

void testPlayers(){
//create players
 Player p1("Jack");
 Player p2("Alyssa");
//create territories
 /*Territory* tr1 = new Territory();
 Territory* tr2=new Territory();*/
//assign terr to players
p1.addTerritory(tr1);
p1.addTerritory(tr2);
p2.addTerritory(tr1);
//print player status
p1.printStatus();
p2.printStatus();
//add cards

 /*Cards* c1 = new Cards();
 Cards* c2 = new Cards();*/

 p1.addCard(c1);
p1.addCard(c2);
//issue orders
p1.issueOrder();
p2.issueOrder();
cout<<p1<<endl;
cout<<p2<<endl;
//delete to avoid memory leaks
delete tr1;
delete tr2;



}
 int main(){
  testPlayers();
 return 0;
 }

