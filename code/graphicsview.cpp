#include "graphicsview.h"

GraphicsView::GraphicsView(QWidget *parent) : QGraphicsView(parent)
{

}

void GraphicsView::wheelEvent(QWheelEvent *event)
{
    if(event->modifiers() == Qt::ControlModifier) {
        double delta = event->delta();
        double sf = 1.0 + delta/600;
        QPoint p0 = mapFromScene( 0.0, 0.0 );
        if ( p0.x() - x()<10 || sf >1.0 )
        {
            scale( sf, sf );
            QPoint p (horizontalScrollBar()->sliderPosition(), verticalScrollBar()->sliderPosition());
            emit changeAll(matrix(), p);
        }
        return;
    }
    QGraphicsView::wheelEvent(event);
}

void GraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    QPoint p (horizontalScrollBar()->sliderPosition(), verticalScrollBar()->sliderPosition());
    emit changeAll(matrix(), p);
    QGraphicsView::mouseMoveEvent(event);
}

void GraphicsView::applyChangesInAll(QMatrix m, QPoint p)
{
    setMatrix(m);
    horizontalScrollBar()->setSliderPosition(p.x());
    verticalScrollBar()->setSliderPosition(p.y());
}
