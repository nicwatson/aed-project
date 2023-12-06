// FILE LCDDisplay.h
//
// Container class for the AED's LCD display

#ifndef LCDDISPLAY_H
#define LCDDISPLAY_H

#include <QObject>
#include <QLabel>
#include <QProgressBar>
#include <QPixmap>
#include <QDir>
#include <QCoreApplication>
#include <QDebug>
#include <QTimer>
#include <QString>
#include <QFrame>
#include  "qcustomplot.h"

namespace aedGui
{
    // Type def of struct to hold the widget component pointers that need to be passed to the constructor
    struct LCDDisplayParams {
        QFrame* lcdDisplayFrame;
        QCustomPlot* ecgGraph;
        QLabel* promptLabel;
        QLabel* helpLabel;
        QLabel* shockCounter;
        QLabel* timer;
        QProgressBar* compressionDepthBar;
    };

    class LCDDisplay : public QObject
    {
        Q_OBJECT

    public:
        explicit LCDDisplay(const LCDDisplayParams& params);
        ~LCDDisplay();
        void plotGraphData();
        void clearGraphData();
        QVector<double> getGraphXData() {return graphXData;}
        QVector<double> getGraphYData() {return graphYData;}

    private:
        bool active;    // Is the LCD Display already on?

        // Component widgets
        QFrame* lcdDisplayFrame;
        QCustomPlot* ecgGraph;
        QLabel* promptLabel;
        QLabel* cprHelpLabel;
        QLabel* shockCounter;
        QLabel* timer;
        QProgressBar* compressionDepthBar;

        // Stores the ECG graph data received from the AED's ECG module
        QVector<double> graphXData;
        QVector<double> graphYData;

        // For the "how long has the AED been running" timer display
        // This repeats at 1-second intervals. At every timeout, it increments elapsedTime and updates the display widget.
        QTimer* runningTimer;

        int elapsedTime;    // AED uptime in seconds

        // Format the time for displaying on the LCD
        QString formatTime(int elapsedSeconds);

        void setLCDDisplayVisible(bool visible);    // Sets LCD Display children visibility

    public slots:

        // Turns on display
        // Trigger: signal AED::signalStartTest() : connection in MainWindow::buildAEDConnections()
        void startLCD();

        // Turns off display
        // Trigger: signal AED::signalPowerOff() : connection in MainWindow::buildAEDConnections()
        void endLCD();

        // Receives ECG graph data to store locally
        // Trigger: no signals; direct invocations in ModuleECGAssessment::startAssessment()
        //      Note: ModuleECGAssessment holds a direct pointer to its client LCDDisplay
        void setGraphData(QVector<double>* xDataToCopy, QVector<double>* yDataToCopy);

        // Sets the compression depth bar - takes depth in 1/10th inches
        // Trigger: no signals; used in LCDDisplay::startLCD() to initialize bar to 0.
        // In the current design, the compression depth bar QProgressBar::setValue(int) accepts signals directly so they do not have to always be passed through this slot.
        void setCompressionDepthBar(int depth);     // Sets compression depth bar. Takes depth in 1/10th inches

        // Sets the shock counter on the display to the given arg
        // Trigger 1: Direct invocation in LCDDisplay::startLCD(), initializing count to 0
        // Trigger 2: signal ModuleShock::signalShockDelivered(int) : connection in MainWindow::buildModuleConnections()
        void setShockCounter(int shockCounter);

        // Sets the main user advice prompt text
        // Trigger: AED::signalUserPrompt(const QString &) : connection in MainWindow::buildAEDConnections()
        void setPromptLabel(const QString & msg) {promptLabel->setText(msg);}

        // Sets the main user advice prompt text
        // Trigger: no signals; direct invocation in LCDDisplay::startLCD() to blank it out
        // In the current design, the target QLabel::setText(...) accepts signals directly so they don't have to always be passed through this slot
        void setCPRHelpLabel(const QString & msg) {cprHelpLabel->setText(msg);}

    private slots:

        // Updates the widget that displays elapsed uptime
        // Trigger 1: direct invocation from LCDDisplay::startLCD to initialize it
        // Trigger 2: runningTimer->QTimer::timeout() : connection in LCDDisplay constructor
        void updateTimer();
    };
}

#endif // LCDDISPLAY_H
