#include "ModuleSelfTest.h"
#include "AED.h"

using namespace aedModel;

bool ModuleSelfTest::FORCE_FAIL = false;

ModuleSelfTest::ModuleSelfTest()
    : QObject()
{
    timer.setSingleShot(true);
    timer.setInterval(TEST_TIME);
    connect(&timer, &QTimer::timeout, this, &aedModel::ModuleSelfTest::finishSelfTest);
}

void ModuleSelfTest::startSelfTest(AED * unit)
{
    if(FORCE_FAIL)
    {
        result = FAIL_OTHER;
    }
    else if(unit->getBattery() < BATTERY_THRESHHOLD)
    {
        result = FAIL_BATTERY;
    }
    if(unit->getCableState() == AED::UNPLUGGED)
    {
        result = FAIL_CABLE;
    }
    else
    {
        result = OK;
    }
    timer.start();
}

void ModuleSelfTest::finishSelfTest()
{
    emit signalResult(result);
}
