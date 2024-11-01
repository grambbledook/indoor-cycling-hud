#include "WorkoutPacer.h"

#include <qtconcurrentrun.h>
#include <spdlog/spdlog.h>

WorkoutPacer::WorkoutPacer(
    const std::shared_ptr<Model> &model,
    const std::shared_ptr<Reconnector> &reconnector
): model(model), reconnector(reconnector) {
    connect(&timer, &QTimer::timeout, this, &WorkoutPacer::tick);
}

auto WorkoutPacer::start() -> void {
    timer.start(500);
}

auto WorkoutPacer::stop() -> void {
    timer.stop();
}

auto WorkoutPacer::tick() const -> void {
    model->tick();

    auto reconnect = [this]() {
        reconnector->tok();
    };
    QtConcurrent::run(reconnect);
}
