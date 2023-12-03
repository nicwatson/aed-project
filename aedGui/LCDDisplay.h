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
#include <QFrame>
#include  "qcustomplot.h"

// Recommended maximum depth of compressions for adult patient (1/10th inches)
// Maybe we should have a constants file?
#define CPR_ADULT_DEPTH_MAX 24

namespace aedGui
{
    struct LCDDisplayParams {
        QFrame* lcdDisplayFrame;
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
        QVector<double> getGraphXData() {return graphXData;}
        QVector<double> getGraphYData() {return graphYData;}

    private:
        bool active;    // Is the LCD Display already on?
        QFrame* lcdDisplayFrame;
        QCustomPlot* ecgGraph;
        QLabel* prompt;
        QLabel* help;
        QLabel* shocks;
        QLabel* timer;
        QProgressBar* compressionDepthBar;
        QVector<double> graphXData;
        QVector<double> graphYData;
        QTimer* runningTimer;
        int elapsedTime;    // Time in seconds

        QString formatTime(int elapsedSeconds);
        void setLCDDisplayVisible(bool visible);    // Sets LCD Display children visibility

    signals:
        void plotGraphSignal();
        void clearGraphSignal();

    public slots:
        void startLCD();    // Turns on display
        void endLCD();      // Turns off display
        void setGraphData(QVector<double>* xDataToCopy, QVector<double>* yDataToCopy);
        void setCompressionDepth(int depth);
        void setShock(int shockCount);
        void setPrompt(QString msg) {prompt->setText(msg);}
        void setHelp(QString msg) {help->setText(msg);}

    private slots:
        void updateTimer();
    };
}

#endif // LCDDISPLAY_H
