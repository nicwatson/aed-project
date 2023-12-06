#include "AdviceEvent.h"

using namespace event;

AdviceEvent::AdviceEvent(const QString & name, int timerInterval, const QString & prompt)
    : SequencedEvent(name, timerInterval), prompt(prompt)
{
    qDebug().noquote().nospace()
            << "[CONSTRUCTOR] AdviceEvent("
            << "name = " << name << ", "
            << "timerInterval = " << QString::number(timerInterval) << ", "
            << "prompt = " << prompt << ") : SequencedEvent id = " << QString::number(id)
            << Qt::endl;
}

AdviceEvent::~AdviceEvent()
{
    qDebug().noquote().nospace() << "[DESTRUCTOR] AdviceEvent w/ name = " << name
                                 << ", id = " << QString::number(id) << Qt::endl;
    // No dynamic allocations to destroy
}

// SLOT
void AdviceEvent::enter()
{
    qDebug().noquote().nospace()
            << "[ENTRY SLOT] AdviceEvent::enter() w/ name " << name << " (id: " << QString::number(id) << ")" << Qt::endl
            << "\tEvent lasts for: " << timerInterval << Qt::endl
            << "\tEvent prompt: " << prompt << Qt::endl;

    qDebug().noquote().nospace()
            << "[SIGNAL] Emitting signal_AdviceEventUserPrompt(" << prompt << ")" << Qt::endl;

    // Usage: slot is ModuleStartupAdvice::userPrompt(const QString &) : connection in ModuleStartupAdvice::add(...)
    emit signal_AdviceEventUserPrompt(prompt);

    SequencedEvent::enter();

    qDebug().noquote().nospace()
            << "[EXIT SLOT] AdviceEvent::enter() w/ name " << name << " (id: " << QString::number(id) << ")" << Qt::endl;
}

// SLOT
void AdviceEvent::exit()
{
    qDebug().noquote().nospace()
            << "[ENTER SLOT] AdviceEvent::exit() w/ name " << name << " (id: " << QString::number(id) << ")" << Qt::endl;

    SequencedEvent::exit();

    qDebug().noquote().nospace()
            << "[EXIT SLOT] AdviceEvent::exit() w/ name " << name << " (id: " << QString::number(id) << ")" << Qt::endl;
}

// SLOT
void AdviceEvent::abort()
{
    qDebug().noquote().nospace()
            << "[ENTER SLOT] AdviceEvent::abort() w/ name " << name << " (id: " << QString::number(id) << ")" << Qt::endl;

    SequencedEvent::abort();

    qDebug().noquote().nospace()
            << "[EXIT SLOT] AdviceEvent::abort() w/ name " << name << " (id: " << QString::number(id) << ")" << Qt::endl;
}
