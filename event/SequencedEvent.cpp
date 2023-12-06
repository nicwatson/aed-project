#include "SequencedEvent.h"

using namespace event;

// Provides a way to clearly indicate one's intention to make an untimed event (any negative timerInterval means untimed event)
const int SequencedEvent::NOTIMER = -1;

// Initialize unique serial ID tracker for new SequencedEvents
int SequencedEvent::nextID = 0;

SequencedEvent::SequencedEvent(const QString & in_name, int in_timerInterval)
    : id(nextID++), name(in_name), timerInterval(in_timerInterval), active(false), untimed(in_timerInterval < 0)
{
    qDebug().noquote() << "[CONSTRUCTOR] SequencedEvent(\"" << in_name << "\", " << QString::number(in_timerInterval) << ")" << Qt::endl;
}

SequencedEvent::~SequencedEvent()
{
    // No deletions required
    qDebug().noquote() << "[DESTRUCTOR] ~SequencedEvent()" << Qt::endl;
}



void SequencedEvent::enter()
{
    qDebug().noquote() << "[ENTRY/EXIT] SequencedEvent::enter(): "
                       << "id: " << QString::number(id) << " |  "
                       << "name: \"" << name << "\" |  "
                       << "timerInterval: " << QString::number(timerInterval) << " | "
                       << "active: " << active << Qt::endl;

    if(active) return;
    active = true;
    emit started(this);
}

void SequencedEvent::exit()
{
    qDebug().noquote() << "[ENTRY/EXIT] SequencedEvent::exit(): "
                       << "id: " << QString::number(id) << " |  "
                       << "name: \"" << name << "\" |  "
                       << "timerInterval: " << QString::number(timerInterval) << " | "
                       << "active: " << active << Qt::endl;

    active = false;
    emit complete(this);
    emit over(this);
}

void SequencedEvent::abort()
{
    qDebug().noquote() << "[ENTRY/EXIT] SequencedEvent::abort(): "
                       << "id: " << QString::number(id) << " |  "
                       << "name: \"" << name << "\" |  "
                       << "timerInterval: " << QString::number(timerInterval) << " | "
                       << "active: " << active << Qt::endl;

    if(!active) return;
    active = false;
    emit skipped(this);
    emit over(this);
}

bool SequencedEvent::operator==(SequencedEvent & other) const
{
    return this->getID() == other.getID();
}

bool SequencedEvent::operator!=(SequencedEvent & other) const
{
    return !((*this) == other);
}
