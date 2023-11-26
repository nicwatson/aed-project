#include "LampWidget.h"

LampWidget::LampWidget(QWidget * parent, const QString & styleUnlit, const QString & styleLit)
    : QFrame(parent), lit(false), flashing(false), styleUnlit(styleUnlit), styleLit(styleLit)
{
    flashTimer.setSingleShot(false);
    flashTimer.setInterval(FLASH_TIME);
    connect(&flashTimer, SIGNAL(timeout()), this, SLOT(flashTimerExpired()));

    setStyleSheet(styleUnlit);
    setVisible(true);
    repaint();
}

LampWidget::LampWidget(QWidget * parent)
    : LampWidget(parent, "","")
{

}

void LampWidget::setLit(bool state)
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
void LampWidget::turnOn()
{
    stopFlash();
    setLit(true);
}

// SLOT
void LampWidget::turnOff()
{
    stopFlash();
    setLit(false);
}

// SLOT
void LampWidget::startFlash()
{
    flashing = true;
    flashTimer.start();
}

// SLOT
void LampWidget::stopFlash()
{
    flashing = false;
    flashTimer.stop();
    setLit(false);
}

// SLOT
void LampWidget::flashTimerExpired()
{
    setLit(!lit);
}

