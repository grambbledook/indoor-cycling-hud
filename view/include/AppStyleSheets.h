#pragma once
#include <string>

namespace StyleSheets {
    const inline std::string SCALE_MEDIUM = R"MEDIUM(
        RoundPanel {
            border-radius: 10px;
        }
        QListWidget {
            font-weight: bold;
            font-size: 20px;
            text-align: center;
        }
        TextLabel {
            font: bold center "Hurmit Nerd Font Mono";
            font-size: 10px;
            text-align: center;
        }
        TextLabel#small {
            font-size: 16px;
        }
        .small {
            font-size: 16px;
        }
        TextLabel#medium {
            font-size: 24px;
        }
       .medium {
            font-size: 24px;
        }
        TextLabel#large {
            font-size: 30px;
        }
        .large {
            font-size: 30px;
        }
        ButtonLabel:hover {
            color: red;
            text-decoration: underline;
        }
        ButtonLabel {
            font-size: 16px;
            font-weight: bold;
        }
        ButtonLabel#with_borders {
            padding: 3px 15px 1px 14px;
            border-radius: 7px;
        }
        QFrame#frame_with_borders {
            padding: 2px 1px -10px 1px;
            border-radius: 10px;
        }
        ClickableLabel {
            padding: 7px 17px 7px 17px;
            border-radius: 10px;
        }
        QTableWidget {
            font-weight: bold;
            font-size: 16px;
            text-align: center;
        }
)MEDIUM";

    const inline std::string THEME_DARK = R"DARK(
        RoundPanel {
            background-color: rgba(0, 0, 0, 0.8);
        }
        QWidget#panel {
            background-color: rgba(0, 0, 0, 0.8);
        }
        QListWidget {
            background-color: rgba(0, 0, 0, 0.8);
            color: white;
        }
        QFrame#frame_with_borders {
            border: 1px solid white;
        }
        QListWidget::item {
            background-color: transparent;
        }
        QListWidget::item:selected {
            background-color: #808080;
        }
        QLabel {
            color: white;
        }
        TextLabel[mode="dimmed"] {
            color: gray;
        }
        ButtonLabel#with_borders {
            border: 2px solid white;
        }
        ClickableLabel {
            border: 1px solid white;
        }
        QTableWidget {
            background-color: rgba(0, 0, 0, 0.8);
            color: white;
        }
    )DARK";

    const inline std::string THEME_BRIGHT = R"BRIGHT(
        QWidget#panel {
            background-color: rgba(255, 255, 255, 0.8);
        }
        QListWidget {
            background-color: rgba(255, 255, 255, 0.8);
            color: black;
        }
        QFrame#frame_with_borders {
            border: 1px solid black;
        }
        QListWidget::item {
            background-color: transparent;
        }
        QListWidget::item:selected {
            background-color: #808080;
        }
        QLabel {
            color: black;
        }
        TextLabel[mode="dimmed"] {
            color: gray;
        }
        ButtonLabel#with_borders {
            border: 2px solid black;
        }
        ClickableLabel {
            border: 1px solid black;
        }
        QTableWidget {
            background-color: rgba(255, 255, 255, 0.8);
            color: black;
        }
    )BRIGHT";
}
