// FILE PictogramAdviceEvent.h
//
// A PictogramAdviceEvent is a kind of advice event that is associated with a pictogram and therefore with a LampWidget.
// This class doesn't actually need to know about the pictogram itself, which is a static graphic, but it does need to
// be able to control the LampWidget, making it blink when this event is "active."

#ifndef PICTOGRAMADVICEEVENT_H
#define PICTOGRAMADVICEEVENT_H

#include "AdviceEvent.h"
#include "aedGui/LampWidget.h"

namespace event
{
    class PictogramAdviceEvent : public AdviceEvent
    {
        Q_OBJECT

        public:

            // Create a new PictogramAdviceEvent with the given name, timer interval, and text prompt to display
            // If using this constructor, it is necessary to then call bindLampWidget(LampWidget *) once a LampWidget is available
            explicit PictogramAdviceEvent(const QString & name, int timerInterval, const QString & prompt);

            // Create a new PictogramAdviceEvent with the given name, timer interval, and text prompt to display, as well as binding it to a lamp widget
            explicit PictogramAdviceEvent(const QString & name, int timerInterval, const QString & prompt, aedGui::LampWidget * lamp);

            ~PictogramAdviceEvent();

            // Make this event control the given lamp widget
            void bindLampWidget(aedGui::LampWidget * widget);

            // Release a bound lamp widget from service
            void releaseLampWidget(aedGui::LampWidget * widget);

        protected:
            // Note: These are not slots. The event belongs to a container (EventSequence), which invokes these directly.

            // Event start (display prompt, turn on light, start timer if applicable)
            virtual void enter() override;

            // Event start (turn off light etc.)
            virtual void exit() override;

            // Event abort (turn off light etc.
            virtual void abort() override;

        signals:

            // Make the associated LampWidget turn on
            // Emitter: Currently none
            // Receiver: slot on the bound LampWidget::turnOn() : connection in PictogramAdviceEvent::bindLampWidget(...)
            void signalLampOn();

            // Make the associated LampWidget turn off (also stops flashing)
            // Emitters: PictogramAdviceEvent::exit(), ::abort()
            // Receiver: slot on the bound LampWidget::turnOn() : connection in PictogramAdviceEvent::bindLampWidget(...)
            void signalLampOff();

            // Make the associated LampWidget start flashing
            // Emitter: PictogramAdviceEvent::enter()
            // Receiver: slot on the bound LampWidget::turnOn() : connection in PictogramAdviceEvent::bindLampWidget(...)
            void signalLampFlash();

    };
}

#endif // PICTOGRAMADVICEEVENT_H
