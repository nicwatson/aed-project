#ifndef MODULECPRHELP_H
#define MODULECPRHELP_H

#include "AED.h"
#include "CPRHelpStrategy.h"

namespace aedModel
{

    class ModuleCPRHelp
    {
        public:
            enum cprStrategy_t { CPR_ADULT, CPR_CHILD };

            ModuleCPRHelp();

            void setStrategy(CPRHelpStrategy *);

        private:
            CPRHelpStrategy * strategy;

        public slots:
            void start();
            void stop();
            void abort();
            void forwardUserPrompt(const QString & prompt);

        signals:
            void signalCPRLight(bool);
            void signalUserPrompt(const QString & prompt);
    };

}

#endif // MODULECPRHELP_H
