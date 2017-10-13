#include "pxt.h"
#include "BluetoothMouseService.h"
using namespace pxt;
/**
 * A set of functions to send mouse commands over Bluetooth
 */
namespace bluetooth
{
BluetoothMouseService *pMouseInstance = nullptr;
BluetoothMouseService *getMouse()
{
    if (pMouseInstance == nullptr)
    {
        pMouseInstance = new BluetoothMouseService(uBit.ble);
    }
    return pMouseInstance;
}

//%
void mouseSpeed(int x, int y, int wheel)
{
    BluetoothMouseService *pMouse = getMouse();
    pMouse->setSpeed(x, y, wheel);
}

//%
void mouseButton(MouseButton button, ButtonState state)
{
    BluetoothMouseService *pMouse = getMouse();
    pMouse->setButton(button, state);
}
}
