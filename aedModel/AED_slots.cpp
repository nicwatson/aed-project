// AED class - slot function defs

#include "AED.h"
#include "aedGui/prompts.h"

using namespace aedModel;


//
// POWER
//

void AED::turnOn()
{
    aedState = SELF_TEST;
    emit signalStartTest(this);
}

void AED::turnOff()
{
    // TODO Need to reset all modules and turn off widgets
    stopActivity();
    clearPrompt();
    emit signalPowerOff();
    aedState = OFF;
}

//
// CABLE & PADS
//

void AED::plugCable(cableState_t newCableState)
{
    // changeCableState determines if it is necessary to go back to self test
    changeCableState(newCableState);
}

void AED::plugCableAdult()
{
    changeCableState(PAD_ADULT);
}

void AED::plugCableChild()
{
    changeCableState(PAD_CHILD);
}

void AED::unplugCable()
{
    changeCableState(UNPLUGGED);
}

void AED::attachPads(bool attached)
{
    if(padsAttached == attached) return;   // Pads unchanged

    padsAttached = attached;

    if(attached)
    {
        // Pads are being attached

        // This tells the ModuleStartupAdvice that pads have been attached
        emit signalPadsAttached();

        if(aedState == STARTUP_ADVICE || aedState == ECG_ASSESS || aedState == SHOCK)
        {
            // Either we are short-circuiting STARTUP_ADVICE because pads were attached,
            // or we are reattaching pads that lost connectivity during ECG_ASSESS or SHOCK.
            // In any case, we go to the start of ECG_ASSESS.
            changeStateSafe(ECG_ASSESS);
            doStartECG();
            return;
        }

    }
    else
    {
        // Pads are being detached
        if(aedState == ECG_ASSESS)
        {
            // Pad connectivity error
            emit signalStopLampStandback();
            emit signalAbortECG();
            emit signalUserPrompt(P_CHECK_ELEC);
        }
        else if(aedState == SHOCK)
        {
            // Pad connectivity error
            emit signalStopLampStandback();
            emit signalAbortShock();
            emit signalUserPrompt(P_CHECK_ELEC);
        }
        // In other states, it doesn't matter if the electrodes get detached from the patient.
    }
}

void AED::attachPads()
{
    attachPads(true);
}

void AED::removePads()
{
    attachPads(false);
}


//
// BATTERIES
//

void AED::setBattery(double newBatt)
{
    if(newBatt >= 0) battery = newBatt;
    emit signalBatteryChanged(battery);

    if(battery < BATTERY_THRESHHOLD)
    {
        failAED();
        errorBattery();
    }
}

void AED::useBattery(double loseBatt)
{
    if(loseBatt >= 0)
    {
        setBattery(std::max(0.0, battery - loseBatt));
    }
}

void AED::changeBatteries()
{
    if(isOn())
    {
        turnOff();
    }
    battery = 1;
    emit signalBatteryChanged(battery);
}


void AED::selfTestResult(ModuleSelfTest::testResult_t result)
{
    switch(result)
    {
        case ModuleSelfTest::OK:
            emit signalDisplayPassTest();
            changeStateSafe(STARTUP_ADVICE);
            doStartupAdvice();
            break;
        case ModuleSelfTest::FAIL_BATTERY:
            failAED();
            errorBattery();
            break;
        case ModuleSelfTest::FAIL_CABLE:
            failAED();
            errorCable();
            break;
        case ModuleSelfTest::FAIL_OTHER:
        default:
            failAED();
            errorOther();
    }
}

void AED::ecgResult(bool shockable)
{
    // Entry guard: must be in ECG_ASSESS state
    if(aedState != ECG_ASSESS)
    {
        qDebug() << "Error: AED received ecgResult signal when ECG_ASSESS was not active!" << Qt::endl;
        return;
    }

    if(shockable)
    {
        emit signalUserPrompt(P_SHOCKABLE);
        doPrepShock();
    }
    else
    {
        // Not shockable
        emit signalUserPrompt(P_NOTSHOCKABLE);
        emit signalStopLampStandback();
        emit signalStartLampCPR();
        changeStateSafe(CPR);

        // Introduce a short delay to give time for the "NOT SHOCKABLE" message to be read
        // Before it gets replaced by "START CPR"
        timer.setSingleShot(true);
        timer.setInterval(TIMER_STANDARD_DELAY);
        connect(&timer, SIGNAL(timeout()), this, SLOT(startCPR()));
        timer.start();
    }
}

void AED::shockDelivered()
{
    // Introduce a short delay to give time for the "SHOCK DELIVERED" message to be read
    // Before it gets replaced by "START CPR"
    timer.setSingleShot(true);
    timer.setInterval(TIMER_STANDARD_DELAY);
    connect(&timer, SIGNAL(timeout()), this, SLOT(startCPR()));
    timer.start();
}


void AED::startCPR()
{
    disconnect(&timer, SIGNAL(timeout()), this, SLOT(startCPR()));
    doStartCPR();
}

void AED::cprStopped()
{
    emit signalStopLampCPR();
    if(aedState != CPR)
    {
        qDebug() << "Error: AED received signal stopCPR() when CPR was not active." << Qt::endl;
        return;
    }

    changeStateSafe(ECG_ASSESS);

    // Introduce a short delay to give time for the "STOP CPR" prompt to be read
    // Before it gets replaced by "DON'T TOUCH PATIENT"
    timer.setSingleShot(true);
    timer.setInterval(TIMER_STANDARD_DELAY);
    connect(&timer, SIGNAL(timeout()), this, SLOT(restartECG()));
    timer.start();
}

void AED::restartECG()
{
    disconnect(&timer, SIGNAL(timeout()), this, SLOT(restartECG()));
    doStartECG();
}



//
// USER PROMPTS
//

void AED::userPrompt(const QString & prompt)
{
    if(aedState == OFF) return;
    emit signalUserPrompt(prompt);
}

