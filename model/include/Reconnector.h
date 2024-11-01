//
// Created by denis on 11/1/2024.
//

#ifndef KEEPALIVESERVICE_H
#define KEEPALIVESERVICE_H
#include <memory>

#include "Model.h"
#include "NotificationService.h"
#include "Service.h"


constexpr unsigned long long RECONNECTION_DELAY_MS = 5000;

class Reconnector {
public:
    explicit Reconnector(
        const std::shared_ptr<Model> &model,
        const std::shared_ptr<HrmNotificationService> &hrmService,
        const std::shared_ptr<CyclingCadenceAndSpeedNotificationService> &cscService,
        const std::shared_ptr<PowerNotificationService> &pwrService
    );

    auto registerDevice(const std::shared_ptr<Device> &device) -> void;

    auto tok() -> void;

    auto handleReconnect(const std::shared_ptr<Device> &device) -> void;

private:
    std::mutex mutex;
    std::shared_ptr<Model> model;

    std::unordered_map<std::string, std::shared_ptr<Device>> devices;
    std::unordered_map<std::string, unsigned long long> reconnects;
    std::shared_ptr<Model> model_;


    std::shared_ptr<HrmNotificationService> hrmService;
    std::shared_ptr<CyclingCadenceAndSpeedNotificationService> cscService;
    std::shared_ptr<PowerNotificationService> pwrService;
};


#endif //KEEPALIVESERVICE_H
