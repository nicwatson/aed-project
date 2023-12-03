#include "SequencedEvent.h"

using namespace event;

const int SequencedEvent::NOTIMER = -1;

int SequencedEvent::nextID = 0;

SequencedEvent::SequencedEvent(const QString & in_name, int in_timerInterval)
    : id(nextID++), name(in_name), timerInterval(in_timerInterval), active(false), untimed(in_timerInterval < 0)
{
    qDebug() << "Constructor: SequencedEvent " << QString::number(id) << " - ensure destructor is called later." << Qt::endl;
}

SequencedEvent::~SequencedEvent()
{
    qDebug() << "Destructor: SequencedEvent " << QString::number(id) << Qt::endl;
}


void SequencedEvent::exit()
{
    active = false;
    emit complete(this);
    emit over(this);
}

void SequencedEvent::enter()
{
    qDebug() << "Entering sequencedevent";
    if(active) return;
    active = true;
    emit started(this);
}

void SequencedEvent::abort()
{
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

QDebug & operator<<(QDebug & out, SequencedEvent & event)
{
    return out.noquote()
               << "SequencedEvent ("
               << event.getID()
               << "): "
               << event.getName()
               << "  :  "
               << (event.getActive() ? "ACTIVE" : "INACTIVE")
               << "  :  "
               <<   (event.isUntimed() ?
                        "Untimed" :
                        (
                            "Timed: "
                            + QString::number(event.getInterval())
                        )
                    )
               << Qt::endl;
}
