// FILE ModuleStartupAdvice.cpp
// Implementations for ModuleStartupAdvice class

#include "ModuleStartupAdvice.h"

#include "aedGui/strings.h"
#include "event/PictogramAdviceEvent.h"
#include "AED.h"
#include <QDebug>

using namespace aedModel;

ModuleStartupAdvice::ModuleStartupAdvice(aedGui::LampWidget * lamp_CheckResponsiveness, aedGui::LampWidget * lamp_CallHelp, aedGui::LampWidget * lamp_AttachPads)
    : event::EventSequence()
{
    qDebug() << "[CONSTRUCTOR ENTRY] ModuleStartupAdvice(LampWidget *, LampWidget *, LampWidget *)" << Qt::endl;

    add(new event::AdviceEvent("Msg: Stay calm", DURATION_STAY_CALM, P_STAY_CALM));
    add(new event::PictogramAdviceEvent("Picto: Check responsiveness", DURATION_CHECK_RESP, P_CHECK_RESP, lamp_CheckResponsiveness));
    add(new event::PictogramAdviceEvent("Picto: Call for help", DURATION_CALL_HELP, P_CALL_HELP, lamp_CallHelp));
    add(new event::PictogramAdviceEvent("Picto: Attach pads", event::SequencedEvent::NOTIMER, P_ATTACH, lamp_AttachPads));

    qDebug() << "[CONSTRUCTOR EXIT] ModuleStartupAdvice(LampWidget *, LampWidget *, LampWidget *)" << Qt::endl;
}

ModuleStartupAdvice::~ModuleStartupAdvice()
{
    qDebug() << "[DESTRUCTOR ENTRY] ~ModuleStartupAdvice()" << Qt::endl;

    // An EventSequence does not itself take responsibility for managing the lifecycle of Events added to it
    // but a derived class like ModuleStartupAdvice can do so. In this case, the ModuleStartupAdvice created the events
    // in the constructor and in startAdvice(...), and will therefore delete them.
    for(int i = 0; i < queue.size(); i++)
    {
        if(queue[i] != nullptr)
        {
            queue[i]->deleteLater();
            queue[i] = nullptr;
        }
    }

    qDebug() << "[DESTRUCTOR EXIT] ~ModuleStartupAdvice()" << Qt::endl;
}

int ModuleStartupAdvice::add(event::AdviceEvent * newEvent)
{
    qDebug() << "[ENTRY] ModuleStartupAdvice::add(AdviceEvent *)" << Qt::endl;

    int ret = EventSequence::add(newEvent);
    connect(newEvent, &event::AdviceEvent::signal_AdviceEventUserPrompt, this, &ModuleStartupAdvice::userPrompt);

    qDebug() << "[EXIT] ModuleStartupAdvice::add(AdviceEvent *)" << Qt::endl;

    return ret;
}

int ModuleStartupAdvice::addBefore(event::AdviceEvent * newEvent, int index)
{
    qDebug().noquote() << "[ENTRY] ModuleStartupAdvice::addBefore(AdviceEvent *, index=" << QString::number(index) << ")" << Qt::endl;

    int ret = EventSequence::addBefore(newEvent, index);
    connect(newEvent, &event::AdviceEvent::signal_AdviceEventUserPrompt, this, &ModuleStartupAdvice::userPrompt);

    qDebug().noquote() << "[EXIT] ModuleStartupAdvice::addBefore(AdviceEvent *, index=" << QString::number(index) << ")" << Qt::endl;
    return ret;
}

int ModuleStartupAdvice::remove(event::AdviceEvent * target)
{
    qDebug().noquote() << "[ENTRY] ModuleStartupAdvice::remove(AdviceEvent *)" << Qt::endl;

    disconnect(target, &event::AdviceEvent::signal_AdviceEventUserPrompt, this, &ModuleStartupAdvice::userPrompt);
    return EventSequence::remove(target);

    qDebug().noquote() << "[EXIT] ModuleStartupAdvice::remove(AdviceEvent *)" << Qt::endl;
}

int ModuleStartupAdvice::remove(int index)
{
    qDebug().noquote() << "[ENTRY] ModuleStartupAdvice::remove(index=" << QString::number(index) << ")" << Qt::endl;

    if(validateIndex(index) && queue[index] != nullptr)
    {
        event::AdviceEvent * target = dynamic_cast<event::AdviceEvent *>(queue[index]);
        disconnect(target, &event::AdviceEvent::signal_AdviceEventUserPrompt, this, &ModuleStartupAdvice::userPrompt);
    }

    qDebug().noquote() << "[EXIT] ModuleStartupAdvice::remove(index=" << QString::number(index) << ")" << Qt::endl;
    return EventSequence::remove(index);
}

// SLOT
void ModuleStartupAdvice::userPrompt(const QString & prompt)
{
    qDebug() << "[ENTRY SLOT] ModuleStartupAdvice::userPrompt(" << prompt << ")" << Qt::endl;

    qDebug() << "[SIGNAL] Emit ModuleStartupAdvice::signalForwardUserPrompt(" << prompt << ")" << Qt::endl;
    emit signalForwardUserPrompt(prompt);

    qDebug() << "[EXIT SLOT] ModuleStartupAdvice::userPrompt(" << prompt << ")" << Qt::endl;
}

// SLOT
void ModuleStartupAdvice::startAdvice(AED::cableState_t cable)
{
    qDebug().noquote() << "[ENTRY SLOT] ModuleStartupAdvice::startAdvice(" << AED::cableStateNames[cable] << ")" << Qt::endl;

    if(cable == AED::UNPLUGGED) return;
    addBefore(new event::AdviceEvent("Msg: Unit OK", DURATION_UNIT_OK, P_UNIT_OK + QString(" / ") + (cable == AED::PAD_ADULT ? P_PAD_ADULT : P_PAD_CHILD)), 0);
    start();

    qDebug().noquote() << "[EXIT SLOT] ModuleStartupAdvice::startAdvice(" << AED::cableStateNames[cable] << ")" << Qt::endl;
}

// SLOT (override)
void ModuleStartupAdvice::stop()
{
    qDebug() << "[ENTRY SLOT] ModuleStartupAdvice::stop()" << Qt::endl;

    event::EventSequence::stop();
    remove(0);

    qDebug() << "[EXIT SLOT] ModuleStartupAdvice::stop()" << Qt::endl;
}
