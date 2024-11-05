//
// Created by denis on 11/5/2024.
//

#ifndef CONFIG_H
#define CONFIG_H
#include <iostream>
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <qcoreapplication.h>
#include <QDir>
#include <string>
#include <vector>

constexpr auto filename = "config.yaml";

struct Services {
    std::optional<std::string> hrm;
    std::optional<std::string> power;
    std::optional<std::string> cadence;
    std::optional<std::string> speed;
};

struct Workout {
    std::vector<std::string> data_screens;
};

struct WorkoutSettings {
    Services services;
    Workout workout;
};

inline auto loadWorkoutSettings() -> WorkoutSettings {
    const auto exeDir = QCoreApplication::applicationDirPath().toStdString();
    const auto absolutePath = exeDir + "/" + filename;

    WorkoutSettings config = {};

    if (!std::filesystem::exists(absolutePath)) {
        spdlog::info("  Config file not found: {}", absolutePath);
        return config;
    }

    YAML::Node yamlConfig = YAML::LoadFile(absolutePath);

    config.services.hrm = yamlConfig["services"] && yamlConfig["services"]["hrm"]
                              ? yamlConfig["services"]["hrm"].as<std::string>()
                              : std::optional<std::string>();

    config.services.power = yamlConfig["services"] && yamlConfig["services"]["power"]
                                ? yamlConfig["services"]["power"].as<std::string>()
                                : std::optional<std::string>();

    config.services.cadence = yamlConfig["services"] && yamlConfig["services"]["cadence"]
                                  ? yamlConfig["services"]["cadence"].as<std::string>()
                                  : std::optional<std::string>();

    config.services.speed = yamlConfig["services"] && yamlConfig["services"]["speed"]
                                ? yamlConfig["services"]["speed"].as<std::string>()
                                : std::optional<std::string>();

    if (yamlConfig["workout"] && yamlConfig["workout"]["data_screens"]) {
        for (const auto &screen: yamlConfig["workout"]["data_screens"]) {
            config.workout.data_screens.push_back(screen.as<std::string>());
        }
    }

    return config;
}

inline auto saveWorkoutSettings(const WorkoutSettings &settings) -> void {
    YAML::Emitter out;

    out << YAML::BeginMap;

    out << YAML::Key << "services" << YAML::Value << YAML::BeginMap;
    if (settings.services.hrm) {
        out << YAML::Key << "hrm" << YAML::Value << settings.services.hrm.value();
    }

    if (settings.services.power) {
        out << YAML::Key << "power" << YAML::Value << settings.services.power.value();
    }

    if (settings.services.cadence) {
        out << YAML::Key << "cadence" << YAML::Value << settings.services.cadence.value();
    }

    if (settings.services.speed) {
        out << YAML::Key << "speed" << YAML::Value << settings.services.speed.value();
    }
    out << YAML::EndMap;

    out << YAML::Key << "workout" << YAML::Value << YAML::BeginMap;
    out << YAML::Key << "data_screens" << YAML::Value << YAML::BeginSeq;
    for (const auto &screen: settings.workout.data_screens) {
        out << screen;
    }
    out << YAML::EndSeq;
    out << YAML::EndMap;

    out << YAML::EndMap;

    const auto exeDir = QCoreApplication::applicationDirPath().toStdString();
    const auto absolutePath = exeDir + "/" + filename;

    std::ofstream fout(absolutePath);
    fout << out.c_str();
}

#endif //CONFIG_H
