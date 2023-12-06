// FILE ModuleShock.cpp
// Imeplementations for ModuleShock class

#include "ModuleShock.h"
#include "aedGui/strings.h"
#include <QDebug>

using namespace aedModel;

// Increasing successive shock energies as per Admin Guide, page 6.
const int ModuleShock::ADULT_SHOCKS[] = { 120, 150, 200 };
const int ModuleShock::CHILD_SHOCKS[] = { 50, 70, 85 };

ModuleShock::ModuleShock() : QObject()
{
    qDebug() << "[CONSTRUCTOR ENTRY] ModuleShock()" << Qt::endl;

    active = false;
    energySet = 0;
    charged = false;
    buttonPressedEarly = false;
    inChargeDelay = false;
    inExitDelay = false;

    timer.setSingleShot(true);

    qDebug() << "[CONSTRUCTOR EXIT] ModuleShock()" << Qt::endl;
}

ModuleShock::~ModuleShock()
{
    qDebug() << "[DESTRUCTOR] ~ModuleShock()" << Qt::endl;
    // No dynamic allocations to delete
}

int ModuleShock::calcShockEnergy(bool usingChildPads)
{
    qDebug().noquote() << "[ENTRY] ModuleShock::calcShockEnergy(" << usingChildPads << ")" << Qt::endl;
    if(usingChildPads)
    {
        qDebug().noquote() << "[EXIT] ModuleShock::calcShockEnergy(" << usingChildPads
                           << "): returning " << QString::number(CHILD_SHOCKS[std::max(shocksDelivered, 2)]) << Qt::endl;
        return CHILD_SHOCKS[std::max(shocksDelivered, 2)];
    }

    qDebug().noquote() << "[EXIT] ModuleShock::calcShockEnergy(" << usingChildPads
                       << "): returning " << QString::number(CHILD_SHOCKS[std::max(shocksDelivered, 2)]) << Qt::endl;
    return ADULT_SHOCKS[std::max(shocksDelivered, 2)];
}

// SLOT
void ModuleShock::chargeReady()
{
    qDebug() << "[ENTRY SLOT] ModuleShock::chargeReady()" << Qt::endl;

    disconnect(&timer, SIGNAL(timeout()), this, SLOT(chargeReady()));
    inChargeDelay = false;
    charged = true;

    qDebug() << "[SIGNAL] Emit ModuleShock::signalCharged()" << Qt::endl;
    emit signalCharged();                       // Makes the button flash

    qDebug() << "[SIGNAL} Emit ModuleShock::signalForwardUserPrompt(" << P_CLEAR << ")" << Qt::endl;
    emit signalForwardUserPrompt(P_CLEAR);      // CLEAR!

    qDebug() << "[EXIT SLOT] ModuleShock::chargeReady()" << Qt::endl;
}

// SLOT
void ModuleShock::start(bool usingChildPads)
{
    qDebug().noquote() << "[ENTRY SLOT] ModuleShock::start(" << usingChildPads << ")" << Qt::endl;

    active = true;
    energySet = calcShockEnergy(usingChildPads);
    connect(&timer, SIGNAL(timeout()), this, SLOT(chargeReady()));
    inChargeDelay = true;
    timer.start(CHARGE_TIME);

    qDebug().noquote() << "[EXIT SLOT] ModuleShock::start(" << usingChildPads << ")" << Qt::endl;
}

// SLOT
void ModuleShock::shockButtonPressed()
{
    qDebug() << "[ENTRY SLOT] ModuleShock::shockButtonPressed()" << Qt::endl;

    if(!active)
    {
        qDebug() << "[EXIT SLOT] ModuleShock::shockButtonPressed() : button pressed but not during shock phase" << Qt::endl;
        return; // Shock button does nothing when not in defib mode
    }
    if(!charged)
    {
        qDebug() << "[INFO] ModuleShock::shockButtonPressed() : Charge was not ready" << Qt::endl;
        // Defib not charged: "Release shock button"
        buttonPressedEarly = true;

        qDebug() << "[SIGNAL] Emit ModuleShock::signalForwardUserPrompt(" << P_RELEASE << ")" << Qt::endl;
        emit signalForwardUserPrompt(P_RELEASE);
    }
    else
    {
        // ZZZZAP!

        qDebug().noquote() << "[SIGNAL] Emit ModuleShock::signalDrainBatt("
                 << QString::number((BATT_RATIO * energySet), 'g', 2)  << ")" << Qt::endl;
        // Assume battery drain when shock is actually delivered. If unit was switched off without delivering shock,
        // let's assume that it can just go back into the battery. (We don't have the information/expertise to know
        // how or whether an unused, charged shock would be discharged from the capacitors.)
        emit signalDrainBatt(BATT_RATIO * energySet);

        qDebug().noquote() << "[SIGNAL] Emit ModuleShock::signalShockDelivered(" << QString::number(shocksDelivered+1) << ")" << Qt::endl;
        emit signalShockDelivered(++shocksDelivered);

        qDebug() << "[SIGNAL] Emit ModuleShock::signalScheduleShockResolution()" << Qt::endl;
        emit signalScheduleShockResolution();
    }
    qDebug() << "[EXIT SLOT] ModuleShock::shockButtonPressed()" << Qt::endl;
}

