// FILE CPRHelpChild.h
//
// This CPR Help "strategy" defines the behaviour of the CPR help module for child patients (i.e.
// with child pads plugged in). As per the Admin Guide spec, RealCPR Help is not available on the AED Plus's
// child pads. Therefore, this strategy accepts signals to its slots but basically does nothing beyond what
// is already defined in CPRHelpStrategy

#ifndef CPRHELPCHILD_H
#define CPRHELPCHILD_H

#include "CPRHelpStrategy.h"

namespace aedModel
{
    class CPRHelpChild : public CPRHelpStrategy
    {
        Q_OBJECT

        public:
            explicit CPRHelpChild();
            ~CPRHelpChild();

        public slots:
            // Update compression depth (overrides abstract)
            // For a child patient, this will do nothing
            // Trigger: No signals currently; direct call from ModuleCPRHelp::updateCompressionDepth(int) which is itself a slot with signal triggers
            virtual void updateCompressionDepth(int depth) override;

            // Update compression rate (overrides abstract)
            // For a child patient, this will do nothing
            // Trigger: No signals currently; direct call from ModuleCPRHelp::updateCompressionDepth(int) which is itself a slot with signal triggers
            virtual void updateCompressionRate(int cpm) override;

            // User starts doing compressions
            // Trigger: No signals currently; direct call from ModuleCPRHelp::stopCompressions() which is itself a slot with signal triggers
            virtual void startCompressions() override;

            // User stops doing compressions
            // Trigger: No signals currently; direct call from ModuleCPRHelp::stopCompressions() which is itself a slot with signal triggers
            virtual void stopCompressions() override;
    };
}

#endif // CPRHELPCHILD_H
