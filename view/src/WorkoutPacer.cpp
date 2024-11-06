#include "WorkoutPacer.h"

#include <spdlog/spdlog.h>

WorkoutPacer::WorkoutPacer(
    const std::shared_ptr<Model> &model
): model(model) {
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
}
