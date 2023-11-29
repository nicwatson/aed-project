#ifndef MODULESELFTEST_H
#define MODULESELFTEST_H

#include <QObject>
#include <QTimer>

#define TEST_TIME 1000
#define BATTERY_THRESHHOLD 0.3

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
        QTimer timer;

        testResult_t result;

    signals:
        void signalResult(testResult_t);

    public slots:
        void startSelfTest(AED * unit);

    protected slots:
        void finishSelfTest();

    };
}

#endif // MODULESELFTEST_H
