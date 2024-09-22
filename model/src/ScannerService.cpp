#include "ScannerService.h"

#include <utility>

ScannerService::ScannerService(const std::shared_ptr<Model> &model, Scanner scanner): model(model),
    scanner(std::move(scanner)) {
}

auto ScannerService::startScan(const std::function<void(const std::shared_ptr<Device> &device)> &receiver)-> void{
    scanner.startScan(receiver);
}

auto ScannerService::startScan()-> void {
    scanner.startScan(std::bind(&Model::addDevice, model, std::placeholders::_1));
}

auto ScannerService::stopScan() const-> void {
    scanner.stopScan();
}
