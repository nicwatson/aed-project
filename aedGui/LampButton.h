// FILE LampButton.h
//
// A LampButton is a QPushButton with an API for swapping its style sheets so that it can
// have "lit" and "unlit" appearances, like turning on and off a light. It can also be
// set to blink on and off at regular intervals. We use this for the shock button.

#ifndef LAMPBUTTON_H
#define LAMPBUTTON_H

#include <QObject>
#include <QPushButton>
#include <QTimer>


namespace aedGui
{
    class LampButton : public QPushButton
    {
        Q_OBJECT

        public:

            // Use this constructor to get a LampButton with blank style sheets. A LampButton added in Qt Designer
            // will use this by default. Then use setStyleUnlit() and setStyleLit() to set style sheets.
            explicit LampButton(QWidget * parent);

            // Use this constructor to make a LampWidget, passing in style sheet strings for the unlit and lit states.
            // Note: LampButtons created in Qt Designer will NOT use this constructor!
            explicit LampButton(QWidget * parent, const QString & styleUnlit, const QString & styleLit, int flashInterval);

            // Basic getters/setters
            inline bool getLit() const { return lit; }
            inline const QString & getStyleUnlit() const { return styleUnlit; }
            inline const QString & getStyleLit() const { return styleLit; }
            inline void setStyleUnlit(const QString & str) { styleUnlit = str; }
            inline void setStyleLit(const QString & str) { styleLit = str; }
            inline void setFlashTime(int interval) { flashInterval = interval; }

        protected:

            // Subclasses can override to add additional behaviour (e.g. emitting a signal when lit)
            // Other objects should not call this directly.
            // Either call public functions turnOn() / turnOff() directly, or some signal from another object to the
            // turnOn() / turnOff() slots.
            virtual void setLit(bool);

            bool lit;               // Is the lamp lit?
            bool flashing;          // Is the lamp in a flashing state?
            QString styleUnlit;     // Stores the stylesheet for the unlit widget
            QString styleLit;       // Stores the stylesheet for the lit widget

            int flashInterval;      // Time (ms) between flash-on/flash-off
            QTimer flashTimer;      // Timer for pacing flash


        protected slots:

            // Used to flash on/off the light at regular intervals
            // Trigger: signal flashTimer.QTimer::timeout() : connection in LampButton constructor
            void flashTimerExpired();

        public slots:

            // Turn on the light (swaps style sheets)
            // Trigger: no signals; because our lights always flash (never solid on), we use startFlash() instead
            virtual void turnOn();

            // Turn off the light (swaps style sheets)
            // Trigger: no signals; direct invocation from MainWindow::turnOff() (setup phase); other cases can call stopFlash()
            virtual void turnOff();

            // Make the light start blinking
            // Trigger (for ui->shockButton): signal from ModuleShock::signalCharged() : connection in MainWindow::buildModuleConnections()
            virtual void startFlash();

            // Make the light stop blinking (and turn off)
            // Triggers (for ui->shockButton): signals ModuleShock::signalShockDelivered(int), ModuleShock::signalAborted
            //      : connections in MainWindow::buildModuleConnections()
            virtual void stopFlash();

    };
}

#endif // LAMPBUTTON_H
