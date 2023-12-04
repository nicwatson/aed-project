// AED class - general function defs

#include "AED.h"
#include "aedGui/strings.h"

using namespace aedModel;


//
// EXECUTORS
// for event progression
//

void AED::doSelfTest()
{
    qDebug() << "HELLO" << Qt::endl;
    changeStateSafe(SELF_TEST);
    signalUserPrompt(P_TESTING);
    emit signalStartTest(this);
}

void AED::doStartupAdvice()
{
    qDebug() << "Starting startup advice by AED emitting signal to ModuleStartupdvice";
    changeStateSafe(STARTUP_ADVICE);
    emit signalStartupAdvice(cableState);
}

void AED::doStartECG()
{
    changeStateSafe(ECG_ASSESS);
    emit signalStartLampStandback();
    emit signalStartECG();
}

void AED::doPrepShock()
{
    changeStateSafe(SHOCK);
    emit signalStopLampStandback();
    emit signalStartLampStandback();
    emit signalPrepShock(cableState == PAD_CHILD);
}

void AED::doStartCPR()
{
    changeStateSafe(CPR);
    emit signalStartLampCPR();
    emit signalStartCPR(cableState);
}

//
// STATE TRANSITION LOGIC
//

void AED::changeState(state_t newState)
{
    qDebug() << "AED transitioning from state " << stateNames[aedState] << " to state " << stateNames[newState] << Qt::endl;
    aedState = newState;
}

bool AED::changeStateSafe(state_t newState)
{
    qDebug() << QString("Changing state to %1").arg(newState);
    bool result = legalStateChange(newState);
    if(result) changeState(newState);
    return result;
}

bool AED::legalStateChange(state_t newState)
{
    switch(newState)
    {
        case OFF:               // Unit can be turned off from any state
        case SELF_TEST:         // Holding pwr button or unplugging cable can trigger self-test from any state
        case FAILURE:           // Unit can go into failure from any state (e.g. low battery, cable unplugged)
            return true;
        case STARTUP_ADVICE:    // STARTUP_ADVICE may only follow a SELF_TEST
            return aedState == SELF_TEST;
        case ECG_ASSESS:        // ECG_ASSESS may follow STARTUP_ADVICE (the first time) or follow a CPR cycle
            return aedState == STARTUP_ADVICE || aedState == CPR;
        case SHOCK:             // Can only shock after an ECG_ASSESS
            return aedState == ECG_ASSESS;
        case CPR:               // CPR is either directly after ECG_ASSESS (rhythm not shockable) or after SHOCK
            return aedState == ECG_ASSESS || aedState == SHOCK;
    }
    return false;
}

bool AED::changeCableState(cableState_t newCableState)
{
    // Nothing to do if new cable state is same as old cable state
    if(newCableState == cableState) return false;

    cableState = newCableState;
    // Any cable change that happens while unit is on should trigger a return to self-test
    // Otherwise, cable state can change "silently" (without side-effects)
    if(aedState != OFF)
    {
        stopActivity();
        doSelfTest();
    }

    return true;
}


//
// OUTPUTS
//

void AED::errorBattery()
{
    userPrompt(P_CHANGE_BATT);
}

void AED::errorCable()
{
    userPrompt(P_PLUG);
}

void AED::errorOther()
{
    userPrompt(P_UNIT_FAILED);
}

void AED::failAED()
{
    changeStateSafe(FAILURE);
    stopActivity();
    emit signalUnitFailed();
}

void AED::clearPrompt()
{
    userPrompt(P_BLANK);
}

void AED::stopActivity()
{
    emit signalAbortAll();
    emit signalStopLampStandback();
    emit signalStopLampCPR();
    timer.blockSignals(true);
    timer.stop();
    timer.blockSignals(false);
}







