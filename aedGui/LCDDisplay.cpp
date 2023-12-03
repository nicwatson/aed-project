#include "LCDDisplay.h"

using namespace aedGui;

LCDDisplay::LCDDisplay(const LCDDisplayParams& params)
    : lcdDisplayFrame(params.lcdDisplayFrame),
        ecgGraph(params.ecgGraph), 
        prompt(params.prompt), 
        help(params.help), 
        shocks(params.shocks), 
        timer(params.timer), 
        compressionDepthBar(params.compressionDepthBar),
        elapsedTime(0)
{
    // initializes running timer for AED
    runningTimer = new QTimer(this);
    connect(runningTimer, SIGNAL(timeout()), this, SLOT(updateTimer()));
}

LCDDisplay::~LCDDisplay() {}

void LCDDisplay::setLCDDisplayVisible(bool visible)
{
    const QObjectList& children = lcdDisplayFrame->children();
    for (QObject* child : children) {
        QWidget* childWidget = qobject_cast<QWidget*>(child);
        if (childWidget != nullptr) {
            childWidget->setVisible(visible);
        }
    }
}

void LCDDisplay::startLCD()
{
    // If LCD Display is already on, don't turn it on again
    if (active) return;

    // Set LCD Display to be active
    active = true;

    // Sets LCDDisplay to be visible
    if (lcdDisplayFrame != nullptr) {
        setLCDDisplayVisible(true);
    }

    setShock(0);  // Sets shocks to 0
    updateTimer();    // Sets timer to be '00:00'. Elapsed time is 0 from initialization, or endLCD()
    clearGraphData();   // Clears graph data
    setPrompt("");    // Clears prompt message
    setHelp("");  // Clears help message
    setCompressionDepth(0);   // Sets compression depth to be 0

    // Starts the timer
    runningTimer->start(1000);
}

void LCDDisplay::endLCD()
{
    // If LCD Display is already off, don't turn it off again
    if (!active) return;

    // Sets LCDDisplay to be invisible
    if (lcdDisplayFrame != nullptr) {
        setLCDDisplayVisible(false);
    }

    active = false;     // Set LCD Display to be inactive
    runningTimer->stop();   // stops timer
    elapsedTime = 0;    // clears timer count
}

void LCDDisplay::setGraphData(QVector<double>* xDataToCopy, QVector<double>* yDataToCopy) // setGraphData(aed::ModuleECGAssessment&  m)
{
    graphXData = *xDataToCopy;
    graphYData = *yDataToCopy;
}

void LCDDisplay::setCompressionDepth(int depth)
{
    if (depth >= 0 && depth <= CPR_ADULT_DEPTH_MAX) {
        // Convert depth in 1/10th inch to percentage for QProgressBar
        int depthPercent = (int) (depth * 100 / CPR_ADULT_DEPTH_MAX);

        compressionDepthBar->setValue(depthPercent);
    } else {
        qDebug() << "Compression depth out of range";
    }
}

void LCDDisplay::plotGraphData()
{
    if (ecgGraph != nullptr) {
        ecgGraph->clearGraphs();
        ecgGraph->addGraph();
        ecgGraph->graph(0)->setData(graphXData, graphYData);
        ecgGraph->xAxis->setRange(0, 22);
        ecgGraph->yAxis->setRange(0, 5);
        ecgGraph->replot();
    }
}

void LCDDisplay::clearGraphData()
{
    if (ecgGraph != nullptr) {
        ecgGraph->clearGraphs();
        ecgGraph->replot();
    }
}

QString LCDDisplay::formatTime(int elapsedSeconds)
{
    int minutes = elapsedSeconds / 60;
    int seconds = elapsedSeconds % 60;

    // Format time to be '00:00'
    QString timeString = QString("%1:%2")
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0'));

    return timeString;
}

void LCDDisplay::updateTimer()
{
    elapsedTime++;
    if (timer != nullptr) {
        timer->setText(formatTime(elapsedTime));
    }
}

void LCDDisplay::setShock(int shockCount)
{
    // Format shocks to be '00'
    QString shocksString = QString("%1").arg(shockCount, 2, 10, QChar('0'));

    if (shocks != nullptr) {
        shocks->setText(shocksString);
    }
}
