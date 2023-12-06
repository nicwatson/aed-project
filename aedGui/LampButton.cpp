// FILE LampButton.cpp
// Implementations for LampButton class

#include <QDebug>
#include "LampButton.h"

using namespace aedGui;

LampButton::LampButton(QWidget * parent, const QString & styleUnlit, const QString & styleLit, int flashInterval)
    : QPushButton(parent), lit(false), flashing(false), styleUnlit(styleUnlit), styleLit(styleLit), flashInterval(flashInterval)
{
    qDebug().noquote() << "[CONSTRUCTOR ENTRY] LampButton(QWidget *, \""
                       << styleUnlit << "\", \""
                       << styleLit << "\", "
                       << QString::number(flashInterval)
                       << ")" << Qt::endl;

    flashTimer.setSingleShot(false);
    flashTimer.setInterval(flashInterval);
    connect(&flashTimer, &QTimer::timeout, this, &aedGui::LampButton::flashTimerExpired);

    setStyleSheet(styleUnlit);
    setVisible(true);
    repaint();

    qDebug().noquote() << "[CONSTRUCTOR EXIT] LampButton(QWidget *, \""
                       << styleUnlit << "\", \""
                       << styleLit << "\", "
                       << QString::number(flashInterval)
                       << ")" << Qt::endl;
}

LampButton::LampButton(QWidget * parent)
    : LampButton(parent, "","", 200)
{
    qDebug().noquote() << "[CONSTRUCTOR ENTRY/EXIT] LampWidget(QWidget *)" << Qt::endl;
}


void LampButton::setLit(bool state)
{
    // qDebug().noquote() << "[ENTRY] LampButton::setLit(" << state << ")" << Qt::endl;

    lit = state;
    if(lit)
    {
        setStyleSheet(styleLit);
    }
    else
    {
        setStyleSheet(styleUnlit);
    }
    repaint();

    // qDebug().noquote() << "[EXIT] LampButton::setLit(" << state << ")" << Qt::endl;
}

// SLOT
void LampButton::turnOn()
{
    qDebug().noquote() << "[ENTRY SLOT] LampButton::turnOn()" << Qt::endl;

    stopFlash();
    setLit(true);

    qDebug().noquote() << "[EXIT SLOT] LampButton::turnOn()" << Qt::endl;
}

// SLOT
void LampButton::turnOff()
{
    qDebug().noquote() << "[ENTRY SLOT] LampButton::turnOff()" << Qt::endl;

    stopFlash();
    setLit(false);

    qDebug().noquote() << "[EXIT SLOT] LampButton::turnOff()" << Qt::endl;
}

// SLOT
void LampButton::startFlash()
{
    qDebug().noquote() << "[ENTRY SLOT] LampButton::startFlash()" << Qt::endl;

    flashing = true;
    flashTimer.start();

    qDebug().noquote() << "[EXIT SLOT] LampButton::startFlash()" << Qt::endl;
}

// SLOT
void LampButton::stopFlash()
{
    qDebug().noquote() << "[ENTRY SLOT] LampButton::stopFlash()" << Qt::endl;

    flashing = false;
    flashTimer.stop();
    setLit(false);

    qDebug().noquote() << "[EXIT SLOT] LampButton::stopFlash()" << Qt::endl;
}

// SLOT
void LampButton::flashTimerExpired()
{
    // qDebug().noquote() << "[ENTRY SLOT] LampButton::flashTimerExpired()" << Qt::endl;

    setLit(!lit);

    // qDebug().noquote() << "[EXIT SLOT] LampButton::flashTimerExpired()" << Qt::endl;
}
