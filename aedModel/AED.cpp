// FILE: AED.cpp
// AED class - general function defs

#include "AED.h"
#include "aedGui/strings.h"

using namespace aedModel;


//
// STATE TRANSITION LOGIC
//

void AED::changeState(state_t newState)
{
    qDebug() << "[ENTRY] AED::changeState() from " << stateNames[aedState] << " to  " << stateNames[newState] << Qt::endl;
    aedState = newState;
    qDebug() << "[EXIT] AED::changeState() from " << stateNames[aedState] << " to  " << stateNames[newState] << Qt::endl;
}

bool AED::changeStateSafe(state_t newState)
{
    qDebug() << "[ENTRY] AED::changeStateSafe() from " << stateNames[aedState] << " to  " << stateNames[newState] << Qt::endl;
    bool result = legalStateChange(newState);
    if(result) changeState(newState);
    qDebug() << "[EXIT] AED::changeStateSafe() from " << stateNames[aedState] << " to  " << stateNames[newState]
             << " : result from legalStateChange() was: " << result << Qt::endl;
    return result;
}

bool AED::legalStateChange(state_t newState)
{
    qDebug() << "[ENTRY/EXIT] AED::legalStateChange() from " << stateNames[aedState] << " to  " << stateNames[newState] << Qt::endl;
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


void AED::scheduleECG()
{
    qDebug() << "[ENTRY] AED::scheduleECG()" << Qt::endl;

    qDebug() << "[SIGNAL] Emit AED::signalStopLampCPR()" << Qt::endl;
    emit signalStopLampCPR();

    qDebug() << "[SIGNAL] Emit AED::signalStartLampStandback()" << Qt::endl;
    emit signalStartLampStandback();

    // After CPR, introduce a short delay to give time for the "STOP CPR" prompt to be read
    // Before it gets replaced by "DON'T TOUCH PATIENT"
    timer.setSingleShot(true);
    timer.setInterval(AED_TIMER_DEFAULT);
    connect(&timer, &QTimer::timeout, this, &AED::restartECG);
    timer.start();

    qDebug() << "[EXIT] AED::scheduleECG()" << Qt::endl;
}

void AED::scheduleCPR()
{
    qDebug() << "[ENTRY] AED::scheduleCPR()" << Qt::endl;

    qDebug() << "[SIGNAL] Emit AED::signalStopLampStandback()" << Qt::endl;
    emit signalStopLampStandback();

    qDebug() << "[SIGNAL] Emit AED::signalStartLampCPR()" << Qt::endl;
    emit signalStartLampCPR();



    // After shock or non-shockable ECG result, introduce a short delay to give time for the
    // "SHOCK DELIVERED" or "NO SHOCK ADVISED" message to be read, before replacing it with "START CPR"
    timer.setSingleShot(true);
    timer.setInterval(AED_TIMER_DEFAULT);
    connect(&timer, &QTimer::timeout, this, &AED::startCPR);
    timer.start();

    qDebug() << "[EXIT] AED::scheduleCPR()" << Qt::endl;
}


bool AED::changeCableState(cableState_t newCableState)
{
    qDebug() << "[ENTRY] AED::changeCableState() from " << cableStateNames[cableState] << " to  " << cableStateNames[newCableState] << Qt::endl;

    // Nothing to do if new cable state is same as old cable state
    if(newCableState == cableState)
    {
        qDebug() << "[EXIT] AED::changeCableState() : cable state unchanged" << Qt::endl;
        return false;
    }

    cableState = newCableState;
    // Any cable change that happens while unit is on should trigger a return to self-test
    // Otherwise, cable state can change "silently" (without side-effects)
    if(aedState != OFF)
    {
        stopActivity();
        doSelfTest();
    }

    qDebug() << "[EXIT] AED::changeCableState() from " << cableStateNames[cableState] << " to  " << cableStateNames[newCableState] << Qt::endl;
    return true;
}


//
// POWER CYCLE
//

void AED::turnOn()
{
    qDebug() << "[ENTRY] AED::turnOn()" << Qt::endl;
    doSelfTest();
    qDebug() << "[EXIT] AED::turnOn()" << Qt::endl;
}

void AED::turnOff()
{
    qDebug() << "[ENTRY] AED::turnOff()" << Qt::endl;

    stopActivity();
    clearPrompt();

    qDebug() << "[SIGNAL] Emit AED::signalPowerOff()" << Qt::endl;
    emit signalPowerOff();
    changeStateSafe(OFF);

    qDebug() << "[EXIT] AED::turnOff()" << Qt::endl;
}

void AED::stopActivity()
{
    qDebug() << "[ENTRY] AED::stopActivity()" << Qt::endl;

    abortAll();     // Make all Modules stop what they are doing

    // Turn off AED-managed pictogram lamps
    qDebug() << "[SIGNAL] Emit AED::signalStopLampStandback()" << Qt::endl;
    emit signalStopLampStandback();
    qDebug() << "[SIGNAL] Emit AED::signalStopLampCPR()" << Qt::endl;
    emit signalStopLampCPR();

    // Stop the AED's event timer (which may or may not be running)
    timer.blockSignals(true);
    timer.stop();
    timer.blockSignals(false);

    qDebug() << "[EXIT] AED::stopActivity()" << Qt::endl;
}


//
// EXECUTORS
// for event progression
//

void AED::doSelfTest()
{
    qDebug() << "[ENTRY] AED::doSelfTest()" << Qt::endl;
    changeStateSafe(SELF_TEST);

    qDebug() << "[SIGNAL] Emit AED::signalUserPrompt(" << P_TESTING << ")" << Qt::endl;
    emit signalUserPrompt(P_TESTING);
    qDebug() << "[SIGNAL] Emit AED::signalStartTest(AED *)" << Qt::endl;
    emit signalStartTest(this);

    qDebug() << "[EXIT] AED::doSelfTest()" << Qt::endl;
}

void AED::doStartupAdvice()
{
    qDebug() << "[ENTRY] AED::doStartupAdvice()" << Qt::endl;

    changeStateSafe(STARTUP_ADVICE);

    qDebug().noquote() << "[SIGNAL] Emit AED::signalStartupAdvice(" << cableStateNames[cableState] << ")" << Qt::endl;
    emit signalStartupAdvice(cableState);

    qDebug() << "[EXIT] AED::doStartupAdvice()" << Qt::endl;
}

void AED::doStartECG()
{
    qDebug() << "[ENTRY] AED::doStartECG()" << Qt::endl;
    changeStateSafe(ECG_ASSESS);
    emit signalStartLampStandback();
    emit signalStartECG();
    qDebug() << "[EXIT] AED::doStartECG()" << Qt::endl;
}

void AED::doPrepShock()
{
    qDebug() << "[ENTRY] AED::doPrepShock()" << Qt::endl;
    changeStateSafe(SHOCK);
    emit signalStartLampStandback();
    emit signalPrepShock(cableState == PAD_CHILD);
    qDebug() << "[EXIT] AED::doPrepShock()" << Qt::endl;
}

void AED::doStartCPR()
{
    qDebug() << "[ENTRY] AED::doStartCPR()" << Qt::endl;
    changeStateSafe(CPR);

    qDebug() << "[SIGNAL] Emit AED::signalStopLampStandback()" << Qt::endl;
    emit signalStopLampStandback();

    qDebug() << "[SIGNAL] Emit AED::signalStartLampCPR()" << Qt::endl;
    emit signalStartLampCPR();
    qDebug().noquote() << "[SIGNAL] Emit AED::signalStartCPR(" << cableStateNames[cableState] << ")" << Qt::endl;
    emit signalStartCPR(cableState);

    qDebug() << "[ENTRY] AED::doStartCPR()" << Qt::endl;
}


//
// OUTPUTS
//

void AED::errorBattery()
{
    qDebug() << "[ENTRY] AED::errorBattery()" << Qt::endl;
    userPrompt(P_CHANGE_BATT);
    qDebug() << "[EXIT] AED::errorBattery()" << Qt::endl;
}

void AED::errorCable()
{
    qDebug() << "[ENTRY] AED::errorCable()" << Qt::endl;
    userPrompt(P_PLUG);
    qDebug() << "[EXIT] AED::errorCable()" << Qt::endl;
}

void AED::errorOther()
{
    qDebug() << "[ENTRY] AED::errorOther()" << Qt::endl;
    userPrompt(P_UNIT_FAILED);
    qDebug() << "[EXIT] AED::errorOther()" << Qt::endl;
}

void AED::failAED()
{
    qDebug() << "[ENTRY] AED::failAED()" << Qt::endl;
    changeStateSafe(FAILURE);
    stopActivity();

    qDebug() << "[SIGNAL] Emit AED::signalUnitFailed()" << Qt::endl;
    emit signalUnitFailed();

    qDebug() << "[EXIT] AED::failAED()" << Qt::endl;
}

void AED::clearPrompt()
{
    qDebug() << "[ENTRY] AED::clearPrompt()" << Qt::endl;
    userPrompt(P_BLANK);
    qDebug() << "[EXIT] AED::clearPrompt()" << Qt::endl;
}

void AED::abortAll()
{
    qDebug() << "[ENTRY] AED::abortAll()" << Qt::endl;

    qDebug() << "[SIGNAL] Emit AED::signalAbortSelfTest()" << Qt::endl;
    emit signalAbortSelfTest();
    qDebug() << "[SIGNAL] Emit AED::signalAbortStartupAdvice()" << Qt::endl;
    emit signalAbortStartupAdvice();
    qDebug() << "[SIGNAL] Emit AED::signalAbortECG()" << Qt::endl;
    emit signalAbortECG();
    qDebug() << "[SIGNAL] Emit AED::signalAbortShock()" << Qt::endl;
    emit signalAbortShock();
    qDebug() << "[SIGNAL] Emit AED::signalAbortCPR()" << Qt::endl;
    emit signalAbortCPR();

    qDebug() << "[EXIT] AED::abortAll()" << Qt::endl;
}









