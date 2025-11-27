//
// Created by Howard on 2025-11-26.
//
#include "GameEngine.h"
#include "../Logging-Observer/LoggingObserver.h"

#include <iostream>

void testPlayerStrategies() {
    cout << "===== GAME ENGINE STATE MACHINE TEST =====" << endl;

    LogObserver* observer = new LogObserver();
    GameEngine engine(observer);   // create a new engine instance
    engine.startupPhase();        // start the game loop

    delete observer;
    cout << "===== GAME ENDED =====" << endl;
}