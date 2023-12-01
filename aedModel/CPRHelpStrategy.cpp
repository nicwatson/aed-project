#include "CPRHelpStrategy.h"
#include "aedGui/prompts.h"

using namespace aedModel;

CPRHelpStrategy::CPRHelpStrategy(int duration) : QObject(), eventActive(false), duration(duration)
{
    timer.setSingleShot(true);
    timer.setInterval(duration);
    connect(&timer, SIGNAL(timeout()), this, SLOT(exit()));
}

CPRHelpStrategy::~CPRHelpStrategy()
{

}

void CPRHelpStrategy::cleanup()
{
    timer.blockSignals(true);
    timer.stop();
    timer.blockSignals(false);
    eventActive = false;
}

void CPRHelpStrategy::reset()
{
    cleanup();
}

// SLOT
void CPRHelpStrategy::start()
{
    if(eventActive) return;
    eventActive = true;
    timer.start();
    emit signalCPRStarted();
    emit signalUserPrompt(P_CPR_START);
}

// SLOT
void CPRHelpStrategy::abort()
{
    if(!eventActive) return;
    reset();
    emit signalCPRAborted();
}

// SLOT
void CPRHelpStrategy::exit()
{
    if(!eventActive) return;
    reset();
    emit signalCPREnded();
    emit signalUserPrompt(P_CPR_STOP);
}
