// FILE: AdviceEvent.h
//
// An AdviceEvent is a SequencedEvent that can store a text string to use as an advice prompt to the
// user (e.g. "REMAIN CALM"), and has a dedicated signal for transmitting that prompt

#ifndef ADVICEEVENT_H
#define ADVICEEVENT_H

#include <QObject>
#include <QString>
#include "SequencedEvent.h"

namespace event
{

    class AdviceEvent : public SequencedEvent
    {
        Q_OBJECT
        public:
            explicit AdviceEvent(const QString & name, int timerInterval, const QString & prompt);
            ~AdviceEvent();

            inline void setPrompt(const QString & newPrompt) { prompt = newPrompt; }
            inline const QString & getPrompt() const { return prompt; }

        protected:
            QString prompt;


        protected:
            // Note: These are not slots. The event belongs to a container (EventSequence), which invokes these directly.

            // Begin the event
            virtual void enter() override;

            // End the event
            virtual void exit() override;

            // Stop the event immediately
            virtual void abort() override;

        signals:
            // Forward a user prompt to the client
            // Usage: slot is ModuleStartupAdvice::userPrompt(const QString &) : connection in ModuleStartupAdvice::add(...)
            void signal_AdviceEventUserPrompt(const QString & prompt);

    };



}

#endif // ADVICEEVENT_H
