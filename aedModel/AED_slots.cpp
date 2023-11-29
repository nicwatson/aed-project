#include "AED.h"

using namespace aedModel;


//
// EVENT PROGRESSION
//

void AED::turnOn()
{
    aedState = SELF_TEST;
    emit signalStartTest();
}

void AED::turnOff()
{
    aedState = OFF;
}

void AED::selfTestResult(ModuleSelfTest::testResult_t result)
{
    switch(result)
    {
        case ModuleSelfTest::OK:
            changeStateSafe(STARTUP_ADVICE);
            doStartupAdvice();
            break;
        case ModuleSelfTest::FAIL_BATTERY:
            changeStateSafe(FAILURE);
            errorBattery();
            break;
        case ModuleSelfTest::FAIL_CABLE:
            changeStateSafe(FAILURE);
            errorCable();
            break;
        case ModuleSelfTest::FAIL_OTHER:
        default:
            changeStateSafe(FAILURE);
            errorOther();
    }
}

void AED::attachPads(bool attached)
{
    if(padsAttached == attached) return;   // Pads unchanged
    padsAttached = attached;

    if(padsAttached)
    {
        emit signalPadsAttached();
    }

    if(aedState == STARTUP_ADVICE)
    {
        changeStateSafe(ECG_ASSESS);
       // doStartECG();
    }
    else if(aedState == ECG_ASSESS)
    {
        // Pad attachment error state
    }
    else if(aedState == SHOCK)
    {
        // Pad attachment error state
    }

    // Note:
    // Only other stage the pads matter for is CPR, because they contain the RealCPR Help sensors.
    // However, if the pad is removed during CPR, the sensors just won't provide valid feedback to the user.
    // The AED won't complain about the pads being off until it tries to do an ECG again.
}

void AED::ecgResult(bool shockable)
{
    if(shockable)
    {
        changeStateSafe(SHOCK);
        // doPrepShock();
    }
    else
    {
        changeStateSafe(CPR);
        // doStartCPR();
    }
}

void AED::startCPR()
{

}

void AED::stopCPR()
{
    changeStateSafe(ECG_ASSESS);
    // doStartECG();
}


//
// CABLE
//

void AED::plugCable(cableState_t newCableState)
{
    changeCableState(newCableState);
}

//
// BATTERIES
//

void AED::setBattery(float newBatt)
{
    if(newBatt >= 0) battery = newBatt;
    emit batteryChanged(battery);
}

void AED::useBattery(float loseBatt)
{
    if(loseBatt >= 0)
    {
        setBattery(std::max(0.0f, battery - loseBatt));
    }
}

void AED::changeBatteries()
{
    if(isOn())
    {
        turnOff();
    }
    battery = 1;
    emit batteryChanged(battery);
}


//
// USER PROMPTS
//

void AED::userPrompt(const QString & prompt)
{
    if(aedState == OFF) return;
    // TO DO: Make it update a label in the LCD on the GUI

    qDebug().noquote().nospace() << "[TODO] VOICE PROMPT: \"" << prompt << "\"\n" << Qt::endl;
}

void AED::clearPrompt()
{
    // TO DO: Make it blank out the LCD label
    qDebug() << "[TODO] Clearing prompt" << Qt::endl;
}
