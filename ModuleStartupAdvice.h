#ifndef MODULESTARTUPADVICE_H
#define MODULESTARTUPADVICE_H

#include <QObject>
#include "EventSequence.h"
#include "LampWidget.h"

#define DURATION_UNIT_OK 3000
#define DURATION_STAY_CALM 3000
#define DURATION_CHECK_RESP 4000
#define DURATION_CALL_HELP 4000

namespace aed
{

    class ModuleStartupAdvice : public EventSequence
    {
        // HEAP ALLOCATIONS:
        //  The queue will have dynamically-allocated SequencedEvents placed into it in the constructor
        //  The destructor should go through the queue and delete all such events.

        Q_OBJECT

        public:
            explicit ModuleStartupAdvice(LampWidget * lamp_CheckResponsiveness, LampWidget * lamp_CallHelp, LampWidget * lamp_AttachPads);
            ~ModuleStartupAdvice();

    };
}

#endif // MODULESTARTUPADVICE_H
