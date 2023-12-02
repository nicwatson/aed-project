#include "ModuleCPRHelp.h"
#include "CPRHelpAdult.h"
#include "CPRHelpChild.h"

using namespace aedModel;

ModuleCPRHelp::ModuleCPRHelp() : QObject(), strategy(nullptr), numStrategies(2), active(false)
{
    strategyList = new CPRHelpStrategy * [numStrategies];

    strategyList[CPR_ADULT] = new CPRHelpAdult();
    connect(strategyList[CPR_ADULT], SIGNAL(signalUserPrompt(const QString &)), this, SLOT(forwardUserPrompt(const QString &)));
    connect(strategyList[CPR_ADULT], SIGNAL(signalCPRCompressionRatePrompt(const QString &)), this, SLOT(forwardCPRCompressionRatePrompt(const QString &)));
    connect(strategyList[CPR_ADULT], SIGNAL(signalDisplayCompressionDepth(int)), this, SLOT(forwardCompressionDepth(int)));
    connect(strategyList[CPR_ADULT], SIGNAL(signalCompressionsStarted()), this, SLOT(compressionsStarted()));
    connect(strategyList[CPR_ADULT], SIGNAL(signalCompressionsStopped()), this, SLOT(compressionsStopped()));
    connect(strategyList[CPR_ADULT], SIGNAL(signalCPREnded()), this, SLOT(cprCompleted()));
    strategyList[CPR_ADULT]->blockSignals(true);

    strategyList[CPR_CHILD] = new CPRHelpChild();
    connect(strategyList[CPR_CHILD], SIGNAL(signalUserPrompt(const QString &)), this, SLOT(forwardUserPrompt(const QString &)));
    connect(strategyList[CPR_CHILD], SIGNAL(signalCPRCompressionRatePrompt(const QString &)), this, SLOT(forwardCPRCompressionRatePrompt(const QString &)));
    connect(strategyList[CPR_CHILD], SIGNAL(signalDisplayCompressionDepth(int)), this, SLOT(forwardCompressionDepth(int)));
    connect(strategyList[CPR_CHILD], SIGNAL(signalCompressionsStarted()), this, SLOT(compressionsStarted()));
    connect(strategyList[CPR_CHILD], SIGNAL(signalCompressionsStopped()), this, SLOT(compressionsStopped()));
    connect(strategyList[CPR_CHILD], SIGNAL(signalCPREnded()), this, SLOT(cprCompleted()));
    strategyList[CPR_CHILD]->blockSignals(true);
}

ModuleCPRHelp::~ModuleCPRHelp()
{
    disconnect(strategyList[CPR_ADULT], SIGNAL(signalUserPrompt(const QString &)), this, SLOT(forwardUserPrompt(const QString &)));
    disconnect(strategyList[CPR_ADULT], SIGNAL(signalCPRCompressionRatePrompt(const QString &)), this, SLOT(forwardCPRCompressionRatePrompt(const QString &)));
    disconnect(strategyList[CPR_ADULT], SIGNAL(signalDisplayCompressionDepth(int)), this, SLOT(forwardCompressionDepth(int)));
    disconnect(strategyList[CPR_ADULT], SIGNAL(signalCompressionsStarted()), this, SLOT(compressionsStarted()));
    disconnect(strategyList[CPR_ADULT], SIGNAL(signalCompressionsStopped()), this, SLOT(compressionsStopped()));
    disconnect(strategyList[CPR_ADULT], SIGNAL(signalCPREnded()), this, SLOT(cprCompleted()));
    disconnect(strategyList[CPR_CHILD], SIGNAL(signalUserPrompt(const QString &)), this, SLOT(forwardUserPrompt(const QString &)));
    disconnect(strategyList[CPR_CHILD], SIGNAL(signalCPRCompressionRatePrompt(const QString &)), this, SLOT(forwardCPRCompressionRatePrompt(const QString &)));
    disconnect(strategyList[CPR_ADULT], SIGNAL(signalCompressionsStarted()), this, SLOT(compressionsStarted()));
    disconnect(strategyList[CPR_ADULT], SIGNAL(signalCompressionsStopped()), this, SLOT(compressionsStopped()));
    disconnect(strategyList[CPR_CHILD], SIGNAL(signalDisplayCompressionDepth(int)), this, SLOT(forwardCompressionDepth(int)));
    disconnect(strategyList[CPR_CHILD], SIGNAL(signalCPREnded()), this, SLOT(cprCompleted()));

    for(int i = 0; i < numStrategies; i++)
    {
        if(strategyList[i] != nullptr)
        {
            delete strategyList[i];
            strategyList[i] = nullptr;
        }
        else
        {
            qDebug() << "Warning: ModuleCPRHelp destructor tried to delete an element of strategyList that was already missing!" << Qt::endl;
        }
    }

    delete [] strategyList;
}

void ModuleCPRHelp::setStrategy(CPRHelpStrategy *strat)
{
    this->strategy = strat;
}

void ModuleCPRHelp::start(AED::cableState_t padType)
{
    if(active) return;
    if(padType == AED::UNPLUGGED) return;

    if(padType == AED::PAD_ADULT)
    {
       setStrategy(strategyList[CPR_ADULT]);
    }
    else if(padType == AED::PAD_CHILD)
    {
        setStrategy(strategyList[CPR_CHILD]);
    }

    strategy->blockSignals(false);
    strategy->start();
}

void ModuleCPRHelp::reset()
{
    strategy->reset();
    active = false;
    this->strategy->blockSignals(true);
    this->strategy = nullptr;
}

void ModuleCPRHelp::cprCompleted()
{
    reset();
    emit signalCPRComplete();
}

void ModuleCPRHelp::abort()
{
    strategy->abort();
    reset();
    emit signalCPRAborted();
}

void ModuleCPRHelp::forwardUserPrompt(const QString & prompt)
{
    emit signalUserPrompt(prompt);
}

void ModuleCPRHelp::forwardCPRCompressionRatePrompt(const QString & prompt)
{
    emit signalCPRCompressionRatePrompt(prompt);
}

void ModuleCPRHelp::forwardCompressionDepth(int depth)
{
    emit signalDisplayCompressionDepth(depth);
}

void ModuleCPRHelp::updateCompressionDepth(int depth)
{
    if(!active || strategy == nullptr) return;
    strategy->updateCompressionDepth(depth);
}

void ModuleCPRHelp::updateCompressionRate(int cpm)
{
    if(!active || strategy == nullptr) return;
    strategy->updateCompressionRate(cpm);
}

void ModuleCPRHelp::toggleCompressions(bool start)
{
    if(!active || strategy == nullptr) return;
    start ? startCompressions() : stopCompressions();
}

void ModuleCPRHelp::startCompressions()
{
    strategy->startCompressions();
}

void ModuleCPRHelp::stopCompressions()
{
    strategy->stopCompressions();
}

void ModuleCPRHelp::compressionsStarted()
{
    emit signalCompressionsStarted();
}

void ModuleCPRHelp::compressionsStopped()
{
    emit signalCompressionsStopped();
}

