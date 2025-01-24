#include "STimer.h"
#include <QElapsedTimer>

static QElapsedTimer m_timer;
static bool m_isRunning;

void STimer::start()
{
    if (!m_isRunning) {
        m_timer.start();
        m_isRunning = true;
    }
}

void STimer::stop()
{
    if (m_isRunning) {
        m_isRunning = false;
    }
}

qint64 STimer::elapsedTime()
{
    if (m_isRunning) {
        return m_timer.elapsed();
    }
    return 0;
}

void STimer::printElapsedTime()
{
    if (m_isRunning) {
        qDebug() << "Elapsed time:" << m_timer.elapsed() << "milliseconds";
    }
    else {
        qDebug() << "STimer is not running.";
    }
}
