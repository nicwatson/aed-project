// FILE: CPRHelpStrategy.h - ABSTRACT CLASS
//
// Generic "strategy" for CPR Help behaviour
// The basic behaviour is to just run a timer when start() is called, and send the
// signalCPREnded() when the timer runs out.
// Other slots and signals provide polymorphic handles that subclasses should override
// to define strategy-specific behaviours.

#ifndef CPRHELPSTRATEGY_H
#define CPRHELPSTRATEGY_H

#include <QObject>
#include <QTimer>
#include <QDebug>
#include "aedconst.h"


// No direct GUI connections needed here.
// CPRHelpStrategy communicates with GUI through ModuleCPRHelp

namespace aedModel
{
    class CPRHelpStrategy : public QObject      // ABSTRACT
    {
        Q_OBJECT

        public:
            explicit CPRHelpStrategy(int duration);
            ~CPRHelpStrategy();

            virtual void reset();

        protected:
            QTimer timer;
            bool eventActive;
            int duration;

            virtual void cleanup();


        public slots:
            virtual void start();
            virtual void abort();
            virtual void exit();

            virtual void updateCompressionDepth(int depth) = 0;
            virtual void updateCompressionRate(int cpm) = 0;

            virtual void startCompressions() = 0;
            virtual void stopCompressions() = 0;

        signals:
            void signalCPRStrategyEnded();

            // Normal LCD/voice prompt signal - use for CPR-related prompts specified in the Admin Guide
            // i.e. relating to compression depth as well as start/stop CPR.
            void signalUserPrompt(const QString & prompt);

            // Special secondary CPR compression rate prompt signal.
            // Use for prompts related to compression rate. This is our alternative to the real AED Plus's
            // adaptive metronome function. This should be linked to the LCD's secondary CPR-specific QLabel.
            // Use two separate QLabels so that depth- and rate-related prompts do not overwrite each other in realtime.
            void signalCPRCompressionRatePrompt(const QString & prompt);

            // These signals are defined in case clients have a use for them.
            // In the current version, there is nothing listening for these particular signals, but they are included
            // as extensiblity hooks. SHAUN: We will use these to update the text of the  "Start compressions" buttonn
            void signalCompressionsStarted();
            void signalCompressionsStopped();

            // Used to update compression depth gauge on LCD (passing through the ModuleCPRHelp), if applicable
            void signalDisplayCompressionDepth(int depth);

    };


}

#endif // CPRHELPSTRATEGY_H
