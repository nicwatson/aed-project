// FILE ModuleSelfTest.h
//
// Used to carry out the startup self-test on the AED
// No direct GUI connections required -- all handled through AED object

#ifndef MODULESELFTEST_H
#define MODULESELFTEST_H

#include <QObject>
#include <QTimer>



// No direct connections needed between ModuleSelfTest and GUI widgets

namespace aedModel
{
    class AED;

    class ModuleSelfTest : public QObject
    {
        Q_OBJECT

    static bool FORCE_FAIL; // Set to always force a FAIL_OTHER result (for testing)

    public:
        // Type def for the four possible test results
        enum testResult_t { OK, FAIL_BATTERY, FAIL_CABLE, FAIL_OTHER };

        // Used for debugging - translate between testResult_t underlying integers and readable strings
        static const QString testResultNames[];

        explicit ModuleSelfTest();
        ~ModuleSelfTest();

    protected:
        bool active;            // Is self-test currently running?
        QTimer timer;           // Timer to time the self-test event
        testResult_t result;    // Stores test result

        // Reset local vars for a future self-test
        void reset();

    public slots:

        // Starts the self-test, using the passed AED pointer to gather data to determine if the test passes
        // Trigger: signal AED::signalStartTest() : connection in AED::addModuleSelfTest(...)
        void startSelfTest(AED * unit);

        // Use if unit is turned off during self-test
        // Trigger: signal AED::signalAbortSelfTest() : connection in AED::addModuleSelfTest(...)
        void abort();

    protected slots:

        // Triggered when the test timer expires, to finalize and transmit test result
        // Trigger: timer->QTimer::timeout() : connection in ModuleSelfTest constructor
        void finishSelfTest();   

    signals:

        // Transmits test result back to the AED
        // Emitter: ModuleSelfTest::finishSelfTest()
        // Receiver: AED::selfTestResult(testResult_t) : connection in AED::addModuleSelfTest(...)v
        void signalResult(ModuleSelfTest::testResult_t);

    };
}

#endif // MODULESELFTEST_H
