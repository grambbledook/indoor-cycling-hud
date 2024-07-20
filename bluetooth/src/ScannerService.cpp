#include "ScannerService.h"

void ScanneService::startScan(const std::function<void(const Device &device)> &receiver) {
    scanner.start_scan(receiver);
}

void ScanneService::stopScan() const {
    scanner.stop_scan();
}
