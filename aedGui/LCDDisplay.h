#ifndef LCDDISPLAY_H
#define LCDDISPLAY_H

#include <QObject>
#include <QLabel>
#include <QProgressBar>
#include <QPixmap>
#include <QDir>
#include <QCoreApplication>
#include <QDebug>

namespace aedGui
{
    class LCDDisplay : public QObject
    {
        Q_OBJECT

        public:
            explicit LCDDisplay(QLabel* graph);
            ~LCDDisplay();

        private:
            QLabel* graph;
            QPixmap* ventTachyPic;
            QPixmap* ventFibPic;
            QPixmap* nonShockablePic;


        public slots:
            void setTachyPic();
            void setFibPic();
            void setNonShockablePic();
            void setNoPic();
    };
}

#endif // LCDDISPLAY_H
