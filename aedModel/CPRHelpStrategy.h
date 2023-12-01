#ifndef CPRHELPSTRATEGY_H
#define CPRHELPSTRATEGY_H

#include <QObject>
#include <QTimer>

#define CPR_TIME_COMMON 120000
#define CPR_TARGET_MIN 95
#define CPR_TOO_SLOW 80

namespace aedModel
{
    class CPRHelpStrategy : public QObject      // ABSTRACT
    {
        Q_OBJECT

        public:
            CPRHelpStrategy(int duration);
            ~CPRHelpStrategy();

        protected:
            QTimer timer;
            bool eventActive;
            int duration;

            virtual void cleanup();
            virtual void reset();

        protected slots:
            virtual void start();
            virtual void abort();
            virtual void exit();

        public slots:
            virtual void updateCompressionDepth(int depth) = 0;
            virtual void updateCompressionRate(int cpm) = 0;

            virtual void startCompressions() = 0;
            virtual void stopCompressions() = 0;

        signals:
            void signalCPRStarted();
            void signalCPRAborted();
            void signalCPREnded();

            // Normal LCD/voice prompt signal - use for CPR-related prompts specified in the Admin Guide
            // i.e. relating to compression depth as well as start/stop CPR.
            // This should be linked to the LCD's main user prompt QLabel.
            void signalUserPrompt(const QString & prompt);

            // Special secondary CPR advice prompt signal.
            // Use for prompts related to compression rate. This is our alternative to the real AED Plus's
            // adaptive metronome function. This should be linked to the LCD's secondary CPR-specific QLabel.
            // Use two separate QLabels so that depth- and rate-related prompts do not overwrite each other in realtime.
            void signalCPRAdvicePrompt(const QString & prompt);

            // These signals are defined in case clients have a use for them.
            // In the current version, there is nothing listening for these particular signals, but they are included
            // as extensiblity hooks.
            void signalCompressionsStarted();
            void signalCompressionsStopped();

    };


}

#endif // CPRHELPSTRATEGY_H
