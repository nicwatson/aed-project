#include "LampButton.h"

using namespace aedGui;

LampButton::LampButton(QWidget * parent, const QString & styleUnlit, const QString & styleLit, int flashInterval)
    : QPushButton(parent), lit(false), flashing(false), styleUnlit(styleUnlit), styleLit(styleLit), flashInterval(flashInterval)
{
    flashTimer.setSingleShot(false);
    flashTimer.setInterval(flashInterval);
    connect(&flashTimer, &QTimer::timeout, this, &aedGui::LampButton::flashTimerExpired);

    setStyleSheet(styleUnlit);
    setVisible(true);
    repaint();
}

LampButton::LampButton(QWidget * parent)
    : LampButton(parent, "","", 200)
{

}

void LampButton::setLit(bool state)
{
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
}

// SLOT
void LampButton::turnOn()
{
    stopFlash();
    setLit(true);
}

// SLOT
void LampButton::turnOff()
{
    stopFlash();
    setLit(false);
}

// SLOT
void LampButton::startFlash()
{
    flashing = true;
    flashTimer.start();
}

// SLOT
void LampButton::stopFlash()
{
    flashing = false;
    flashTimer.stop();
    setLit(false);
}

// SLOT
void LampButton::flashTimerExpired()
{
    setLit(!lit);
}

