#include "LampWidget.h"

using namespace aedGui;

LampWidget::LampWidget(QWidget * parent, const QString & styleUnlit, const QString & styleLit, int flashInterval)
    : QFrame(parent), lit(false), flashing(false), styleUnlit(styleUnlit), styleLit(styleLit), flashInterval(flashInterval)
{
    flashTimer.setSingleShot(false);
    flashTimer.setInterval(flashInterval);
    connect(&flashTimer, &QTimer::timeout, this, &aedGui::LampWidget::flashTimerExpired);

    setStyleSheet(styleUnlit);
    setVisible(true);
    repaint();
}

LampWidget::LampWidget(QWidget * parent)
    : LampWidget(parent, "","", 200)
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

