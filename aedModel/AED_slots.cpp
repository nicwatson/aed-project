// FILE: AED_slots.cpp
// AED class - slot function defs

#include "AED.h"
#include "aedGui/strings.h"
#include "ModuleSelfTest.h"

using namespace aedModel;


//
// POWER
//

void AED::togglePowerButton()
{
    qDebug() << "[ENTRY SLOT] AED::togglePowerButton()" << Qt::endl;

    if (aedState == OFF) // If aed is OFF, turn it on
    {
        turnOn();
    }
    else // If aed is in one of the 'on' states, turn it off
    {
        turnOff();
    }

    qDebug() << "[EXIT SLOT] AED::togglePowerButton()" << Qt::endl;
}

//
// CABLE & PADS
//

void AED::plugCable(cableState_t newCableState)
{
    qDebug().noquote() << "[ENTRY SLOT] AED::plugCable(" << cableStateNames[newCableState] << ")" << Qt::endl;

    // changeCableState determines if it is necessary to go back to self test
    changeCableState(newCableState);

    qDebug().noquote() << "[EXIT SLOT] AED::plugCable(" << cableStateNames[newCableState] << ")" << Qt::endl;
}

void AED::plugCableAdult()
{
    qDebug() << "[ENTRY SLOT] AED::plugCableAdult()" << Qt::endl;
    changeCableState(PAD_ADULT);
    qDebug() << "[EXIT SLOT] AED::plugCableAdult()" << Qt::endl;
}

void AED::plugCableChild()
{
    qDebug() << "[ENTRY SLOT] AED::plugCableChild()" << Qt::endl;
    changeCableState(PAD_CHILD);
    qDebug() << "[EXIT SLOT] AED::plugCableChild()" << Qt::endl;
}

void AED::unplugCable()
{
    qDebug() << "[ENTRY SLOT] AED::unplugCable()" << Qt::endl;
    changeCableState(UNPLUGGED);
    qDebug() << "[EXIT SLOT] AED::unplugCable()" << Qt::endl;
}

void AED::attachPads(bool attached)
{
    qDebug().noquote() << "[ENTRY SLOT] AED::attachPads(" << attached << ")" << Qt::endl;

    if(padsAttached == attached)
    {
        qDebug().noquote() << "[EXIT SLOT] AED::attachPads(" << attached << "): no change" << Qt::endl;
        return;   // Pads unchanged
    }

    padsAttached = attached;

    if(attached)
    {
        // Pads are being attached

        // This tells the ModuleStartupAdvice that pads have been attached

        qDebug() << "[SIGNAL] Emit AED::signalPadsAttached()" << Qt::endl;
        emit signalPadsAttached();

        if(aedState == STARTUP_ADVICE || aedState == ECG_ASSESS || aedState == SHOCK)
        {
            // Either we are short-circuiting STARTUP_ADVICE because pads were attached,
            // or we are reattaching pads that lost connectivity during ECG_ASSESS or SHOCK.
            // In any case, we go to the start of ECG_ASSESS.
            doStartECG();

            qDebug().noquote() << "[EXIT SLOT] AED::attachPads(" << attached << "): pads reattached, recovered to ECG" << Qt::endl;

            return;
        }

    }
    else
    {
        // Pads are being detached
        if(aedState == ECG_ASSESS)
        {
            // Pad connectivity error

            qDebug() << "[SIGNAL] Emit AED::signalStopLampStandback()" << Qt::endl;
            emit signalStopLampStandback();
            qDebug() << "[SIGNAL] Emit AED::signalAbortECG()" << Qt::endl;
            emit signalAbortECG();
            userPrompt(P_CHECK_ELEC);
        }
        else if(aedState == SHOCK)
        {
            // Pad connectivity error
            qDebug() << "[SIGNAL] Emit AED::signalStopLampStandback()" << Qt::endl;
            emit signalStopLampStandback();
            qDebug() << "[SIGNAL] Emit AED::signalAbortShock()" << Qt::endl;
            emit signalAbortShock();
            userPrompt(P_CHECK_ELEC);
        }
        // In other states, it doesn't matter if the electrodes get detached from the patient.
        qDebug().noquote() << "[EXIT SLOT] AED::attachPads(" << attached << "): pads detached" << Qt::endl;
    }
}



//
// BATTERIES
//

void AED::setBattery(double newBatt)
{
    qDebug().noquote() << "[ENTRY SLOT] AED::setBattery(" << QString::number(newBatt, 'g', 2)
                       << "): prev batt=" << QString::number(battery, 'g', 2) << Qt::endl;

    if(newBatt >= 0) battery = newBatt;

    qDebug().noquote() << "[SIGNAL] Emit AED::signalBatteryLevelChanged(" << QString::number(battery, 'g', 2) << ")" << Qt::endl;
    emit signalBatteryLevelChanged(battery);

    if(battery < BATTERY_THRESHHOLD)
    {
        failAED();
        errorBattery();
    }

    qDebug().noquote() << "[EXIT SLOT] AED::setBattery(" << QString::number(newBatt, 'g', 2) << ")" << Qt::endl;
}

void AED::useBattery(double loseBatt)
{
    qDebug().noquote() << "[ENTRY SLOT] AED::useBattery(" << QString::number(loseBatt, 'g', 2)
                       << "): prev batt=" << QString::number(battery, 'g', 2) << Qt::endl;

    if(loseBatt >= 0)
    {
        setBattery(std::max(0.0, battery - loseBatt));
    }

    qDebug().noquote() << "[EXIT SLOT] AED::useBattery(" << QString::number(loseBatt, 'g', 2) << ")" << Qt::endl;
}

