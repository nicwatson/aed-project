#include "AED.h"
#include "aedGui/prompts.h"

using namespace aedModel;


//
// EXECUTORS
// for event progression
//

void AED::doSelfTest()
{
    emit signalStartTest();
    // Invoke (signal?) the self-test module
    // Wait for response to slot selfTestResult(testResult_t)
}

void AED::doStartupAdvice()
{
    emit signalStartupAdvice(cableState);
}

void AED::doStartECG()
{
    emit signalStartECG();
}

void AED::doPrepShock()
{
    emit signalPrepShock();
}


//
// STATE TRANSITION LOGIC
//

void AED::changeState(state_t newState)
{
    // TODO
    aedState = newState;
}

bool AED::changeStateSafe(state_t newState)
{
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

    // Any cable change that happens while unit is on and not in self-test should trigger a return to self-test
    // Otherwise, cable state can change "silently" (without side-effects)
    if(aedState != OFF && aedState != SELF_TEST)
    {
        changeStateSafe(SELF_TEST);     // MOAR TODO HERE
    }

    cableState = newCableState;
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








