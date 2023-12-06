// FILE CompressionsToggleButton.h
//
// Container class for the button to start/stop active CPR compressions
// Conveniently wraps associated operations like checking/unchecking and enabling/disabling

#ifndef COMPRESSIONSTOGGLEBUTTON_H
#define COMPRESSIONSTOGGLEBUTTON_H

#include <QPushButton>

namespace aedGui
{
    class CompressionsToggleButton : public QPushButton
    {
        Q_OBJECT

        public:
            explicit CompressionsToggleButton(QWidget * parent);
            ~CompressionsToggleButton();

        public slots:

            // Disables the ability to use the button, and unchecks it
            // Trigger 1: signal ModuleCPRHelp::signalCPRComplete() : connection in MainWindow::buildModuleConnections()
            // Trigger 2: signal ModuleCPRHelp::signalCPRAborted() : connection in MainWindow::buildModuleConnections()
            void disableCPR();

            // Enables the ability to use the button, and unchecks it
            // Trigger: signal ModuleCPRHelp::signalCPRStarted() : connection in MainWindow::buildModuleConnections()
            void enableCPR();

            // Custom addition to the QPushButton's setChecked(bool) slot; changes the text on the button itself
            // Trigger: this->QPushButton::toggled(bool) : connection in CompressionsToggleButton constructor
            void setCheckedCustom(bool);

        signals:

            // Signal that the button has been toggled on (true) or off (false)
            // Emitter: CompressionsToggleButton::setCheckedCustom(bool)
            // Receiver: ModuleCPRHelp, with connections in MainWindow::buildModuleConnections(), as follows:
            //      slot ModuleCPRHelp::toggleCompressions() to notify CPR module that compressions have started or stopped
            //      slot ModuleCPRHelp::updateCompressionDepth(int) with value from ui->cprDepthSlider
            //      slot ModuleCPRHelp::updateCompressionRate(int) with value from ui->cprRateSlider
            void signalButtonToggleCompressions(bool);
    };
}

#endif // COMPRESSIONSTOGGLEBUTTON_H
