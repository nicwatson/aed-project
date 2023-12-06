#include "EventSequence.h"

using namespace event;

EventSequence::EventSequence() : QObject(), position(0), active(false)
{
    qDebug() << "[CONSTRUCTOR] EventSequence" << Qt::endl;
    connect(&timer, SIGNAL(timeout()), this, SLOT(advance()));
}

EventSequence::~EventSequence()
{
    qDebug() << "[Destructor] EventSequence" << Qt::endl;
    // The object that creates allocations for the pointers in the queue is responsible
    // for deleting them. This could be a subclass of EventSequence, or it could be some
    // other object, which may need to maintain a separate list of pointers so that it
    // can properly delete them all

}

SequencedEvent * EventSequence::getCurrent() const
{
    qDebug().noquote() << "[ENTRY] EventSequence::getCurrent() : current position is " << QString::number(position) << Qt::endl;
    if(active && validateIndex(position))
    {
        qDebug().noquote() << "[EXIT] EventSequence::getCurrent() : active and index is valid, returning (SequencedEvent *)" << Qt::endl;
        return queue[position];
    }

    qDebug().noquote() << "[EXIT] EventSequence::getCurrent() : inactive or invalid index, returning nullptr" << Qt::endl;
    return nullptr;
}

SequencedEvent * EventSequence::getNext() const
{
    qDebug().noquote() << "[ENTRY] EventSequence::getNext() : current position is " << QString::number(position) << Qt::endl;
    if(!validateIndex(position+1))
    {
        qDebug().noquote() << "[EXIT] EventSequence::getNext() : index+1 is invalid, returning nullptr" << Qt::endl;
        return nullptr;
    }

    qDebug().noquote() << "[EXIT] EventSequence::getNext() : index+1 is valid, returning (SequencedEvent *)" << Qt::endl;
    return queue[position];
}

SequencedEvent * EventSequence::getAt(int i) const
{
    qDebug().noquote() << "[ENTRY] EventSequence::getAt(" << QString::number(i) << ")" << Qt::endl;
    if(validateIndex(i))
    {
        qDebug().noquote() << "[EXIT] EventSequence::getAt(" << QString::number(i) << "): index valid, returning (SequencedEvent *)" << Qt::endl;
        return queue[i];
    }

    qDebug().noquote() << "[EXIT] EventSequence::getAt(" << QString::number(i) << "): invalid index, returning nullptr" << Qt::endl;
    return nullptr;
}

int EventSequence::add(SequencedEvent * newEvent)
{
    qDebug().noquote() << "[ENTRY] EventSequence::add(SequencedEvent *): size is " << QString::number(queue.size()) << Qt::endl;

    if(!active && newEvent != nullptr)
    {
        qDebug().noquote() << "[INFO] EventSequence::add(SequencedEvent *): appending newEvent to queue" << Qt::endl
                           << "\t[ id: " << QString::number(newEvent->getID()) << ", "
                           << "name: \"" << newEvent->getName() << "\", "
                           << "timeInterval: " << QString::number(newEvent->getInterval()) << " ]" << Qt::endl;

        queue.append(newEvent);
    }
    return queue.size();

    qDebug().noquote() << "[EXIT] EventSequence::add(SequencedEvent *): size is now " << QString::number(queue.size()) << Qt::endl;
}

int EventSequence::addAfter(SequencedEvent * newEvent, int index)
{
    qDebug().noquote() << "[ENTRY] EventSequence::addAfter(SequencedEvent *, index=" << QString::number(index) << ")" << Qt::endl;
    return addBefore(newEvent, index+1);
    qDebug().noquote() << "[EXIT] EventSequence::addAfter(SequencedEvent *, index=" << QString::number(index) << ")" << Qt::endl;
}

int EventSequence::addBefore(SequencedEvent * newEvent, int index)
{
    qDebug().noquote() << "[ENTRY] EventSequence::addBefore(SequencedEvent *, index=" << QString::number(index) << ")" << Qt::endl
                       << "\t Size is " << QString::number(queue.size()) << Qt::endl;

    if(!active && newEvent != nullptr)
    {
        qDebug().noquote() << "[INFO] EventSequence::add(SequencedEvent *): inserting newEvent in queue" << Qt::endl
                           << "\t[ id: " << QString::number(newEvent->getID()) << ", "
                           << "name: \"" << newEvent->getName() << "\", "
                           << "timeInterval: " << QString::number(newEvent->getInterval()) << " ]" << Qt::endl;
        queue.insert(index, newEvent);
    }

    qDebug().noquote() << "[EXIT] EventSequence::addBefore(SequencedEvent *, index=" << QString::number(index) << ")" << Qt::endl
                       << "\t Size is now " << QString::number(queue.size()) << Qt::endl;
    return queue.size();
}

