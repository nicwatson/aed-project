#include "CPRHelpChild.h"

using namespace aedModel;

CPRHelpChild::CPRHelpChild() : CPRHelpStrategy(CPR_TIME_CHILD)
{

}

// RealCPR Help does not monitor CPR for child patients
void CPRHelpChild::updateCompressionDepth(int depth) { }

// RealCPR Help does not monitor CPR for child patients
void CPRHelpChild::updateCompressionRate(int cpm) { }
