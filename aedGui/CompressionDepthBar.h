#ifndef COMPRESSIONDEPTHBAR_H
#define COMPRESSIONDEPTHBAR_H

#include <QProgressBar>

namespace aedGui
{
  class CompressionDepthBar : public QProgressBar 
  {
    Q_OBJECT

    public:
        using QProgressBar::QProgressBar; // Inherit constructors

    protected:
        void paintEvent(QPaintEvent * event) override;
  };
}

#endif // COMPRESSIONDEPTHBAR_H