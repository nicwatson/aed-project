#include "EventSequence.h"

using namespace event;

EventSequence::EventSequence() : QObject(), position(0), active(false)
{
   connect(&timer, SIGNAL(timeout()), this, SLOT(advance()));
}

EventSequence::~EventSequence()
{

}

SequencedEvent * EventSequence::getCurrent() const
{
    if(active && validateIndex(position))
    {
        return queue[position];
    }
    return nullptr;
}

SequencedEvent * EventSequence::getAt(int i) const
{
    if(validateIndex(i))
    {
        return queue[i];
    }
    return nullptr;
}

int EventSequence::add(SequencedEvent * newEvent)
{
    if(!active && newEvent != nullptr)
    {
        queue.append(newEvent);
    }
    return queue.size();
}

int EventSequence::addAfter(SequencedEvent * newEvent, int index)
{
    return addBefore(newEvent, index+1);
}

int EventSequence::addBefore(SequencedEvent * newEvent, int index)
{
    if(!active && newEvent != nullptr)
    {
        queue.insert(index, newEvent);
    }
    return queue.size();
}

int EventSequence::remove(SequencedEvent * target)
{
    if(!active && target != nullptr)
    {
        queue.removeAll(target);
    }
    return queue.size();
}

int EventSequence::remove(int index)
{
    if(validateIndex(index) && !active)
    {
        //disconnectFrom(queue[index]);
        queue.removeAt(index);

    }
    return queue.size();
}

bool EventSequence::startFrom(int index)
{
     qDebug() << "Starting from " << QString::number(index) << " with active: " << (active ? "true " : "false ") << " and index " << (validateIndex(index) ? "valid" : "invalid") << Qt::endl;
    if(active || !validateIndex(index)) return false;
    position = index;
    return startFromHere();
}

bool EventSequence::startFromHere()
{
    if(active || !validateIndex(position)) return false;
    active = true;
    proceed();
    return true;
}

bool EventSequence::proceed()
{
    if(!active || !validateIndex(position)) return false;
    qDebug() << "In proceed function for EventSequence";
    if(!queue[position]->isUntimed())
    {
        timer.setSingleShot(true);
        timer.start(queue[position]->getInterval());
    }
    queue[position]->enter();
    return true;
}

bool EventSequence::startFromBeginning()
{
    // qDebug() << "Starting sequence" << Qt::endl;
    qDebug() << "Event sequence starting from beginning";
    return startFrom(0);
}


bool EventSequence::validateIndex(int index) const
{
    return index >= 0 && index < queue.size();
}

// SLOT
void EventSequence::start()
{
    qDebug() << "Event sequence starting";
    if(!active) startFromBeginning();
}

// SLOT
void EventSequence::stop()
{
    if(active)
    {
        queue[position]->abort();
        timer.blockSignals(true);
        timer.stop();
        timer.blockSignals(false);
        active = false;
    }
}

SequencedEvent * EventSequence::getNext() const
{
    if(!validateIndex(position+1))
    {
        return nullptr;
    }
    return queue[position];
}

// SLOT
void EventSequence::advance()
{
    //qDebug() << "Advancing" << Qt::endl;
    if(!active) return;
    queue[position]->exit();
    if(getNext() != nullptr)
    {
        //qDebug() << "Next stage found" << Qt::endl;
        ++position;
        proceed();
    }
}

