#include "LCDDisplay.h"

using namespace aedGui;

LCDDisplay::LCDDisplay(QCustomPlot* g, QLabel* p): graph(g), prompt(p)
{

}

LCDDisplay::~LCDDisplay()
{
}

void LCDDisplay::setGraphData(QVector<double>* xDataToCopy, QVector<double>* yDataToCopy) // setGraphData(aed::ModuleECGAssessment&  m)
{
    graphXData = *xDataToCopy;
    graphYData = *yDataToCopy;
}

void LCDDisplay::plotGraphData()
{
    if (graph != nullptr) {
        graph->clearGraphs();
        graph->addGraph();
        graph->graph(0)->setData(graphXData, graphYData);
        graph->xAxis->setRange(0, 22);
        graph->yAxis->setRange(0, 5);
        graph->replot();
    }
}

void LCDDisplay::clearGraphData()
{
    if (graph != nullptr) {
        graph->clearGraphs();
        graph->replot();
    }
}
