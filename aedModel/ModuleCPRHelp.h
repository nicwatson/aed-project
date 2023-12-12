// FILE: ModuleCPRHelp.h
//
// Helper module for CPR stage
// Handles sensing user's CPR actions and giving advice
// According to the Admin Guide, RealCPR Help is only available on adult pads.
// ModuleCPRHelper uses a strategy design pattern with a polymorphic CPRHelpStrategy
// object to swap behaviours for child vs. adult pads
//
// To use: First instantiate the ModuleCPRHelp (e.g. in MainWindow) and connect slots and signals
// to GUI elements. Then call addModuleCPR() on the AED object, passing in a pointer to the ModuleCPRHelp.
// The AED object will link its own signals/slots with the ModuleCPRHelp.
//
// Note: ModuleCPRHelp and its component strategies distinguish between the CPR event/phase wherein the
// AED says that it's time to do two minutes of CPR, and the actual start/stop of active compressions.
// The former (CPR event) is triggered by the AED transitioning from ECG (nonshockable) or shock stages,
// and automatically terminates after two minutes.
// The latter (active compressions) is a user action: they click the "Start CPR" button to indicate that
// they are now administering compressions; its text changes to "Stop CPR," and when they click that, they
// stop doing compressions.
// Compressions cannot be started when CPR event is not active (button is disabled); compressions automatically
// stop if user does not manually stop them before the end of the CPR event.
//
// ModuleCPRHelp is responsible for informing strategies about the start/end of the CPR event as well as
// the start/stop of active compressions. In practice, only CPRHelpAdult cares about them.


#ifndef MODULECPRHELP_H
#define MODULECPRHELP_H

#include <QDebug>
#include "AED.h"
#include "CPRHelpStrategy.h"


namespace aedModel
{

    class ModuleCPRHelp : public QObject
    {
        Q_OBJECT

        public:
            // Type for distinguishing the different strategy objects available
            enum cprStrategy_t { CPR_ADULT, CPR_CHILD };

            explicit ModuleCPRHelp();
            ~ModuleCPRHelp();

            // Getters/setters
            inline bool getActive() { return active; }

            void setStrategy(CPRHelpStrategy *);

        private:
            // Array of available strategies
            CPRHelpStrategy ** strategyList;

            // Number of available strategies
            int numStrategies;

            // Pointer to current active strategy
            CPRHelpStrategy * strategy;

            // Is CPR event currently active?
            bool active;

            // Reset vars for next use
            void reset();

            // Start CPR compressions (pass through to strategy)
            void startCompressions();
            // Stop CPR compressions (pass through to strategy)
            void stopCompressions();

        public slots:

            // Start the CPR event, choosing a strategy based on the type of attached pads
            // Trigger: AED::signalStartCPR() : connection in AED::addModuleCPR(...)
            void start(AED::cableState_t padType);

            // Immediately abort the CPR phase
            // Trigger: AED::signalAbortCPR() : connection in AED::addModuleCPR(...)
            void abort();

            // Updates the CPR module on the compression depth the user has chosen, using the GUI slider
            // Compression depth is represented as an int in 1/10th inches (e.g. 2.4 inches = 24)
            // Triggers: signal ui->cprDepthSlider->QSlider::valueChanged(int), signal ui->toggleCompressionsButton->CompressionsToggleButton::signalButtonToggleCompressions()
            //      : connections are in MainWindow::buildModuleConnections()
            void updateCompressionDepth(int depth);

            // Updates the CPR module on the compression rate the user has chosen, using the GUI slider
            // Compression rate is measured in compressions per minute (CPM)
            // Triggers: ui->cprRateSlider->QSlider::valueChanged(int), ui->toggleCompressionsButton->CompressionsToggleButton::signalButtonToggleCompressions()
            //      : connections are in MainWindow::buildModuleConnections()
            void updateCompressionRate(int cpm);                // Info from GUI, incoming to Strategy

            // Shaun- I did this differently, added compressionsStarted and compressionsStopped to toggle the text on the QPushButton.
            // I wanted the message to come from the Strategy because I don't want the text to change unless the command was successful.
            // Nic [reply]: Looks like I may have bulldozed over what you did with the compressionsStarted/Stopped signals without realizing it, when I
            // made a dedicated CompressionsToggleButton class. I was trying to simplify the toggle button (reduce the signals and slots) so that I could
            // debug the CPR module which was very broken at the time. Sorry about that. I understand what you're saying about wanting the toggle button
            // text to change on a callback from the CPR module or its active strategy, rather than directly from the click action -- but can we get by without
            // it? Otherwise it adds multiple additional signals, slots, and connectors.
            //
            // Toggle the start/stop of active compressions
            // Trigger: ui->toggleCompressionsButton->CompressionsToggleButton::signalButtonToggleCompressions() : connection in MainWindow::buildModuleConnections
            void toggleCompressions(bool start); // Info from GUI, incoming to Strategy

