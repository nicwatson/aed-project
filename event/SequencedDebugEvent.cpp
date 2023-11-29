#include <QDebug>

#include "SequencedDebugEvent.h"


using namespace event;

SequencedDebugEvent::SequencedDebugEvent(const QString & name, int timerInterval) : SequencedEvent(name, timerInterval)
{

}

void SequencedDebugEvent::start()
{
    this->SequencedEvent::start();
    qDebug() << "STARTING EVENT: \n" << (*this);
}

void SequencedDebugEvent::skip()
{
    qDebug() << "ABORTING EVENT: \n" << (*this);
    this->SequencedEvent::skip();
}

void SequencedDebugEvent::exit()
{
    qDebug() << "EXITING EVENT: \n" << (*this);
    this->SequencedEvent::exit();
}

/*
QDebug & operator<<(QDebug & out, SequencedDebugEvent & event)
{
    return out << "SequencedEvent ("
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
                            + QString::number(event.getRemainingTime())
                            + "/"
                            + QString::number(event.getInterval())
                        )
                    )
                 << Qt::endl;
}
*/
