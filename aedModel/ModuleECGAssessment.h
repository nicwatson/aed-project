#ifndef AED_MODULEECGASSESSMENT_H
#define AED_MODULEECGASSESSMENT_H

#include <QObject>
#include <QTimer>
#include <QDir>
#include <QDebug>
#include <QString>
#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include "aedGui/LCDDisplay.h"
#include "AED.h"

namespace aedModel {

// CONTROL IN:
//      * a slot receives a signal telling the module to begin reading an ECG
//      * a slot receives a signal telling the module to stop reading an ECG
// DATA OUT:
//      * A signal for shockable rhythm
//      * A signal for non-shockable rhythm
//
// AED or MainWindow will handle connecting the signals and slots between ModuleECGAssessment and AED
//
// Set "active" bool flag to true when ECG is running, set to false when it's done.
//
// OTHER CONSIDERATIONS
//
//      * ECG cannot run if pads are disconnected or cable unplugged. The endAssessment() slot can be used for this.
//      * AED will be aware of the reason (pad/cable disconnection), will issue the stop signal and handle the error message itself.
//          ModuleECGAnalysis doesn't need to know the specific reason - it just needs to stop the analysis and blank out the
//          waveform diagram (if applicable).
//      * Deciding on the type of rhythm detected: will we just use radio buttons so that the user can specify whether to simulate
//          V-FIB, V-TACH, or OTHER (non-shockable)? If so, ModuleECGAssessment can perhaps poll/read that widget directly instead
//          of having to have the data sent in by AED when ECG starts.
//      * Outputting the ECG waveform:
//          If we are doing this, ModuleECGAssessment needs to talk in some way to a widget that will display the graph. This could
//          be a pointer to the widget directly, or to the LCD class (which is an organizing container for all the widgets that appear
//          in the LCD scren area); or it could be done by signals and slots. But the logistics depend on whether we're using static
//          images or some kind of dynamic/animated line graph drawing (send some numbers representing points and lines to the widget,
//          which draws them).
//      * In any case, AED itself doesn't need that information -- ModuleECGAssessment should deal directly with LCD or GUI widget for this.
//          * Also means that ModuleECGAssessment is responsible for blanking out the waveform display on the LCD/GUI once analysis is
//              complete.

    class ModuleECGAssessment : public QObject
    {
        Q_OBJECT
    public:

        enum rhythm_t { VENT_FIB, VENT_TACHY, NON_SHOCKABLE };

        // Used for debugging - translate between rhythm_t underlying integers and readable strings
        static const QString rhythmNames[];

        explicit ModuleECGAssessment(aedGui::LCDDisplay* l);
        ~ModuleECGAssessment();

    private:
        bool active;        // Is the ECG assessment happening right now?
        rhythm_t rhythm;    // Rhythm type of the patient (see type def above). We can set this via GUI
        QTimer timer;       // Simulates the 5 seconds of analysis
        aedGui::LCDDisplay* lcdDisplay;         // Pointer to the LCD display so that we can directly update graph data

        // Pre-configured graph plots for the three ECG scenarios (loaded from CSV files)
        QVector<double> ventTachyXData;
        QVector<double> ventTachyYData;
        QVector<double> ventFibXData;
        QVector<double> ventFibYData;
        QVector<double> nonShockableXData;
        QVector<double> nonShockableYData;

        // Load graph data from CSV file
        void readCSVFile(const QString & fileName);


    public slots:

        // When a signal is received from AED to this slot, the assessment will begin.
        // AED will be responsible for making sure that cables and pads are hooked up properly, so no need to recheck that.
        // Trigger: signal AED::signalStartECG() : connection in AED::addModuleECG(...)
        void startAssessment();

        // When a signal is received to this slot, the assessment should end immediately. The ECG module can also invoke this function (directly)
        // when ending an ECG assessment normally.
        // Trigger 1: signal AED::signalAbortECG() : connection in AED::addModuleECG(...)
        // Trigger 2: Direct call from ModuleECGAssessment::sendShockableSignal()
        // Trigger 3: Direct call from ModuleECGAssessment::sendNonShockableSignal()
        void endAssessment();

        // Sets the rhythm (v-fib, v-tach, or generic unshockable) that will be simulated in the ECG
        // Triggers: QradioButton::pressed() signals from GUI radio buttons for selecting the rhythm:
        //      ui->tachyButton, ui->fibbutton, ui->nonShockableButton
        // Connection is in MainWindow::buildModuleConnections()
        void setRhythm(rhythm_t  r) { rhythm = r; }

    private slots:

        // Receives signal from the module's timer when assessment is complete, triggering the module to send a signal to the AED
        // Trigger: timer->QTimer::timeout() : connection in ModuleECGAssessment::startAssessment()
        void sendShockableSignal();

        // Receives signal from the module's timer when assessment is complete, triggering the module to send a signal to the AED
        // Trigger: timer->QTimer::timeout() : connection in ModuleECGAssessment::startAssessment()
        void sendNonShockableSignal();


    signals:
        // Talks to the AED object, which updates the GUI
        // Emitters: ModuleECGAssessment::startAssessment(), ::endAssessment(), ::sendShockableSignal(), ::sendNonShockableSignal()
        // Receiver: slot AED::userPrompt() : connection is in AED::addModuleECG(...)
        void signalForwardUserPrompt(const QString & prompt);

        // Emit this signal if the rhythm is shockable.
        // Emitter: ModuleECGAssessment::sendShockableSignal()
        // Receiver: Currently none - using the general purpose signalResult(bool) instead
        void signalShockable();

        // Emit this signal if the rhythm is not shockable.
        // Emitter: ModuleECGAssessment::sendNonShockableSignal()
        // Receiver: Currently none - using the general purpose signalResult(bool) instead
        void signalNotShockable();

        // Emit this signal with a 'true' argument if the rhythm is shockable, and 'false' if it's non-shockable. AED will listen for this signal.
        // Emitters: ModuleECGAssesment::sendShockableSignal(), ::sendNonShocableSignal()
        // Receiver: slot AED::ecgResult(bool) : connection in AED::addModuleECG(...)
        void signalResult(bool);

    };

} // namespace aed

#endif // AED_MODULEECGASSESSMENT_H
