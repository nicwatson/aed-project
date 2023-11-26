#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "PictogramAdviceEvent.h"
#include <QKeyEvent>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    numKeysDown = 0;

    QString lit_style = "background: rgb(248,228,92);";
    QString unlit_style = "background: rgb(30,30,30);";
    ui->lamp_CheckResp->setStyleUnlit(unlit_style);
    ui->lamp_CheckResp->setStyleLit(lit_style);
    ui->lamp_CheckResp->turnOff();
    ui->lamp_CallHelp->setStyleUnlit(unlit_style);
    ui->lamp_CallHelp->setStyleLit(lit_style);
    ui->lamp_CallHelp->turnOff();
    ui->lamp_AttPads->setStyleUnlit(unlit_style);
    ui->lamp_AttPads->setStyleLit(lit_style);
    ui->lamp_AttPads->turnOff();

    repaint();

    startupSequence = new aed::ModuleStartupAdvice(ui->lamp_CheckResp, ui->lamp_CallHelp, ui->lamp_AttPads);

    connect(this, SIGNAL(startSequence()), startupSequence, SLOT(start()));
    connect(ui->padsButton, SIGNAL(pressed()), startupSequence, SLOT(stop()));

    startupSequence->startFromBeginning();


}

MainWindow::~MainWindow()
{

    delete ui;
}

void MainWindow::quitProgram()
{
    QApplication::quit();
}


void MainWindow::keyPressEvent(QKeyEvent * e)
{
    /*if(e->text().contains(acceptedKeys))
    {
        e->accept();
        ++numKeysDown;
        qDebug() << "Accepted key pressed: " << e->text() << " : Now " << QString::number(numKeysDown) << " keys pressed." << Qt::endl;
    }*/
}

void MainWindow::keyReleaseEvent(QKeyEvent * e)
{
    /*if(e->text().contains(acceptedKeys))
    {
        e->accept();
        --numKeysDown;
        qDebug() << "Accepted key released: " << e->text() << " : Now " << QString::number(numKeysDown) << " keys pressed." << Qt::endl;
    }*/
}
