#ifndef __BLEMOUSE_H__
#define __BLEMOUSE_H__

#include "ble/BLE.h"
#include "ble/GattAttribute.h"

#define BLE_UUID_DESCRIPTOR_REPORT_REFERENCE 0x2908

enum ButtonState
{
    BUTTON_UP,
    BUTTON_DOWN
};

enum MouseButton
{
    MOUSE_BUTTON_LEFT = 0x1,
    MOUSE_BUTTON_RIGHT = 0x2,
    MOUSE_BUTTON_MIDDLE = 0x4,
};

#define INPUT_REPORT 0x1
#define OUTPUT_REPORT 0x2
#define FEATURE_REPORT 0x3

#define BOOT_PROTOCOL 0x0
#define REPORT_PROTOCOL 0x1

typedef struct
{
    uint8_t ID;
    uint8_t type;
} report_reference_t;

/** 
 * A class to communicate a BLE Mouse device
 */
class BluetoothMouseService
{
  public:
    /**
     * Constructor
     * @param dev BLE device
     */
    BluetoothMouseService(BLEDevice *device);

    /**
     * Set X, Y, wheel speed of the mouse. Parameters are sticky and will be
     * transmitted on every tick. Users should therefore reset them to 0 when
     * the device is immobile.
     *
     * @param x Speed on horizontal axis [-127, 127]
     * @param y Speed on vertical axis [-127, 127]
     * @param wheel Scroll speed [-127, 127]
     */
    void setSpeed(int8_t x, int8_t y, int8_t wheel);

    /**
     * Toggle the state of one button
     */
    void setButton(MouseButton button, ButtonState state);

  private:
    BLEDevice &ble;
    bool connected;

    Ticker reportTicker;
    bool reportTickerIsActive;

    uint8_t protocolMode;
    uint8_t controlPointCommand;
    uint8_t inputReportData[4];

    uint8_t buttonsState;
    uint8_t speed[3];

    void onConnection(const Gap::ConnectionCallbackParams_t *params);
    void onDisconnection(const Gap::DisconnectionCallbackParams_t *params);

    GattCharacteristic *protocolModeCharacteristic;
    GattCharacteristic *inputReportCharacteristic;
    GattCharacteristic *reportMapCharacteristic;
    GattCharacteristic *hidInformationCharacteristic;
    GattCharacteristic *hidControlPointCharacteristic;

    GattAttribute *inputReportReferenceDescriptor;
    GattAttribute *inputReportDescriptors[1];

    void startReportTicker();

    void stopReportTicker();

    void onDataSent(unsigned count);

    void sendCallback();

    void startAdvertise();

    void startService();
};

#endif /* __BLEMOUSE_H__ */
