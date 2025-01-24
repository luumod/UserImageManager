#ifndef STIMER_H
#define STIMER_H

#include <QDebug>

class STimer
{
public:
    static void start();
    static void stop();
    static qint64 elapsedTime();
    static void printElapsedTime();
private:
    
};

#endif // TIMER_H