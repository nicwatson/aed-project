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
