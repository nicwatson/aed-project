#include "ModuleStartupAdvice.h"

#include "aedGui/strings.h"
#include "event/PictogramAdviceEvent.h"
#include "AED.h"

using namespace aedModel;

// DO NOT USE THIS CONSTRUCTOR
// It's only here because QT's MOC requires it
//ModuleStartupAdvice::ModuleStartupAdvice() : event::EventSequence()
//{

//}

ModuleStartupAdvice::ModuleStartupAdvice(aedGui::LampWidget * lamp_CheckResponsiveness, aedGui::LampWidget * lamp_CallHelp, aedGui::LampWidget * lamp_AttachPads)
    : event::EventSequence()
{
    add(new event::AdviceEvent("Msg: Stay calm", DURATION_STAY_CALM, P_STAY_CALM));
    add(new event::PictogramAdviceEvent("Picto: Check responsiveness", DURATION_CHECK_RESP, P_CHECK_RESP, lamp_CheckResponsiveness));
    add(new event::PictogramAdviceEvent("Picto: Call for help", DURATION_CALL_HELP, P_CALL_HELP, lamp_CallHelp));
    add(new event::PictogramAdviceEvent("Picto: Attach pads", event::SequencedEvent::NOTIMER, P_ATTACH, lamp_AttachPads));
}

ModuleStartupAdvice::~ModuleStartupAdvice()
{
    for(int i = 0; i < queue.size(); i++)
    {
        if(queue[i] != nullptr)
        {
            queue[i]->deleteLater();
            queue[i] = nullptr;
        }
    }
}

int ModuleStartupAdvice::add(event::AdviceEvent * newEvent)
{
    int ret = EventSequence::add(newEvent);
    connect(newEvent, SIGNAL(signalUserPrompt(const QString &)), this, SLOT(userPrompt(const QString &)));
    return ret;
}

int ModuleStartupAdvice::addBefore(event::AdviceEvent * newEvent, int index)
{
    int ret = EventSequence::addBefore(newEvent, index);
    connect(newEvent, SIGNAL(signalUserPrompt(const QString &)), this, SLOT(userPrompt(const QString &)));
    return ret;
}

int ModuleStartupAdvice::remove(event::AdviceEvent * target)
{
    disconnect(target, SIGNAL(signalUserPrompt(const QString &)), this, SLOT(userPrompt(const QString &)));
    return EventSequence::remove(target);
}

int ModuleStartupAdvice::remove(int index)
{
    if(validateIndex(index) && queue[index] != nullptr)
    {
        disconnect(queue[index], SIGNAL(signalUserPrompt(const QString &)), this, SLOT(userPrompt(const QString &)));
    }
    return EventSequence::remove(index);
}

// SLOT
void ModuleStartupAdvice::userPrompt(const QString & prompt)
{
    // qDebug() << "Received prompt: " << prompt << Qt::endl;
    emit signalUserPrompt(prompt);
}

// SLOT
void ModuleStartupAdvice::startAdvice(AED::cableState_t cable)
{
    qDebug() << "Startadvice slot called on sModuleStartupPAdvice";
    if(cable == AED::UNPLUGGED) return;
    addBefore(new event::AdviceEvent("Msg: Unit OK", DURATION_UNIT_OK, P_UNIT_OK + QString(" / ") + (cable == AED::PAD_ADULT ? P_PAD_ADULT : P_PAD_CHILD)), 0);
    start();
}

// SLOT (override)
void ModuleStartupAdvice::stop()
{
    event::EventSequence::stop();
    remove(0);
}
