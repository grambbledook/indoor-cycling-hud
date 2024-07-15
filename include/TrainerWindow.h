#pragma once
#include "AppWindow.h"

class TrainerWindow final : public AppWindow {
    Q_OBJECT

public:
    explicit TrainerWindow(QWidget *parent = nullptr);

    void next() override;
};
