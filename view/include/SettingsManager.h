#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H
#include <memory>

#include "NotificationService.h"


class SettingsManager {
public:
    explicit SettingsManager(
        const std::shared_ptr<Model> &model,
        const std::shared_ptr<EventBus> &eventbus,
        const std::shared_ptr<HrmNotificationService> &hrmNotificationService,
        const std::shared_ptr<CyclingCadenceAndSpeedNotificationService> &cscNotificationService,
        const std::shared_ptr<PowerNotificationService> &powerNotificationService
    )
        : model(model),
          eventbus(eventbus),
          hrmNotificationService(hrmNotificationService),
          cscNotificationService(cscNotificationService),
          powerNotificationService(powerNotificationService) {
    }

    auto initialise() const -> void;

    auto store() const -> void;

private:
    std::shared_ptr<Model> model;
    std::shared_ptr<EventBus> eventbus;
    std::shared_ptr<HrmNotificationService> hrmNotificationService;
    std::shared_ptr<CyclingCadenceAndSpeedNotificationService> cscNotificationService;
    std::shared_ptr<PowerNotificationService> powerNotificationService;
};


#endif //SETTINGSMANAGER_H
