#include "LCDDisplay.h"

using namespace aedGui;

LCDDisplay::LCDDisplay(const LCDDisplayParams& params)
    : ecgGraph(params.ecgGraph), 
        prompt(params.prompt), 
        help(params.help), 
        shocks(params.shocks), 
        timer(params.timer), 
        compressionDepthBar(params.compressionDepthBar),
        elapsedTime(0),
        numShocks(0)
{
    // initializes and starts running timer for AED
    runningTimer = new QTimer(this);
    connect(runningTimer, SIGNAL(timeout()), this, SLOT(updateTimer()));
    runningTimer->start(1000);
}

LCDDisplay::~LCDDisplay() {}

void LCDDisplay::setGraphData(QVector<double>* xDataToCopy, QVector<double>* yDataToCopy) // setGraphData(aed::ModuleECGAssessment&  m)
{
    graphXData = *xDataToCopy;
    graphYData = *yDataToCopy;
}

void LCDDisplay::setCompressionDepth(int depth)
{
    if (depth >= 0 && depth <= 24) {
        // Convert depth out of 24 (2.4 inches) to percentage for QProgressBar
        int depthPercent = (int) (depth * 100 / 24);

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

void LCDDisplay::addShock()
{
    numShocks++;
    // Format shocks to be '00'
    QString shocksString = QString("%1").arg(numShocks, 2, 10, QChar('0'));

    if (shocks != nullptr) {
        shocks->setText(shocksString);
    }
}
