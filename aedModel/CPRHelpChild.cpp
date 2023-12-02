#include "CPRHelpChild.h"

// Nothing to do here except provide empty implementations for the abstract virtual functions

using namespace aedModel;

CPRHelpChild::CPRHelpChild() : CPRHelpStrategy(CPR_TIME_CHILD)
{

}

// RealCPR Help does not monitor CPR for child patients
void CPRHelpChild::updateCompressionDepth(int depth) { }

// RealCPR Help does not monitor CPR for child patients
void CPRHelpChild::updateCompressionRate(int cpm) { }

void CPRHelpChild::startCompressions() { emit signalCompressionsStarted(); }
void CPRHelpChild::stopCompressions() { emit signalCompressionsStopped(); }