void AED::changeBatteries()
{
    qDebug().noquote() << "[ENTRY SLOT] AED::changeBatteries()" << Qt::endl;

    if(isOn())
    {
        turnOff(); // turn it off

        qDebug().noquote() << "[SIGNAL] AED::signalBatteriesPulled()" << Qt::endl;
        emit AED::signalBatteriesPulled();
    }
    battery = 1;

    qDebug().noquote() << "[SIGNAL] Emit AED::signalBatteryChanged(" << QString::number(battery, 'g', 2) << ")" << Qt::endl;
    emit signalBatteryLevelChanged(battery);

    qDebug().noquote() << "[EXIT SLOT] AED::changeBatteries()" << Qt::endl;
}


void AED::selfTestResult(ModuleSelfTest::testResult_t result)
{
    qDebug().noquote() << "[ENTRY SLOT] AED::selfTestResult(" << ModuleSelfTest::testResultNames[result] << ")" << Qt::endl;

    switch(result)
    {
        case ModuleSelfTest::OK:
            qDebug() << "[SIGNAL] Emit AED::signalDisplayPassTest()" << Qt::endl;
            emit signalDisplayPassTest();
            userPrompt(P_UNIT_OK + QString(" / ") + ((cableState == PAD_CHILD) ? "CHILD PADS" : "ADULT PADS" ));

            // Let "UNIT OK" display for a moment before allowing the AED to overwrite it
            connect(&timer, &QTimer::timeout, this, &AED::completeSelfTest);
            timer.setInterval(DURATION_UNIT_OK);
            timer.start();
            return;

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

    qDebug().noquote() << "[EXIT SLOT] AED::selfTestResult(" << ModuleSelfTest::testResultNames[result] << ")" << Qt::endl;
}

// SLOT
void AED::completeSelfTest()
{
    qDebug().noquote() << "[ENTRY/EXIT SLOT] AED::completeSelfTest()" << Qt::endl;

    disconnect(&timer, &QTimer::timeout, this, &AED::completeSelfTest);
    doStartupAdvice();
}

void AED::ecgResult(bool shockable)
{
    qDebug().noquote() << "[ENTRY SLOT] AED::ecgResult(" << shockable << ")" << Qt::endl;

    // Entry guard: must be in ECG_ASSESS state
    if(aedState != ECG_ASSESS)
    {
        qDebug().noquote() << "[EXIT - WARNING] AED::ecgResult(" << shockable<< ") "
                           << "triggered when ECG_ASSESS was not active!" << Qt::endl;
        return;
    }

    if(shockable)
    {
        userPrompt(P_SHOCKABLE);
        doPrepShock();
    }
    else
    {
        // Not shockable
        userPrompt(P_NOTSHOCKABLE);

        qDebug() << "[SIGNAL] Emit AED::signalStopLampStandback()" << Qt::endl;
        emit signalStopLampStandback();

        scheduleCPR();
    }

    qDebug().noquote() << "[EXIT SLOT] AED::ecgResult(" << shockable << ")" << Qt::endl;
}

void AED::shockDelivered()
{
    qDebug().noquote() << "[ENTRY SLOT] AED::shockDelivered()" << Qt::endl;
    scheduleCPR();
    qDebug().noquote() << "[EXIT SLOT] AED::shockDelivered()" << Qt::endl;
}

void AED::scheduleShockResolution()
{
    qDebug().noquote() << "[ENTRY SLOT] AED::scheduleShockResolution(): aedState is " << stateNames[aedState] << Qt::endl;

    if(aedState == SHOCK)
    {
        changeStateSafe(SHOCK_RESOLVE);

        qDebug() << "[SIGNAL] Emit ModuleShock::signalResolveShock()" << Qt::endl;
        emit signalResolveShock();
    }

    qDebug().noquote() << "[EXIT SLOT] AED::scheduleShockResolution()" << Qt::endl;
}

void AED::startCPR()
{
    qDebug().noquote() << "[ENTRY SLOT] AED::startCPR()" << Qt::endl;

    disconnect(&timer, &QTimer::timeout, this, &AED::startCPR);
    doStartCPR();

    qDebug().noquote() << "[EXIT SLOT] AED::startCPR()" << Qt::endl;
}

void AED::cprStopped()
{
    qDebug().noquote() << "[ENTRY SLOT] AED::cprStopped()" << Qt::endl;

    if(aedState != CPR)
    {
        qDebug().noquote() << "[WARNING] AED::cprStopped() "
                           << "triggered when CPR was not active!" << Qt::endl;
    }

    qDebug() << "[SIGNAL] Emit AED::signalStopLampCPR()" << Qt::endl;
    emit signalStopLampCPR();

    scheduleECG();

    qDebug().noquote() << "[EXIT SLOT] AED::cprStopped()" << Qt::endl;
}

void AED::restartECG()
{
    qDebug().noquote() << "[ENTRY SLOT] AED::restartECG()" << Qt::endl;

    disconnect(&timer, &QTimer::timeout, this, &AED::restartECG);
    changeStateSafe(ECG_ASSESS);
    doStartECG();

    qDebug().noquote() << "[EXIT SLOT] AED::restartECG()" << Qt::endl;
}


//
// USER PROMPTS
//

void AED::userPrompt(const QString & prompt)
{
    qDebug() << "[ENTRY SLOT] AED::userPrompt(" << prompt << ")" << Qt::endl;

    if(aedState == OFF)
    {
        qDebug() << "[EXIT - WARNING] AED::userPrompt(" << prompt << "): AED is off!" << Qt::endl;
        return;
    }

    qDebug() << "[SIGNAL] Emit AED::signalUserPrompt(" << prompt << ")" << Qt::endl;
    emit signalUserPrompt(prompt);

    qDebug() << "[EXIT SLOT] AED::userPrompt(" << prompt << ")" << Qt::endl;
}
