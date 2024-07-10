#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QMouseEvent>
#include <QPaintEvent>

class AppWindow: public QMainWindow {
    Q_OBJECT

public:
    explicit AppWindow(QWidget *parent = nullptr);
    ~AppWindow() override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
private:
    bool m_drag;
    QPointF m_DragPosition;
};

