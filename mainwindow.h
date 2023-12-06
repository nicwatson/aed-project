#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QString>
#include <QRegExp>

#include "aedModel/ModuleCPRHelp.h"
#include "aedModel/ModuleSelfTest.h"
#include "aedModel/ModuleShock.h"
#include "aedModel/ModuleStartupAdvice.h"
#include "aedModel/ModuleECGAssessment.h"
#include "aedModel/AED.h"

#include "aedGui/LCDDisplay.h"
#include "aedGui/CompressionsToggleButton.h"
#include "aedGui/styles.h"


// Uncomment this to run with extra debug features (ECG test buttons etc.)
// #define BUILD_DEBUG


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

    // Main AED model object
    aedModel::AED * aed;

    // AED's LCD container object
    aedGui::LCDDisplay* lcdDisplay;

    // Function-specific modules/containers for AED features
    // AED function has five basic phases:
    //  self-test, startup advice ("check responsiveness" etc.), ECG assessment, shock (if applicable), and CPR
    //  Each of these is represented by a "Module" object.
    aedModel::ModuleSelfTest* selfTestModule;
    aedModel::ModuleStartupAdvice* startupAdviceModule;
    aedModel::ModuleECGAssessment* ecgModule;
    aedModel::ModuleShock* shockModule;
    aedModel::ModuleCPRHelp* cprHelpModule;


    // Setup helper functions
    void prepareLampWidgets();
    void buildModules();
    void buildModuleConnections();
    void buildAEDConnections();

    // Extinguish all lamps etc.
    void turnoff();

    // Special helper function to set up debug features (e.g. ECG test buttons)
    void buildDebugFeatures();

    // Whether to build extra debug features - control by defining BUILD_DEBUG macro at the top of this file
    bool debug;

};
#endif // MAINWINDOW_H
