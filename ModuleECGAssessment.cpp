#include "ModuleECGAssessment.h"

using namespace aed;

ModuleECGAssessment::ModuleECGAssessment() : active(false), rhythm(VENT_FIB)
{
    timer = new QTimer(this);
}

ModuleECGAssessment::~ModuleECGAssessment()
{
    if (timer != nullptr) { delete timer; }
}


void ModuleECGAssessment::startAssessment() {
    active = true;
    // Start the 5-second timer to simulate analysis. Once timer runs out, the appropriate function is called, depending on which rhythm is shockable.
    timer->setInterval(5000);
    timer->setSingleShot(true);
    if (rhythm == VENT_FIB) {
        // Cause a vent fib graph to be plotted to the gui
        connect(timer, &QTimer::timeout, this, &ModuleECGAssessment::sendShockableSignal);
    } else if (rhythm == VENT_TACHY) {
        // Cause a vent tachy graph to be plotted to the gui
         connect(timer, &QTimer::timeout, this, &ModuleECGAssessment::sendShockableSignal);
    } else {
        // Cause a nonshock graph to be plotted to the gui
        connect(timer, &QTimer::timeout, this, &ModuleECGAssessment::sendNonShockableSignal);
    }


}

void ModuleECGAssessment::endAssessment() {
    active = false;
    if (rhythm == VENT_FIB || rhythm == VENT_TACHY) {
        disconnect(timer, &QTimer::timeout, this, &ModuleECGAssessment::sendShockableSignal);
    } else {
        disconnect(timer, &QTimer::timeout, this, &ModuleECGAssessment::sendNonShockableSignal);
    }
    // Cause the current graph to be removed from the gui

}

void ModuleECGAssessment::sendShockableSignal() {
    emit signalShockable();
}

void ModuleECGAssessment::sendNonShockableSignal() {
    emit signalNotShockable();
}
