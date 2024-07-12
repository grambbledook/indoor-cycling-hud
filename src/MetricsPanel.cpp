#include "SelectDevicePanel.h"
#include "StyleSheets.h"
#include "WorkoutWindow.h"
#include "MetricsPanel.h"
#include <Labels.h>

#include <QGridLayout>

#include "Constants.h"

MetricsPanel::MetricsPanel(std::string text, QWidget *parent): QWidget(parent), text(std::move(text)) {
    auto const panel_layout = new QVBoxLayout(this);
    auto const button_layout = new QHBoxLayout(this);
    auto const value_layout = new QGridLayout(this);

    auto const left_button = new ButtonLabel(Constants::Buttons::LEFT, false, this);
    auto const right_button = new ButtonLabel(Constants::Buttons::RIGHT, false, this);

    auto const label = new TextLabel(this->text, LabelSize::SMALL, this);
    auto const value_label = new ValueLabel("0", LabelSize::LARGE, this);
    auto const spacer = new QSpacerItem(20, 10, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

    value_layout->addWidget(label, 0, 0, 1, 1, Qt::AlignmentFlag::AlignCenter);
    value_layout->addItem(spacer, 1, 0, 1, 1, Qt::AlignmentFlag::AlignCenter);
    value_layout->addWidget(value_label, 2, 0, 1, 1, Qt::AlignmentFlag::AlignCenter);

    button_layout->addWidget(left_button, Qt::AlignmentFlag::AlignLeft);
    button_layout->addLayout(value_layout, Qt::AlignmentFlag::AlignCenter);
    button_layout->addWidget(right_button, Qt::AlignmentFlag::AlignRight);

    panel_layout->addLayout(button_layout);
    auto const panel = new QFrame(this);
    panel->setLayout(panel_layout);
    panel->setObjectName(Constants::Classes::FRAME_WITH_BORDERS);

    auto const main_layout = new QVBoxLayout(this);
    main_layout->addWidget(panel);
    setLayout(main_layout);

    setStyleSheet((StyleSheets::THEME_BRIGHT + StyleSheets::SCALE_MEDIUM).data());
}
