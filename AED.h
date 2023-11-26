#ifndef AED_H
#define AED_H

#include <QObject>
#include <QString>

#include "EventSequence.h"
#include "ModuleSelfTest.h"

#define DURATION_UNIT_OK 4000

namespace aed
{

    class AED : public QObject
    {
        Q_OBJECT

        public:

            enum state_t { OFF, SELF_TEST, FAILURE, STARTUP_ADVICE, DETECT_PAD, ECG_ASSESS, SHOCK, CPR };
            enum cableState_t { UNPLUGGED, PAD_ADULT, PAD_CHILD };

            AED();
            ~AED();

            inline bool isOn() const { return aedState != OFF; }
            inline state_t getState() const { return aedState; }
            inline cableState_t getCableState() const { return cableState; }
            inline float getBattery() const { return battery; }
            inline bool willPassTest() const { return passTest; }

        private:
            state_t aedState;
            cableState_t cableState;
            float battery;          // Battery from 0 (depleted) to 1 (full charge)
            bool passTest;          // Assuming battery and cable fine, will AED pass self-test? (Use for modeling misc. unit failures)

            EventSequence seq;

            QTimer timer;

        public:

            /*
            void addStartupAdviceModule(StartupAdviceModule *);
            void addSelfTestModule(SelfTestModule *);
            void addShockModule(ShockModule *);
            void addECGModule(ECGModule *);
            void addCPRHelpModule(CPRHelpModule *);

            bool removeStartupAdviceModule(StartupAdviceModule *);
            bool removeSelfTestModule(SelfTestModule *);
            bool removeShockModule(ShockModule *);
            bool removeECGModule(ECGModule *);
            bool removeCPRHelpModule(CPRHelpModule *);
            */

        private:
            void startSelfTest();
            void reportLowBattery();
            void reportUnitFailed();

            void plugCable(cableState_t);
            void reportNoCable();

            void doStartupAdvice();

            void attachPads(bool);

            void startECG();
            void reportElectrodeProblem();

            void prepShock();

            void startCPR();




        public slots:

            void changeBatteries();

            void selfTestResult(ModuleSelfTest::testResult_t);


            void turnOn();
            void turnOff();

            void setBattery(float newBatt);
            void eatBattery(float loseBatt);

            void userPrompt(const QString & prompt);

        signals:
            void batteryChanged(float newBatt);

    };
}

#endif // AED_H
