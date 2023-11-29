#include "LCDDisplay.h"

using namespace aedGui;

LCDDisplay::LCDDisplay(QLabel* graph)
    : graph(graph)
{
//    QString currentDir = QDir::currentPath();
    QDir parentDir = QDir::current();
    parentDir.cdUp();
    QString pictureDirPath = parentDir.path() + "/comp3004-team14";
    ventTachyPic = new QPixmap(pictureDirPath + QString("/v_tachy.png"), nullptr, Qt::AutoColor);
    ventFibPic = new QPixmap(pictureDirPath + QString("/v_fib.png"), nullptr, Qt::AutoColor);
    nonShockablePic = new QPixmap(pictureDirPath + QString("/non_shockable.png"), nullptr, Qt::AutoColor);
}

LCDDisplay::~LCDDisplay()
{
    if (graph != nullptr) delete graph;
    if (ventTachyPic != nullptr) delete ventTachyPic;
    if (ventFibPic != nullptr) delete ventFibPic;
    if (nonShockablePic != nullptr) delete nonShockablePic;
}

void LCDDisplay::setFibPic()
{
    graph->setPixmap(ventFibPic->scaled(graph->width(), graph->height(), Qt::KeepAspectRatio));
}

void LCDDisplay::setTachyPic()
{
    graph->setPixmap(ventTachyPic->scaled(graph->width(), graph->height(), Qt::KeepAspectRatio));
}

void LCDDisplay::setNonShockablePic()
{
    graph->setPixmap(nonShockablePic->scaled(graph->width(), graph->height(), Qt::KeepAspectRatio));
}

void LCDDisplay::setNoPic()
{
    graph->clear();
}
