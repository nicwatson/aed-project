// FILE LCDDisplay.cpp
// Implementations for LCDDisplay class

#include "LCDDisplay.h"
#include "aedGui/strings.h"
#include "styles.h"
#include "aedconst.h"
#include <QDebug>

using namespace aedGui;

LCDDisplay::LCDDisplay(const LCDDisplayParams& params)
    : active(false),
        lcdDisplayFrame(params.lcdDisplayFrame),
        ecgGraph(params.ecgGraph), 
        promptLabel(params.promptLabel), 
        cprHelpLabel(params.helpLabel), 
        shockCounter(params.shockCounter), 
        timer(params.timer), 
        compressionDepthBar(params.compressionDepthBar),
        elapsedTime(-1) // Allows startLCD() to set timer to '00:00'
{
    qDebug() << "[CONSTRUCTOR ENTRY] LCDDisplay(params)" << Qt::endl;

    // QProgressBar can be set to an arbitrary min/max integer scale
    compressionDepthBar->setMaximum(CPR_ADULT_TARGET_DEPTH_MAX + 1);
    compressionDepthBar->setMinimum(0);

    // initializes running timer for AED
    runningTimer = new QTimer(this);
    connect(runningTimer, &QTimer::timeout, this, &LCDDisplay::updateTimer);

    // Set LCD Display to off when app first starts
    if (lcdDisplayFrame != nullptr) {
        setLCDDisplayVisible(false);
    }

    qDebug() << "[CONSTRUCTOR EXIT] LCDDisplay(params)" << Qt::endl;
}

LCDDisplay::~LCDDisplay()
{
    qDebug() << "[DESTRUCTOR ENTRY] ~LCDDisplay(params)" << Qt::endl;

    if(runningTimer != nullptr)
    {
        qDebug() << "[DESTRUCTOR INFO] ~LCDDisplay(params) : scheduling delete for runningTimer" << Qt::endl;
        runningTimer->blockSignals(true);
        runningTimer->stop();
        runningTimer->deleteLater();
    }

    qDebug() << "[DESTRUCTOR EXIT] ~LCDDisplay(params)" << Qt::endl;
}

void LCDDisplay::setLCDDisplayVisible(bool visible)
{
    qDebug().noquote() << "[ENTRY] LCDDisplay::setLCDDisplayVisible(" << visible << ")" << Qt::endl;

    // Sets visibility of all children of lcdDisplayFrame
    const QObjectList& children = lcdDisplayFrame->children();
    for (QObject* child : children) {
        QWidget* childWidget = qobject_cast<QWidget*>(child);
        if (childWidget != nullptr) {
            childWidget->setVisible(visible);
        }
    }

    // Sets background color of lcdDisplayFrame depending on if visible or not (on or off)
    visible ? lcdDisplayFrame->setStyleSheet(STYLE_LCD_ON) : lcdDisplayFrame->setStyleSheet(STYLE_LCD_OFF);

    qDebug().noquote() << "[EXIT] LCDDisplay::setLCDDisplayVisible(" << visible << ")" << Qt::endl;
}

// SLOT
void LCDDisplay::startLCD()
{
    qDebug() << "[ENTRY SLOT] LCDDisplay::startLCD()" << Qt::endl;

    // If LCD Display is already on, don't turn it on again
    if (active) return;

    // Set LCD Display to be active
    active = true;

    // Sets LCDDisplay to be visible
    if (lcdDisplayFrame != nullptr) {
        setLCDDisplayVisible(true);
    }

    setShockCounter(0);  // Sets shocks to 0
    updateTimer();    // Sets timer to be '00:00'. Elapsed time is 0 from initialization, or endLCD()
    clearGraphData();   // Clears graph data
    // setPromptLabel("");    // Clears prompt message      // Delegate this to AED so that it can initialize prompts before LCD has fully started
    setCPRHelpLabel(P_BLANK);  // Clears help message
    setCompressionDepthBar(0);   // Sets compression depth to be 0

    // Starts the timer
    runningTimer->setSingleShot(false);
    runningTimer->start(1000);

    qDebug() << "[EXIT SLOT] LCDDisplay::startLCD()" << Qt::endl;
}

