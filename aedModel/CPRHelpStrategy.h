// FILE: CPRHelpStrategy.h - ABSTRACT CLASS
//
// Generic abstract "strategy" for CPR Help behaviour (strategy design pattern)
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

            // Reset local vars at the end of the CPR cycle so they are ready for another use
            // This is a public-facing handle that delegates to cleanup()
            virtual void reset();

        protected:
            QTimer timer;
            bool eventActive;       // Is the CPR phase currently active?
            int duration;           // How long (ms) CPR should last

            virtual void cleanup(); // Clean up after CPR event (revert variables)


        public slots:

            // Begin executing the CPR help strategy
            // Trigger: No signals currently; direct call from ModuleCPRHelp::start(...)
            virtual void start();

            // Abort executing the CPR help strategy
            // Trigger: TODO
            virtual void abort();

            // Normal finish and exit of CPR help strategy tasks
            // Trigger: TODO
            virtual void exit();

            // Pure virtual / abstract slots

            // Update compression depth
            // Trigger: No signals currently; direct call from ModuleCPRHelp::updateCompressionDepth(int) which is itself a slot with signal triggers
            virtual void updateCompressionDepth(int depth) = 0;

            // Update compression rate (overrides abstract)
            // Trigger: No signals currently; direct call from ModuleCPRHelp::updateCompressionDepth(int) which is itself a slot with signal triggers
            virtual void updateCompressionRate(int cpm) = 0;

            // User starts doing compressions
            // Trigger: No signals currently; direct call from ModuleCPRHelp::startCompressions() which is itself a slot with signal triggers
            virtual void startCompressions() = 0;

            // User stops doing compressions
            // Trigger: No signals currently; direct call from ModuleCPRHelp::stopCompressions() which is itself a slot with signal triggers
            virtual void stopCompressions() = 0;

        signals:

            // Signal that the strategy has finished its work
            // Emitter: CPRHelpStrategy::exit()
            // Receiver: slot ModuleCPRHelp::cprCompleted() : connection in ModuleCPRHelp() constructor
            void signalCPRStrategyEnded();

            // Normal LCD/voice prompt signal - use for CPR-related prompts specified in the Admin Guide
            // i.e. relating to compression depth as well as start/stop CPR.
            // Emitters: CPRHelpAdult::noCPRDetected(), CPRHelpStrategy::start(), CPRHelpStrategy::stop()
            // Receiver: slot ModuleCPRHelp::forwardUserPrompt(...) : connection in ModuleCPRHelp constructor
            void signalForwardUserPrompt(const QString & prompt);

            // Special secondary CPR compression rate prompt signal.
            // Use for prompts related to compression rate. This is our alternative to the real AED Plus's
            // adaptive metronome function. This should be linked to the LCD's secondary CPR-specific QLabel.
            // Use two separate QLabels so that depth- and rate-related prompts do not overwrite each other in realtime.
            // Emitter: CPRHelpAdult::doCompression()
            // Receiver: slot ModuleCPRHelp::forwardCPRCompressionRatePrompt(...) : connection in ModuleCPRHelp constructor
            void signalCPRCompressionRatePrompt(const QString & prompt);


            // Use these to update the text of the "Start/Stop CPR" button

            // Emitters: CPRHelpAdult::startCompressions(), CPRHelpChild::startCompressions()
            // Receiver: slot ModuleCPRHelp::compressionsStarted(...) : connection in ModuleCPRHelp constructor
            void signalCompressionsStarted();
            // Emitters: CPRHelpAdult::startCompressions(), CPRHelpChild::startCompressions()
            // Receiver: slot ModuleCPRHelp::compressionsStopped(...) : connection in ModuleCPRHelp constructor
            void signalCompressionsStopped();

            // Used to update compression depth gauge on LCD (passing through the ModuleCPRHelp), if applicable
            // Emitter: CPRHelpAdult::doCompression()
            // Receiver: ModuleCPRHelp::forwardCompressionDepth(int) : connection in ModuleCPRHelp constructor
            void signalDisplayCompressionDepth(int depth);

    };


}

#endif // CPRHELPSTRATEGY_H
