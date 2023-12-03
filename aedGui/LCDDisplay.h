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
#define CPR_ADULT_DEPTH_MAX 24
#define LCD_ON_STYLE "background-color: rgb(255, 255, 255);"
#define LCD_OFF_STYLE "background-color: rgb(61, 56, 70);"

namespace aedGui
{
    struct LCDDisplayParams {
        QFrame* lcdDisplayFrame;
        QCustomPlot* ecgGraph;
        QLabel* promptLabel;
        QLabel* helpLabel;
        QLabel* shockCounter;
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
        QLabel* promptLabel;
        QLabel* helpLabel;
        QLabel* shockCounter;
        QLabel* timer;
        QProgressBar* compressionDepthBar;
        QVector<double> graphXData;
        QVector<double> graphYData;
        QTimer* runningTimer;
        int elapsedTime;    // Time in seconds

        QString formatTime(int elapsedSeconds);
        void setLCDDisplayVisible(bool visible);    // Sets LCD Display children visibility

    public slots:
        void startLCD();    // Turns on display
        void endLCD();      // Turns off display
        void setGraphData(QVector<double>* xDataToCopy, QVector<double>* yDataToCopy);
        void setCompressionDepthBar(int depth);     // Sets compression depth bar. Takes depth in 1/10th inches
        void setShockCounter(int shockCounter);
        void setPromptLabel(QString msg) {promptLabel->setText(msg);}
        void setHelpLabel(QString msg) {helpLabel->setText(msg);}

    private slots:
        void updateTimer();
    };
}

#endif // LCDDISPLAY_H
