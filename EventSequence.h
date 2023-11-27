#ifndef EVENTSEQUENCE_H
#define EVENTSEQUENCE_H

#include <vector>
#include <QObject>
#include <QDebug>

#include "SequencedEvent.h"

namespace aed
{
    class EventSequence : public QObject
    {
        // Heap allocations by this class: TBD


        Q_OBJECT

        public:
            explicit EventSequence();
            ~EventSequence();

            inline int getPosition() const { return this->position; }
            inline int getActive() const { return this->active; }
            inline int getLength() const { return this->queue.size(); }

            SequencedEvent * getCurrent() const;
            SequencedEvent * getNext() const;
            SequencedEvent * getAt(int i) const;

            virtual int add(SequencedEvent * newEvent);
            virtual int addAfter(SequencedEvent * newEvent, int index);
            virtual int addBefore(SequencedEvent * newEvent, int index);
            virtual int remove(SequencedEvent * target);
            virtual int remove(int index);

            bool startFrom(int index);
            bool startFromBeginning();
            bool startFromHere();
            bool proceed();


        protected:
            QList<SequencedEvent *> queue;
            int position;
            bool active;

            QTimer timer;

            bool validateIndex(int index) const;

        private:

            void connectTo(SequencedEvent * event);
            void disconnectFrom(SequencedEvent * event);

        public slots:
            void start();
            void stop();

        protected slots:
            void advance();

        signals:
            void signalDone();
    };
}

#endif // EVENTSEQUENCE_H
