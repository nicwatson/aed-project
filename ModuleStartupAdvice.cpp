#include "ModuleStartupAdvice.h"

#include "prompts.h"
#include "PictogramAdviceEvent.h"

using namespace aed;

ModuleStartupAdvice::ModuleStartupAdvice(LampWidget * lamp_CheckResponsiveness, LampWidget * lamp_CallHelp, LampWidget * lamp_AttachPads)
    : EventSequence()
{
    add(new AdviceEvent("Msg: Unit OK", DURATION_UNIT_OK, P_UNIT_OK));
    add(new AdviceEvent("Msg: Stay calm", DURATION_STAY_CALM, P_STAY_CALM));
    add(new PictogramAdviceEvent("Picto: Check responsiveness", DURATION_CHECK_RESP, P_CHECK_RESP, lamp_CheckResponsiveness));
    add(new PictogramAdviceEvent("Picto: Call for help", DURATION_CALL_HELP, P_CALL_HELP, lamp_CallHelp));
    add(new PictogramAdviceEvent("Picto: Attach pads", SequencedEvent::NOTIMER, P_ATTACH, lamp_AttachPads));
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

int ModuleStartupAdvice::add(AdviceEvent * newEvent)
{
    qDebug() << "Special add with " << newEvent->getID() << Qt::endl;
    int ret = EventSequence::add(newEvent);
    connect(newEvent, SIGNAL(sendUserPrompt(const QString &)), this, SLOT(userPrompt(const QString &)));
    return ret;
}

int ModuleStartupAdvice::addBefore(AdviceEvent * newEvent, int index)
{
    int ret = EventSequence::addBefore(newEvent, index);
    connect(newEvent, SIGNAL(sendUserPrompt(const QString &)), this, SLOT(userPrompt(const QString &)));
    return ret;
}

int ModuleStartupAdvice::remove(AdviceEvent * target)
{
    disconnect(target, SIGNAL(sendUserPrompt(const QString &)), this, SLOT(userPrompt(const QString &)));
    return EventSequence::remove(target);
}

int ModuleStartupAdvice::remove(int index)
{
    if(validateIndex(index) && queue[index] != nullptr)
    {
        disconnect(queue[index], SIGNAL(sendUserPrompt(const QString &)), this, SLOT(userPrompt(const QString &)));
    }
    return EventSequence::remove(index);
}

// SLOT
void ModuleStartupAdvice::userPrompt(const QString & prompt)
{
    qDebug() << "Received prompt: " << prompt << Qt::endl;
    emit sendUserPrompt(prompt);
}

