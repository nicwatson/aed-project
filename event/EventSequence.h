// FILE EventSequence.h
//
// An EventSequence is wrapper for a queue of multiple SequencedEvents
// which can be used for situations in which multiple different things need to happen
// but they typically happen in the same way, in the same order, and with automatic
// transitions from one to the next.
// We use this for the "startup advice sequence" wherein after the self-test, the AED will
// light up pictograms and give advice prompts in a sequence, advancing automatically until
// interrupted by pads being attached to the patient
//
// Note the "queue" is not a typical Queue ADT as it doesn't necessarily remove items when they
// are activated; instead, an index tracker moves through the list

#ifndef EVENTSEQUENCE_H
#define EVENTSEQUENCE_H

#include <QObject>
#include <QDebug>

#include "SequencedEvent.h"

namespace event
{
    class EventSequence : public QObject
    {

        Q_OBJECT

        public:
            explicit EventSequence();
            ~EventSequence();

            // Getters
            inline int getPosition() const { return this->position; }
            inline int getActive() const { return this->active; }
            inline int getLength() const { return this->queue.size(); }

            SequencedEvent * getCurrent() const;
            SequencedEvent * getNext() const;
            SequencedEvent * getAt(int i) const;

            // Queue management (add/remove)
            virtual int add(SequencedEvent * newEvent);
            virtual int addAfter(SequencedEvent * newEvent, int index);
            virtual int addBefore(SequencedEvent * newEvent, int index);
            virtual int remove(SequencedEvent * target);
            virtual int remove(int index);

            // Executors to start the event queue from specific locations
            bool startFrom(int index);
            bool startFromBeginning();
            bool startFromHere();
            bool proceed();


        protected:

            QList<SequencedEvent *> queue;  // The queue is simply a QList of SequencedEvent *
            int position;                   // Current position (index) in the queue
            bool active;                    // Is the event sequence currently active?

            QTimer timer;                   // Used for timing sequenced events

            // Utility function: checks if the given index number is valid for this sequence (not out of bounds)
            bool validateIndex(int index) const;


        public slots:

            // Slot interface to start the event sequence from the beginning
            // Triggers: no signals connected; subclass ModuleStartupAdvice has a direct virtual invocation from startAdvice(...)
            virtual void start();

            // Slot interface to stop the event sequence, aborting the current event
            // Triggers: virtual from signal AED::signalPadsAttached(), signal AED::signalAbortStartupAdvice()
            //      via ModuleStartupAdvice's overridden defs; connections in AED::addModuleStartupAdvice(...)
            virtual void stop();


        protected slots:

            // When triggered, advances to the next event in the sequence (if possible)
            // Trigger: timer.QTimer::timeout() (end of each timed event) : connection in EventSequence constructor
            virtual void advance();


        signals:
            // Signals that the sequence has completed - there are no more events to do
            // Emitters: EventSequence::advance()
            // Receivers: Currently none, because the event sequence used for our purposes ends on an untimed event, and therefore
            //  always terminates via the "stop()" slot -- i.e. it doesn't have to tell another object that it's done, because
            //  another object is telling it to stop.
            void signalDone();
    };
}

#endif // EVENTSEQUENCE_H
