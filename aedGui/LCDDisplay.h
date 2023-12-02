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
#include  "qcustomplot.h"

namespace aedGui
{
    struct LCDDisplayParams {
        QCustomPlot* ecgGraph;
        QLabel* prompt;
        QLabel* help;
        QLabel* shocks;
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
        void setPrompt(QString p) {prompt->setText(p);}
        void setHelp(QString h) {help->setText(h);}
        QVector<double> getGraphXData() {return graphXData;}
        QVector<double> getGraphYData() {return graphYData;}

    private:
        QCustomPlot* ecgGraph;
        QLabel* prompt;
        QLabel* help;
        QLabel* shocks;
        QLabel* timer;
        QProgressBar* compressionDepthBar;
        QVector<double> graphXData;
        QVector<double> graphYData;
        QTimer* runningTimer;
        QTimer* flashTimer;
        QTimer* revertTimer;
        int elapsedTime;    // time in seconds
        int numShocks;

        QString formatTime(int elapsedSeconds);

    signals:
        void plotGraphSignal();
        void clearGraphSignal();

    public slots:
        void setGraphData(QVector<double>* xDataToCopy, QVector<double>* yDataToCopy);
        void setCompressionDepth(int depth);
        void addShock();
    
    private slots:
        void updateTimer();
        void flashProgressBar();
        void revertProgressBar();

    };
}

#endif // LCDDISPLAY_H
