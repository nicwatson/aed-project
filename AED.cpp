#include "AED.h"
#include "prompts.h"

using namespace aed;

AED::AED()
{

}

AED::~AED()
{

}

void AED::startSelfTest()
{
    // Invoke (signal?) the self-test module
    // Wait for response to slot selfTestResult(testResult_t)
}


void AED::turnOn() {}
void AED::turnOff() {}

void AED::setBattery(float newBatt) {}
void AED::eatBattery(float loseBatt) {}

void AED::userPrompt(const QString & prompt) {}


// SLOT
void AED::selfTestResult(ModuleSelfTest::testResult_t result)
{
    switch(result)
    {
        case ModuleSelfTest::OK:
            userPrompt(P_UNIT_OK);
            aedState = STARTUP_ADVICE;
            timer.setSingleShot(true);
            timer.setInterval(DURATION_UNIT_OK);
            connect(&timer, SIGNAL(timeout()), this, SLOT(doStartupAdvice()));
            timer.start();
            break;
        case ModuleSelfTest::FAIL_BATTERY:
            userPrompt(P_CHANGE_BATT);
            aedState = FAILURE;
            break;
        case ModuleSelfTest::FAIL_CABLE:
            userPrompt(P_PLUG);
            aedState = FAILURE;
            break;
        case ModuleSelfTest::FAIL_OTHER:
        default:
            userPrompt(P_UNIT_FAILED);
            aedState = FAILURE;
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
