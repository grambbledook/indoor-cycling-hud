#pragma once

#include "Model.h"
#include "Scanner.h"

class ScannerService {
public:
    ScannerService(const std::shared_ptr<Model> &model, Scanner scanner);

    auto startScan(const std::function<void(const std::shared_ptr<Device> &device)> &receiver) -> void;

    auto startScan() -> void;

    auto stopScan() const -> void;

private:
    std::shared_ptr<Model> model;
    Scanner scanner;
};