// SLOT
void LCDDisplay::endLCD()
{
    qDebug().noquote() << "[ENTRY SLOT] LCDDisplay::endLCD() (LCD is currently " << (active ? "on" : "off") << ")" << Qt::endl;

    // If LCD Display is already off, don't turn it off again
    if (!active) return;

    // Sets LCDDisplay to be invisible
    if (lcdDisplayFrame != nullptr) {
        setLCDDisplayVisible(false);
    }

    active = false;     // Set LCD Display to be inactive
    runningTimer->stop();   // stops timer
    elapsedTime = -1;    // clears timer count. Allows startLCD() to set timer to '00:00'

    qDebug() << "[EXIT SLOT] LCDDisplay::endLCD()" << Qt::endl;
}

// SLOT
void LCDDisplay::setGraphData(QVector<double>* xDataToCopy, QVector<double>* yDataToCopy) // setGraphData(aed::ModuleECGAssessment&  m)
{
    qDebug() << "[ENTRY SLOT] LCDDisplay::setGraphData( ... )" << Qt::endl;

    graphXData = *xDataToCopy;
    graphYData = *yDataToCopy;

    qDebug() << "[EXIT SLOT] LCDDisplay::setGraphData( ... )" << Qt::endl;
}

// SLOT
void LCDDisplay::setCompressionDepthBar(int depth)
{
    qDebug().noquote() << "[ENTRY SLOT] LCDDisplay::setCompressionDepthBar(" << QString::number(depth) << ")" << Qt::endl;

    if (depth >= 0 && depth <= CPR_ADULT_TARGET_DEPTH_MAX) {
        // Convert depth in 1/10th inches to percentage for QProgressBar
        int depthPercent = (int) (depth * 100 / CPR_ADULT_TARGET_DEPTH_MAX);

        compressionDepthBar->setValue(depthPercent);
    } else {
        qDebug().noquote() << "[WARNING] LCDDisplay::setCompressionDepthBar(" << QString::number(depth) << ") "
                           << "Compression depth out of range" << Qt::endl;
    }

    qDebug().noquote() << "[ENTRY SLOT] LCDDisplay::setCompressionDepthBar(" << QString::number(depth) << ")" << Qt::endl;
}

void LCDDisplay::plotGraphData()
{
    qDebug() << "[ENTRY] LCDDisplay::plotGraphData()" << Qt::endl;

    if (ecgGraph != nullptr) {
        ecgGraph->clearGraphs();
        ecgGraph->addGraph();
        ecgGraph->graph(0)->setData(graphXData, graphYData);
        ecgGraph->xAxis->setRange(0, 22);
        ecgGraph->yAxis->setRange(0, 5);
        ecgGraph->replot();
    }

    qDebug() << "[EXIT] LCDDisplay::plotGraphData()" << Qt::endl;
}

void LCDDisplay::clearGraphData()
{
    qDebug() << "[ENTRY] LCDDisplay::clearGraphData()" << Qt::endl;

    if (ecgGraph != nullptr) {
        ecgGraph->clearGraphs();
        ecgGraph->replot();
    }

    qDebug() << "[EXIT] LCDDisplay::clearGraphData()" << Qt::endl;
}

QString LCDDisplay::formatTime(int elapsedSeconds)
{
    // No debug stubs in this function as they would just flood the screen

    int minutes = elapsedSeconds / 60;
    int seconds = elapsedSeconds % 60;

    // Format time to be '00:00'
    QString timeString = QString("%1:%2")
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0'));

    return timeString;
}

// SLOT
void LCDDisplay::updateTimer()
{
    // No debug stubs in this function as they would just flood the screen

    elapsedTime++;
    if (timer != nullptr) {
        timer->setText(formatTime(elapsedTime));
    }
}

// SLOT
void LCDDisplay::setShockCounter(int shockCount)
{
    qDebug().noquote() << "[ENTRY SLOT] LCDDisplay::setShockCounter(" << QString::number(shockCount) << ")" << Qt::endl;

    // Format shock counter to be '00'
    QString shocksString = QString("%1").arg(shockCount, 2, 10, QChar('0'));

    if (shockCounter != nullptr) {
        shockCounter->setText(shocksString);
    }

    qDebug().noquote() << "[EXIT SLOT] LCDDisplay::setShockCounter(" << QString::number(shockCount) << ")" << Qt::endl;
}
