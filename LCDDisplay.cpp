#include "LCDDisplay.h"

LCDDisplay::LCDDisplay(QCustomPlot* g, QLabel* t): graph(g),  testLabel(t)
//    : graph(graph) // , graphXData(nullptr), graphYData(nullptr)
{

}

LCDDisplay::~LCDDisplay()
{
}

void LCDDisplay::setGraphData(QVector<double>* xDataToCopy, QVector<double>* yDataToCopy) // setGraphData(aed::ModuleECGAssessment&  m)
{
    //  LCDDisplay is like driver
    // ModuleECGAssessment is like distributor
    // ModuleECGAssessment::ventFibXData is  not public, so can't say setGraphData(&ventFibXData, &ventFibYData)  is
//    m.LCDDownload(graphXData, graphYData);
    qDebug() << "GRAPH DATA SET";
    QVector<double> xdata(*xDataToCopy);
    QVector<double> ydata(*yDataToCopy);
    graphXData = xdata; //*xDataToCopy;
    graphYData = ydata; //*yDataToCopy;

    qDebug() << QString("%1").arg(graphXData.at(0));
    qDebug() << QString("%1").arg(graphYData.at(0));
    qDebug() << QString("%1").arg(graphXData.at(1));
    qDebug() << QString("%1").arg(graphYData.at(1));
}

void LCDDisplay::plotGraphData()
{
    emit plotGraphSignal();
//    if (graph != nullptr) {
//        graph->clearGraphs();
//        testLabel->setText("teehee");


//        graph->addGraph();
//        graph->graph(0)->setData(graphXData, graphYData);
//        graph->xAxis->setRange(0, 22);
//        graph->yAxis->setRange(0, 5);
//        qDebug() << "GRAPH DATA PLOTTED";
//        qDebug() << QString("%1").arg(graphXData.at(0));
//        qDebug() << QString("%1").arg(graphYData.at(0));
//        qDebug() << QString("%1").arg(graphXData.at(1));
//        qDebug() << QString("%1").arg(graphYData.at(1));
//    }
}

void LCDDisplay::clearGraphData()
{
    emit clearGraphSignal();
//    if (graph != nullptr) {
//        graph->clearGraphs();
//    }
}
