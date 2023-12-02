#include "CPRHelpAdult.h"
#include "aedGui/prompts.h"

using namespace aedModel;

CPRHelpAdult::CPRHelpAdult()
    : CPRHelpStrategy(CPR_TIME_ADULT), compressionsActive(false), compressionDepth(0), compressionRate(1), pushHarder(false)
{
    noCPRTimer.setSingleShot(true);
    noCPRTimer.setInterval(CPR_DELAY_TOLERANCE);
    cprCyclicTimer.setSingleShot(false);
    cprCyclicTimer.blockSignals(true);
    connect(&cprCyclicTimer, SIGNAL(timeout()), this, SLOT(doCompression()));
}

void CPRHelpAdult::cleanup()
{
    noCPRTimer.blockSignals(true);
    cprCyclicTimer.blockSignals(true);
    noCPRTimer.stop();
    cprCyclicTimer.stop();
    noCPRTimer.blockSignals(false);
    cprCyclicTimer.blockSignals(false);
    compressionsActive = false;
    compressionDepth = 0;
    compressionRate = 1;
    pushHarder = false;
}

// SLOT
void CPRHelpAdult::updateCompressionDepth(int depth)
{
    compressionDepth = depth;
}

// SLOT
void CPRHelpAdult::updateCompressionRate(int cpm)
{
    compressionRate = std::min(40, cpm);
}

// SLOT
void CPRHelpAdult::startCompressions()
{
    if(!eventActive || compressionsActive) return;
    compressionsActive = true;
    if(compressionRate < 0) compressionRate = 40;
    if(compressionRate > 120) compressionRate = 120;
    cprCyclicTimer.blockSignals(false);
    cprCyclicTimer.setInterval(60000 / compressionRate);
    cprCyclicTimer.start();
    emit signalCompressionsStarted();
}

// SLOT
void CPRHelpAdult::stopCompressions()
{
    if(!eventActive || !compressionsActive) return;
    compressionsActive = false;
    cprCyclicTimer.blockSignals(true);
    cprCyclicTimer.stop();
    emit signalCompressionsStopped();
}


// SLOT
void CPRHelpAdult::start()
{
    if(eventActive) return;
    CPRHelpStrategy::start();
    noCPRTimer.start();
}

// SLOT
void CPRHelpAdult::doCompression()
{
    emit signalDisplayCompressionDepth(compressionDepth);    // TESTING ONLY pdate QProgressBar in LCD (passing through the ModuleCPRHelp)
    if(!eventActive || !compressionsActive) return;

    // Reset the noCPRTimer
    noCPRTimer.blockSignals(true);
    noCPRTimer.stop();
    noCPRTimer.blockSignals(false);

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
}

// SLOT
void CPRHelpAdult::noCPRDetected()
{
    if(!eventActive) return;
    emit signalUserPrompt(P_CPR_CONT);
}



