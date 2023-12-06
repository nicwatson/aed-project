// FILE CPRHelpAdult.cpp
// Implemenetations for CPRHelpAdult class (a concrete CPR help strategy)

#include "CPRHelpAdult.h"
#include "aedGui/strings.h"

using namespace aedModel;

CPRHelpAdult::CPRHelpAdult()
    : CPRHelpStrategy(CPR_TIME_ADULT), compressionsActive(false), compressionDepth(0), compressionRate(1), pushHarder(false), continueCPR(false)
{
    qDebug() << "[CONSTRUCTOR] CPRHelpAdult()" << Qt::endl;

    noCPRTimer.setSingleShot(true);
    noCPRTimer.setInterval(CPR_DELAY_TOLERANCE);
    cprCyclicTimer.setSingleShot(false);
    cprCyclicTimer.blockSignals(true);
    connect(&cprCyclicTimer, &QTimer::timeout, this, &CPRHelpAdult::doCompression);
    connect(&noCPRTimer, &QTimer::timeout, this, &CPRHelpAdult::noCPRDetected);
}

CPRHelpAdult::~CPRHelpAdult()
{
    qDebug() << "[DESTRUCTOR] ~CPRHelpAdult()" << Qt::endl;
    // No dynamic allocations to delete
}

void CPRHelpAdult::cleanup()
{
    qDebug() << "[ENTRY] CPRHelpAdult::cleanup()" << Qt::endl;

    if(compressionsActive) stopCompressions();


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
    continueCPR = false;

    CPRHelpStrategy::cleanup();

    qDebug() << "[EXIT] CPRHelpAdult::cleanup()" << Qt::endl;
}

// SLOT
void CPRHelpAdult::updateCompressionDepth(int depth)
{
    qDebug().noquote() << "[ENTRY/EXIT] CPRHelpAdult::updateCompressionDepth(" << QString::number(depth) << ")" << Qt::endl;
    compressionDepth = depth;
}

// SLOT
void CPRHelpAdult::updateCompressionRate(int cpm)
{
    qDebug().noquote() << "[ENTRY/EXIT] CPRHelpAdult::updateCompressionRate(" << QString::number(cpm) << ")" << Qt::endl;
    compressionRate = std::max(40, cpm);
}

// SLOT
void CPRHelpAdult::startCompressions()
{
    qDebug() << "[ENTRY SLOT] CPRHelpAdult::startCompressions()" << Qt::endl;
    if(!eventActive || compressionsActive)
    {
        qDebug() << "[EXIT SLOT - WARNING] CPRHelpAdult::startCompressions() called -> eventActive: "
                 << eventActive << "    compressionsActive: " << compressionsActive
                 << Qt::endl;
        return;
    }

    compressionsActive = true;
    if(compressionRate < CPR_RATE_SLIDER_MIN) compressionRate = CPR_RATE_SLIDER_MIN;
    if(compressionRate > 100) compressionRate = 100;
    cprCyclicTimer.blockSignals(false);
    cprCyclicTimer.setInterval(60000 / compressionRate);
    cprCyclicTimer.start();

    qDebug() << "[SIGNAL] Emit signalCompressionsStarted()" << Qt::endl;
    emit signalCompressionsStarted();

    qDebug() << "[EXIT SLOT] CPRHelpAdult::startCompressions()" << Qt::endl;
}

// SLOT
void CPRHelpAdult::stopCompressions()
{
    qDebug() << "[ENTRY SLOT] CPRHelpAdult::stopCompressions()" << Qt::endl;
    if(!eventActive || !compressionsActive)
    {
        qDebug() << "[EXIT SLOT - WARNING] CPRHelpAdult::startCompressions() called -> eventActive: "
                 << eventActive << "    compressionsActive: " << compressionsActive
                 << Qt::endl;
        return;
    }

    compressionsActive = false;
    cprCyclicTimer.blockSignals(true);
    cprCyclicTimer.stop();

    qDebug() << "[SIGNAL] Emit signalCompressionsStopped()" << Qt::endl;
    emit signalCompressionsStopped();

    qDebug() << "[EXIT SLOT] CPRHelpAdult::stopCompressions()" << Qt::endl;
}


// SLOT
void CPRHelpAdult::start()
{
    qDebug() << "[ENTRY SLOT] CPRHelpAdult::start()" << Qt::endl;

    if(eventActive)
    {
        qDebug() << "[EXIT SLOT - WARNING] CPRHelpAdult::start(): CPR event was already active" << Qt::endl;
        return;
    }
    CPRHelpStrategy::start();
    noCPRTimer.start();

    qDebug() << "[EXIT SLOT] CPRHelpAdult::start()" << Qt::endl;
}

