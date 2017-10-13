/** 
 * A class to communicate a BLE Mouse device
 */
#include "MicroBit.h"
#include "pxt.h"
#include "ble/GapAdvertisingData.h"
#include "ble/GattService.h"
#include "ble/GattCharacteristic.h"
#include "BluetoothMouseService.h"
#include "USBHID_Types.h"

namespace
{

static const uint8_t RESPONSE_HID_INFORMATION[] = {0x11, 0x01, 0x00, 0x03};

static const uint16_t uuid16_list[] = {GattService::UUID_HUMAN_INTERFACE_DEVICE_SERVICE,
                                       GattService::UUID_DEVICE_INFORMATION_SERVICE};

/**
 * Characteristic Data(Report Map)
 */
static const uint8_t REPORT_MAP[] = {
    USAGE_PAGE(1), 0x01, // Generic Desktop
    USAGE(1), 0x02,      // Mouse
    COLLECTION(1), 0x01, // Application
    USAGE(1), 0x01,      // Pointer
    COLLECTION(1), 0x00, // Physical
    USAGE_PAGE(1), 0x09, // Buttons
    USAGE_MINIMUM(1), 0x01,
    USAGE_MAXIMUM(1), 0x03,
    LOGICAL_MINIMUM(1), 0x00,
    LOGICAL_MAXIMUM(1), 0x01,
    REPORT_COUNT(1), 0x03, // 3 bits (Buttons)
    REPORT_SIZE(1), 0x01,
    INPUT(1), 0x02,        // Data, Variable, Absolute
    REPORT_COUNT(1), 0x01, // 5 bits (Padding)
    REPORT_SIZE(1), 0x05,
    INPUT(1), 0x01,           // Constant
    USAGE_PAGE(1), 0x01,      // Generic Desktop
    USAGE(1), 0x30,           // X
    USAGE(1), 0x31,           // Y
    USAGE(1), 0x38,           // Wheel
    LOGICAL_MINIMUM(1), 0x81, // -127
    LOGICAL_MAXIMUM(1), 0x7f, // 127
    REPORT_SIZE(1), 0x08,     // Three bytes
    REPORT_COUNT(1), 0x03,
    INPUT(1), 0x06, // Data, Variable, Relative
    END_COLLECTION(0),
    END_COLLECTION(0),
};

static const uint8_t emptyInputReportData[] = {0, 0, 0, 0};
}

/**
 * Constructor
 * @param dev BLE device
 */
BluetoothMouseService::BluetoothMouseService(BLEDevice *dev) : ble(*dev)
{

    startService();
    startAdvertise();
}

void BluetoothMouseService::startService()
{
    memset(inputReportData, 0, sizeof(inputReportData));
    connected = false;
    protocolMode = REPORT_PROTOCOL;
    reportTickerIsActive = false;

    protocolModeCharacteristic = new GattCharacteristic(GattCharacteristic::UUID_PROTOCOL_MODE_CHAR,
                                                        &protocolMode, 1, 1,
                                                        GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE_WITHOUT_RESPONSE);

    inputReportCharacteristic = new GattCharacteristic(GattCharacteristic::UUID_REPORT_CHAR,
                                                       inputReportData, sizeof(inputReportData), sizeof(inputReportData),
                                                       GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ |
                                                           GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY |
                                                           GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE);

    reportMapCharacteristic = new GattCharacteristic(GattCharacteristic::UUID_REPORT_MAP_CHAR,
                                                     const_cast<uint8_t *>(REPORT_MAP), sizeof(REPORT_MAP), sizeof(REPORT_MAP),
                                                     GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ);

    hidInformationCharacteristic = new GattCharacteristic(GattCharacteristic::UUID_HID_INFORMATION_CHAR,
                                                          const_cast<uint8_t *>(RESPONSE_HID_INFORMATION), sizeof(RESPONSE_HID_INFORMATION), sizeof(RESPONSE_HID_INFORMATION),
                                                          GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ);

    hidControlPointCharacteristic = new GattCharacteristic(GattCharacteristic::UUID_HID_CONTROL_POINT_CHAR,
                                                           &controlPointCommand, 1, 1,
                                                           GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE_WITHOUT_RESPONSE);

    SecurityManager::SecurityMode_t securityMode = SecurityManager::SECURITY_MODE_ENCRYPTION_NO_MITM;
    protocolModeCharacteristic->requireSecurity(securityMode);
    inputReportCharacteristic->requireSecurity(securityMode);
    reportMapCharacteristic->requireSecurity(securityMode);
    hidInformationCharacteristic->requireSecurity(securityMode);
    hidControlPointCharacteristic->requireSecurity(securityMode);

    GattCharacteristic *mouseCharacteristics[]{
        hidInformationCharacteristic,
        reportMapCharacteristic,
        protocolModeCharacteristic,
        hidControlPointCharacteristic,
        inputReportCharacteristic};

    ble.gap().onConnection(this, &BluetoothMouseService::onConnection);
    ble.gap().onDisconnection(this, &BluetoothMouseService::onDisconnection);

    GattService mouseService(GattService::UUID_HUMAN_INTERFACE_DEVICE_SERVICE, mouseCharacteristics, sizeof(mouseCharacteristics) / sizeof(GattCharacteristic *));

    ble.gattServer().addService(mouseService);

    ble.gattServer().onDataSent(this, &BluetoothMouseService::onDataSent);
}

