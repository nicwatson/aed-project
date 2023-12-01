#ifndef AED_H
#define AED_H

#include <QObject>
#include <QString>
#include <QDebug>
#include "ModuleSelfTest.h"

namespace aedModel
{
    class ModuleStartupAdvice;
    class ModuleECGAssessment;
    class ModuleShock;
    class ModuleCPRHelp;

    class AED : public QObject
    {
        Q_OBJECT

        public:

            // Enumeration type defs
            enum state_t { OFF, SELF_TEST, FAILURE, STARTUP_ADVICE, ECG_ASSESS, SHOCK, CPR };
            enum cableState_t { UNPLUGGED, PAD_ADULT, PAD_CHILD };

            // Lifecycle
            AED();
            AED(float initBatt);    // Initialize AED with initBatt amount of battery charge (in range of 0 to 1)
            ~AED();

            // Getters
            inline bool isOn() const { return aedState != OFF; }
            inline state_t getState() const { return aedState; }
            inline cableState_t getCableState() const { return cableState; }
            inline float getBattery() const { return battery; }
            // inline bool willPassTest() const { return passTest; }

            // Function legalStateChange()
            // Returns whether it would be valid, according to our model, to transition to newState from the current state
            bool legalStateChange(state_t newState);

            // Signal/slot setup
            bool addModuleSelfTest(ModuleSelfTest *);
            bool addModuleStartupAdvice(ModuleStartupAdvice *);
            bool addModuleECG(ModuleECGAssessment *);
            bool addModuleShock(ModuleShock *);
            bool addModuleCPR(ModuleCPRHelp *);

        private:

            // Function changeState()
            // Changes AED state, performing other housekeeping (e.g. sending signals) as necessary.
            // Should generally avoid using changeState() directly. Use changeStateSafe() instead.
            void changeState(state_t newState);

            // Function changeStateSafe()
            // Checks whether state change is legal (calls legalStateChange()) before changing states. Returns true if state was
            // changed, false if it was determined to be an invalid change.
            bool changeStateSafe(state_t newState);

            bool changeCableState(cableState_t newState);


            // AED's high-level state abstraction (what stage of treatment are we in?)
            // Do not change aedState directly. Use changeStateSafe() to validate change and ensure signals are sent.
            state_t aedState;

            // State abstraction for cables (unplugged, child pads, or adult pads)
            cableState_t cableState;
            bool padsAttached;

            double battery;          // Battery from 0 (depleted) to 1 (full charge)
            // bool passTest;          // Assuming battery and cable fine, will AED pass self-test? (Use for modeling misc. unit failures)


        private:
            void doSelfTest();
            void doStartupAdvice();
            void doStartECG();
            void doPrepShock();
            void doStartCPR();

            void reportLowBattery();
            void reportUnitFailed();

            void errorBattery();
            void errorCable();
            void errorOther();

        protected slots:
            void selfTestResult(ModuleSelfTest::testResult_t result);
            void ecgResult(bool shockable);
            void startCPR();
            void stopCPR();


        public slots:
            void turnOn();
            void turnOff();

            void plugCable(cableState_t newCableState);
            void attachPads(bool);

            void setBattery(double newBatt);
            void useBattery(double loseBatt);
            void changeBatteries();

            void userPrompt(const QString & prompt);
            void clearPrompt();






        signals:
            void signalStartTest();
            void signalDisplayTestResult(bool);         // Update GUI
            void signalStartupAdvice(cableState_t);
            void signalPadsAttached();
            void signalStartECG();
            void signalEndECG();
            void signalPrepShock();
            void signalShockReady();    // Update GUI (flashing button)
            void signalShockGiven();    // Update LCD
            void signalStartCPR(cableState_t);
            void signalStopCPR();

            void signalUserPrompt(); // Update LCD

            void batteryChanged(float newBatt); // Update LCD
    };
}

#endif // AED_H
