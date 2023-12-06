// FILE CPRHelpAdult.h
//
// This concrete CPR Help "strategy" defines the behaviour of the CPR help module for adult patients (i.e.
// with adult pads plugged in). As per the Admin Guide spec, RealCPR Help provides feedback based
// on the rate and depth of CPR compressions.

#ifndef CPRHELPADULT_H
#define CPRHELPADULT_H

#include <QObject>
#include <QTimer>
#include "CPRHelpStrategy.h"


namespace aedModel
{
    class CPRHelpAdult : public CPRHelpStrategy
    {
        Q_OBJECT

        public:
            explicit CPRHelpAdult();
            ~CPRHelpAdult();

            // Getters
            inline bool getCompressionsActive() const { return compressionsActive; }

        protected:
            // Reset vars after CPR event cycle
            virtual void cleanup() override;

        private:

            // compressionsActive if the user has clicked the "START CPR" button. This is distinct
            // from the CPR "event" itself being active, meaning it's time for CPR but it's not necessarily
            // happening without the user taking action.
            bool compressionsActive;

            // Keeps track of compression depth (1/10th inches) and rate (cpm).
            int compressionDepth;
            int compressionRate;

            // Flag determines if the user was previously issued a "PUSH HARDER" prompt.
            // The "GOOD COMPRESSIONS" prompt is contingent on having received "PUSH HARDER" previously.
            bool pushHarder;

            // Flag determines if the "Continue CPR" prompt is currently active
            bool continueCPR;

            // This timer will be reset every time a compression occurs. If it ever expires, the AED will
            // issue the "CONTINUE CPR" prompt.
            QTimer noCPRTimer;

            // This timer's interval is set to (60000 / compressionRate) and is used to issue compression events
            // at regular intervals.
            QTimer cprCyclicTimer;

        public slots:

            // Update compression depth (overrides abstract)
            // Trigger: No signals currently; direct call from ModuleCPRHelp::updateCompressionDepth(int) which is itself a slot with signal triggers
            virtual void updateCompressionDepth(int depth) override;

            // Update compression rate (overrides abstract)
            // Trigger: No signals currently; direct call from ModuleCPRHelp::updateCompressionDepth(int) which is itself a slot with signal triggers
            virtual void updateCompressionRate(int cpm) override;

            // User starts doing compressions
            // Trigger: No signals currently; direct call from ModuleCPRHelp::startCompressions() which is itself a slot with signal triggers
            virtual void startCompressions() override;

            // User stops doing compressions
            // Trigger: No signals currently; direct call from ModuleCPRHelp::stopCompressions() which is itself a slot with signal triggers
            virtual void stopCompressions() override;

            // Begin executing the CPR help strategy
            // Trigger: No signals currently; direct call from ModuleCPRHelp::start(...)
            virtual void start() override;

        private slots:

            // A compression event - occurs whenever cprCyclicTimer times out
            // If CPR event and compressions are active, this will:
            // + reset the noCPRtimer
            // + issue a prompt to the special secondary CPR QLabel advising on the compression rate
            // + update the compression depth bar on the LCD
            // + if applicable, issue a prompt to the primary LCD QLabel advising on compression depth
            // This way, although we don't make the user click repetitively to emulate CPR, we still simulate
            // the inner workings of a sensor that has to make decisions based on repeatedly receiving a signal.
            // Trigger: signal cprCyclicTimer.QTimer::timeout() : connection in CPRHelpAdult() constructor
            void doCompression();

            // Triggered when too long since a compression occurred - used for "CONTINUE CPR" prompt
            // Trigger: signal noCPRTimer.QTimer::timeout() : connection in CPRHelpAdult() constructor
            void noCPRDetected();

    };


}

#endif // CPRHELPADULT_H
