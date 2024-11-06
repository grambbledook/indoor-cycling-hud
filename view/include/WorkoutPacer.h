#ifndef WORKOUTPACER_H
#define WORKOUTPACER_H
#include <memory>
#include <qtimer.h>

#include "Model.h"
#include "Reconnector.h"


class WorkoutPacer final : public QObject {
    Q_OBJECT

public:
    explicit WorkoutPacer(
        const std::shared_ptr<Model> &model
    );

    auto start() -> void;

    auto stop() -> void;

    auto tick() const -> void;

private:
    std::shared_ptr<Model> model;
    QTimer timer;
};


#endif //WORKOUTPACER_H
