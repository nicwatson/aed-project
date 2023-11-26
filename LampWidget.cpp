#include "LampWidget.h"

LampWidget::LampWidget(QWidget * parent, const QString & styleUnlit, const QString & styleLit)
    : QFrame(parent), lit(false), styleUnlit(styleUnlit), styleLit(styleLit)
{
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
    setLit(true);
}

// SLOT
void LampWidget::turnOff()
{
    setLit(false);
}