// SLOT
void ModuleShock::resolveShock()
{
    qDebug() << "[ENTRY SLOT] ModuleSHock::resolveShock()" << Qt::endl;

    qDebug() << "[SIGNAL] Emit ModuleShock::signalForwardUserPrompt(" << P_DELIVERED << ")" << Qt::endl;
    emit signalForwardUserPrompt(P_DELIVERED);

    // Brief delay on exit to avoid overwriting "Shock delivered" with "Start CPR" immediately
    connect(&timer, SIGNAL(timeout()), this, SLOT(exit()));
    inExitDelay = true;
    timer.start(STOP_TIME);

    qDebug() << "[EXIT SLOT] ModuleSHock::resolveShock()" << Qt::endl;
}

// SLOT
void ModuleShock::shockButtonReleased()
{
    qDebug().noquote() << "[ENTRY SLOT] ModuleShock::shockButtonReleased() : buttonPressedEarly=" << buttonPressedEarly << Qt::endl;

    if(buttonPressedEarly)
    {
        qDebug() << "[SIGNAL] Emit ModuleShock::signalForwardUserPrompt(P_BLANK)" << Qt::endl;
        // Blank out the "release shock button" prompt
        emit signalForwardUserPrompt(P_BLANK);
        buttonPressedEarly = false;
    }

    qDebug().noquote() << "[EXIT SLOT] ModuleShock::shockButtonReleased()" << Qt::endl;
}

// SLOT
void ModuleShock::exit()
{
    qDebug().noquote() << "[ENTRY SLOT] ModuleShock::exit()" << Qt::endl;

    cleanup();

    qDebug() << "[SIGNAL] Emit ModuleShock::signalDone()" << Qt::endl;
    emit signalDone();

    qDebug().noquote() << "[EXIT SLOT] ModuleShock::exit()" << Qt::endl;
}

// SLOT
void ModuleShock::abort()
{
    qDebug().noquote() << "[ENTRY SLOT] ModuleShock::abort()" << Qt::endl;

    cleanup();

    qDebug() << "[SIGNAL] Emit ModuleShock::signalAborted()" << Qt::endl;
    emit signalAborted();

    qDebug().noquote() << "[EXIT SLOT] ModuleShock::abort()" << Qt::endl;
}

// SLOT
void ModuleShock::fullReset()
{
    qDebug().noquote() << "[ENTRY SLOT] ModuleShock::fullReset()" << Qt::endl;

    reset();

    qDebug().noquote() << "[EXIT SLOT] ModuleShock::fullReset()" << Qt::endl;
}

void ModuleShock::cleanup()
{
    qDebug().noquote() << "[ENTRY] ModuleShock::cleanup()" << Qt::endl;

    timer.blockSignals(true);
    timer.stop();
    timer.blockSignals(false);

    if(inChargeDelay)
    {
        disconnect(&timer, SIGNAL(timeout()), this, SLOT(chargeReady()));
    }
    else if(inExitDelay)
    {
        disconnect(&timer, SIGNAL(timeout()), this, SLOT(exit()));
    }

    energySet = 0;
    active = false;
    charged = false;
    buttonPressedEarly = false;
    inChargeDelay = false;
    inExitDelay = false;

    qDebug().noquote() << "[EXIT] ModuleShock::cleanup()" << Qt::endl;
}

void ModuleShock::reset()
{
    qDebug().noquote() << "[ENTRY] ModuleShock::reset()" << Qt::endl;

    cleanup();
    shocksDelivered = 0;

    qDebug().noquote() << "[EXIT] ModuleShock::reset()" << Qt::endl;
}


