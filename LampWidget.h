#ifndef LAMPWIDGET_H
#define LAMPWIDGET_H

#include <QWidget>
#include <QFrame>
#include <QTimer>

#define FLASH_TIME 200

class LampWidget : public QFrame
{
    Q_OBJECT

    public:

        // Use this constructor to get a LampWidget with blank style sheets. A LampWidget added in Qt Designer
        // will use this by default. Then use setStyleUnlit() and setStyleLit() to set style sheets.
        explicit LampWidget(QWidget * parent);

        // Use this constructor to make a LampWidget, passing in style sheet strings for the unlit and lit states.
        explicit LampWidget(QWidget * parent, const QString & styleUnlit, const QString & styleLit);

        // Basic getters/setters
        inline bool getLit() const { return lit; }
        inline const QString & getStyleUnlit() const { return styleUnlit; }
        inline const QString & getStyleLit() const { return styleLit; }
        inline void setStyleUnlit(const QString & str) { styleUnlit = str; }
        inline void setStyleLit(const QString & str) { styleLit = str; }

    protected:

        // Subclasses can override to add additional behaviour (e.g. emitting a signal when lit)
        // Other objects should not call this directly.
        // Either call public functions turnOn() / turnOff() directly, or some signal from another object to the
        // turnOn() / turnOff() slots.
        virtual void setLit(bool);

        bool lit;
        bool flashing;
        QString styleUnlit;
        QString styleLit;

        QTimer flashTimer;
        QTimer flashTimerOff;
        QTimer synchronizingTimer;


    protected slots:
        void flashTimerExpired();

    public slots:

        // Turn on or off the light (swaps style sheets)
        virtual void turnOn();
        virtual void turnOff();
        virtual void startFlash();
        virtual void stopFlash();

};

#endif // LAMPWIDGET_H
