#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>

#include "AppWindow.h"

#include <iostream>

#include "Constants.h"

AppWindow::AppWindow(QWidget *parent) : QMainWindow(parent), m_drag(false), m_DragPosition(QPoint(0, 0)) {
    setWindowFlags(Qt::WindowType::WindowStaysOnTopHint | Qt::WindowType::FramelessWindowHint);
    setAttribute(Qt::WidgetAttribute::WA_TranslucentBackground);
    setObjectName(Constants::Classes::APP_WINDOW);
}

AppWindow::~AppWindow() = default;

void AppWindow::paintEvent(QPaintEvent *event) {
    const auto painter = std::make_unique<QPainter>(this);

    painter->setOpacity(0.25);
    painter->setBrush(QBrush(QColor(200, 200, 200, 128)));
    painter->drawRect(this->rect());
}


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
