#include "ModuleECGAssessment.h"
#include "aedGui/strings.h"
#include "aedconst.h"
#include <QDebug>

using namespace aedModel;

// Used for debugging - translate between rhythm_t underlying integers and readable strings
const QString ModuleECGAssessment::rhythmNames[3] = { "VENT_FIB", "VENT_TACHY", "NON_SHOCKABLE" };

ModuleECGAssessment::ModuleECGAssessment(aedGui::LCDDisplay* l) : active(false), rhythm(VENT_FIB), lcdDisplay(l)
{
    qDebug() << "[CONSTRUCTOR ENTRY] ModuleECGAssessment(LCDDisplay *)" << Qt::endl;

    // Iterate over all the csv files and load them into class attributes (to save time, no need to read from file each time the startaAsessment slot is called)
    QStringList csvList = {":/ecgs/v_fib.csv", ":/ecgs/v_tachy.csv", ":/ecgs/non_shockable.csv"};
    for (int i = 0; i < csvList.length(); i++)
    {
        readCSVFile(csvList[i]);
    }

    qDebug() << "[CONSTRUCTOR EXIT] ModuleECGAssessment(LCDDisplay *)" << Qt::endl;
}

ModuleECGAssessment::~ModuleECGAssessment()
{
    // No dynamic allocations to delete
    qDebug() << "[DESTRUCTOR] ~ModuleECGAssessment()" << Qt::endl;
}


// When a signal is received from AED to this slot, the assessment will begin.
// SLOT
void ModuleECGAssessment::startAssessment()
{
    qDebug().noquote() << "[ENTRY SLOT] ModuleECGAssessment::startAssessment(): "
                       << "Assessment was previously " << (active ? "active (exiting function)" : "inactive") << ": "
                       << "Rhythm is: " << rhythmNames[rhythm] << Qt::endl;

    // If an assessment is in progress, can't start a new one
    if (active == true) return;

    // Set to true as assessment is in progress
    active = true;

    qDebug() << "[SIGNAL] Emit signalForwardUserPrompt(" << P_ANALYZING << ")" << Qt::endl;
    emit signalForwardUserPrompt(P_ANALYZING);

    // Start the 5-second timer to simulate analysis. Once timer runs out, the appropriate function is called, depending on which rhythm is shockable.
    timer.setInterval(ECG_TIME);
    timer.setSingleShot(true);
    if (rhythm == VENT_FIB)
    {
        // This causes a vent fib graph to be plotted to the gui
        lcdDisplay->setGraphData(&ventFibXData, &ventFibYData);
        lcdDisplay->plotGraphData();

        // When the timer runs out, send a shockable signal (this'll be received by the AED)
        connect(&timer, &QTimer::timeout, this, &ModuleECGAssessment::sendShockableSignal);
    } else if (rhythm == VENT_TACHY)
    {
        // This causes a vent tachy graph to be plotted to the gui
        lcdDisplay->setGraphData(&ventTachyXData, &ventTachyYData);
        lcdDisplay->plotGraphData();

        // When the timer runs out, send a shockable signal (this'll be received by the AED)
         connect(&timer, &QTimer::timeout, this, &ModuleECGAssessment::sendShockableSignal);
    } else
    {
        // This causes a nonshockable graph to be plotted to the gui
        lcdDisplay->setGraphData(&nonShockableXData, &nonShockableYData);
        lcdDisplay->plotGraphData();

        // When the timer runs out, send anon shockable signal (this'll be received by the AED)
        connect(&timer, &QTimer::timeout, this, &ModuleECGAssessment::sendNonShockableSignal);
    }
    timer.start();

    qDebug() << "[EXIT SLOT] ModuleECGAssessment::startAssessment()" << Qt::endl;
}

