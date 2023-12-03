// FILE AED.h
//
// This represents the AED device itself.
// It only models the internal logic. The GUI itself consists of separate elements that the AED
// communicates with via signals.
// AED has five modules for handling special sub-functions: Self test, startup advice, ECG assessment, shock, and CPR advice.
// AED handles the state transitions between these, and also mediates some (but not all) of the interactions with the GUI.
// Some modules have direct signalling with the GUI.
//
// General usage pattern:
//      Instantiate the five modules in MainWindow
//      Make any necessary direct signal/slot connections between GUI widgets and modules.
//      Instantiate the AED
//      Call the addModule...() functions on AED to pass pointers to each of the modules. This links signals/slots between
//          AED object and each module.
//      Finally, link GUI signals/slots to AED object itself.

#ifndef AED_H
#define AED_H

#include <QObject>
#include <QString>
#include <QDebug>
#include "ModuleSelfTest.h"

// This class needs direct connections to GUI elements. Look for "TODO" comments in this header file.
// Change "TODO" to "COMPLETE" after code has been written to establish connection.

#define TIMER_STANDARD_DELAY 2000

namespace aedModel
{
    // To avoid circular dependencies, we can forward-declare every class except for ModuleSelfTest.h (because of enums that get used from that class)
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
            explicit AED();
            explicit AED(float initBatt);    // Initialize AED with initBatt amount of battery charge (in range of 0 to 1)
            ~AED();

            // Getters
            inline bool isOn() const { return aedState != OFF; }
            inline state_t getState() const { return aedState; }
            inline cableState_t getCableState() const { return cableState; }
            inline float getBattery() const { return battery; }

            // Returns whether it would be valid, according to our model, to transition to newState from the current state
            bool legalStateChange(state_t newState);

            // Setup - instantiate individual modules and then pass them in here
            bool addModuleSelfTest(ModuleSelfTest *);
            bool addModuleStartupAdvice(ModuleStartupAdvice *);
            bool addModuleECG(ModuleECGAssessment *);
            bool addModuleShock(ModuleShock *);
            bool addModuleCPR(ModuleCPRHelp *);
            void dummy() {

                emit signalBatteryChanged(0.255);
                emit signalStartLampStandback();
                emit signalStartLampCPR();

                QTimer::singleShot(5000, this, [=]() {
                    emit signalUserPrompt("dumbodore");
                    emit signalStopLampStandback();
                    emit signalStopLampCPR();
                });
            }

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

            QTimer timer;

        private:
            void doSelfTest();
            void doStartupAdvice();
            void doStartECG();
            void doPrepShock();
            void doStartCPR();

            void errorBattery();
            void errorCable();
            void errorOther();
            void failAED();

            void clearPrompt();

            void stopActivity();        // Stop activity going on in all modules; reset in-device GUI elements
            void turnOn();
            void turnOff();

        public slots:

            // COMPLETE Connect to power button - trigger when turned on / off
            void togglePowerButton();

            // Nothing to do
            void plugCable(cableState_t newCableState);

            // COMPLETE Connect all three cable signals to corresponding radio buttons being selected
            void plugCableAdult();
            void plugCableChild();
            void unplugCable();

            // Attaches pads to patient (pass true to attach, false to detach)
            // COMPLETE connect these to the corresponding radio buttons
            void attachPads(bool);

            void setBattery(double newBatt);
            void useBattery(double loseBatt);

            // COMPLETE Connect to the change/recharge batteries button
            void changeBatteries();

            // TODO Any Module that needs to update the main prompt should send a signal to this slot
            void userPrompt(const QString & prompt);


        protected slots:
            void selfTestResult(ModuleSelfTest::testResult_t result);
            void ecgResult(bool shockable);
            void shockDelivered();
            void startCPR();
            void cprStopped();

            void restartECG();

        signals:
            void signalStartTest(AED *);

            // COMPLETE Connect this to some mainwindow/GUI logic that will make the green checkmark show
            void signalDisplayPassTest();

            // COMPLETE connect this to some mainwindow/GUI logic that will make the red x show
            void signalUnitFailed();

            void signalStartupAdvice(cableState_t);
            void signalPadsAttached();
            void signalStartECG();
            void signalCompleteECG();
            void signalLCD();

            void signalPrepShock(bool usingChildPads);

            void signalStartCPR(cableState_t);
            void signalStopCPR();

            void signalAbortSelfTest();
            void signalAbortStartupAdvice();
            void signalAbortECG();
            void signalAbortShock();
            void signalAbortCPR();

            // Stop anything that is happening in any module
            void signalAbortAll();

            // COMPLETE Connect thiLCDo the main prompt QLabel (in the LCD) to update its text with the string
            // Any strings sent to AED::userPrompt() will get forwarded out again on this signal
            void signalUserPrompt(const QString & prompt); // Update LCD

            // COMPLETE This needs to update the battery percentage QLabel.
            // Might be necessary to add a slot to MainWindLCDand connect this to it. That slot would take the double,
            // convert it to an int from 0 to 100, and then embed it in a string so that it can be sent to the QLabel.
            void signalBatteryChanged(double newBatt); // Update LCD

            // COMPLETE connect these to the startFlash() and stopFlash() slots for the Lamp Widgets being used for the
            // "stand back / ECG / shock" and "CPR" pictograms.
            void signalStartLampStandback();        // startFlash
            void signalStopLampStandback();         // stopFlash
            void signalStartLampCPR();              // startFlash
            void signalStopLampCPR();               // stopFlash
            // TODO? Not sure if we need these
            void signalStartLampECG();               // startFlash
            void signalStopLampECG();                // stopFlash
            void signalStartLampShock();        // startFlash
            void signalStopLampShock();         // stopFlash

            void signalPowerOff();
    };
}

#endif // AED_H
