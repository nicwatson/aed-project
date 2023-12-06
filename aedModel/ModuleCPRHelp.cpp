#include <QDebug>
#include "ModuleCPRHelp.h"
#include "CPRHelpAdult.h"
#include "CPRHelpChild.h"
#include "aedGui/strings.h"

using namespace aedModel;

ModuleCPRHelp::ModuleCPRHelp() : QObject(), numStrategies(2), strategy(nullptr), active(false)
{
    qDebug() << "[CONSTRUCTOR] ModuleCPRHelp() : building 2 strategies" << Qt::endl;

    // Initialize the child and adult CPR help strategy objects and connect them to the ModuleCPRHelp

    strategyList = new CPRHelpStrategy * [numStrategies];

    strategyList[CPR_ADULT] = new CPRHelpAdult();
    connect(strategyList[CPR_ADULT], SIGNAL(signalForwardUserPrompt(const QString &)), this, SLOT(forwardUserPrompt(const QString &)));
    connect(strategyList[CPR_ADULT], SIGNAL(signalCPRCompressionRatePrompt(const QString &)), this, SLOT(forwardCPRCompressionRatePrompt(const QString &)));
    connect(strategyList[CPR_ADULT], SIGNAL(signalDisplayCompressionDepth(int)), this, SLOT(forwardCompressionDepth(int)));
    //connect(strategyList[CPR_ADULT], SIGNAL(signalCompressionsStarted()), this, SLOT(compressionsStarted()));
    //connect(strategyList[CPR_ADULT], SIGNAL(signalCompressionsStopped()), this, SLOT(compressionsStopped()));
    connect(strategyList[CPR_ADULT], SIGNAL(signalCPRStrategyEnded()), this, SLOT(cprCompleted()));
    strategyList[CPR_ADULT]->blockSignals(true);

    strategyList[CPR_CHILD] = new CPRHelpChild();
    connect(strategyList[CPR_CHILD], SIGNAL(signalForwardUserPrompt(const QString &)), this, SLOT(forwardUserPrompt(const QString &)));
    connect(strategyList[CPR_CHILD], SIGNAL(signalCPRCompressionRatePrompt(const QString &)), this, SLOT(forwardCPRCompressionRatePrompt(const QString &)));
    connect(strategyList[CPR_CHILD], SIGNAL(signalDisplayCompressionDepth(int)), this, SLOT(forwardCompressionDepth(int)));
    //connect(strategyList[CPR_CHILD], SIGNAL(signalCompressionsStarted()), this, SLOT(compressionsStarted()));
    //connect(strategyList[CPR_CHILD], SIGNAL(signalCompressionsStopped()), this, SLOT(compressionsStopped()));
    connect(strategyList[CPR_CHILD], SIGNAL(signalCPRStrategyEnded()), this, SLOT(cprCompleted()));
    strategyList[CPR_CHILD]->blockSignals(true);

    qDebug() << "[EXIT CONSTRUCTOR] ModuleCPRHelp()" << Qt::endl;
}

ModuleCPRHelp::~ModuleCPRHelp()
{
    qDebug() << "[DESTRUCTOR] ~ModuleCPRHelp()" << Qt::endl;

    disconnect(strategyList[CPR_ADULT], SIGNAL(signalForwardUserPrompt(const QString &)), this, SLOT(forwardUserPrompt(const QString &)));
    disconnect(strategyList[CPR_ADULT], SIGNAL(signalCPRCompressionRatePrompt(const QString &)), this, SLOT(forwardCPRCompressionRatePrompt(const QString &)));
    disconnect(strategyList[CPR_ADULT], SIGNAL(signalDisplayCompressionDepth(int)), this, SLOT(forwardCompressionDepth(int)));
    //disconnect(strategyList[CPR_ADULT], SIGNAL(signalCompressionsStarted()), this, SLOT(compressionsStarted()));
    //disconnect(strategyList[CPR_ADULT], SIGNAL(signalCompressionsStopped()), this, SLOT(compressionsStopped()));
    disconnect(strategyList[CPR_ADULT], SIGNAL(signalCPREnded()), this, SLOT(cprCompleted()));

    disconnect(strategyList[CPR_CHILD], SIGNAL(signalForwardUserPrompt(const QString &)), this, SLOT(forwardUserPrompt(const QString &)));
    disconnect(strategyList[CPR_CHILD], SIGNAL(signalCPRCompressionRatePrompt(const QString &)), this, SLOT(forwardCPRCompressionRatePrompt(const QString &)));
    //disconnect(strategyList[CPR_ADULT], SIGNAL(signalCompressionsStarted()), this, SLOT(compressionsStarted()));
    //disconnect(strategyList[CPR_ADULT], SIGNAL(signalCompressionsStopped()), this, SLOT(compressionsStopped()));
    disconnect(strategyList[CPR_CHILD], SIGNAL(signalDisplayCompressionDepth(int)), this, SLOT(forwardCompressionDepth(int)));
    disconnect(strategyList[CPR_CHILD], SIGNAL(signalCPRStrategyEnded()), this, SLOT(cprCompleted()));

    // Safely set active strategy pointer to nullptr
    // This does not get deleted because it is a secondary pointer
    // We still have pointers to every strategy object in the strategyList[] array
    strategy = nullptr;

    // Delete strategy objects from the array
    for(int i = 0; i < numStrategies; i++)
    {
        if(strategyList[i] != nullptr)
        {
            delete strategyList[i];
            strategyList[i] = nullptr;
        }
        else
        {
            qDebug() << "[Warning] ModuleCPRHelp destructor tried to delete an element of strategyList that was already missing!" << Qt::endl;
        }
    }

    // Delete the array
    delete [] strategyList;

    qDebug() << "[EXIT DESTRUCTOR] ~ModuleCPRHelp() : Two CPRHelpStrategy destructors should have been called" << Qt::endl;
}

