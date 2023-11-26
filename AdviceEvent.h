#ifndef ADVICEEVENT_H
#define ADVICEEVENT_H

#include "SequencedEvent.h"
#include <QObject>

namespace aed
{

    class AdviceEvent : public SequencedEvent
    {


        Q_OBJECT
        public:
            explicit AdviceEvent(const QString & name, int timerInterval, const QString & prompt);

            inline void setPrompt(const QString & newPrompt) { prompt = newPrompt; }
            inline const QString & getPrompt() const { return prompt; }

        protected:
            QString prompt;


        protected:
            virtual void enter() override;
            virtual void exit() override;
            virtual void abort() override;

    };



}

#endif // ADVICEEVENT_H
