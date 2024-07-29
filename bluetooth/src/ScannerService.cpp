#include "ScannerService.h"

#include <utility>

ScannerService::ScannerService(const std::shared_ptr<Model> &model, Scanner scanner): model(model),
    scanner(std::move(scanner)) {
}

void ScannerService::startScan(const std::function<void(const std::shared_ptr<Device> &device)> &receiver) {
    scanner.startScan(receiver);
}

void ScannerService::startScan() {
    scanner.startScan(std::bind(&Model::addDevice, model, std::placeholders::_1));
}

void ScannerService::stopScan() const {
    scanner.stopScan();
}