void ModuleCPRHelp::setStrategy(CPRHelpStrategy *strat)
{
    qDebug() << "[ENTRY] ModuleCPRHelp::setStrategy(CPRHelpStrategy *)" << Qt::endl;

    this->strategy = strat;

    qDebug() << "[EXIT] ModuleCPRHelp::setStrategy(CPRHelpStrategy *)" << Qt::endl;
}

// SLOT
void ModuleCPRHelp::start(AED::cableState_t padType)
{
    qDebug().noquote() << "[ENTRY SLOT] ModuleCPRHelp::start(" << AED::cableStateNames[padType] <<")" << Qt::endl;

    if(active)
    {
        qDebug().noquote() << "[EXIT SLOT - WARNING] ModuleCPRHelp::start(" << AED::cableStateNames[padType] <<")"
                           << ": CPR event was already active" << Qt::endl;
        return;
    }

    active = true;

    if(padType == AED::UNPLUGGED)
    {
        qDebug().noquote() << "[EXIT SLOT - WARNING] ModuleCPRHelp::start(" << AED::cableStateNames[padType] <<")"
                           << ": UNPLUGGED is an invalid cable state for CPR event" << Qt::endl;
        return;
    }

    if(padType == AED::PAD_ADULT)
    {
        setStrategy(strategyList[CPR_ADULT]);
    }
    else if(padType == AED::PAD_CHILD)
    {
        setStrategy(strategyList[CPR_CHILD]);
    }

    qDebug() << "[SIGNAL] Emit ModuleCPRHelp::signalCPRStarted()" << Qt::endl;
    emit signalCPRStarted();

    strategy->blockSignals(false);

    strategy->start();

    qDebug().noquote() << "[EXIT SLOT] ModuleCPRHelp::start(" << AED::cableStateNames[padType] <<")" << Qt::endl;
}

void ModuleCPRHelp::reset()
{
    qDebug() << "[ENTRY] ModuleCPRHelp::reset()" << Qt::endl;

    strategy->reset();
    active = false;
    strategy->blockSignals(true);
    strategy = nullptr;

    qDebug() << "[EXIT] ModuleCPRHelp::reset()" << Qt::endl;
}

// SLOT
void ModuleCPRHelp::cprCompleted()
{
    qDebug() << "[ENTRY SLOT] ModuleCPRHelp::cprCompleted()" << Qt::endl;

    reset();

    qDebug() << "[SIGNAL] Emit ModuleCPRHelp::signalCPRCompressionRatePrompt(P_BLANK)" << Qt::endl;
    emit signalCPRCompressionRatePrompt(P_BLANK);

    qDebug() << "[SIGNAL] Emit ModuleCPRHelp::signalCPRComplete()" << Qt::endl;
    emit signalCPRComplete();

    qDebug() << "[EXIT SLOT] ModuleCPRHelp::reset()" << Qt::endl;
}

// SLOT
void ModuleCPRHelp::abort()
{
    qDebug() << "[ENTRY SLOT] ModuleCPRHelp::abort()" << Qt::endl;

    if(!active)
    {
        qDebug() << "[EXIT SLOT - WARNING] ModuleCPRHelp::abort(): CPR event was not active" << Qt::endl;
        return;
    }

    if(strategy != nullptr)
    {
        strategy->abort();
    }
    reset();

    qDebug() << "[SIGNAL] Emit ModuleCPRHelp::signalCPRCompressionRatePrompt(P_BLANK)" << Qt::endl;
    emit signalCPRCompressionRatePrompt(P_BLANK);

    qDebug() << "[SIGNAL] Emit ModuleCPRHelp::signalCPRAborted()" << Qt::endl;
    emit signalCPRAborted();

    qDebug() << "[EXIT SLOT] ModuleCPRHelp::abort()" << Qt::endl;
}

