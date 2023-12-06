// FILE SequencedEvent.h
//
// A SequencedEvent represents a phase of the AED's operation that has a start (enter) and an end (exit).
// Between the start and end, the event is marked "active." The exit can either arrive automatically on
// a timer (managed by the containing EventSequence), or require some other trigger (an "untimed" event).
// SequencedEvents can be arranged into an EventSequence, which will execute them in a fixed order, starting
// each one after the previous one exits.
// SequencedEvents can send signals when they start and finish. These signals are not used to communicate directly
// with the EventSequence container. A SequencedEvent has no slots, so other objects should not try to make it do
// anything directly - they should always go through the EventSequence. Consequently, the EventSequence never needs
// the SequencedEvent to signal what's going on, because the EventSequence is controlling it so already knows.
//
// We use this for the sequence of initial startup advice prompts / pictogram lighting.

#ifndef SEQUENCEDEVENT_H
#define SEQUENCEDEVENT_H

#include <QObject>
#include <QTimer>
#include <QDebug>

namespace event
{
    class SequencedEvent : public QObject
    {
        // Heap memory allocations by this class: none

        Q_OBJECT

        friend class EventSequence;

        public:
            explicit SequencedEvent(const QString & name, int timerInterval);
            ~SequencedEvent();

            // Basic getters
            inline int getID() const { return this->id; }
            inline const QString & getName() const { return this->name; }
            inline int getInterval() const { return this->timerInterval; }
            inline bool getActive() const { return this->active; }
            inline bool isUntimed() const { return this->untimed; }

            bool operator==(SequencedEvent & other) const;
            bool operator!=(SequencedEvent & other) const;

            // See comment in timerInterval declaration, below
            static const int NOTIMER;

        protected:

            // Stores the next serial ID to use for newly created events
            static int nextID;

            // Every event has a unique serial ID which is set automatically.
            int id;

            // Every event takes a name, which should be something the programmer can identify.
            // These names are not generally displayed to end users, unless the programmer adds additional functionality to do so.
            QString name;

            // For timed events, the timerInterval is the number of milliseconds after enter() that exit() should occur
            // If the timer is a negative number, the event is considered untimed and will not exit unless something else triggers
            // the exit() or abort() functions.
            // The static constant SequencedEvent::NOTIMER has a value of -1, so can be used as way to more clearly signal the intention
            // to create an untimed event.
            int timerInterval;

            bool active;        // Is the event currently active?
            bool untimed;       // Is the event untimed?

            // Note: These are not slots. The event belongs to a container (EventSequence), which invokes these directly.
            virtual void enter();
            virtual void exit();
            virtual void abort();

        signals:

            // Inform connected clients that this event has started (includes a pointer for callback)
            // Emitter: SequencedEvent::start()
            // Receiver: none currently
            void started(SequencedEvent *);

            // Inform connected clients that this event has completed normally (includes a pointer for callback)
            // Emitter: SequencedEvent::stop()
            // Receiver: none currently
            void complete(SequencedEvent *);

            // Inform connected clients that this event has aborted abnormally (been skipped by the EventSequence) (includes a pointer for callback)
            // Emitter: SequencedEvent::abort()
            // Receiver: none currently
            void skipped(SequencedEvent *);

            // Inform connected clients that this event has stopped for any reason (abort or normal completion) (includes a pointer for callback)
            // Emitter: SequencedEvent::abort(), ::exit()
            // Receiver: none currently
            void over(SequencedEvent *);

    };
}

#endif // SEQUENCEDEVENT_H
