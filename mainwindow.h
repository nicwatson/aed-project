#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "SequencedEvent.h"
#include "ModuleStartupAdvice.h"
#include "AED.h"

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

    QList<aed::SequencedEvent *> eventHandles;
    aed::ModuleStartupAdvice * startupSequence;

    QTimer timer;
    aed::AED * aed;

    const QRegExp acceptedKeys = QRegExp("[asdfjkl;]");
    int numKeysDown;

protected:
    void keyPressEvent(QKeyEvent * e) override;
    void keyReleaseEvent(QKeyEvent * e) override;

private slots:
    void quitProgram();

signals:
    void startSequence();

};
#endif // MAINWINDOW_H
