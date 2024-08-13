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

void AppWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::MouseButton::LeftButton) {
        m_drag = true;
        m_DragPosition = event->globalPosition() - this->pos();
        event->accept();
    }
}

void AppWindow::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() == Qt::MouseButton::LeftButton && m_drag) {
        const auto newPosition = event->globalPosition() - m_DragPosition;
        this->move(newPosition.toPoint());
        event->accept();
    }
}

void AppWindow::mouseReleaseEvent(QMouseEvent *event) {
    m_drag = false;
    update();
}

void AppWindow::back() {
    spdlog::info("AppWindow::back");
}

void AppWindow::next() {
    spdlog::info("AppWindow::next");
}

bool AppWindow::event(QEvent *event) {
    if (event->type() <= QEvent::MaxUser and event->type() >= QEvent::User) {
        if (eventHandlers.contains(event->type())) {

            eventHandlers[event->type()](event);
            return true;
        }
    }

    if (eventHandlers.contains(event->type())) {
        eventHandlers[event->type()](event);
        return true;
    }

    return QMainWindow::event(event);
}