// SLOT
void ModuleCPRHelp::forwardUserPrompt(const QString & prompt)
{
    qDebug() << "[ENTRY/SIGNAL/EXIT SLOT] ModuleCPRHelp::forwardUserPrompt(" << prompt << ")" << Qt::endl
             << "\t=> emit signalForwardUserPrompt(" << prompt << ")" << Qt::endl;
    emit signalForwardUserPrompt(prompt);
}

// SLOT
void ModuleCPRHelp::forwardCPRCompressionRatePrompt(const QString & prompt)
{
    qDebug() << "[ENTRY/SIGNAL/EXIT SLOT] ModuleCPRHelp::forwardCPRCompressionRatePrompt(" << prompt << ")" << Qt::endl
             << "\t=> emit signalCPRCompressionRatePrompt(" << prompt << ")" << Qt::endl;
    emit signalCPRCompressionRatePrompt(prompt);
}

// SLOT
void ModuleCPRHelp::forwardCompressionDepth(int depth)
{
    qDebug() << "[ENTRY/SIGNAL/EXIT SLOT] ModuleCPRHelp::forwardCompressionDepth(" << QString::number(depth) << ")" << Qt::endl
             << "\t=> emit forwardCompressionDepth(" << QString::number(depth) << ")" << Qt::endl;
    emit signalDisplayCompressionDepth(depth);
}

// SLOT
void ModuleCPRHelp::updateCompressionDepth(int depth)
{
    qDebug().noquote() << "[ENTRY SLOT] ModuleCPRHelp::updateCompressionDepth(" << QString::number(depth) << ")" << Qt::endl;

    if(!active || strategy == nullptr)
    {
        qDebug() << "[EXIT SLOT - WARNING] ModuleCPRHelp::updateCompressionDepth(): could not update compression depth" << Qt::endl
                 << "\t[active: " << active
                 << "]  [strategy != nullptr: " << (strategy != nullptr) << "]" << Qt::endl;
        return;
    }

    strategy->updateCompressionDepth(depth);

    qDebug().noquote() << "[EXIT SLOT] ModuleCPRHelp::updateCompressionDepth(" << QString::number(depth) << ")" << Qt::endl;
}

// SLOT
void ModuleCPRHelp::updateCompressionRate(int cpm)
{
    qDebug().noquote() << "[ENTRY SLOT] ModuleCPRHelp::updateCompressionRate(" << QString::number(cpm) << ")" << Qt::endl;

    if(!active || strategy == nullptr)
    {
        qDebug() << "[EXIT SLOT - WARNING] ModuleCPRHelp::updateCompressionRate(): could not update compression rate" << Qt::endl
                 << "\t[active: " << active
                 << "]  [strategy != nullptr: " << (strategy != nullptr) << "]" << Qt::endl;
        return;
    }
    strategy->updateCompressionRate(cpm);

    qDebug().noquote() << "[EXIT SLOT] ModuleCPRHelp::updateCompressionRate(" << QString::number(cpm) << ")" << Qt::endl;
}

// SLOT
void ModuleCPRHelp::toggleCompressions(bool start)
{
    qDebug().noquote() << "[ENTRY SLOT] ModuleCPRHelp::toggleCompressions(" << (start ? "starting" : "stopping") << ")" << Qt::endl;
    if(!active || strategy == nullptr)
    {
        qDebug() << "[EXIT SLOT - WARNING] ModuleCPRHelp::toggleCompressions(): could not toggle compressions" << Qt::endl
                 << "\t[active: " << active
                 << "]  [strategy != nullptr: " << (strategy != nullptr) << "]" << Qt::endl;
        return;
    }

    start ? startCompressions() : stopCompressions();

    qDebug().noquote() << "[EXIT SLOT] ModuleCPRHelp::toggleCompressions(" << (start ? "starting" : "stopping") << ")" << Qt::endl;
}

// SLOT
void ModuleCPRHelp::startCompressions()
{
    qDebug() << "[ENTRY SLOT] ModuleCPRHelp::startCompressions()" << Qt::endl;
    strategy->startCompressions();
    qDebug() << "[EXIT SLOT] ModuleCPRHelp::startCompressions()" << Qt::endl;
}

// SLOT
void ModuleCPRHelp::stopCompressions()
{
    qDebug() << "[ENTRY SLOT] ModuleCPRHelp::stopCompressions()" << Qt::endl;
    strategy->stopCompressions();
    qDebug() << "[EXIT SLOT] ModuleCPRHelp::stopCompressions()" << Qt::endl;
}

/*
// TODO REMOVE THIS
void ModuleCPRHelp::compressionsStarted()
{
    qDebug() << "ModuleCPRHelp::compressionsStarted()" << Qt::endl;
    emit signalCompressionsStarted();
}

void ModuleCPRHelp::compressionsStopped()
{
    qDebug() << "ModuleCPRHelp::compressionsStopped()" << Qt::endl;
    emit signalCompressionsStopped();
}
*/
