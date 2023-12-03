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
        void setPrompt(QString msg) {prompt->setText(msg);}
        void setHelp(QString msg) {help->setText(msg);}
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

    };
}

	