// FILE LampWidget.h
//
// A LampWidget is a QFrame with an API for swapping its style sheets so that it can
// have "lit" and "unlit" appearances, like turning on and off a light. It can also be
// set to blink on and off at regular intervals. We use this for the indicator lights
// that accompany the advice pictograms.

#ifndef LAMPWIDGET_H
#define LAMPWIDGET_H

#include <QWidget>
#include <QFrame>
#include <QTimer>

namespace aedGui
{
    class LampWidget : public QFrame
    {
        Q_OBJECT

        public:

            // Use this constructor to get a LampWidget with blank style sheets. A LampWidget added in Qt Designer
            // will use this by default. Then use setStyleUnlit() and setStyleLit() to set style sheets.
            explicit LampWidget(QWidget * parent);

            // Use this constructor to make a LampWidget, passing in style sheet strings for the unlit and lit states.
            // Note: LampWidget created in Qt Designer will NOT use this constructor!
            explicit LampWidget(QWidget * parent, const QString & styleUnlit, const QString & styleLit, int flashInterval);

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
            // Trigger: signal flashTimer.QTimer::timeout() : connection in LampWidget constructor
            void flashTimerExpired();

        public slots:



            // Turn on the light (swaps style sheets)
            // Trigger: signal PictogramAdviceEvent::signalLampOn() : connection in PictogramAdviceEvent::bindLampWidget(LampWidget *)
            //      That signal is not actually used because all of our cases use startFlash() instead.
            virtual void turnOn();

            // Turn off the light (swaps style sheets)
            // Trigger 1: signal PictogramAdviceEvent::signalLampOff() : connection in PictogramAdviceEvent::bindLampWidget(LampWidget *)
            // Trigger 2: direct invocation from MainWindow::turnOff() (setup phase)
            virtual void turnOff();

            // Make the light start blinking
            // Trigger 1: (ui->lamp_Analyzing) : signal from AED::signalStartLampStandback() : connection in MainWindow::buildAEDConnections()
            // Trigger 2: (ui->lamp_CPR) : signal from AED::signalStartLampCPR() : connection in MainWindow::buildAEDConnections()
            // Trigger 3: signal PictogramAdviceEvent::signalLampFlash() : connection in PictogramAdviceEvent::bindLampWidget(LampWidget *)
            virtual void startFlash();

            // Make the light stop blinking (and turn off)
            // Trigger 1: (ui->lamp_Analyzing) : signal from AED::signalStopLampStandback() : connection in MainWindow::buildAEDConnections()
            // Trigger 2: (ui->lamp_CPR) : signal from AED::signalStopLampCPR() : connection in MainWindow::buildAEDConnections()
            // Trigger 3: direct invocation from LampWidget::turnOn(), LampWidget::turnOff()
            // Note: turnOn() and turnOff() both invoke stopFlash() directly, so sending two signals to stop a flashing light is unnecessary
            //      stopFlash() on its own will leave the lamp in an off state
            virtual void stopFlash();

    };
}

#endif // LAMPWIDGET_H
