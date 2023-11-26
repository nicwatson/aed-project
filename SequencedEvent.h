#ifndef SEQUENCEDEVENT_H
#define SEQUENCEDEVENT_H

#include <QObject>
#include <QTimer>
#include <QDebug>

namespace aed
{
    class SequencedEvent : public QObject
    {
        // Heap memory allocations by this class: none

        Q_OBJECT


        public:
            explicit SequencedEvent(const QString & name, int timerInterval);
            ~SequencedEvent();

            inline int getID() const { return this->id; }
            inline const QString & getName() const { return this->name; }
            inline int getInterval() const { return this->timerInterval; }
            inline bool getActive() const { return this->active; }
            inline bool isUntimed() const { return this->untimed; }

            static const int NOTIMER;

            bool operator==(SequencedEvent & other) const;
            bool operator!=(SequencedEvent & other) const;

            virtual void enter();
            virtual void exit();
            virtual void abort();

        protected:
            int id;
            QString name;
            int timerInterval;

            static int nextID;
            bool active;
            bool untimed;


        signals:
            void started(SequencedEvent *);
            void complete(SequencedEvent *);
            void skipped(SequencedEvent *);
            void over(SequencedEvent *);

    };
}

QDebug & operator<<(QDebug & out, aed::SequencedEvent & event);

#endif // SEQUENCEDEVENT_H
