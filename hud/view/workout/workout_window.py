from PySide6.QtCore import Qt
from PySide6.QtWidgets import QGridLayout, QWidget

from hud.configuration.config import Config
from hud.model.model import Model
from hud.view import DeviceController
from hud.view.app_state import AppState
from hud.view.primitives.draggable_window import AppWindow
from hud.view.primitives.labels import ButtonLabel
from hud.view.primitives.theme_switch import with_switchable_theme
from hud.view.workout.metrics_panel import MetricsPanel


@with_switchable_theme
class WorkoutWindow(AppWindow):
    def __init__(
            self,
            app_config: Config,
            controller: DeviceController,
            model: Model,
            parent=None,
    ):
        super().__init__(app_config, parent)
        self.model = model
        self.controller = controller

        self.layout = None
        self.centralWidget = None

        self.heart_rate_monitor_panel = MetricsPanel(
            model=model,
            controller=controller,
            text="Heart Rate",
            app_config=self.app_config,
        )
        self.heart_rate_monitor_panel.bind_to_model(model.hrm_notifications)

        self.cadence_sensor_panel = MetricsPanel(
            model=model,
            controller=controller,
            text="Cadence",
            app_config=self.app_config,
        )
        self.cadence_sensor_panel.bind_to_model(model.cad_notifications)

        self.power_meter_panel = MetricsPanel(
            model=model,
            controller=controller,
            text="Instant Power",
            app_config=self.app_config,
        )
        self.power_meter_panel.bind_to_model(model.pwr_notifications)

        self.speed_sensor_panel = MetricsPanel(
            model=model,
            controller=controller,
            text="Speed",
            app_config=self.app_config,
        )
        self.speed_sensor_panel.bind_to_model(model.spd_notifications)

        self.finishLabel = ButtonLabel(
            text="Finish",
            theme=self.app_config.hud_layout.theme,
        )
        self.finishLabel.clicked.connect(lambda: self.next.emit(AppState.WORKOUT_FINISHED))

        self.layout = QGridLayout()
        self.centralWidget = QWidget(self)
        self.centralWidget.setLayout(self.layout)
        self.setCentralWidget(self.centralWidget)

        self.layout.addWidget(self.heart_rate_monitor_panel, 0, 0)
        self.layout.addWidget(self.cadence_sensor_panel, 1, 0)
        self.layout.addWidget(self.power_meter_panel, 2, 0)
        self.layout.addWidget(self.speed_sensor_panel, 3, 0)
        self.layout.addWidget(self.finishLabel, 4, 0, 1, 1, Qt.AlignmentFlag.AlignCenter)
