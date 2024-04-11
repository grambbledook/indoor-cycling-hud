from PySide6.QtCore import Qt
from PySide6.QtWidgets import QGridLayout, QWidget

from hud.configuration.config import Config
from hud.view.app_state import AppState
from hud.view.primitives.draggable_window import AppWindow
from hud.view.primitives.labels import ButtonLabel
from hud.view.primitives.theme_switch import with_switchable_theme


@with_switchable_theme
class WorkoutStatisticsWindow(AppWindow):
    def __init__(self, app_config: Config, parent=None):
        super().__init__(app_config, parent)

        theme = self.app_config.hud_layout.theme

        self.newWorkoutLabel = ButtonLabel(text="New", theme=theme)
        self.newWorkoutLabel.clicked.connect(lambda: self.next.emit(AppState.IN_WORKOUT))

        self.exitLabel = ButtonLabel(text="Exit", theme=theme)
        self.exitLabel.clicked.connect(lambda: self.next.emit(AppState.EXITING))

        self.layout = QGridLayout()
        self.centralWidget = QWidget(self)
        self.centralWidget.setLayout(self.layout)
        self.setCentralWidget(self.centralWidget)

        self.layout.addWidget(self.newWorkoutLabel, 0, 0, Qt.AlignmentFlag.AlignCenter)
        self.layout.addWidget(self.exitLabel, 0, 1, Qt.AlignmentFlag.AlignCenter)
