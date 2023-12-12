// FILE ModuleShock.h
//
// Handles charging and delivering shock to patient.

#ifndef MODULESHOCK_H
#define MODULESHOCK_H

#include <QObject>
#include <QTimer>
#include <QString>
#include "AED.h"

namespace aedModel
{
    class ModuleShock : public QObject
    {
            Q_OBJECT
        public:
            static const int ADULT_SHOCKS[];    // Stores the energy levels for adult shocks
            static const int CHILD_SHOCKS[];    // Stores the energy levels for child shocks

            explicit ModuleShock();
            ~ModuleShock();

            // Getters
            inline bool getActive() { return active; }
            inline bool getCharged() { return charged; }
            inline bool getShocksDelivered() { return shocksDelivered; }

            // Retrieves the proper shock energy based on how many shocks have been delivered so
            // far, and whether adult or child pads are plugged in (pass true for child pads, false
            // for adult)
            int calcShockEnergy(bool childPads);


        private:
            bool active;                // Shock event is active
            int shocksDelivered;        // Number of shocks delivered
            bool charged;               // Shock ready
            bool buttonPressedEarly;    // Button pressed before shock ready
            int energySet;              // Energy to discharge for current shock

            bool inChargeDelay;         // Shock is charging up
            bool inExitDelay;           // Brief delay on exit avoids overwriting user prompts too quickly

            QTimer timer;

            void cleanup();             // Resets all local vars and states after shock event is completed, EXCEPT shocksDelivered which is remembered
            void reset();               // Does cleanup() and resets shocksDelivered to zero - used when machine is turned off


        private slots:

            // Slot triggered when shock is charged and ready
            // Trigger: signal timer->QTimer::timeout() : connection in ModuleShock::start(bool) (dynamic connection, made and broken as needed)
            void chargeReady();

            // Slot triggered when charge is delivered and it's time to exit the shock phase
            // Trigger: signal timer->QTimer::timeout() : connection in ModuleShock::shockButtonPressed() (dynamic connection, made and broken as needed)
            void exit();

        public slots:

            // Starts the shock prep and shock delivery phase of treatment. The passed arg determines if child or adult pads are in use.
            // Trigger: signal AED::signalPrepShock() : connection in AED::addModuleShock(...)
            void start(bool usingChildPads);

            // Abort because pads detached, cable disconnected, unit failure (battery), or unit turned off
            // Trigger: signal AED::signalAbortShock() : connection in AED::addModuleShock(...)
            void abort();

            // Used when unit power is turned off. This is triggers the reset() function, and is different from the cleanup() used after every shock phase completion
            // in that this resets the shock counter as well.
            // Trigger: signal AED::signalPowerOff : connection in AED::addModuleShock(...)
            void fullReset();

            // Tells the shock module that the button was pressed
            // Trigger: ui->shockButton::pressed() and ::released() : connections in MainWindow::buildModuleConnections()
            void shockButtonPressed();
            void shockButtonReleased();

            // After delivering a shock, the battery might be too depleted to continue
            // ModuleShock signals to the AED to signal it back if (and only if) there is enough battery to continue
            // If it does get a reply, it goes to this slot, allowing the shock event to resolve (reporting "SHOCK DELIVERED") and go to CPR phase
            // Trigger: signal AED::signalResolveShock() : connection in AED::addModuleShock(...)
            void resolveShock();


        signals:

            // This signal talks to the AED object and doesn't need to be connected to widgets directly
            // Emitters: ModuleShock::chargeReady(), ::shockButtonPressed(), ::shockButtonReleased()
            // Receiver: slot AED::userPrompt(const QString &) : connection in AED::addModuleShock(...)
            void signalForwardUserPrompt(const QString & prompt);

            // This signal talks to the AED object and doesn't need to be connected to widgets directly
            // Emitter: ModuleShock::shockButtonPressed()
            // Receiver: slot AED::useBattery(double) : connection in AED::addModuleShock(...)
            void signalDrainBatt(double amt);

            // Tells the GUI that the shock is charged, so that the button will start flashing
            // Emitter: ModuleShock::chargeReady()
            // Receiver: slot ui->shockButton->LampButton::startFlash() : connection in MainWindow::buildModuleConnections()
            void signalCharged();

            // Tells the GUI to make the shock button stop flashing (shock is aborted for some reason such as unit turned off)
            // Emitter: ModuleShock::abort()
            // Receiver: slot ui->shockButton->LampButton::stopFlash() : connection in MainWindow::buildModuleConnections()
            void signalAborted();

            // Tells the AED that the shock prep / delivery phase has completed
            // Emitter: ModuleShock::exit()
            // Receiver: slot AED::shockDelivered() : connection in AED::addModuleShock(...)
            void signalDone();

            // When a shock is delivered, this will stpo the button flashing, and also update the shock counter on the LCD screen
            // Emitter: ModuleShock::shockButtonPressed()
            // Receiver 1: slot ui->shockButton->LampButton::stopFlash() : connection in MainWindow::buildModuleConnections()
            // Receiver 2: slot LCDDisplay::setShockCounter(int) : connection in MainWindow::buildModuleConnections()
            void signalShockDelivered(int count);

            // After delivering a shock, the battery might be too depleted to continue
            // ModuleShock signals to the AED to signal it back if (and only if) there is enough battery to continue
            // This is the signal it sends.
            // Emitter: ModuleShock::shockButtonPressed()
            // Receiver: AED::scheduleShockResolution() : connection in AED::addModuleShock(...)
            void signalScheduleShockResolution();

    };
}

#endif // MODULESHOCK_H
