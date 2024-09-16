#pragma once
#include <string>

namespace Constants {
    namespace Icons {
        const std::string BIKE_TRAINER = "assets/train.png";
        const std::string BIKE_TRAINER_HOVER = "assets/train_high.png";

        const std::string CADENCE_SENSOR = "assets/cad.png";
        const std::string CADENCE_SENSOR_HOVER = "assets/cad_high.png";

        const std::string HEART_RATE_MONITOR = "assets/hrm.png";
        const std::string HEART_RATE_MONITOR_HOVER = "assets/hrm_high.png";

        const std::string POWER_SENSOR = "assets/pwr.png";
        const std::string POWER_SENSOR_HOVER = "assets/pwr_high.png";

        const std::string SPEED_SENSOR = "assets/spd.png";
        const std::string SPEED_SENSOR_HOVER = "assets/spd_high.png";
    }

    namespace Buttons {
        const std::string BACK = "Back";
        const std::string FINISH = "Finish";
        const std::string NEXT = "Next";
        const std::string OK = "Ok";
        const std::string START = "New Workout";
        const std::string EXIT = "Exit";

        const std::string LEFT = "<";
        const std::string RIGHT = ">";
    }

    namespace Labels {
        const std::string CADENCE = "Cadence";
        const std::string DURATION = "Duration";
        const std::string DISTANCE = "Distance";
        const std::string HEART_RATE = "Heart Rate";
        const std::string INSTANT_POWER = "Instant Power";
        const std::string POWER = "Power";
        const std::string SPEED = "Speed";

        const std::string MAX = "Max";
        const std::string MIN = "Min";
        const std::string AVG = "Avg";

        const std::string MAX_HEART_RATE = "Max Heart Rate";
        const std::string MIN_HEART_RATE = "Min Heart Rate";
        const std::string AVG_HEART_RATE = "Avg Heart Rate";

        const std::string MAX_SPEED = "Max Speed";
        const std::string MIN_SPEED = "Min Speed";
        const std::string AVG_SPEED = "Avg Speed";

        const std::string MAX_CADENCE = "Max Cadence";
        const std::string MIN_CADENCE = "Min Cadence";
        const std::string AVG_CADENCE = "Avg Cadence";

        const std::string MAX_POWER = "Max Power";
        const std::string MIN_POWER = "Min Power";
        const std::string AVG_POWER = "Avg Power";
    }

    namespace Values {
        const std::string EMPTY_DATA = "-";
    }

    namespace Classes {
        const std::string WITH_BORDERS = "with_borders";
        const std::string FRAME_WITH_BORDERS = "frame_with_borders";
        const std::string PANEL = "panel";

        const std::string APP_WINDOW = "AppWindow";
    }

    namespace Screens {
        const std::string TRAINER = "trainer";
        const std::string SENSORS = "sensors";
        const std::string SELECT_WORKOUT = "select_workout";
        const std::string WORKOUT = "workout";
        const std::string DEVICE_DIALOG = "device_dialog";
        const std::string WORKOUT_SUMMARY = "workout_summary";
    }

    namespace Commands {
        const auto CONNECT = "CONNECT";
        const auto SWITCH_THEME = "SWITCH_THEME";
        const auto SET_WHEEL_SIZE = "SET_WHEEL_SIZE";
        const auto SET_SPEED_UNIT = "SET_SPEED_UNIT";
        const auto QUIT = "QUIT";
    }
}
