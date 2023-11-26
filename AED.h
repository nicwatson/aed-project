#ifndef AED_H
#define AED_H

#include <QObject>
#include <QString>

namespace aed
{

    class AED : public QObject
    {
        Q_OBJECT

        public:
            enum padState_t { UNPLUGGED, ADULT_OFF, CHILD_OFF, ADULT_ON, CHILD_ON }


            AED();

            inline bool isOn() const { return on; }
            inline const padState_t getPadState() const { return padState; }
            inline float getBattery() const { return battery; }
            inline bool willPassTest() const { return passTest; }

        private:
            bool on;                // AED is powered on
            padState_t padState;    // Pad connectvity (see type def above)
            float battery;          // Battery from 0 (depleted) to 1 (full charge)
            bool passTest;          // Assuming battery and cable fine, will AED pass self-test? (Use for modeling misc. unit failures)

        public:

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

        public slots:
            void turnOn();
            void turnOff();

            void setBattery(float newBatt);
            void eatBattery(float loseBatt);

            void voicePrompt(QString & prompt);


    };

#endif // AED_H
