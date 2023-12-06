// FILE: CPRHelpStrategy.cpp
// Implementations for CPRHelpStrategy

#include "CPRHelpStrategy.h"
#include "aedGui/strings.h"

using namespace aedModel;

CPRHelpStrategy::CPRHelpStrategy(int duration) : QObject(), eventActive(false), duration(duration)
{
    qDebug().noquote().nospace() << "[CONSTRUCTOR] CPRHelpStrategy(" << QString::number(duration) << ")" << Qt::endl;

    timer.setSingleShot(true);
    timer.setInterval(duration);
    connect(&timer, &QTimer::timeout, this, &CPRHelpStrategy::exit);
}

CPRHelpStrategy::~CPRHelpStrategy()
{
    qDebug().noquote().nospace() << "[DESTRUCTOR] CPRHelpStrategy" << Qt::endl;
    // No dynamic allocations to delete
}

void CPRHelpStrategy::cleanup()
{
    qDebug() << "[ENTRY] CPRHelpStrategy::cleanup()" << Qt::endl;

    timer.blockSignals(true);
    timer.stop();
    timer.blockSignals(false);
    eventActive = false;

    qDebug() << "[EXIT] CPRHelpStrategy::cleanup()" << Qt::endl;
}

void CPRHelpStrategy::reset()
{
    qDebug() << "[ENTRY] CPRHelpStrategy::reset()" << Qt::endl;

    cleanup();

    qDebug() << "[ENTRY] CPRHelpStrategy::reset()" << Qt::endl;
}

// SLOT
void CPRHelpStrategy::start()
{
    qDebug() << "[ENTRY SLOT] CPRHelpStrategy::start()" << Qt::endl;

    if(eventActive) return;
    eventActive = true;
    timer.start();

    qDebug() << "[SIGNAL] Emit CPRHelpStrategy::signalForwardUserPrompt(" << P_CPR_START << ")" << Qt::endl;
    emit signalForwardUserPrompt(P_CPR_START);

    qDebug() << "[EXIT SLOT] CPRHelpStrategy::start()" << Qt::endl;
}

// SLOT
void CPRHelpStrategy::abort()
{
    qDebug() << "[ENTRY SLOT] CPRHelpStrategy::abort()" << Qt::endl;
    qDebug() << "\tIf no [EXIT], abortable event was not active" << Qt::endl;

    if(!eventActive) return;
    reset();

    qDebug() << "[EXIT SLOT] CPRHelpStrategy::abort()" << Qt::endl;
}

// SLOT
void CPRHelpStrategy::exit()
{
    qDebug() << "[ENTRY SLOT] CPRHelpStrategy::exit()" << Qt::endl;
    qDebug() << "\tIf no [EXIT], abortable event was not active" << Qt::endl;

    if(!eventActive) return;

    qDebug() << "[SIGNAL] Emit CPRHelpStrategy::signalCPRStrategyEnded()" << Qt::endl;
    emit signalCPRStrategyEnded();
    qDebug() << "[SIGNAL] Emit CPRHelpStrategy::signalForwardUserPrompt(" << P_CPR_STOP << ")" << Qt::endl;
    emit signalForwardUserPrompt(P_CPR_STOP);

    qDebug() << "[EXIT SLOT] CPRHelpStrategy::exit()" << Qt::endl;
}
