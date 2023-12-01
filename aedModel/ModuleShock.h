#ifndef MODULESHOCK_H
#define MODULESHOCK_H

#include <QObject>
#include <QTimer>
#include <QString>
#include "AED.h"

#define BATT_RATIO 1.0/700.0
#define CHARGE_TIME 3000
#define STOP_TIME 1500

namespace aedModel
{

    class ModuleShock : public QObject
    {
            Q_OBJECT
        public:
            static const int ADULT_SHOCKS[];
            static const int CHILD_SHOCKS[];


            ModuleShock();

            QTimer timer;

            int calcShockEnergy(bool childPads);

            inline bool getActive() { return active; }
            inline bool getCharged() { return charged; }
            inline bool getShocksDelivered() { return shocksDelivered; }

        private:
            bool active;
            int shocksDelivered;
            bool charged;
            bool buttonPressedEarly;
            int energySet;

            bool inChargeDelay;
            bool inExitDelay;

            void cleanup();
            void reset();



        private slots:
            void chargeReady();



        public slots:
            void start(bool usingChildPads);

            // Abort because pads detached, cable disconnected, unit failure (battery), or unit turned off
            void abort();

            void exit();

            void shockButtonPressed();
            void shockButtonReleased();

        signals:
            void signalUserPrompt(const QString & prompt);
            void signalCharged();
            void signalShockDelivered(int count);
            void signalDrainBatt(float amt);
            void signalAborted();
            void signalDone();


    };
}

#endif // MODULESHOCK_H
