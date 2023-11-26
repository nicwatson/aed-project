#ifndef PICTOGRAMADVICEEVENT_H
#define PICTOGRAMADVICEEVENT_H

#include "AdviceEvent.h"
#include "LampWidget.h"

namespace aed
{
    class PictogramAdviceEvent : public AdviceEvent
    {
        Q_OBJECT

        public:
            explicit PictogramAdviceEvent(const QString & name, int timerInterval, const QString & prompt);
            explicit PictogramAdviceEvent(const QString & name, int timerInterval, const QString & prompt, LampWidget * lamp);

            void bindLampWidget(LampWidget * widget);
            void releaseLampWidget(LampWidget * widget);

        protected:
            virtual void enter() override;
            virtual void exit() override;
            virtual void abort() override;

        signals:
            void signalLampOn();
            void signalLampOff();
            void signalLampFlash();

    };
}

#endif // PICTOGRAMADVICEEVENT_H