int EventSequence::remove(SequencedEvent * target)
{
    qDebug().noquote() << "[ENTRY] EventSequence::remove(SequencedEvent *): size is " << QString::number(queue.size()) << Qt::endl;

    if(!active && target != nullptr)
    {
        qDebug().noquote() << "[INFO] EventSequence::remove(SequencedEvent *): removing target from queue" << Qt::endl
                           << "\t[ id: " << QString::number(target->getID()) << ", "
                           << "name: \"" << target->getName() << "\", "
                           << "timeInterval: " << QString::number(target->getInterval()) << " ]" << Qt::endl;
        queue.removeAll(target);
    }

    qDebug().noquote() << "[ENTRY] EventSequence::remove(SequencedEvent *): size is now " << QString::number(queue.size()) << Qt::endl;
    return queue.size();
}

int EventSequence::remove(int index)
{
    qDebug().noquote() << "[ENTRY] EventSequence::remove(" << QString::number(index) << "): size is "
                       << QString::number(queue.size()) << Qt::endl;

    if(validateIndex(index) && !active)
    {
        qDebug().noquote() << "[INFO] EventSequence::remove(" << QString::number(index) << "): removing item from queue" << Qt::endl;
        queue.removeAt(index);

    }

    qDebug().noquote() << "[EXIT] EventSequence::remove(" << QString::number(index) << "): size is now "
                       << QString::number(queue.size()) << Qt::endl;
    return queue.size();
}

bool EventSequence::startFrom(int index)
{
    qDebug().noquote() << "[ENTRY] EventSequence::startFrom(" << QString::number(index) << ")" << Qt::endl
                       << "\tIf no [EXIT] then event sequence is inactive or index is invalid" << Qt::endl;

    if(active || !validateIndex(index)) return false;

    position = index;

    qDebug().noquote() << "[EXIT->RETURN] EventSequence::startFrom(" << QString::number(index) << ")" << Qt::endl;

    return startFromHere();
}

bool EventSequence::startFromBeginning()
{
    qDebug() << "[ENTRY/EXIT] EventSequence::startFromBeginning" << Qt::endl;
    return startFrom(0);
}

bool EventSequence::startFromHere()
{
    qDebug().noquote() << "[ENTRY] EventSequence::startFromHere() : current position is " << QString::number(position) << Qt::endl
                       << "\tIf no [EXIT] then event sequence is already active or index is invalid" << Qt::endl;

    if(active || !validateIndex(position)) return false;
    active = true;
    proceed();

    qDebug().noquote() << "[EXIT] EventSequence::startFromHere()" << Qt::endl;
    return true;
}

bool EventSequence::proceed()
{
    qDebug().noquote() << "[ENTRY] EventSequence::proceed() : current position is " << QString::number(position) << Qt::endl
                       << "\tIf no [EXIT] then event sequence is inactive or index is invalid" << Qt::endl;

    if(!active || !validateIndex(position)) return false;

    if(!queue[position]->isUntimed())
    {
        qDebug() << "[INFO] Event is timed - setting and starting timer" << Qt::endl;

        timer.setSingleShot(true);
        timer.start(queue[position]->getInterval());
    }
    queue[position]->enter();

    qDebug().noquote() << "[EXIT] EventSequence::proceed()" << QString::number(position) << Qt::endl;
    return true;
}

bool EventSequence::validateIndex(int index) const
{
    return index >= 0 && index < queue.size();
}

// SLOT
void EventSequence::start()
{
    qDebug() << "[ENTRY SLOT] EventSequence::start()" << Qt::endl;
    if(!active) startFromBeginning();
    qDebug() << "[EXIT SLOT] EventSequence::start()" << Qt::endl;
}

// SLOT
void EventSequence::stop()
{
    qDebug() << "[ENTRY SLOT] EventSequence::stop()" << Qt::endl;
    if(active)
    {
        queue[position]->abort();
        timer.blockSignals(true);
        timer.stop();
        timer.blockSignals(false);
        active = false;
    }
    qDebug() << "[EXIT SLOT] EventSequence::stop()" << Qt::endl;
}

// SLOT
void EventSequence::advance()
{
    qDebug() << "[ENTRY SLOT] EventSequence::advance()" << Qt::endl;
    if(!active)
    {
        qDebug() << "[EXIT - WARNING] EventSequence::advance() : event sequence wasn't active!" << Qt::endl;
        return;
    }

    queue[position]->exit();
    if(getNext() != nullptr)
    {
        qDebug() << "[INFO SLOT] EventSequence::advance() : next stage found" << Qt::endl;
        ++position;
        proceed();
    }
    qDebug() << "[EXIT SLOT] EventSequence::advance()" << Qt::endl;
}

