

#ifndef LCDDISPLAY_H
#define LCDDISPLAY_H
#include <QObject>
#include <QLabel>
#include <QDebug>
#include <QVector>
#include "qcustomplot.h"

class LCDDisplay : public QObject
{
    Q_OBJECT

    public:
        explicit LCDDisplay(QCustomPlot* graph, QLabel* t);
        ~LCDDisplay();
        void plotGraphData();
        void clearGraphData();
        QVector<double> getGraphXData() {return graphXData;}
        QVector<double> getGraphYData() {return graphYData;}

    private:
        QLabel* testLabel;
        QCustomPlot* graph;
        QVector<double> graphXData;
        QVector<double> graphYData;

    signals:
        void plotGraphSignal();
        void clearGraphSignal();

    public slots:
        void setGraphData(QVector<double>* xDataToCopy, QVector<double>* yDataToCopy); // setGraphData(aed::ModuleECGAssessment&  m) // setGraphData(QVector<double>* xDataToCopy, QVector<double>* yDataToCopy)
};

#endif // LCDDISPLAY_H
