#include "CPRHelpStrategy.h"
#include "aedGui/strings.h"

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
    qDebug() << "CPRHelpStrategy now starting." << Qt::endl;
    if(eventActive) return;
    eventActive = true;
    timer.start();
    emit signalUserPrompt(P_CPR_START);
}

// SLOT
void CPRHelpStrategy::abort()
{
    if(!eventActive) return;
    reset();
}

// SLOT
void CPRHelpStrategy::exit()
{
    if(!eventActive) return;
    emit signalCPRStrategyEnded();
    emit signalUserPrompt(P_CPR_STOP);
}
