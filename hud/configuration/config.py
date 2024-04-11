from dataclasses import dataclass, field
from os import path
from typing import Optional, Tuple


@dataclass
class Theme:
    color_scheme_stylesheet: str
    scale_stylesheet: str
    background_colour: Tuple[int, int, int]
    button_height: int = 28

    @property
    def stylesheet(self) -> str:
        return self.color_scheme_stylesheet + self.scale_stylesheet


@dataclass
class HudLayout:
    theme: Theme
    show_buttons: bool = True


SCALE_MEDIUM = """
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
        ValueLabel#small {
            font-size: 16px;
        }        
        ValueLabel#medium {
            font-size: 24px;
        }        
        ValueLabel#large {
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
        QFrame#MetricsPanel {
            padding: 2px 1px -10px 1px;
            border-radius: 10px; 
        }
        ClickableLabel {
            padding: 7px 17px 7px 17px;
            border-radius: 10px; 
        }
   """

DARK = Theme(
    color_scheme_stylesheet="""
        QListWidget {
            background-color: transparent;
            color: white;
            font-weight: bold;
            font-size: 16px;
            text-align: center;
        }
        QFrame#MetricsPanel {
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
        ButtonLabel#with_borders {
            border: 2px solid white;             
        }
        ClickableLabel {
            border: 1px solid white; 
        }
    """,
    scale_stylesheet=SCALE_MEDIUM,
    background_colour=(0, 0, 0),
)

BRIGHT = Theme(
    color_scheme_stylesheet="""
        QListWidget {
            background-color: transparent;
            color: black;
            font-weight: bold;
            font-size: 20px;
            text-align: center;
        }
        QFrame#MetricsPanel {
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
        ButtonLabel#with_borders {
            border: 2px solid black;
        }
        ClickableLabel {
            border: 1px solid black; 
        }
    """,
    scale_stylesheet=SCALE_MEDIUM,
    background_colour=(255, 255, 255),
)


@dataclass
class Config:
    connect_on_start: bool = False
    hud_layout: HudLayout = field(default_factory=lambda: HudLayout(theme=DARK, show_buttons=True))
    app_dir: str = ""
    config_file: str = "config.yaml"
    log_file: Optional[str] = None
    assets_directory: str = "assets"

    def asset(self, name: str) -> str:
        return path.join(self.assets_directory, name)
