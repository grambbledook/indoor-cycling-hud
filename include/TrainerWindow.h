#pragma once
#include "AppWindow.h"
#include "ControllerHandler.h"

class TrainerWindow final : public AppWindow {
    Q_OBJECT

public:
    explicit TrainerWindow(const std::shared_ptr<ControllerHandler> &handler, QWidget *parent = nullptr);

    void next() override;
};
