#ifndef SEQUENCEDDEBUGEVENT_H
#define SEQUENCEDDEBUGEVENT_H

#include "SequencedEvent.h"
#include <QObject>

namespace event
{
    class SequencedDebugEvent : public SequencedEvent
    {
        Q_OBJECT

        public:
            explicit SequencedDebugEvent(const QString & name, int timerInterval);
            void exit();

        public slots:
            void start();
            void skip();


    };
}

#endif // SEQUENCEDDEBUGEVENT_H
