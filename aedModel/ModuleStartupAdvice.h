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


#define DURATION_UNIT_OK 3000
#define DURATION_STAY_CALM 3000
#define DURATION_CHECK_RESP 4000
#define DURATION_CALL_HELP 4000

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
            // DO NOT USE THIS DEFAULT CONSTRUCTOR
            // It's only here because QT's MOC requires it
            explicit ModuleStartupAdvice();

            explicit ModuleStartupAdvice(aedGui::LampWidget * lamp_CheckResponsiveness, aedGui::LampWidget * lamp_CallHelp, aedGui::LampWidget * lamp_AttachPads);
            ~ModuleStartupAdvice();

            virtual int add(event::AdviceEvent * newEvent);
            virtual int addBefore(event::AdviceEvent * newEvent, int index);
            virtual int remove(event::AdviceEvent * target);
            virtual int remove(int index) override;


        public slots:
            void userPrompt(const QString & prompt);
            void startAdvice(AED::cableState_t cable);
            virtual void stop() override;

        signals:
            void signalUserPrompt(const QString & prompt);

    };
}

#endif // MODULESTARTUPADVICE_H
