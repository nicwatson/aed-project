#ifndef CPRHELPCHILD_H
#define CPRHELPCHILD_H

#include "CPRHelpStrategy.h"

// Time interval (ms) that CPR should last for a child patient (happens to be the same as for an adult patient)
#define CPR_TIME_CHILD CPR_TIME_COMMON

namespace aedModel
{
    class CPRHelpChild : public CPRHelpStrategy
    {
        Q_OBJECT

        public:
            CPRHelpChild();

        public slots:
            // Update compression depth (overrides abstract)
            // For a child patient, this will do nothing
            virtual void updateCompressionDepth(int depth) override;
            // Update compression rate (overrides abstract)
            // For a child patient, this will do nothing
            virtual void updateCompressionRate(int cpm) override;
    };
}

#endif // CPRHELPCHILD_H
