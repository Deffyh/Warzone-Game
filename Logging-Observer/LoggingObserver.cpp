//
// Created by Malak Sefrioui on 2025-11-03.
//

#include "LoggingObserver.h"

#include <fstream>
#include <iostream>
#include <set>
#include <string>
using std::cout;
using std::endl;
using std::ofstream;

//IMPLEMENTATION OF THE SUBJECT CLASS

//ADDING CLASS THAT ARE/DERIVE FROM OBSERVER TO THE SET ATTRIBUTE OF SUBJECT
void Subject::addObserver(Observer *observer) {
    observer_ = observer;
}

//REMOVING THE GIVEN OBSERVER
void Subject::removeObserver(Observer *observer)
{
    observer_=nullptr;
}

// METHOD NOTIFY IS NOT IMPLEMENTED IN THIS CLASS BECAUSE
// EACH DERIVED CLASS OF SUBJECT WILL HAVE TO IMPLEMENT THEIR OWN METHOD

//COPY CONSTRUCTOR, ASSIGNMENT OPERATOR AND STREAM INSERTION OPERATOR
Subject::Subject(const Subject &other)
{
    observer_ = other.observer_;
}

Subject& Subject::operator=(const Subject &other)
{
    if (this != &other){
        observer_ = other.observer_;
    }
    return *this;
}

ostream& operator <<(ostream& out, const Subject& subject)
{
    out<<"This is a Subject Object with the following observers attached :\n";

        out<<"\t- "<<subject.observer_<<endl;

    return out;
}

//IMPLEMENTATION OF LOGOBSERVER CLASS

LogObserver::LogObserver()
{
    filename = "gamelog.txt";
    // open in append mode so we don't overwrite previous log entries
    outputFileStream.open(filename, std::ios::out | std::ios::app);
    if (!outputFileStream.is_open()) {
        cout << "Failed to open " << filename << " for appending" << endl;
    }
}

LogObserver::LogObserver(const LogObserver& o)
{
    filename = o.filename.empty() ? "gamelog.txt" : o.filename;
    outputFileStream.open(filename, std::ios::out | std::ios::app);
    if (!outputFileStream.is_open()) {
        cout << "Failed to open " << filename << " for appending (copy ctor)" << endl;
    }
}

LogObserver::~LogObserver()
{
 if (outputFileStream.is_open())
 {
     outputFileStream.close();
 }
}

void LogObserver::update(ILoggable& s)
{
    if (outputFileStream.is_open())
    {
        outputFileStream<<s.stringToLog()<<endl;
    }else
    {
        cout<<filename<< "is not open!"<<endl;
    }
}

LogObserver& LogObserver::operator=(const LogObserver& o)
{
    if (this != &o) {
        if (outputFileStream.is_open()) outputFileStream.close();
        filename = o.filename;
        // reopen in append mode
        outputFileStream.open(filename, std::ios::out | std::ios::app);
        if (!outputFileStream.is_open()) {
            cout << "Failed to open " << filename << " for appending (operator=)" << endl;
        }
    }
    return *this;
}

ostream &operator<<(ostream &out, const Observer &o)
{
    out<<"This is an Observer with the address : "<<&o<<endl;
    return out;
}


//IMPLEMENTATION OF ILOGGABLE
ostream& operator<<(ostream &out, ILoggable &i)
{
    out<<i.stringToLog()<<endl;
    return out;
}

void logWinners(std::vector<std::vector<std::string>> &winnerList, int numberOfGames, int numberOfMaps){
    std::ofstream out("gamelog.txt", std::ios::out | std::ios::app);
    if (!out.is_open()) {
        std::cout << "Couldn't open gamelog.txt." << std::endl;
        return;
    }

    out << "Results:\n";

    out << "\t\t\t\t\t\t\t\t\t";
    for (int g = 0; g < numberOfGames; g++) {
        out << "Game " << g + 1 << "\t\t\t\t\t\t\t";
    }
    out << "\n";


    for (int m = 0; m < numberOfMaps; m++) {
        out << "Map " << m + 1 << "\t\t\t\t\t\t\t";

        for (int g = 0; g < numberOfGames; g++) {
            // if (g < winnerList.size() && m < winnerList[g].size()) {
            //     out << winnerList[m][g] << "!\t\t\t\t\t\t\t";

            // }
            out << winnerList[m][g] << "\t\t\t\t\t\t\t";
        }

        out << "\n";
    }

    out << "LINE REACHED \n";  // extra line break
};