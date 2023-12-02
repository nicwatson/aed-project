#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "aedModel/ModuleCPRHelp.h"
#include "aedModel/ModuleSelfTest.h"
#include "aedModel/ModuleShock.h"
#include "aedModel/ModuleStartupAdvice.h"
#include "aedModel/ModuleECGAssessment.h"
#include "aedModel/AED.h"

#include "aedGui/LCDDisplay.h"


#include <QString>
#include <QRegExp>

#define INITIAL_ADVICE_DELAY 3000

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QList<event::SequencedEvent *> eventHandles;
    aedModel::ModuleStartupAdvice * startupSequence;

    QTimer timer;
    aedModel::AED * aed;

    const QRegExp acceptedKeys = QRegExp("[asdfjkl;]");
    int numKeysDown;

    aedGui::LCDDisplay* LCDDisplay;
    aedModel::ModuleECGAssessment* ecgModule;
    aedModel::ModuleCPRHelp* cprHelpModule;
    aedModel::ModuleSelfTest* selfTestModule;
    aedModel::ModuleShock* shockModule;
    aedModel::ModuleStartupAdvice* startupAdviceModule;

private slots:
    void quitProgram();

signals:
    void startSequence();

};
#endif // MAINWINDOW_H
