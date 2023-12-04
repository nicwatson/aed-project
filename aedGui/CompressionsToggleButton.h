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

        public slots:
            void disableCPR();
            void enableCPR();
            void setCheckedCustom(bool);

        signals:
            void signalButtonToggleCompressions(bool);
    };
}

#endif // COMPRESSIONSTOGGLEBUTTON_H
