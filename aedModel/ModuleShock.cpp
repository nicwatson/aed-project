#include "ModuleShock.h"
#include "aedGui/prompts.h"

using namespace aedModel;

// Increasing successive shock energies as per Admin Guide, page 6.
const int ModuleShock::ADULT_SHOCKS[] = { 120, 150, 200 };
const int ModuleShock::CHILD_SHOCKS[] = { 50, 70, 85 };

ModuleShock::ModuleShock() : QObject()
{
    active = false;
    energySet = 0;
    charged = false;
    buttonPressedEarly = false;
    inChargeDelay = false;
    inExitDelay = false;

    timer.setSingleShot(true);
}

int ModuleShock::calcShockEnergy(bool usingChildPads)
{
    if(usingChildPads)
    {
        return CHILD_SHOCKS[std::max(shocksDelivered, 2)];
    }
    return ADULT_SHOCKS[std::max(shocksDelivered, 2)];
}

void ModuleShock::chargeReady()
{
    disconnect(&timer, SIGNAL(timeout()), this, SLOT(chargeReady()));
    inChargeDelay = false;
    charged = true;

    emit signalCharged();               // Makes the button flash
    emit signalUserPrompt(P_CLEAR);     // CLEAR!
}

void ModuleShock::start(bool usingChildPads)
{
    active = true;
    energySet = calcShockEnergy(usingChildPads);
    connect(&timer, SIGNAL(timeout()), this, SLOT(chargeReady()));
    inChargeDelay = true;
    timer.start(CHARGE_TIME);
}

void ModuleShock::shockButtonPressed()
{
    if(!active) return; // Shock button does nothing when not in defib mode
    if(!charged)
    {
        // Defib not charged: "Release shock button"
        buttonPressedEarly = true;
        emit signalUserPrompt(P_RELEASE);
    }
    else
    {
        // ZZZZAP!

        // Assume battery drain when shock is actually delivered. If unit was switched off without delivering shock,
        // let's assume that it can just go back into the battery. (We don't have the information/expertise to know
        // how or whether an unused, charged shock would be discharged from the capacitors.)
        emit signalDrainBatt(BATT_RATIO * energySet);

        emit signalShockDelivered(++shocksDelivered);
        emit signalUserPrompt(P_DELIVERED);
        connect(&timer, SIGNAL(timeout()), this, SLOT(exit()));
        inExitDelay = true;
        timer.start(STOP_TIME);
    }
}

void ModuleShock::shockButtonReleased()
{
    if(buttonPressedEarly)
    {
        emit signalUserPrompt(P_BLANK);
        buttonPressedEarly = false;
    }
}

void ModuleShock::exit()
{
    cleanup();
    emit signalDone();
}

void ModuleShock::abort()
{
    cleanup();
    emit signalAborted();
}

void ModuleShock::cleanup()
{
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
}

void ModuleShock::reset()
{
    cleanup();
    shocksDelivered = 0;
}
