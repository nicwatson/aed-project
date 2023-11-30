#include "CompressionDepthBar.h"
#include <QPainter>

using namespace aedGui;

void CompressionDepthBar::paintEvent(QPaintEvent * event) {
    QPainter painter(this);
    QTransform transform;

    // Rotate 90 degrees around the center of the widget
    transform.translate(width() / 2, height() / 2);
    transform.rotate(90);
    transform.translate(-height() / 2, -width() / 2);

    painter.setTransform(transform);

    // Call the base class paintEvent to draw the progress bar
    QProgressBar::paintEvent(event);
}
