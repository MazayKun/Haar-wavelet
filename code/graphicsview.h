#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsView>
#include <QWheelEvent>
#include <QDebug>
#include <QScrollBar>

class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    GraphicsView(QWidget *parent);

protected slots:
    void wheelEvent(QWheelEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void applyChangesInAll(QMatrix m, QPoint p);

signals:
    void changeAll(QMatrix m, QPoint p);
};

#endif // GRAPHICSVIEW_H
