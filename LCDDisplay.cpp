#include "LCDDisplay.h"

LCDDisplay::LCDDisplay(QCustomPlot* g)
//    : graph(graph) // , graphXData(nullptr), graphYData(nullptr)
{
    graph = g;
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
    graphXData = *xDataToCopy;
    graphYData = *yDataToCopy;
}

void LCDDisplay::plotGraphData()
{
    if (graph != nullptr) {
//        graph->clearGraphs();

        QVector<double> x(5), y(5);
        for (int i = 0; i < 5; i++) {
            x[i] = i;
            y[i] = i;
        }

        graph->addGraph();
        graph->graph(0)->setData(x, y);
        graph->xAxis->setRange(0, 22);
        graph->yAxis->setRange(0, 5);


//        graph->addGraph();
//        graph->graph(0)->setData(graphXData, graphYData);
//        graph->xAxis->setRange(0, 22);
//        graph->yAxis->setRange(0, 5);
//        qDebug() << "GRAPH DATA PLOTTED";
//        qDebug() << QString("%1").arg(graphXData.at(0));
//        qDebug() << QString("%1").arg(graphYData.at(0));
//        qDebug() << QString("%1").arg(graphXData.at(1));
//        qDebug() << QString("%1").arg(graphYData.at(1));
    }
}

void LCDDisplay::clearGraphData()
{
    if (graph != nullptr) {
        graph->clearGraphs();
    }
}
