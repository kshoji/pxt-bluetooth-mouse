bluetooth.onBluetoothConnected(() => {
    basic.showString("C")
})

bluetooth.onBluetoothDisconnected(() => {
    basic.showString("D")
})

bluetooth.startMouseService();
basic.showString("S")

input.onPinPressed(TouchPin.P0, () => {
    bluetooth.setMouseButton(MouseButton.MOUSE_BUTTON_LEFT, ButtonState.BUTTON_DOWN)
})
input.onPinReleased(TouchPin.P0, () => {
    bluetooth.setMouseButton(MouseButton.MOUSE_BUTTON_LEFT, ButtonState.BUTTON_UP)
})

input.onPinPressed(TouchPin.P1, () => {
    bluetooth.setMouseButton(MouseButton.MOUSE_BUTTON_RIGHT, ButtonState.BUTTON_DOWN)
})
input.onPinReleased(TouchPin.P1, () => {
    bluetooth.setMouseButton(MouseButton.MOUSE_BUTTON_RIGHT, ButtonState.BUTTON_UP)
})

basic.forever(() => {
    bluetooth.setMouseSpeed(input.acceleration(Dimension.X) / 8, input.acceleration(Dimension.Y) / 8, 0)
})
