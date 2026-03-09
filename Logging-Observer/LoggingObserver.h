#ifndef LOGGINGOBSERVER_H
#define LOGGINGOBSERVER_H

#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <vector>
#include <filesystem>
using std::cout;
using std::string;
using std::set;
using std::vector;
using std::ofstream;
using std::ostream;
using std::endl;


class ILoggable{
    public:
        //pure virtual function so that all derived classes will implement their own version
        virtual string stringToLog() =0;


        //all default because this is an abstract class
        virtual ~ILoggable()=default;
        ILoggable()=default;
        ILoggable(const ILoggable&) = default;
        ILoggable& operator=(const ILoggable&) = default;
        friend ostream& operator<<(ostream& out,  ILoggable& i);
};

class Observer{
    public:
    //pure virtual function that class which will extend Observer will HAVE to override
        virtual ~Observer()= default;
        virtual void update( ILoggable& s)=0;

        Observer()=default;
        Observer( const Observer& o )= default;
        Observer& operator=( const Observer& o )= default;
        friend ostream& operator<<(ostream& out, const Observer& o);
};

class LogObserver : public Observer{
    private:
        string filename;
        ofstream outputFileStream;
    public:
        //overridden virtual method from parent class
    //NOTE APPEND INSTEAD OF WRITE
        void update( ILoggable& s) override;

        //constructors, copy constructors, destructors, assignment operator and stream insertion operator
        LogObserver();
        LogObserver(const LogObserver& o);
        ~LogObserver() override;
        LogObserver& operator = (const LogObserver& o);
        friend ostream& operator << (ostream& out, const LogObserver& logObserver);
};

class Subject{

//protected members and methods for the Subject class
//so that only derived classes can directly access them
//without having to implement them again (except for the notify method that NEEDS to be implemented in derived classes)
protected:
    Observer* observer_;
    virtual void addObserver(Observer* observer) ;
    virtual void removeObserver(Observer* observer);
    virtual void notify(ILoggable& subject) =0;

    Subject() = default;
    virtual ~Subject() = default;
    Subject( const Subject& other);
    Subject& operator = (const Subject& other);
    friend ostream& operator<<(ostream& out, const Subject& subject);
};

void logWinners(std::vector<std::vector<std::string>> &winnerList, int numberOfGames, int numberOfMaps);



#endif
