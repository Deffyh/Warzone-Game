cmake_minimum_required(VERSION 4.0)
project(Warzone_Game1)

set(CMAKE_CXX_STANDARD 17)

include_directories(CommandProcessor)
include_directories(Engine)
include_directories(Logging-Observer)
include_directories(Orders)
include_directories(Part-4-Deck)
include_directories(part1-map)
include_directories(Player)

add_executable(Warzone_Game1
        CommandProcessor/CommandProcessing.cpp
        CommandProcessor/CommandProcessing.h
        CommandProcessor/CommandProcessingDriver.cpp
        Engine/GameEngine.cpp
        Engine/GameEngine.h
        Engine/GameEngineDriver.cpp
        Logging-Observer/LoggingObserver.cpp
        Logging-Observer/LoggingObserver.h
        Logging-Observer/LoggingObserverDriver.cpp
        Orders/Orders.cpp
        Orders/Orders.h
        Orders/OrdersDriver.cpp
        Part-4-Deck/Cards.cpp
        Part-4-Deck/Cards.h
        Part-4-Deck/cardsDriver.cpp
        part1-map/Map.cpp
        part1-map/Map.h
        part1-map/MapDriver.cpp
        Player/Player.h
        Player/PlayerDriver.cpp
        Player/PlayerTest.cpp
        MainDriver.cpp)
