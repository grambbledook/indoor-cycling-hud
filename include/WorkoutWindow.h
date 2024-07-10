#pragma once
#include "AppWindow.h"

class WorkoutWindow final : public AppWindow {
    Q_OBJECT

public:
    explicit WorkoutWindow(QWidget *parent = nullptr);
};
