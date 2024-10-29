#include <QPainter>
#include "AppWindow.h"
#include <spdlog/spdlog.h>
#include "Constants.h"

AppWindow::AppWindow(const std::shared_ptr<ControllerHandler> &handler, QWidget *parent) : QMainWindow(parent),
    m_drag(false), m_DragPosition(QPoint(0, 0)), controllerHandler(handler) {
    setWindowFlags(Qt::WindowType::WindowStaysOnTopHint | Qt::WindowType::FramelessWindowHint);
    setAttribute(Qt::WidgetAttribute::WA_TranslucentBackground, true);
    setObjectName(Constants::Classes::APP_WINDOW);
}

AppWindow::~AppWindow() = default;

auto AppWindow::mousePressEvent(QMouseEvent *event) -> void {
    if (event->button() != Qt::MouseButton::LeftButton) {
        return;
    }
    m_drag = true;
    m_DragPosition = event->globalPosition() - this->pos();
    event->accept();
}

auto AppWindow::mouseMoveEvent(QMouseEvent *event) -> void {
    if (event->buttons() != Qt::MouseButton::LeftButton || !m_drag) {
        return;
    }
    const auto newPosition = event->globalPosition() - m_DragPosition;
    this->move(newPosition.toPoint());
    event->accept();
}

auto AppWindow::mouseReleaseEvent(QMouseEvent *event) -> void {
    m_drag = false;
    update();
}

auto AppWindow::back() -> void {
    spdlog::info("AppWindow::back");
}

auto AppWindow::next() -> void {
    spdlog::info("AppWindow::next");
}

auto AppWindow::event(QEvent *event) -> bool {
    const auto eventType = event->type();

    if (eventType < QEvent::User or eventType > QEvent::MaxUser) {
        return QMainWindow::event(event);
    }

    if (eventHandlers.contains(eventType)) {
        eventHandlers[eventType](event);
        return true;
    }

    return QMainWindow::event(event);
}
