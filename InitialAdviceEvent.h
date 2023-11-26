#ifndef INITIALADVICEEVENT_H
#define INITIALADVICEEVENT_H

#include "SequencedEvent.h"
#include "LampWidget.h"
#include <QObject>

namespace aed
{
    class InitialAdviceEvent : public SequencedEvent
    {
        Q_OBJECT

        public:
            explicit InitialAdviceEvent(const QString & name, int timerInterval);
            explicit InitialAdviceEvent(const QString & name, int timerInterval, const QString & voicePrompt, LampWidget * lamp);

            void bindLampWidget(LampWidget * widget);
            void releaseLampWidget(LampWidget * widget);

            inline void setPrompt(const QString & newPrompt) { prompt = newPrompt; }
            inline const QString & getPrompt() const { return prompt; }

        private:
            QString prompt;


        protected:
            virtual void enter();
            virtual void exit();
            virtual void abort();

        signals:
            void signalLampOn();
            void signalLampOff();

    };
}

#endif // INITIALADVICEEVENT_H
