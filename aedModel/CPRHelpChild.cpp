// FILE CPRHelpChild.cpp
// Implementations for CPRHelpChild class

#include "CPRHelpChild.h"

// Nothing to do here except provide empty implementations for the abstract virtual functions

using namespace aedModel;

CPRHelpChild::CPRHelpChild() : CPRHelpStrategy(CPR_TIME_CHILD)
{
    qDebug() << "[CONSTRUCTOR] CPRHelpChild()" << Qt::endl;
}

CPRHelpChild::~CPRHelpChild()
{
    qDebug() << "[DESTRUCTOR] ~CPRHelpChild()" << Qt::endl;
    // Nothing to do
}

// RealCPR Help does not monitor CPR for child patients
void CPRHelpChild::updateCompressionDepth(int depth) { (void)depth; }   // (void)depth suppresses unused parameter warning

// RealCPR Help does not monitor CPR for child patients
void CPRHelpChild::updateCompressionRate(int cpm) { (void)cpm; }        // (void)cpm suppresses unused parameter warning

void CPRHelpChild::startCompressions()
{
    qDebug() << "[SIGNAL] Emit CPRHelpChild::signalCompressionsStarted()" << Qt::endl;
    emit signalCompressionsStarted();
}
void CPRHelpChild::stopCompressions()
{
    qDebug() << "[SIGNAL] Emit CPRHelpChild::signalCompressionsStopped()" << Qt::endl;
    emit signalCompressionsStopped();
}
