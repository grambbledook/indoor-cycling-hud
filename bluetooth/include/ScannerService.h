#pragma once

#include "Scanner.h"

class ScanneService {
public:
    void startScan(const std::function<void(const Device &device)> &receiver);
    void stopScan() const;

private:
    Scanner scanner;
};