            //void compressionsStarted(); // Info from Strategy, incoming to GUI 'Start/Stop Compressions' toggle button
            //void compressionsStopped(); // Info from Strategy, incoming to GUI 'Start/Stop Compressions' toggle button

        protected slots:

            // Forward a user prompt (generally incoming from the active Strategy) to a listener (e.g. the AED) via ModuleCPRHelp::signalForwardUserPrompt(...)
            // Trigger: CPRHelpStrategy::signalForwardUserPrompt(...) : connection in ModuleCPRHelp constructor
            void forwardUserPrompt(const QString & prompt);

            // Forward a CPR-specific user prompt (incoming from Strategy) to a GUI listener (in this case, the LCD) via ModuleCPRHelp::signalCPRCompressionRatePrompt(...)
            // Trigger: CPRHelpStrategy::signalCPRCompressionRatePrompt(...) : connection in ModuleCPRHelp constructor
            void forwardCPRCompressionRatePrompt(const QString & prompt);

            // Forward compression depth info (incoming from Strategy) to the GUI (LCD) via ModuleCPRHelp::signalDisplayCompressionDepth(int)
            // Trigger: CPRHelpStrategy::signalDisplayCompressionDepth(int) : connection in ModuleCPRHelp constructor
            void forwardCompressionDepth(int depth);

            // Receives signal from the Strategy that CPR tasks have concluded, and carries out wrap-up of CPR phase accordingly
            // Trigger: CPRHelpStrategy::signalCPRStrategyEnded() : connection in ModuleCPRHelp constructor
            void cprCompleted();

        signals:

            // Signals general user advice prompt for printing on the LCD (but it is forwarded through AED)
            // Emitter: ModuleCPRHelp::forwardUserPrompt(...), itself a slot receiving prompt from Strategy object
            // Receiver: AED::userPrompt(...) : connection in AED::addModuleCPR(...)
            void signalForwardUserPrompt(const QString & prompt);

            // Signals CPR-specific user prompt to the LCD
            // Emitter: ModuleCPRHelp::forwardCPRCompressionRatePrompt(...), itself a slot receiving prompt from Strategy object
            // Receiver: slot ui->lcdCPRHelp->QLabel::setText(...) : connection in MainWindow::buildModuleConnections()
            void signalCPRCompressionRatePrompt(const QString & prompt); // Info from Strategy, outgoing to GUI

            // This signal connects to the QProgressBar in the LCD that shows compression depth.
            // This will be the same value that was received by the updateCompressionDepth() slot, but the compression depth slider is
            // a meta-element of the GUI and should not communicate directly with the LCD, so the ModuleCPRHelp should instead update
            // the LCD, simulating the idea that it is sensing compression depth directly from user action
            // Emitter: ModuleCPRHelp::forwardCompressionDepth(int)
            // Receiver: slot ui->compressionDepthBar->QProgressBar::setValue(int) : connection in MainWindow::buildModuleConnections()
            void signalDisplayCompressionDepth(int depth);

            // Signals to control the availability of the start/stop CPR button based on the start/stop of the main CPR event phase
            // Connections are all in MainWindow::buildModuleConnections()
            // Emitters (in order, respectively): ModuleCPRHelp::start(), ModuleCPRHelp::cprCompleted(), ModuleCPRHelp::abort()
            // Receiver object is ui->toggleCompressionsButton :
            // signalCPRStarted() => CompressionsToggleButton::enableCPR()
            // signalCPRComplete(), signalCPRAborted() => CompressionsToggleButton::disableCPR()
            void signalCPRStarted();  // (Unchecks and) enables GUI button toggleCompressionsButton
            void signalCPRComplete(); // Slot is cprStopped() on AED class ... Also unchecks and disables GUI button toggleCompressionsButton
            void signalCPRAborted();  // Unchecks and disables GUI button toggleCompressionsButton

            //void signalCompressionsStarted(); // Info from Strategy, incoming to GUI 'Start/Stop Compressions' toggle button
            //void signalCompressionsStopped(); // Info from Strategy, incoming to GUI 'Start/Stop Compressions' toggle button
    };

}

#endif // MODULECPRHELP_H
