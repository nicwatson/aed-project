// FILE ModuleSelfTest.cpp
// Implementations for ModuleSelfTest class

#include "ModuleSelfTest.h"
#include "AED.h"
#include <QDebug>

using namespace aedModel;

// Unless doing special debugging tests, FORCE_FAIL should be set false
bool ModuleSelfTest::FORCE_FAIL = false;

// Used for debugging - translate between testResult_t underlying integers and readable strings
const QString ModuleSelfTest::testResultNames[4] = { "OK", "FAIL_BATTERY", "FAIL_CABLE", "FAIL_OTHER" };

ModuleSelfTest::ModuleSelfTest()
    : QObject(), active(false), result(FAIL_OTHER)
{
    qDebug() << "[CONSTRUCTOR ENTRY] ModuleSelfTest()" << Qt::endl;

    timer.setSingleShot(true);
    timer.setInterval(TEST_TIME);
    connect(&timer, &QTimer::timeout, this, &aedModel::ModuleSelfTest::finishSelfTest);

    qDebug() << "[CONSTRUCTOR EXIT] ModuleSelfTest()" << Qt::endl;
}

ModuleSelfTest::~ModuleSelfTest()
{
    qDebug() << "[DESTRUCTOR] ~ModuleSelfTest()" << Qt::endl;
    // No dynamic allocations to delete
}

void ModuleSelfTest::reset()
{
    qDebug() << "[ENTRY] ModuleSelfTest::reset()" << Qt::endl;

    active = false;
    result = FAIL_OTHER;

    qDebug() << "[EXIT] ModuleSelfTest::reset()" << Qt::endl;
}

// SLOT
void ModuleSelfTest::startSelfTest(AED * unit)
{
    if(active == true)
    {
        qDebug() << "[ENTRY/WARNING/EXIT SLOT] ModuleSelfTest::startSelfTest(AED*): started self-test but it was already active" << Qt::endl;
        return;
    }

    qDebug().noquote()  << "[ENTRY SLOT] ModuleSelfTest::startSelfTest(AED*)" << Qt::endl
                        << "\tFORCE_FAIL: " << FORCE_FAIL
                        << "   Battery: " << QString::number(unit->getBattery())
                        << "   Cable: " << AED::cableStateNames[unit->getCableState()]
                        << Qt::endl;

    active = true;
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
    timer.start();  // Simulate short delay before reporting test result

    qDebug().noquote() << "[EXIT SLOT] ModuleSelfTest::startSelfTest(AED*) : Result: " << testResultNames[result] << Qt::endl;
}

// SLOT
void ModuleSelfTest::abort()
{
    qDebug() << "[ENTRY SLOT] ModuleSelfTest::abort()" << Qt::endl;

    timer.blockSignals(true);
    timer.stop();
    timer.blockSignals(false);
    reset();

    qDebug() << "[EXIT SLOT] ModuleSelfTest::abort()" << Qt::endl;
}

// SLOT
void ModuleSelfTest::finishSelfTest()
{
    qDebug() << "[ENTRY SLOT] ModuleSelfTest::finishSelfTest()" << Qt::endl;

    qDebug().noquote() << "[SIGNAL] Emit ModuleSelfTest::signalResult(" << testResultNames[result] << ")" << Qt::endl;
    emit signalResult(result);
    reset();

    qDebug() << "[EXIT SLOT] ModuleSelfTest::finishSelfTest()" << Qt::endl;
}