void BluetoothMouseService::startAdvertise()
{
    ble.gap().stopAdvertising();
    ble.gap().clearAdvertisingPayload();

    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED |
                                           GapAdvertisingData::LE_GENERAL_DISCOVERABLE);

    ManagedString BLEName("BBC micro:bit");
    ManagedString namePrefix(" [");
    ManagedString namePostfix("]");
    BLEName = BLEName + namePrefix + uBit.getName() + namePostfix;

    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME,
                                           (uint8_t *)BLEName.toCharArray(), BLEName.length());

    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS,
                                           (uint8_t *)uuid16_list, sizeof(uuid16_list));

    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::MOUSE);

    uint16_t minInterval = Gap::MSEC_TO_GAP_DURATION_UNITS(25);
    if (minInterval < 6)
    {
        minInterval = 6;
    }
    uint16_t maxInterval = minInterval * 2;
    Gap::ConnectionParams_t params = {minInterval, maxInterval, 0, 3200}; // timeout in 32 seconds
    ble.gap().setPreferredConnectionParams(&params);

    ble.gap().setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
    ble.gap().setAdvertisingInterval(50);
    ble.gap().setAdvertisingPolicyMode(Gap::ADV_POLICY_IGNORE_WHITELIST);
    ble.gap().startAdvertising();
}

void BluetoothMouseService::startReportTicker()
{
    if (reportTickerIsActive)
    {
        return;
    }
    reportTicker.attach_us(this, &BluetoothMouseService::sendCallback, 24000);
    reportTickerIsActive = true;
}

void BluetoothMouseService::stopReportTicker()
{
    if (!reportTickerIsActive)
    {
        return;
    }
    reportTicker.detach();
    reportTickerIsActive = false;
}

void BluetoothMouseService::onDataSent(unsigned count)
{
    startReportTicker();
}

void BluetoothMouseService::onConnection(const Gap::ConnectionCallbackParams_t *params)
{
    ble.gap().stopAdvertising();
    connected = true;
}

void BluetoothMouseService::onDisconnection(const Gap::DisconnectionCallbackParams_t *params)
{
    connected = false;
    stopReportTicker();
    startAdvertise();
}

/**
 * Set X, Y, wheel speed of the mouse. Parameters are sticky and will be
 * transmitted on every tick. Users should therefore reset them to 0 when
 * the device is immobile.
 *
 * @param x Speed on horizontal axis [-127, 127]
 * @param y Speed on vertical axis [-127, 127]
 * @param wheel Scroll speed [-127, 127]
 */
void BluetoothMouseService::setSpeed(int8_t x, int8_t y, int8_t wheel)
{
    speed[0] = x;
    speed[1] = y;
    speed[2] = wheel;

    startReportTicker();
}

/**
 * Toggle the state of one button
 */
void BluetoothMouseService::setButton(MouseButton button, ButtonState state)
{
    if (state == BUTTON_UP)
    {
        buttonsState &= ~(button);
    }
    else
    {
        buttonsState |= button;
    }

    startReportTicker();
}

void BluetoothMouseService::sendCallback()
{
    if (!connected)
    {
        return;
    }

    if (
        inputReportData[0] == 0 &&
        inputReportData[1] == 0 &&
        inputReportData[2] == 0 &&
        inputReportData[3] == 0 &&
        (buttonsState & 0x7) == 0 &&
        speed[0] == 0 &&
        speed[1] == 0 &&
        speed[2] == 0)
    {
        stopReportTicker();
        return;
    }

    inputReportData[0] = buttonsState & 0x7;
    inputReportData[1] = speed[0];
    inputReportData[2] = speed[1];
    inputReportData[3] = speed[2];

    ble.gattServer().write(inputReportCharacteristic->getValueHandle(), inputReportData, 4);
}
