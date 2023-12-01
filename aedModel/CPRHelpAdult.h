#ifndef CPRHELPADULT_H
#define CPRHELPADULT_H

#include <QObject>
#include <QTimer>
#include "CPRHelpStrategy.h"

// CONSTANTS

// Time interval (ms) that CPR should last for an adult patient (happens to be the same as for a child patient)
#define CPR_TIME_ADULT CPR_TIME_COMMON

// Recommended minimum depth of compressions for adult patient
#define CPR_ADULT_DEPTH_MIN 20

// Recommended maximum depth of compressions for adult patient
#define CPR_ADULT_DEPTH_MAX 24

// How long should we wait for compressions, before displaying "CONTINUE CPR" prompt?
#define CPR_DELAY_TOLERANCE 4000

// CPM thressholds for giving "go slightly faster" or "go faster" prompts
#define CPR_SLOW_THRESHHOLD 95
#define CPR_VERYSLOW_THRESHHOLD 80

namespace aedModel
{
    class CPRHelpAdult : public CPRHelpStrategy
    {
        Q_OBJECT

        public:
            CPRHelpAdult();

        protected:
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

            // This timer will be reset every time a compression occurs. If it ever expires, the AED will
            // issue the "CONTINUE CPR" prompt.
            QTimer noCPRTimer;

            // This timer's interval is set to (60000 / compressionRate) and is used to issue compression events
            // at regular intervals.
            QTimer cprCyclicTimer;

        public slots:

            // Update compression depth (overrides abstract)
            virtual void updateCompressionDepth(int depth) override;
            // Update compression rate (overrides abstract)
            virtual void updateCompressionRate(int cpm) override;

            // User starts doing compressions
            virtual void startCompressions() override;
            // User stops doing compressions
            virtual void stopCompressions() override;

        protected slots:
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
            void doCompression();

            // Triggered when too long since a compression occurred - used for "CONTINUE CPR" prompt
            void noCPRDetected();

        signals:
            // Used to update compression depth gauge on LCD
            void signalUpdateCompressionDepth(int depth);

    };


}

#endif // CPRHELPADULT_H
