#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QMouseEvent>
#include <QPaintEvent>

#include "ControllerHandler.h"
#include "Events.h"

class AppWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit AppWindow(const std::shared_ptr<ControllerHandler> &handler, QWidget *parent = nullptr);

    ~AppWindow() override;

    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void paintEvent(QPaintEvent *event) override;

    bool event(QEvent *event) override;

public slots:
    virtual void back();

    virtual void next();

protected:
    std::shared_ptr<ControllerHandler> controllerHandler;
    std::map<int, std::function<void(QEvent *)> > eventHandlers;

private:
    bool m_drag;
    QPointF m_DragPosition;
};
