from PySide6.QtCore import Qt
from PySide6.QtWidgets import QGridLayout, QWidget

from hud.configuration.config import Config
from hud.model import LEGACY_BIKE_TRAINER
from hud.model.model import Model
from hud.view import DeviceController
from hud.view.app_state import AppState
from hud.view.primitives.draggable_window import AppWindow
from hud.view.primitives.labels import ButtonLabel
from hud.view.primitives.theme_switch import with_switchable_theme
from hud.view.device.select_device_panel import SelectDevicePanel


@with_switchable_theme
class TrainerWindow(AppWindow):
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

        self.trainer_panel = SelectDevicePanel(
            normal_icon_path=self.app_config.asset("train.png"),
            highlighted_icon_path=self.app_config.asset("train_high.png"),
            ble_service_type=LEGACY_BIKE_TRAINER,
            model=model,
            controller=controller,
            app_config=self.app_config,
            parent=self,
        )
        self.trainer_panel.bind_to_model(model.trainer_notifications)

        theme = self.app_config.hud_layout.theme
        self.nextLabel = ButtonLabel(text="Next", theme=theme)
        self.nextLabel.clicked.connect(lambda: self.next.emit(AppState.WAITING_FOR_SENSORS))

        self.layout = QGridLayout()
        self.centralWidget = QWidget(self)
        self.centralWidget.setLayout(self.layout)
        self.setCentralWidget(self.centralWidget)

        self.layout.addWidget(self.trainer_panel, 0, 0)
        self.layout.addWidget(self.nextLabel, 1, 0, 1, 1, Qt.AlignmentFlag.AlignCenter)
