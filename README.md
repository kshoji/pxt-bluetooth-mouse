# bluetooth-mouse

This PXT package allows the micro:bit to act as a Mouse peripheral.

## Usage

Place a ``||bluetooth start mouse service||`` block in your program to enable Bluetooth LE Mouse.
With this block, the `micro:bit` starts advertise BLE packets as a Mouse peripheral.

```blocks
bluetooth.startMouseService();
```

For example, hold left mouse button :

```blocks
bluetooth.setMouseButton(MouseButton.MOUSE_BUTTON_LEFT, ButtonState.BUTTON_DOWN);
```

For example, move mouse pointer using acceleration of micro:bit :

```blocks
basic.forever(() => {
    bluetooth.setMouseSpeed(input.acceleration(Dimension.X) / 8, input.acceleration(Dimension.Y) / 8, 0);
}
```

## Supported Platforms

Currently, tested with `micro:bit` and `Android` host only.
Mac OS X can connect with `micro:bit`, but it can't receive Mouse message.

## Supported targets

* for PXT/microbit

(The metadata above is needed for package search.)

## License

MIT

icon.png : copyright By Darkone - Own work, CC BY-SA 2.5, https://commons.wikimedia.org/w/index.php?curid=235633

```package
bluetooth
bluetooth-mouse=github:kshoji/pxt-bluetooth-mouse
```
