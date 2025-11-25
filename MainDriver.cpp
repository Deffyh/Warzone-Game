#include "./Engine/GameEngine.h"
#include "./Orders/Orders.h"
#include "./Part-4-Deck/Cards.h"
#include "./part1-map/Map.h"
#include "./Player/Player.h"

extern void testManualCommands();
extern void testCommandProcessor();
extern void testFileCommandProcessorAdapter();
extern void testOrderExecution();
extern void testGameStates();
extern void testMainGameLoop();
extern void testLoggingObserver();

int main(){
    //testCommandProcessor();
    // testGameStates();
    // testMainGameLoop();
    // testOrderExecution();
    // testLoggingObserver();

    Player* p1 = new Player("p1");
    Territory* Ontario = new Territory("Ontario", new Continent("Canada", 5), 5, 6,{});
    auto* o1 = new OrdersBomb(p1, Ontario,new LogObserver());

    if (typeid(o1)==typeid(Orders))
    {
        cout<<"OrdersAdvance"<<endl;
    }


    return 0;
}