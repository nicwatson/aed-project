// FILE AED.h
//
// This represents the AED device itself.
// It only models the internal logic. The GUI itself consists of separate elements that the AED
// communicates with via signals.
// AED has five modules for handling special sub-functions: Self test, startup advice, ECG assessment, shock, and CPR advice.
// AED handles the state transitions between these, and also mediates some (but not all) of the interactions with the GUI.
// Some modules have direct signaling with the GUI.
//
// General usage pattern:
//      Instantiate the five modules in MainWindow
//      Make any necessary direct signal/slot connections between GUI widgets and modules (some of these are handled by
//          the modules' constructors or helper functions with arguments passed in to them).
//      Instantiate the AED
//      Call the addModule...() functions on AED to pass pointers to each of the modules. This links signals/slots between
//          AED object and each module.
//      Finally, link GUI signals/slots to AED object itself.

#ifndef AED_H
#define AED_H

#include <QObject>
#include <QString>
#include <QDebug>
#include "aedconst.h"
#include "ModuleSelfTest.h"

namespace aedModel
{
    // To avoid circular dependencies, we can forward-declare every class except for ModuleSelfTest.h
    // (because of enums that get used from that class).
    // The full headers must be explicitly included in AED's corresponding .cpp files
    class ModuleStartupAdvice;
    class ModuleECGAssessment;
    class ModuleShock;
    class ModuleCPRHelp;

    class AED : public QObject
    {
        Q_OBJECT

        public:     // TYPE DEFS AND CONSTANTS

            // Type def: High-level state abstraction for the AED
            enum state_t { OFF, SELF_TEST, FAILURE, STARTUP_ADVICE, ECG_ASSESS, SHOCK, SHOCK_RESOLVE, CPR };

            // Type def: Cable state: either it's unplugged, adult pads are plugged in, or child pads are plugged in
            enum cableState_t { UNPLUGGED, PAD_ADULT, PAD_CHILD };

            // Used for debugging - translate between state_t underlying integers and readable strings
            static const QString stateNames[];

            // Used for debugging - translate between cableState_t underlying integers and readable strings
            static const QString cableStateNames[];


        private:    // MEMBER VARS AND FIELDS

            // AED's high-level state abstraction (what stage of treatment are we in?)
            // Do not change aedState directly. Use changeStateSafe() to validate change and ensure signals are sent.
            state_t aedState;

            // State abstraction for cables (unplugged, child pads, or adult pads)
            cableState_t cableState;

            bool padsAttached;  // Flag - are the pads attached?

            double battery;     // Battery from 0 (depleted) to 1 (full charge)

            QTimer timer;       // Timer for AED to use for the pacing between state transitions
                                // Modules have their own timers for pacing their own events/functions
                                // But this timer gets used for some transitions between different Module-managed phases


        public:     // FUNCTIONS

            // Lifecycle - defs in AED_setup.cpp
            explicit AED();
            explicit AED(float initBatt);    // Initialize AED with initBatt amount of battery charge (in range of 0 to 1)
            ~AED();

            // Getters - inline
            inline bool isOn() const { return aedState != OFF; }
            inline state_t getState() const { return aedState; }
            inline cableState_t getCableState() const { return cableState; }
            inline float getBattery() const { return battery; }

            // Setup - instantiate individual modules and then pass them in here - this connects signals and slots with AED
            // These return true if setup is successful (mostly, the passed arg is not nullptr)
            // Defs in AED_setup.cpp
            bool addModuleSelfTest(ModuleSelfTest *);
            bool addModuleStartupAdvice(ModuleStartupAdvice *);
            bool addModuleECG(ModuleECGAssessment *);
            bool addModuleShock(ModuleShock *);
            bool addModuleCPR(ModuleCPRHelp *);

            // Returns whether it would be valid, according to our model, to transition to newState from the current state
            // Def in AED.cpp
            bool legalStateChange(state_t newState);


        private:    // FUNCTIONS

            //
            // State transition logic - defs in AED.cpp
            //

            // Changes AED state, performing other housekeeping (e.g. sending signals) as necessary.
            // Should generally avoid using changeState() directly. Use changeStateSafe() instead.
            void changeState(state_t newState);

            // Checks whether state change is legal (calls legalStateChange()) before changing states. Returns true if state was
            // changed, false if it was determined to be an invalid change.
            bool changeStateSafe(state_t newState);

