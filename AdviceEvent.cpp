#include "AdviceEvent.h"

using namespace aed;

AdviceEvent::AdviceEvent(const QString & name, int timerInterval, const QString & prompt)
    : SequencedEvent(name, timerInterval), prompt(prompt)
{

}

void AdviceEvent::enter()
{
    QDebug debug = qDebug().noquote().nospace();
    debug
          << "[DEBUG] Starting advice stage: \""
          << name << "\""
          << " (id: "
          << QString::number(id)
          << ")"
          << Qt::endl;
    debug << "[TODO] VOICE PROMPT: \"" << prompt << "\"" << Qt::endl;
    SequencedEvent::enter();
}

void AdviceEvent::exit()
{
    QDebug debug = qDebug().noquote().nospace();
    debug
            << "[DEBUG] Normal exit to stage: \""
            << name << "\""
            << " (id: "
            << QString::number(id)
            << ")"
            << Qt::endl;
    SequencedEvent::exit();
}

void AdviceEvent::abort()
{
    QDebug debug = qDebug().noquote().nospace();
    debug
            << "[DEBUG] Aborted advice stage: \""
            << name << "\""
            << " (id: "
            << QString::number(id)
            << ")"
            << Qt::endl;
    SequencedEvent::abort();
}
