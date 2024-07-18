#include "Data.h"
#include <memory>
#include <any>
#include <vector>
#include <functional>

using namespace winrt::Windows::Devices::Bluetooth;

class BleClient {
public:
    explicit BleClient(Device device);

    void connect();

    bool subscribe(const UUID &characteristicUuid, const std::function<void(std::vector<uint8_t>)> &receiver) const;

    static void onCharacteristicValueChanged(GenericAttributeProfile::GattCharacteristic const &sender,
                                             GenericAttributeProfile::GattValueChangedEventArgs const &args);

    void disconnect() const;

    bool isConnected() const;

    void read(UUID serviceUuid, UUID characteristicUuid);

private:
    void fetchCharacteristics();

private:
    Device device;
    std::shared_ptr<BluetoothLEDevice> connection;
    std::unordered_map<UUID, std::any> characteristics;
};