            // Changes the state of the plugged in cable/pads
            // According to the Admin Guide, cables are supposed to be plugged in before turning on the AED
            // Therefore, unless the AED is OFF, changing the cable state will trigger the AED to immediately go back to self-test.
            // Returns false if the newState is the same as the current state (i.e. no change - also will not trigger self-test).
            // Otherwise, returns true.
            bool changeCableState(cableState_t newState);

            // Schedules the ECG phase to start after a short delay (for transition from CPR phase)
            // The delay prevents the "STOP CPR" message from being immediately overwritten.
            void scheduleECG();
            // Schedules the CPR phase to start after a short delay (for transition from ECG or shock phases)
            // The delay prevents the ECG result or "shock delivered" message from being immediately overwritten.
            void scheduleCPR();


            //
            // Power cycle - managing turning on/off the AED - defs in AED.cpp
            //
            //
            void turnOn();
            void turnOff();
            void stopActivity();        // Stop activity going on in all modules; reset in-device GUI elements


            //
            // Executors - defs in AED.cpp
            //
            //
            // Executors to launch each of the five module-managed treatment stages
            void doSelfTest();
            void doStartupAdvice();
            void doStartECG();
            void doPrepShock();
            void doStartCPR();


            //
            // Helper functions - defs in AED.cpp
            //

            // Helper functions for self-test-failed situations
            void errorBattery();
            void errorCable();
            void errorOther();
            void failAED();

            // Helper function to blank out the connected user advice prompt
            void clearPrompt();

            // Emits all of the event-abort signals to all Modules - used for situations like when the AED is suddenly turned off
            void abortAll();


        public slots:       // SLOTS - defs are in AED_slots.cpp

            // Connect to power button - trigger when turned on / off
            // Trigger: signal is (QPushButton) ui->powerButton::pressed() : connection in MainWindow::buildAEDConnections()
            void togglePowerButton();

            // Inform the AED that a cable has been plugged in or unplugged
            // This is a general-purpose version of the specific plug-state cable slots below
            // Trigger: No connected signals at this time
            void plugCable(cableState_t newCableState);

            // Connect all three cable signals to corresponding radio buttons being selected
            // Trigger: signals (x3) from GUI radio buttons for cable state (QRadioButton::clicked()):
            //      adultPadsButton, pediatricPadsButton, unpluggedCablesButton
            //      Connection is in MainWindow::buildAEDConnections()
            void plugCableAdult();
            void plugCableChild();
            void unplugCable();

            // Attaches pads to patient (pass true to attach, false to detach)
            // Trigger: signals from GUI radio buttons for pad attachment state (QRadioButton::clicked())
            //      attachedPadsButton, detachedPadsButton
            //      Connection is in MainWindow::buildAEDConnections()
            void attachPads(bool);

            // Updates battery level
            // Trigger: No connected signals at this time. Direct call from useBattery(double)
            void setBattery(double newBatt);

            // Subtracts an amount from battery level
            // Trigger: signal from ModuleShock::signalDrainBatt(double) : Connection in AED::addModuleShock(...)
            void useBattery(double loseBatt);

            // Exchanges the batteries for a fresh, fully-charged set. Side effect: powers off the AED.
            // Trigger: signal from (QPushButton) ui->rechargeBatteryButton::pressed() : connection in MainWindow::buildAEDConnections()
            void changeBatteries();

            // Modules that need to update the main prompt should send a signal to this slot, instead of sending directly to the LCD
            // Triggers: 4 different signals named signalForwardUserPrompt,
            //      one for each of ModuleStartupAdvice, ModuleECGAssessment, ModuleShock, ModuleCPRHelp
            void userPrompt(const QString & prompt);


        protected slots:    // SLOTS - defs are in AED_slots.cpp

            // Receives and processes the result of the startup self-test from the ModuleSelfTest
            // Trigger: signal from ModuleSelfTest::signalResult(testResult_t) : connection in
            void selfTestResult(ModuleSelfTest::testResult_t result);

            // Allows startup advice phase to proceed after a short delay to display "UNIT OK" following successful self test
            // Trigger: timer->QTimer::timeout : dynamic connection made in selfTestResult(testResult_t), broken in completeSelfTest()
            void completeSelfTest();

