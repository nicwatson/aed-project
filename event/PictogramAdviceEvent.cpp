#include "PictogramAdviceEvent.h"

using namespace event;

PictogramAdviceEvent::PictogramAdviceEvent(const QString & name, int timerInterval, const QString & prompt)
    : AdviceEvent(name, timerInterval, prompt)
{

}

PictogramAdviceEvent::PictogramAdviceEvent(const QString & name, int timerInterval, const QString & prompt, aedGui::LampWidget * lamp)
    : PictogramAdviceEvent(name, timerInterval, prompt)
{
    bindLampWidget(lamp);
}

void PictogramAdviceEvent::bindLampWidget(aedGui::LampWidget * widget)
{
    if(widget == nullptr) return;
    connect(this, SIGNAL(signalLampOff()), widget, SLOT(turnOff()));
    connect(this, SIGNAL(signalLampOn()), widget, SLOT(turnOn()));
    connect(this, SIGNAL(signalLampFlash()), widget, SLOT(startFlash()));
}

void PictogramAdviceEvent::releaseLampWidget(aedGui::LampWidget * widget)
{
    if(widget == nullptr) return;
    disconnect(this, SIGNAL(signalLampOff()), widget, SLOT(turnOff()));
    disconnect(this, SIGNAL(signalLampOn()), widget, SLOT(turnOn()));
    disconnect(this, SIGNAL(signalLampFlash()), widget, SLOT(startFlash()));
}

void PictogramAdviceEvent::enter()
{
    qDebug() << "Entering pictogram advice event";
    emit signalLampFlash();
    QDebug debug = qDebug().noquote().nospace();
    debug
          << "[DEBUG] Starting advice stage: \""
          << name << "\""
          << " (id: "
          << QString::number(id)
          << ")"
          << "   :   The light should now be FLASHING."
          << Qt::endl;
    emit signalUserPrompt(prompt);
    SequencedEvent::enter();
}

void PictogramAdviceEvent::exit()
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

void PictogramAdviceEvent::abort()
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