// SLOT
void CPRHelpAdult::doCompression()
{
    qDebug() << "[ENTRY SLOT] CPRHelpAdult::doCompression()" << Qt::endl;

    if(!eventActive || !compressionsActive)
    {
        qDebug() << "[EXIT SLOT - WARNING] CPRHelpAdult::doCompression() called -> eventActive: "
                 << eventActive << "    compressionsActive: " << compressionsActive
                 << Qt::endl;
        return;
    }

    qDebug().noquote()  << "[INFO] CPRHelpAdult::doCompression()"          << Qt::endl
                        << "\tDepth: " << QString::number(compressionDepth)   << Qt::endl
                        << "\tRate: " << QString::number(compressionRate)     << Qt::endl;

    // Reset the noCPRTimer
    noCPRTimer.blockSignals(true);
    noCPRTimer.stop();
    noCPRTimer.blockSignals(false);
    if(continueCPR)
    {
        signalForwardUserPrompt(P_BLANK);
        continueCPR = false;
    }

    // The compressionRate is set by the GUI slider
    // So even though we're issuing periodic doCompression() calls, we're
    // not actually sampling them in order to process a CPM for giving advice.
    // We just use the CPM we already know about.
    if(compressionRate > CPR_SLOW_THRESHHOLD)
    {
        qDebug() << "[SIGNAL] Emit signalCPRCompressionRatePrompt(" << P_GOOD_SPEED << ")" << Qt::endl;
        emit signalCPRCompressionRatePrompt(P_GOOD_SPEED);
    }
    else if(compressionRate > CPR_VERYSLOW_THRESHHOLD)
    {
        qDebug() << "[SIGNAL] Emit signalCPRCompressionRatePrompt(" << P_SLIGHTLY_FASTER << ")" << Qt::endl;
        emit signalCPRCompressionRatePrompt(P_SLIGHTLY_FASTER);
    }
    else
    {
        qDebug() << "[SIGNAL] Emit signalCPRCompressionRatePrompt(" << P_FASTER << ")" << Qt::endl;
        emit signalCPRCompressionRatePrompt(P_FASTER);
    }

    // Compressions too shallow - issue prompt ("PUSH HARDER")
    if(compressionDepth < 20)
    {
        pushHarder = true;
        qDebug() << "[SIGNAL] Emit signalForwardUserPrompt(" << P_CPR_HARDER << ")" << Qt::endl;
        signalForwardUserPrompt(P_CPR_HARDER);
    }
    else if(pushHarder)
    {
        // For the "GOOD COMPRESSIONS" prompt, if the "PUSH HARDER" prompt was previously issued
        pushHarder = false;
        qDebug() << "[SIGNAL] Emit signalForwardUserPrompt(" << P_CPR_GOOD << ")" << Qt::endl;
        signalForwardUserPrompt(P_CPR_GOOD);
    }

    qDebug().noquote() << "[SIGNAL] Emit signalDisplayCompressionDepth(" << QString::number(compressionDepth) << ")" << Qt::endl;
    emit signalDisplayCompressionDepth(compressionDepth);    // Update QProgressBar in LCD (passing through the ModuleCPRHelp)

    noCPRTimer.start();

    qDebug() << "[EXIT SLOT] CPRHelpAdult::doCompression()" << Qt::endl;
}

// SLOT
void CPRHelpAdult::noCPRDetected()
{
    qDebug() << "[ENTRY SLOT] CPRHelpAdult::noCPRDetected()" << Qt::endl;

    if(!eventActive)
    {
        qDebug() << "[EXIT SLOT - WARNING] CPRHelpAdult::noCPRDetected() but CPR event was not active" << Qt::endl;
        return;
    }

    qDebug() << "[SIGNAL] Emit signalForwardUserPrompt(" << P_CPR_CONT << ")" << Qt::endl;
    emit signalForwardUserPrompt(P_CPR_CONT);
    qDebug() << "[SIGNAL] Emit signalCPRCompressionRatePrompt(P_BLANK)" << Qt::endl;
    emit signalCPRCompressionRatePrompt(P_BLANK);

    continueCPR = true;

    qDebug() << "[EXIT SLOT] CPRHelpAdult::noCPRDetected()" << Qt::endl;
}