            // Receives the simplified result of the ECG (shockable true/false) and uses it to determine the next action (shock or CPR)
            // Trigger: signal from ModuleECGAssessment::signalResult(bool) : connection in AED::addModuleECG(...)
            void ecgResult(bool shockable);

            // Receives notification that the ModuleShock delivered a shock to the patient
            // Connects and sets timer to trigger startCPR() a short time later (see below)
            // Trigger: signal from ModuleShock::signalDone() : connection in AED::addModuleShock
            void shockDelivered();

            // After delivering a shock, the battery might be too depleted to continue
            // ModuleShock signals to the AED to signal it back if (and only if) there is enough battery to continue
            // This slot accepts the signal from ModuleShock and decides whether to send a reply signal based on the current aedState
            // Trigger: signal ModuleShock::signalScheduleShockResolution() : connection in AED::addModuleShock(...)
            void scheduleShockResolution();

            // Receives notification that it is time to start CPR.
            // Trigger: signal from AED's timer timeout()
            // Dynamic connection: Connected and timer started in AED::shockDelivered(), disconnected in AED::startCPR()
            void startCPR();

            // Receives notification from ModuleCPRHelp that CPR phase has ended (i.e. two minutes up, prompt already says "STOP CPR")
            // Connects and sets timer to trigger restartECG() a short time later (see below)
            // Trigger: signal from ModuleCPRHelp::signalCPRComplete() : connection in AED::addModuleCPR(...)
            void cprStopped();

            // Receives notification that it's time to restart the ECG (following CPR)
            // Trigger: signal from AED's timer timeout()
            // Dynamic connection: Connected and timer started in AED::cprStopped(), disconnected in AED::restartECG()
            void restartECG();


    signals:

            // Used to make the green checkmark indicator show when self-test is passed
            // Emitter: AED::showPassTest()
            // Receivers: slot is (QGraphicsView) ui->selfTestIndicator::setStyleSheet(const QString &) : connection in MainWindow::buildAEDConnections()
            void signalDisplayPassTest();

            // Used to make the red X indicator show when self-test fails
            // Emitter: AED::failAED()
            // Receiver: slot is (QGraphicsView) ui->selfTestIndicator::setStyleSheet(const QString &) : connection in MainWindow::buildAEDConnections()
            void signalUnitFailed();

            // Signals to the ModuleSelfTest to star the self test, passing a pointer to the AED so that the Module can check its states
            // Emitter: AED::doSelfTest()
            // Receiver: slot is ModuleSelfTest::startSelfTest(...) : connection in AED::addModuleSelfTest(...)
            void signalStartTest(AED *);

            // Signals to the ModuleStartupAdvice to start the "startup advice sequence" (lighted pictograms and voice prompts)
            // ModuleStartupAdvice needs to know the cable state because it will begin by announcing "UNIT OK" with the type of pads connected
            // Emitter: AED::doStartupAdvice()
            // Receiver: slot is ModuleStartupAdvice::startAdvice(cableState_t) : connection in AED::addModuleStartupAdvice(...)
            void signalStartupAdvice(AED::cableState_t);

            // Signals to the ModuleStartupAdvice that the pads have been attached to the patient
            // This causes the startup advice cycle to stop, so that ECG can start
            // (At this point, the ModuleStartupAdvice no longer cares whether they are adult or child pads)
            // Emitter: AED::attachPads(bool) (itself a slot triggered by GUI radio buttons)
            // Receiver: slot is ModuleStartupAdvice::padsAttached() : connection in AED::addModuleStartupAdvice(...)
            void signalPadsAttached();

            // Signals to the ModuleECGAssessment to start running an ECG analysis
            // Emitter: AED::doStartECG()
            // Receiver: slot is ModuleECGAssessment::startAssessment() : connection in AED::addModuleECG(...)
            void signalStartECG();

            // Signals to the ModuleShock to prepare for delivering a therapeutic shock
            // Argument is true for child pads, false for adult pads. ModuleShock needs to know to callibrate energy output levels.
            // Emitter: AED::doPrepShock()
            // Receiver: slot is ModuleShock::start(bool) : connection in AED::addModuleShock(...)
            void signalPrepShock(bool usingChildPads);

            // Signals to the ModuleCPRHelp to start the CPR help/advice phase
            // Passes the cable state because ModuleCPRHelp will choose which "strategy" to use based on whether child or adult
            // pads are connected. (Note however that the cable state should not be UNPLUGGED, and if it is, the receiving slot
            // will ignore the signal.)
            // Emitter: AED::doStartCPR()
            // Receiver: slot is ModuleCPRHelp::signalStartCPR(cableState_t) : connection in AED::addModuleCPR
            void signalStartCPR(cableState_t);

