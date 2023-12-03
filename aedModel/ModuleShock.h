// FILE ModuleShock.h
//
// Handles charging and delivering shock to patient.

#ifndef MODULESHOCK_H
#define MODULESHOCK_H

#include <QObject>
#include <QTimer>
#include <QString>
#include "AED.h"

#define BATT_RATIO 1.0/700.0
#define CHARGE_TIME 3000
#define STOP_TIME 1500

// TODO there are GUI widget connections needed for this class: Look for "TODO" comments below
// SLOT shockButtonPressed
// SLOT shockButtonReleased
// SIGNAL signalShockDelivered

namespace aedModel
{
    class ModuleShock : public QObject
    {
            Q_OBJECT
        public:
            static const int ADULT_SHOCKS[];    // Stores the energy levels for adult shocks
            static const int CHILD_SHOCKS[];    // Stores the energy levels for child shocks

            ModuleShock();

            inline bool getActive() { return active; }
            inline bool getCharged() { return charged; }
            inline bool getShocksDelivered() { return shocksDelivered; }

            // Retrieves the proper shock energy based on how many shocks have been delivered so
            // far, and whether adult or child pads are plugged in (pass true for child pads, false
            // for adult)
            int calcShockEnergy(bool childPads);
            void dummy() {
                emit signalCharged();
                QTimer::singleShot(10000, this, [=]() {
                    emit signalAborted();
                    emit signalShockDelivered(4);
                });
            }

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
            void chargeReady();         // Charge is ready to go

        public slots:
            void start(bool usingChildPads);

            // Abort because pads detached, cable disconnected, unit failure (battery), or unit turned off
            void abort();

            void exit();

            // COMPLETE Connect the GUI's shock button's pressed and released signals to these slots
            void shockButtonPressed();
            void shockButtonReleased();

            // Used when unit power is turned off
            void fullReset();

        signals:
            // These two talk to the AED object and don't need to be connected to widgets directly
            void signalUserPrompt(const QString & prompt);
            void signalDrainBatt(double amt);

            // COMPLETE connect this to the shock button to make it flash
            void signalCharged();

            // COMPLETE connect this to the flashing shock button to make it stop flashing if the shock event is aborted (e.g. unit turned off)
            void signalAborted();

            void signalDone();

            // COMPLETE This should connect to the LCD to update the shock count.
            // Not sure if it can connect directly to the QLabel. The LCD may need some intermediate slot
            // to convert the number to a string?
            // This should also stop the shock button from flashing.
            void signalShockDelivered(int count);


    };
}

#endif // MODULESHOCK_H
