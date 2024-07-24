#include "ScannerService.h"

#include <utility>

ScanneService::ScanneService(const std::shared_ptr<Model> &model, Scanner scanner): model(model),
    scanner(std::move(scanner)) {
}

void ScanneService::startScan(const std::function<void(const std::shared_ptr<Device> &device)> &receiver) {
    scanner.start_scan(receiver);
}

void ScanneService::startScan() {
    scanner.start_scan(std::bind(&Model::addDevice, model, std::placeholders::_1));
}

void ScanneService::stopScan() const {
    scanner.stop_scan();
}
