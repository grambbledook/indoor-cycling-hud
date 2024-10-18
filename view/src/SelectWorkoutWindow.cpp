#include "SelectWorkoutWindow.h"

#include <Constants.h>
#include "StyleSheets.h"

#include <spdlog/spdlog.h>
#include <Labels.h>

#include <QGridLayout>
#include <QListWidget>

#include "BluetoothConstants.h"
#include "Events.h"
#include "SupportedServices.h"

SelectWorkoutWindow::SelectWorkoutWindow(
    const std::shared_ptr<ControllerHandler> &handler,
    QWidget *parent
): AppWindow(handler, parent) {
    const auto listWidget = new QListWidget(this);
    listWidget->addItem("Basic workout");

    const auto nextLabel = new ButtonLabel(Constants::Buttons::START, true, this);
    connect(nextLabel, &ButtonLabel::clicked, this, &SelectWorkoutWindow::next);

    auto *layout = new QGridLayout(this);
    layout->addWidget(listWidget, 0, 0, Qt::AlignCenter);
    layout->addWidget(nextLabel, 1, 0, Qt::AlignCenter);

    const auto centralWidget = new QWidget(this);
    centralWidget->setObjectName(Constants::Classes::PANEL);
    centralWidget->setLayout(layout);

    setCentralWidget(centralWidget);
}

auto SelectWorkoutWindow::next()  -> void {
    controllerHandler->next(Constants::Screens::WORKOUT);
}
