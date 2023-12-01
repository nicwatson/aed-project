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
// Event entry point: start() slot
// Event exit signal: signalCPRComplete

#ifndef MODULECPRHELP_H
#define MODULECPRHELP_H

#include "AED.h"
#include "CPRHelpStrategy.h"

// TODO there are GUI widget connections needed for this class: Look for "TODO" comments below
// SLOT updateCompressionDepth
// SLOT updateCompressionRate
// SLOT startCompressions
// SLOT stopCompressions
// SIGNAL signalUserPrompt
// SIGNAL signalCPRAdvicePrompt
// SIGNAL signalDisplayCompressionDepth
// Note that nothing needs to be connected directly to the CPRHelpStrategies. The ModuleCPRHelp
// acts as a middleperson betwen UI widgets and strategy objects.
//
// Some options:
//      Instantiate ModuleCPRHelp in MainWindow and then connect signals and slots individually from there
//      Change the constructor to take all the widgets as arguments (pointers), and link them in the constructor.
//          (No need for the ModuleCPRHelp to hang on to widget pointers after it has finished connecting them.)

namespace aedModel
{

    class ModuleCPRHelp : public QObject
    {
        Q_OBJECT

        public:
            enum cprStrategy_t { CPR_ADULT, CPR_CHILD };

            explicit ModuleCPRHelp();
            ~ModuleCPRHelp();

            inline bool getActive() { return active; }

            void setStrategy(CPRHelpStrategy *);

        private:
            CPRHelpStrategy ** strategyList;

            CPRHelpStrategy * strategy;

            int numStrategies;

            bool active;

            void reset();

        public slots:
            void start(AED::cableState_t padType);
            void abort();


            // TODO Connect this to the user input slider for compression depth
            // Compression depth should be represented as an int in 1/10th inches (e.g. 2.4 inches = 24)
            // Min and max of the QProgressBar should be set accordingly.
            void updateCompressionDepth(int depth);             // Info from GUI, incoming to Strategy

            // TODO Connect this to the user input slider for compression rate
            void updateCompressionRate(int cpm);                // Info from GUI, incoming to Strategy

            // Not sure how signals work for the toggleable/checkable type of QPushButton --NW
            // TODO If necessary, add a slot here compressionsToggled(bool) that will be connected to the button's toggled(bool) signal
            // in order to see if the click was to check or uncheck the button, and then directly call startCompressions() or
            // stopCompressions() as appropriate
            // TODO connect the GUI "Start compressions" button to this (when toggled ON)
            void startCompressions();                           // Info from GUI, incoming to Strategy
            // TODO connect the GUI "Stop compressions" button to this (when toggled OFF)
            void stopCompressions();                            // Info from GUI, incoming to Strategy

        protected slots:
            void forwardUserPrompt(const QString & prompt);     // Info from Strategy, outgoing to GUI via signalUserPrompt
            void forwardCPRPrompt(const QString & prompt);      // Info from Strategy, outgoing to GUI via signalCPRAdvicePrompt
            void forwardCompressionDepth(int depth);            // Info from Strategy, outgoing to GUI via signalDisplayCompressionDepth

            void cprCompleted();

        signals:
            // This doens't need a direct connection to the LCD - it can be passed through the AED
            void signalUserPrompt(const QString & prompt);      // Info from Strategy, outgoing to GUI

            // TODO connect this signal to the secondary CPR advice QLabel in the LCD
            void signalCPRAdvicePrompt(const QString & prompt); // Info from Strategy, outgoing to GUI

            // TODO connect this signal to the QProgressBar in the LCD that shows compression depth
            // depth will represent 1/10th inches
            // This will be the same value that was received by the updateCompressionDepth() slot, but the compression depth slider is
            // a meta-element of the GUI and should not communicate directly with the LCD, so the ModuleCPRHelp should instead update
            // the LCD, simulating the idea that it is sensing compression depth directly from user action
            void signalDisplayCompressionDepth(int depth);      // Info from Strategy, outgoing to GUI

            void signalCPRComplete();
            void signalCPRAborted();
    };

}

#endif // MODULECPRHELP_H
