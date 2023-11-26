#include "InitialAdviceEvent.h"

using namespace aed;

InitialAdviceEvent::InitialAdviceEvent(const QString & name, int timerInterval) : SequencedEvent(name, timerInterval)
{
    prompt = "";
}

InitialAdviceEvent::InitialAdviceEvent(const QString & name, int timerInterval, const QString & voicePrompt, LampWidget * lamp)
    : SequencedEvent(name, timerInterval), prompt(voicePrompt)
{
    bindLampWidget(lamp);
}

void InitialAdviceEvent::bindLampWidget(LampWidget * widget)
{
    if(widget == nullptr) return;
    connect(this, SIGNAL(signalLampOff()), widget, SLOT(turnOff()));
    connect(this, SIGNAL(signalLampOn()), widget, SLOT(turnOn()));
}

void InitialAdviceEvent::releaseLampWidget(LampWidget * widget)
{
    if(widget == nullptr) return;
    disconnect(this, SIGNAL(signalLampOff()), widget, SLOT(turnOff()));
    disconnect(this, SIGNAL(signalLampOn()), widget, SLOT(turnOn()));
}

// OVERRIDE
void InitialAdviceEvent::enter()
{
    emit signalLampOn();
    QDebug debug = qDebug().noquote().nospace();
    debug
          << "[DEBUG] Starting advice stage: \""
          << name << "\""
          << " (id: "
          << QString::number(id)
          << ")"
          << "   :   The light should now be ON."
          << Qt::endl;
    debug << "[TODO] VOICE PROMPT: \"" << prompt << "\"" << Qt::endl;
    SequencedEvent::enter();
}

// OVERRIDE
void InitialAdviceEvent::exit()
{
    emit signalLampOff();
    QDebug debug = qDebug().noquote().nospace();
    debug
          << "[DEBUG] Normal exit to stage: \""
          << name << "\""
          << " (id: "
          << QString::number(id)
          << ")"
          << "   :   The light should now be OFF, and the next stage should occur shortly."
          << Qt::endl;
    SequencedEvent::exit();
}

// OVERRIDE
void InitialAdviceEvent::abort()
{
    emit signalLampOff();
    QDebug debug = qDebug().noquote().nospace();
    debug
          << "[DEBUG] Aborted advice stage: \""
          << name << "\""
          << " (id: "
          << QString::number(id)
          << ")"
          << "   :   The light should now be OFF."
          << Qt::endl;
    SequencedEvent::abort();
}
