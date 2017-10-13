namespace bluetooth {
    /**
     * Starts the Mouse service over Bluetooth and registers it as the Mouse transport.
     */
    //% blockId="bluetooth_start_mouse"
    //% block="bluetooth start mouse service"
    //% parts="bluetooth"
    export function startMouseService() {
        bluetooth.setMouseSpeed(0, 0, 0);
    }

    /**
     * Sets a Mouse speed
     */
    //% blockId="bluetooth_mouse_set_speed"
    //% block="mouse|set speed to x: %x, y: %y, and wheel: %wheel"
    //% parts="bluetooth"
    //% shim=bluetooth::mouseSpeed
    //% advanced=true
    export function setMouseSpeed(x: number, y: number, wheel: number) {
        return;
    }

    /**
     * Sets a Mouse button
     */
    //% blockId="bluetooth_mouse_set_speed"
    //% block="mouse|set button %button to %state"
    //% parts="bluetooth"
    //% shim=bluetooth::mouseButton
    //% advanced=true
    export function setMouseButton(button: MouseButton, state: ButtonState) {
        return;
    }

    /**
     * Gets the button
     */
    //% weight=2 blockGap=8
    //% blockId="button" block="%button"
    //% advanced=true
    export function button(button: MouseButton): MouseButton {
        return button;
    }

    /**
     * Gets the button state
     */
    //% weight=2 blockGap=8
    //% blockId="buttonState" block="%buttonState"
    //% advanced=true
    export function buttonState(buttonState: ButtonState): ButtonState {
        return buttonState;
    }
}
