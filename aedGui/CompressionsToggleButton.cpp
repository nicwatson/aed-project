#include "CompressionsToggleButton.h"
#include "strings.h"

using namespace aedGui;

CompressionsToggleButton::CompressionsToggleButton(QWidget * parent) : QPushButton(parent)
{
    disableCPR();
    connect(this, &QPushButton::toggled, this, &CompressionsToggleButton::setCheckedCustom);
}

void CompressionsToggleButton::disableCPR()
{
    setText(GUI_BUTTON_STARTCPR);
    setChecked(false);
    setEnabled(false);
}

void CompressionsToggleButton::enableCPR()
{
    setText(GUI_BUTTON_STARTCPR);
    setChecked(false);
    setEnabled(true);
}

// SLOT
void CompressionsToggleButton::setCheckedCustom(bool checked)
{
    if(checked)
    {
        setText(GUI_BUTTON_STOPCPR);
    }
    else
    {
        setText(GUI_BUTTON_STARTCPR);
    }
    emit signalButtonToggleCompressions(checked);
}
