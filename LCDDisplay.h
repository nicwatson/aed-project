

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
        explicit LCDDisplay(QCustomPlot* graph);
        ~LCDDisplay();

    private:
        QCustomPlot* graph;
        QVector<double> graphXData;
        QVector<double> graphYData;

    public slots:
        void setGraphData(QVector<double>* xDataToCopy, QVector<double>* yDataToCopy); // setGraphData(aed::ModuleECGAssessment&  m) // setGraphData(QVector<double>* xDataToCopy, QVector<double>* yDataToCopy)
        void plotGraphData();
        void clearGraphData();
};

#endif // LCDDISPLAY_H
