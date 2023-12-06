#include "PictogramAdviceEvent.h"
#include <QDebug>

using namespace event;

PictogramAdviceEvent::PictogramAdviceEvent(const QString & name, int timerInterval, const QString & prompt)
    : AdviceEvent(name, timerInterval, prompt)
{
    qDebug().noquote() << "[CONSTRUCTOR ENTRY/EXIT] PictogramAdviceEvent("
                       << "name=\"" << name << "\", "
                       << "timerInterval=" << QString::number(timerInterval) << ", "
                       << "prompt=\"" << prompt << "\")"
                       << Qt::endl;
}

PictogramAdviceEvent::PictogramAdviceEvent(const QString & name, int timerInterval, const QString & prompt, aedGui::LampWidget * lamp)
    : PictogramAdviceEvent(name, timerInterval, prompt)
{
    qDebug().noquote() << "[CONSTRUCTOR ENTRY] PictogramAdviceEvent("
                       << "name=\"" << name << "\", "
                       << "timerInterval=" << QString::number(timerInterval) << ", "
                       << "prompt=\"" << prompt << "\", "
                       << "LampWidget *)"
                       << Qt::endl;

    bindLampWidget(lamp);

    qDebug().noquote() << "[CONSTRUCTOR EXIT] PictogramAdviceEvent("
                       << "name=\"" << name << "\", "
                       << "timerInterval=" << QString::number(timerInterval) << ", "
                       << "prompt=\"" << prompt << "\", "
                       << "LampWidget *)"
                       << Qt::endl;
}

PictogramAdviceEvent::~PictogramAdviceEvent()
{
    qDebug() << "[DESTRUCTOR] ~PictogramAdviceEvent()" << Qt::endl;
    // No dynamic allocations to deal with
}

void PictogramAdviceEvent::bindLampWidget(aedGui::LampWidget * widget)
{
    qDebug() << "[ENTRY] PictogramAdviceEvent::bindLampWidget(LampWidget *) : if no [EXIT] then arg was nullptr" << Qt::endl;

    if(widget == nullptr) return;
    connect(this, &PictogramAdviceEvent::signalLampOff, widget, &aedGui::LampWidget::turnOff);
    connect(this, &PictogramAdviceEvent::signalLampOn, widget, &aedGui::LampWidget::turnOn);
    connect(this, &PictogramAdviceEvent::signalLampFlash, widget, &aedGui::LampWidget::startFlash);

    qDebug() << "[EXIT] PictogramAdviceEvent::bindLampWidget(LampWidget *)" << Qt::endl;
}

void PictogramAdviceEvent::releaseLampWidget(aedGui::LampWidget * widget)
{
    qDebug() << "[ENTRY] PictogramAdviceEvent::releaseLampWidget(LampWidget *) : if no [EXIT] then arg was nullptr" << Qt::endl;

    if(widget == nullptr) return;
    disconnect(this, &PictogramAdviceEvent::signalLampOff, widget, &aedGui::LampWidget::turnOff);
    disconnect(this, &PictogramAdviceEvent::signalLampOn, widget, &aedGui::LampWidget::turnOn);
    disconnect(this, &PictogramAdviceEvent::signalLampFlash, widget, &aedGui::LampWidget::startFlash);

    qDebug() << "[EXIT] PictogramAdviceEvent::bindLampWidget(LampWidget *)" << Qt::endl;
}

void PictogramAdviceEvent::enter()
{
    qDebug() << "[ENTRY] PictogramAdviceEvent::enter()" << Qt::endl;

    qDebug() << "[SIGNAL] Emit PictogramAdviceEvent::signalLampFlash()" << Qt::endl;
    emit signalLampFlash();

    qDebug().noquote().nospace()  << "[INFO] PictogramAdviceEvent::enter(): Starting advice stage: \""
                                  << name << "\""
                                  << " (id: "
                                  << QString::number(id)
                                  << ")"
                                  << "   :   The light should now be FLASHING."
                                  << Qt::endl;

    qDebug() << "[SIGNAL] Emit (AdviceEvent) PictogramAdviceEvent::signal_AdviceEventUserPrompt(" << prompt << ")" << Qt::endl;
    emit signal_AdviceEventUserPrompt(prompt);

    SequencedEvent::enter();

    qDebug() << "[EXIT] PictogramAdviceEvent::enter()" << Qt::endl;
}

void PictogramAdviceEvent::exit()
{
    qDebug() << "[ENTRY] PictogramAdviceEvent::exit()" << Qt::endl;

    qDebug() << "[SIGNAL] Emit PictogramAdviceEvent::signalLampOff()" << Qt::endl;
    emit signalLampOff();

    qDebug().noquote().nospace()  << "[INFO] PictogramAdviceEvent::exit(): Normal exit to stage: \""
                                  << name << "\""
                                  << " (id: "
                                  << QString::number(id)
                                  << ")"
                                  << "   :   The light should now be OFF, and the next stage should occur shortly."
                                  << Qt::endl;
    SequencedEvent::exit();

    qDebug() << "[EXIT] PictogramAdviceEvent::exit()" << Qt::endl;
}

void PictogramAdviceEvent::abort()
{
    qDebug() << "[ENTRY] PictogramAdviceEvent::abort()" << Qt::endl;

    qDebug() << "[SIGNAL] Emit PictogramAdviceEvent::signalLampOff()" << Qt::endl;
    emit signalLampOff();

    qDebug().noquote().nospace()  << "[INFO] PictogramAdviceEvent::abort(): Aborting event \""
                                  << name << "\""
                                  << " (id: "
                                  << QString::number(id)
                                  << ")"
                                  << "   :   The light should now be OFF."
                                  << Qt::endl;
    SequencedEvent::abort();

    qDebug() << "[EXIT] PictogramAdviceEvent::abort()" << Qt::endl;
}
