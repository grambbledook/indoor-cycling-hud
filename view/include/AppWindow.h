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

    auto mousePressEvent(QMouseEvent *event) -> void override;

    auto mouseMoveEvent(QMouseEvent *event) -> void override;

    auto mouseReleaseEvent(QMouseEvent *event) -> void override;

    auto event(QEvent *event) -> bool override;

public slots:
    virtual auto back() -> void;

    virtual auto next() -> void;

protected:
    std::shared_ptr<ControllerHandler> controllerHandler;
    std::map<int, std::function<void(QEvent *)> > eventHandlers;

private:
    bool m_drag;
    QPointF m_DragPosition;
};