            // Signals to the various Modules to abort their events immediately
            // Emitter: All of these are emitted in AED::abortAll() (mostly for when unit is switched off)
            //      Some have additional emitters as noted
            // Receivers as noted below
            void signalAbortSelfTest();         // Receiver: slot ModuleSelfTest::abort() : connection in AED::addModuleSelfTest(...)
            void signalAbortStartupAdvice();    // Receiver: slot ModuleStartupAdvice::stop() : connection in AED::addModuleStartupAdvice(...)
            void signalAbortECG();              // Receiver: slot ModuleECGAssessment::abort() : connection in AED::addModuleECG(...)
                                                // Additional emitter: AED::attachPads(bool) (for when pads are detached during ECG)
            void signalAbortShock();            // Receiver: slot ModuleShock::abort() : connection in AED::addModuleShock(...)
                                                // Additional emitter: AED::attachPads(bool) (for when pads are detached during shock phase)
            void signalAbortCPR();              // Receiver: slot ModuleCPRHelp::abort() : connection in AED::addModuleCPR(...)

            // Signals to Modules as needed that the device has been powered off
            // Emitter: AED::turnOff()
            // Receiver 1: ModuleShock::fullReset : connection in AED::addModuleShock(...) -- needed to reset shock counter
            // Receiver 2: ui->lcdDisplay->LCDDisplay::endLCD() : connection in MainWindow::buildAEDConnections()
            // Receiver 3: ui->selfTestIndicator->QGraphicsView::setStyleSheet(...) : connection in MainWindow::buildAEDConnections() -- turns off the test result indicator light
            void signalPowerOff();

            // Signals to GUI to turn off the power light when batteries are pulled - need a separate signal from signalPowerOff() because otherwise clicking to turn off would
            // cause interference with that signal
            // Emitter: AED::changeBatteries()
            // Receiver: ui->powerButton->QPushButton::setChecked(false) : connection in MainWindow::buildAEDconnections() -- turn off the power light
            void signalBatteriesPulled();

            // Signals to LCD's user advice prompt widget to update the displayed text
            // Any strings sent to AED::userPrompt() will get forwarded out again on this signal
            // Emitters: AED::userPrompt(...)
            // Receiver: slot ui->lcdDisplay->LCDDisplay::setPromptLabel(...) : connection in MainWindow::buildAEDConnections()
            void signalUserPrompt(const QString & prompt);

            // Signals to update the GUI label that shows the battery level as a percentage
            // Emitters: AED::setBattery(double), AED::changeBatteries()
            // Receiver: slot ui->batteryPercentLabel->QLabel::setText(...) : connection in MainWindow::buildAEDConnections
            void signalBatteryLevelChanged(double newBatt);

            // After delivering a shock, the battery might be too depleted to continue
            // ModuleShock signals to the AED to signal it back if (and only if) there is enough battery to continue
            // This signal is the reply sent back to ModuleShock if it is OK to continue
            // Emitter: AED::scheduleShockResolution()
            // Receiver: ModuleShock::resolveShock() : connection in AED::addModuleShock(...)
            void signalResolveShock();

            // Signals to start/stop flashing lights on the interface (only directly AED-managed lights, not for those managed by ModuleStartupAdvice)
            // This means the lamps on the "Stand back" (ECG/shock) and "Start CPR" pictograms
            // Each "start" signal is received by a lamp's startFlash(), and each "stop" signal by a lamp widget's stopFlash()
            // Connections are in MainWindow::buildAEDConnections()
            void signalStartLampStandback();      // Emitters:  AED::scheduleECG(),  AED::doStartECG(),  AED::doPrepShock()
            void signalStopLampStandback();       // Emitters:  AED::attachPads(bool),  AED::ecgResult(bool), AED::scheduleCPR(), AED::doStartCPR(), AED::stopActivity(),
            void signalStartLampCPR();            // Emitters:  AED::scheduleCPR(),  AED::doStartCPR()
            void signalStopLampCPR();             // Emitters:  AED::cprStopped(),  AED::scheduleECG(), AED::stopActivity()
    };
}

#endif // AED_H
