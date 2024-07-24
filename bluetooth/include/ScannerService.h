#pragma once

#include "Model.h"
#include "Scanner.h"

class ScanneService {
public:
    ScanneService(const std::shared_ptr<Model> &model, Scanner scanner);

    void startScan(const std::function<void(const std::shared_ptr<Device> &device)> &receiver);

    void startScan();

    void stopScan() const;

private:
    std::shared_ptr<Model> model;
    Scanner scanner;
};
