#include "CPRHelpAdult.h"
#include "aedGui/strings.h"

using namespace aedModel;

CPRHelpAdult::CPRHelpAdult()
    : CPRHelpStrategy(CPR_TIME_ADULT), compressionsActive(false), compressionDepth(0), compressionRate(1), pushHarder(false), continueCPR(false)
{
    noCPRTimer.setSingleShot(true);
    noCPRTimer.setInterval(CPR_DELAY_TOLERANCE);
    cprCyclicTimer.setSingleShot(false);
    cprCyclicTimer.blockSignals(true);
    connect(&cprCyclicTimer, SIGNAL(timeout()), this, SLOT(doCompression()));
    connect(&noCPRTimer, &QTimer::timeout, this, &aedModel::CPRHelpAdult::noCPRDetected);
}

void CPRHelpAdult::cleanup()
{
    if(compressionsActive) stopCompressions();
    qDebug() << "CPRHelpAdult::cleanup()" << Qt::endl;
    noCPRTimer.blockSignals(true);
    cprCyclicTimer.blockSignals(true);
    noCPRTimer.stop();
    cprCyclicTimer.stop();
    noCPRTimer.blockSignals(false);
    cprCyclicTimer.blockSignals(false);
    compressionsActive = false;
    compressionDepth = CPR_DEPTH_SLIDER_MIN;
    compressionRate = CPR_RATE_SLIDER_MIN;
    pushHarder = false;
    CPRHelpStrategy::cleanup();
}

// SLOT
void CPRHelpAdult::updateCompressionDepth(int depth)
{
    qDebug().noquote() << "CPRHelpAdult::updateCompressionDepth(" << QString::number(depth) << ")" << Qt::endl;
    compressionDepth = depth;
}

// SLOT
void CPRHelpAdult::updateCompressionRate(int cpm)
{
    qDebug().noquote() << "CPRHelpAdult::updateCompressionRate(" << QString::number(cpm) << ")" << Qt::endl;
    compressionRate = std::max(40, cpm);
}

// SLOT
void CPRHelpAdult::startCompressions()
{
    qDebug() << "CPRHelpAdult::startCompressions()" << Qt::endl;
    if(!eventActive || compressionsActive) return;
    compressionsActive = true;
    if(compressionRate < CPR_RATE_SLIDER_MIN) compressionRate = CPR_RATE_SLIDER_MIN;
    if(compressionRate > 100) compressionRate = 100;
    cprCyclicTimer.blockSignals(false);
    cprCyclicTimer.setInterval(60000 / compressionRate);
    cprCyclicTimer.start();
    emit signalCompressionsStarted();
}

// SLOT
void CPRHelpAdult::stopCompressions()
{
    qDebug() << "CPRHelpAdult::stopCompressions()" << Qt::endl;
    if(!eventActive || !compressionsActive) return;
    compressionsActive = false;
    cprCyclicTimer.blockSignals(true);
    cprCyclicTimer.stop();
    emit signalCompressionsStopped();
}


// SLOT
void CPRHelpAdult::start()
{
    qDebug() << "Now starting CPRHelpAdult strategy" << Qt::endl;
    if(eventActive) return;
    CPRHelpStrategy::start();
    noCPRTimer.start();
}

// SLOT
void CPRHelpAdult::doCompression()
{

    //emit signalDisplayCompressionDepth(compressionDepth);    // TESTING ONLY update QProgressBar in LCD (passing through the ModuleCPRHelp)
    if(!eventActive || !compressionsActive) return;

    qDebug().noquote()  << "CPRHelpAdult::doCompression()"                  << Qt::endl
                        << "Depth: " << QString::number(compressionDepth)   << Qt::endl
                        << "Rate: " << QString::number(compressionRate)     << Qt::endl;

    // Reset the noCPRTimer
    noCPRTimer.blockSignals(true);
    noCPRTimer.stop();
    noCPRTimer.blockSignals(false);
    if(continueCPR)
    {
        signalUserPrompt(P_BLANK);
        continueCPR = false;
    }

    // The compressionRate is set by the GUI slider
    // So even though we're issuing periodic doCompression() calls, we're
    // not actually sampling them in order to process a CPM for giving advice.
    // We just use the CPM we already know about.
    if(compressionRate > CPR_SLOW_THRESHHOLD)
    {
        emit signalCPRCompressionRatePrompt(P_GOOD_SPEED);
    }
    else if(compressionRate > CPR_VERYSLOW_THRESHHOLD)
    {
        emit signalCPRCompressionRatePrompt(P_SLIGHTLY_FASTER);
    }
    else
    {
        emit signalCPRCompressionRatePrompt(P_FASTER);
    }

    // Compressions too shallow - issue prompt ("PUSH HARDER")
    if(compressionDepth < 20)
    {
        pushHarder = true;
        signalUserPrompt(P_CPR_HARDER);
    }
    else if(pushHarder)
    {
        // For the "GOOD COMPRESSIONS" prompt, if the "PUSH HARDER" prompt was previously issued
        pushHarder = false;
        signalUserPrompt(P_CPR_GOOD);
    }

    emit signalDisplayCompressionDepth(compressionDepth);    // Update QProgressBar in LCD (passing through the ModuleCPRHelp)

    noCPRTimer.start();
}

// SLOT
void CPRHelpAdult::noCPRDetected()
{
    qDebug() << "noCPRDetected()" << Qt::endl;
    if(!eventActive) return;
    emit signalUserPrompt(P_CPR_CONT);
    emit signalCPRCompressionRatePrompt(P_BLANK);
    continueCPR = true;
}



