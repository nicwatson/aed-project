#include "ModuleECGAssessment.h"

using namespace aed;

ModuleECGAssessment::ModuleECGAssessment(LCDDisplay* l) : active(false), rhythm(VENT_FIB), lcdDisplay(l)
{
    timer = new QTimer(this);

    // Get the path of the directory containing the ECG data (in csv format)
    QDir parentDir = QDir::current();
    parentDir.cdUp();
    QString csvDirPath = parentDir.path() + "/comp3004-team14";

    // Load the v_fib file into memory
    QFile CSVFile(csvDirPath + "/v_fib.csv");
    if (CSVFile.open(QIODevice::ReadWrite))
    {
        QTextStream Stream(&CSVFile);
        while(Stream.atEnd() == false)
        {
            // Read the current line (i.e. row) in the CSV
            QString lineData = Stream.readLine();
            // Split the data in the row
            QStringList data = lineData.split(",");
            for (int i = 0; i < data.length(); i++)
            {
                // The datum in the 1st column of the current row is the x-coordinate
                ventFibXData.push_back(data[0].toDouble());
                // The datum in the 2nd column of the current row is the y-coordinate
                ventFibYData.push_back(data[1].toDouble());
            }
        }
    }
    CSVFile.close();

    // Load the v_fib file into memory
    QFile CSVFile2(csvDirPath + "/v_fib.csv");
    if (CSVFile2.open(QIODevice::ReadWrite))
    {
        QTextStream Stream(&CSVFile2);
        while(Stream.atEnd() == false)
        {
            // Read the current line (i.e. row) in the CSV
            QString lineData = Stream.readLine();
            // Split the data in the row
            QStringList data = lineData.split(",");
            for (int i = 0; i < data.length(); i++)
            {
                // The datum in the 1st column of the current row is the x-coordinate
                ventFibXData.push_back(data[0].toDouble());
                // The datum in the 2nd column of the current row is the y-coordinate
                ventFibYData.push_back(data[1].toDouble());
            }
        }
    }
    CSVFile2.close();

    // Load the v_fib file into memory
    QFile CSVFile3(csvDirPath + "/v_fib.csv");
    if (CSVFile3.open(QIODevice::ReadWrite))
    {
        QTextStream Stream(&CSVFile3);
        while(Stream.atEnd() == false)
        {
            // Read the current line (i.e. row) in the CSV
            QString lineData = Stream.readLine();
            // Split the data in the row
            QStringList data = lineData.split(",");
            for (int i = 0; i < data.length(); i++)
            {
                // The datum in the 1st column of the current row is the x-coordinate
                ventFibXData.push_back(data[0].toDouble());
                // The datum in the 2nd column of the current row is the y-coordinate
                ventFibYData.push_back(data[1].toDouble());
            }
        }
    }
    CSVFile3.close();
}

ModuleECGAssessment::~ModuleECGAssessment()
{
    if (timer != nullptr) { delete timer; }
}


void ModuleECGAssessment::startAssessment()
{
    qDebug()  << "TESTING start assessment";
    if (active == true) return; // If an assessment is in progress, can't start a new one
    active = true;
    // Start the 5-second timer to simulate analysis. Once timer runs out, the appropriate function is called, depending on which rhythm is shockable.
    timer->setInterval(5000);
    timer->setSingleShot(true);
    if (rhythm == VENT_FIB)
    {
        // Cause a vent fib graph to be plotted to the gui
        lcdDisplay->setGraphData(&ventFibXData, &ventFibYData);
        lcdDisplay->plotGraphData();
        connect(timer, &QTimer::timeout, this, &ModuleECGAssessment::sendShockableSignal);
    } else if (rhythm == VENT_TACHY)
    {
        // Cause a vent tachy graph to be plotted to the gui
        lcdDisplay->setGraphData(&ventTachyXData, &ventTachyYData);
        lcdDisplay->plotGraphData();
         connect(timer, &QTimer::timeout, this, &ModuleECGAssessment::sendShockableSignal);
    } else
    {
        // Cause a nonshock graph to be plotted to the gui
        lcdDisplay->setGraphData(&nonShockableXData, &nonShockableYData);
        lcdDisplay->plotGraphData();
        connect(timer, &QTimer::timeout, this, &ModuleECGAssessment::sendNonShockableSignal);
    }
    timer->start();
}

void ModuleECGAssessment::endAssessment()
{
    qDebug() << "TESTING end assessment";
    active = false;
    if (rhythm == VENT_FIB || rhythm == VENT_TACHY)
    {
        disconnect(timer, &QTimer::timeout, this, &ModuleECGAssessment::sendShockableSignal);
    } else
    {
        disconnect(timer, &QTimer::timeout, this, &ModuleECGAssessment::sendNonShockableSignal);
    }
    // Cause the current graph to be removed from the gui
    lcdDisplay->clearGraphData();
}

//void ModuleECGAssessment::LCDDownload(QVector<double>& xData, QVector<double>& yData)
//{
//    // Clear the data points stored on the LCDDisplay
//    xData.clear();
//    yData.clear();
//    // Add the appropriate data points to the LCDDisplay
//    xData.append();
//}

void ModuleECGAssessment::sendShockableSignal()
{
    emit signalShockable();
}

void ModuleECGAssessment::sendNonShockableSignal()
{
    emit signalNotShockable();
}
