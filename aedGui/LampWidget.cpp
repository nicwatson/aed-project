// FILE LampWidget.cpp
// Implementations for LampWidget class

#include <QDebug>
#include "LampWidget.h"

using namespace aedGui;

LampWidget::LampWidget(QWidget * parent, const QString & styleUnlit, const QString & styleLit, int flashInterval)
    : QFrame(parent), lit(false), flashing(false), styleUnlit(styleUnlit), styleLit(styleLit), flashInterval(flashInterval)
{
    qDebug().noquote() << "[CONSTRUCTOR ENTRY] LampWidget(QWidget *, \""
                       << styleUnlit << "\", \""
                       << styleLit << "\", "
                       << QString::number(flashInterval)
                       << ")" << Qt::endl;

    flashTimer.setSingleShot(false);
    flashTimer.setInterval(flashInterval);
    connect(&flashTimer, &QTimer::timeout, this, &aedGui::LampWidget::flashTimerExpired);

    setStyleSheet(styleUnlit);
    setVisible(true);
    repaint();

    qDebug().noquote() << "[CONSTRUCTOR EXIT] LampWidget(QWidget *, \""
                       << styleUnlit << "\", \""
                       << styleLit << "\", "
                       << QString::number(flashInterval)
                       << ")" << Qt::endl;
}

LampWidget::LampWidget(QWidget * parent)
    : LampWidget(parent, "","", 200)
{
    qDebug().noquote() << "[CONSTRUCTOR ENTRY/EXIT] LampWidget(QWidget *)" << Qt::endl;
}

void LampWidget::setLit(bool state)
{
    // qDebug().noquote() << "[ENTRY] LampWidget::setLit(" << state << ")" << Qt::endl;

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

    // qDebug().noquote() << "[EXIT] LampWidget::setLit(" << state << ")" << Qt::endl;
}

// SLOT
void LampWidget::turnOn()
{
    qDebug().noquote() << "[ENTRY SLOT] LampWidget::turnOn()" << Qt::endl;

    stopFlash();
    setLit(true);

    qDebug().noquote() << "[EXIT SLOT] LampWidget::turnOn()" << Qt::endl;
}

// SLOT
void LampWidget::turnOff()
{
    qDebug().noquote() << "[ENTRY SLOT] LampWidget::turnOff()" << Qt::endl;

    stopFlash();
    setLit(false);

    qDebug().noquote() << "[EXIT SLOT] LampWidget::turnOff()" << Qt::endl;
}

// SLOT
void LampWidget::startFlash()
{
    qDebug().noquote() << "[ENTRY SLOT] LampWidget::startFlash()" << Qt::endl;

    flashing = true;
    flashTimer.start();

    qDebug().noquote() << "[EXIT SLOT] LampWidget::startFlash()" << Qt::endl;
}

// SLOT
void LampWidget::stopFlash()
{
    qDebug().noquote() << "[ENTRY SLOT] LampWidget::stopFlash()" << Qt::endl;

    flashing = false;
    flashTimer.stop();
    setLit(false);

    qDebug().noquote() << "[EXIT SLOT] LampWidget::stopFlash()" << Qt::endl;
}

// SLOT
void LampWidget::flashTimerExpired()
{
    // qDebug().noquote() << "[ENTRY SLOT] LampWidget::flashTimerExpired()" << Qt::endl;

    setLit(!lit);

    // qDebug().noquote() << "[EXIT SLOT] LampWidget::flashTimerExpired()" << Qt::endl;
}

