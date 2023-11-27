#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "SequencedEvent.h"
#include "EventSequence.h"
#include "LCDDisplay.h"
#include "ModuleECGAssessment.h"

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

    QList<aed::SequencedEvent *> eventHandles;
    aed::EventSequence * seq;
    LCDDisplay* lcdDisplay;
    aed::ModuleECGAssessment* ecgModule;

    QTimer timer;

private slots:
    void quitProgram();

signals:
    void startSequence();

};
#endif // MAINWINDOW_H
