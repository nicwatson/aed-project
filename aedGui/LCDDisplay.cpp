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

    flashTimer = new QTimer(this);
    connect(flashTimer, &QTimer::timeout, this, &LCDDisplay::flashProgressBar);
    flashTimer->start(1000); // Adjust for how often it flashes

    revertTimer = new QTimer(this);
    connect(revertTimer, &QTimer::timeout, this, &LCDDisplay::revertProgressBar);
    revertTimer->setSingleShot(true); // Only trigger once per timeout
}

LCDDisplay::~LCDDisplay() {}

void LCDDisplay::setGraphData(QVector<double>* xDataToCopy, QVector<double>* yDataToCopy) // setGraphData(aed::ModuleECGAssessment&  m)
{
    graphXData = *xDataToCopy;
    graphYData = *yDataToCopy;
}

void LCDDisplay::setCompressionDepth(int depth)
{
    if (depth >= 0 && depth <= 100) {
        compressionDepthBar->setValue(depth);
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

void LCDDisplay::flashProgressBar()
{
    compressionDepthBar->setStyleSheet("selection-background-color: rgba(0, 0, 0, 0);");
    revertTimer->start(200); // Adjust for how long it stays white
}

void LCDDisplay::revertProgressBar()
{
    compressionDepthBar->setStyleSheet("selection-background-color: rgb(61, 56, 70);");
}
