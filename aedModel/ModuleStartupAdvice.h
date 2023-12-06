// FILE ModuleStartupAdvice.h
//
// Handles the initial sequence of lighted pictograms and prompts when the unit starts up, after the self-test
// It stays on the final event in the sequence, "Attach pads," until pads are attached to the patient,
// but will end early if pads are attached early.

#ifndef MODULESTARTUPADVICE_H
#define MODULESTARTUPADVICE_H

#include <QObject>
#include "AED.h"
#include "event/EventSequence.h"
#include "event/AdviceEvent.h"
#include "aedGui/LampWidget.h"


// NOTE: This class establishes its connections to GUI elements by having them (the three blinking lights, LampWidgets) passed to its constructor.
// The object will not keep the pointers to the lamp widgets - it just uses them to connect. MainWindow (or other client) is responsible for the
// LampWidgets' lifecycles.

namespace aedModel
{
    class ModuleStartupAdvice : public event::EventSequence
    {
        // HEAP ALLOCATIONS:
        //  The queue will have dynamically-allocated SequencedEvents placed into it in the constructor
        //  The destructor should go through the queue and delete all such events.

        Q_OBJECT

        public:
            explicit ModuleStartupAdvice(aedGui::LampWidget * lamp_CheckResponsiveness, aedGui::LampWidget * lamp_CallHelp, aedGui::LampWidget * lamp_AttachPads);
            ~ModuleStartupAdvice();

            // Queue/list management
            virtual int add(event::AdviceEvent * newEvent);
            virtual int addBefore(event::AdviceEvent * newEvent, int index);
            virtual int remove(event::AdviceEvent * target);
            virtual int remove(int index) override;


        public slots:

            // Receive (from an AdviceEvent) and forward (to AED) a user advice prompt
            // Trigger: signal AdviceEvent::signalForwardUserPrompt(const QString &) : connections in ModuleStarupAdvice::add(...), ::addBefore(...)
            void userPrompt(const QString & prompt);

            // Start the advice sequence - have to know the cable type to display the initial "UNIT OK / ADULT PADS" or "UNIT OK / CHILD PADS" prompt
            // Trigger: signal AED::signalStartAdvice() : connection in AED::addModuleStartupAdvice()
            void startAdvice(AED::cableState_t cable);

            // Stop the advice sequence immediately
            // Trigger 1: signal AED::signalPadsAttached(...) : connection in AED::addModuleStartupAdvice()
            // Trigger 2: signal AED::signalAbortStartupAdvice(...) : connection in AED::addModuleStartupAdvice()
            virtual void stop() override;

        signals:

            // Send the user prompt on to the AED
            // Emitter: ModuleStartupAdvice::userPrompt(const QString &)
            // Receiver: AED::userPrompt(const QString &) : connection in AED::addModuleStartupAdvice()
            void signalForwardUserPrompt(const QString & prompt);

    };
}

#endif // MODULESTARTUPADVICE_H
