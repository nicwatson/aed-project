// FILE CompressionsToggleButton.cpp
// Implementations for CompressionToggleButton class

#include <QDebug>
#include "CompressionsToggleButton.h"
#include "strings.h"

using namespace aedGui;

CompressionsToggleButton::CompressionsToggleButton(QWidget * parent) : QPushButton(parent)
{
    qDebug() << "[CONSTRUCTOR] CompressionsToggleButton()" << Qt::endl;

    disableCPR();
    connect(this, &QPushButton::toggled, this, &CompressionsToggleButton::setCheckedCustom);

}

CompressionsToggleButton::~CompressionsToggleButton()
{
    qDebug() << "[DESTRUCTOR] CompressionsToggleButton()" << Qt::endl;
    // No dynamic allocations to delete
}

// SLOT
void CompressionsToggleButton::disableCPR()
{
    qDebug() << "[ENTRY SLOT] CompressionsToggleButton::disableCPR()" << Qt::endl;

    setText(GUI_BUTTON_STARTCPR);
    setChecked(false);
    setEnabled(false);

    qDebug() << "[EXIT SLOT] CompressionsToggleButton::disableCPR()" << Qt::endl;
}

// SLOT
void CompressionsToggleButton::enableCPR()
{
    qDebug() << "[ENTRY SLOT] CompressionsToggleButton::enableCPR()" << Qt::endl;

    setText(GUI_BUTTON_STARTCPR);
    setChecked(false);
    setEnabled(true);

    qDebug() << "[EXIT SLOT] CompressionsToggleButton::enableCPR()" << Qt::endl;
}

// SLOT
void CompressionsToggleButton::setCheckedCustom(bool checked)
{
    qDebug().noquote() << "[ENTRY SLOT] CompressionsToggleButton::setCheckedCustom(" << checked << ")" << Qt::endl;

    if(checked)
    {
        setText(GUI_BUTTON_STOPCPR);
    }
    else
    {
        setText(GUI_BUTTON_STARTCPR);
    }

    qDebug().noquote() << "[SIGNAL] Emit CompressionsToggleButton::signalButtonToggleCompressions(" << checked << ")" << Qt::endl;
    emit signalButtonToggleCompressions(checked);

    qDebug().noquote() << "[EXIT SLOT] CompressionsToggleButton::setCheckedCustom(" << checked << ")" << Qt::endl;
}
