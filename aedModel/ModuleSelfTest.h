// FILE ModuleSelfTest.h
//
// Used to carry out the startup self-test on the AED
// No direct GUI connections required -- all handled through AED object

#ifndef MODULESELFTEST_H
#define MODULESELFTEST_H

#include <QObject>
#include <QTimer>

#define TEST_TIME 1000
#define BATTERY_THRESHHOLD 0.3

// No direct connections needed between ModuleSelfTest and GUI widgets

namespace aedModel
{
    class AED;

    class ModuleSelfTest : public QObject
    {
        Q_OBJECT

    static bool FORCE_FAIL;

    public:
        enum testResult_t { OK, FAIL_BATTERY, FAIL_CABLE, FAIL_OTHER };

        explicit ModuleSelfTest();


    protected:
        bool active;
        QTimer timer;
        testResult_t result;

        // Reset local vars for a future self-test
        void reset();

    public slots:
        void startSelfTest(AED * unit);

        // Use if unit is turned off during self-test
        void abortSelfTest();

    protected slots:
        void finishSelfTest();   

    signals:
        void signalResult(testResult_t);

    };
}

#endif // MODULESELFTEST_H