// When a signal is received to this slot, the assessment should end immediately (this'll happen in case of error)
// SLOT
void ModuleECGAssessment::endAssessment()
{
    qDebug().noquote() << "[ENTRY SLOT] ModuleECGAssessment::endAssessment(): "
                       << "Assessment was previously " << (active ? "active (exiting function)" : "inactive") << ": "
                       << "Rhythm is: " << rhythmNames[rhythm] << Qt::endl;

    // Stops the analysis (the 5-second timer) prematurely. If-else statement checks wwhich timer to stop.
    if (rhythm == VENT_FIB || rhythm == VENT_TACHY)
    {
        disconnect(&timer, &QTimer::timeout, this, &ModuleECGAssessment::sendShockableSignal);
    } else
    {
        disconnect(&timer, &QTimer::timeout, this, &ModuleECGAssessment::sendNonShockableSignal);
    }

    // Cause the current graph to be removed from the gui
    lcdDisplay->clearGraphData();

    //qDebug() << "[SIGNAL] Emit signalForwardUserPrompt(P_BLANK)" << Qt::endl;
    //emit signalForwardUserPrompt(P_BLANK);

    // An asssessment is no longer in process
    active = false;

    qDebug() << "[EXIT SLOT] ModuleECGAssessment::endAssessment()" << Qt::endl;
}

// Iterate over all the csv files and load them into class attributes (to save time, no need to read from file each time the startaAsessment slot is called)
void ModuleECGAssessment::readCSVFile(const QString & fileName)
{
    qDebug() << "[ENTRY] ModuleECGAssessment::readCSVFile(" << fileName << ")" << Qt::endl;

    QFile CSVFile(fileName);
    if (CSVFile.open(QIODevice::ReadOnly))
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
                if (fileName == ":/ecgs/v_fib.csv")
                {
                    ventFibXData.push_back(data[0].toDouble()); // The datum in the 1st column of the current row is the x-coordinate
                    ventFibYData.push_back(data[1].toDouble()); // The datum in the 2nd column of the current row is the y-coordinate
                } else if (fileName == ":/ecgs/v_tachy.csv")
                {
                    ventTachyXData.push_back(data[0].toDouble()); // The datum in the 1st column of the current row is the x-coordinate
                    ventTachyYData.push_back(data[1].toDouble()); // The datum in the 2nd column of the current row is the y-coordinate
                } else if (fileName == ":/ecgs/non_shockable.csv")
                {
                    nonShockableXData.push_back(data[0].toDouble()); // The datum in the 1st column of the current row is the x-coordinate
                    nonShockableYData.push_back(data[1].toDouble()); // The datum in the 2nd column of the current row is the y-coordinate
                }
            }
        }
    }
    CSVFile.close();

    qDebug() << "[EXIT] ModuleECGAssessment::readCSVFile(" << fileName << ")" << Qt::endl;
}

// When the timer has run out, this function is called. The assessment is ended and a shockable signnal emitted
// SLOT
void ModuleECGAssessment::sendShockableSignal()
{
    qDebug() << "[ENTRY SLOT] ModuleECGAssessment::sendShockableSignal()" << Qt::endl;

    endAssessment();

    qDebug() << "[SIGNAL] Emit ModuleECGAssessment::signalForwardUserPrompt(" << P_SHOCKABLE << ")" << Qt::endl;
    emit signalForwardUserPrompt(P_SHOCKABLE);
    qDebug() << "[SIGNAL] Emit ModuleECGAssessment::signalShockable()" << Qt::endl;
    emit signalShockable();
    qDebug() << "[SIGNAL] Emit ModuleECGAssessment::signalResult(true)" << Qt::endl;
    emit signalResult(true);

    qDebug() << "[EXIT SLOT] ModuleECGAssessment::sendShockableSignal()" << Qt::endl;

}

// When the timer has run out, this function is called. The assessment is ended and a non-shockable signnal emitted
// SLOT
void ModuleECGAssessment::sendNonShockableSignal()
{
    qDebug() << "[ENTRY SLOT] ModuleECGAssessment::sendNonShockableSignal()" << Qt::endl;

    endAssessment();

    qDebug() << "[SIGNAL] Emit ModuleECGAssessment::signalForwardUserPrompt(" << P_NOTSHOCKABLE << ")" << Qt::endl;
    emit signalForwardUserPrompt(P_NOTSHOCKABLE);
    qDebug() << "[SIGNAL] Emit ModuleECGAssessment::signalNotShockable()" << Qt::endl;
    emit signalNotShockable();
    qDebug() << "[SIGNAL] Emit ModuleECGAssessment::signalResult(false)" << Qt::endl;
    emit signalResult(false);

    qDebug() << "[EXIT SLOT] ModuleECGAssessment::sendNonShockableSignal()" << Qt::endl;
}
