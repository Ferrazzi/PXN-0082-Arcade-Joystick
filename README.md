# PXN-0082-Arcade-Joystick
PXN-0082 Arcade Joystick from USB to BLUETOOTH with ESP32

This is a tutorial to convert the "PXN-0082 Arcade Joystick" from USB to Bluetooth

Download the lemmingDev ESP-32-BLE-Gamepad library accessible from the site https://github.com/lemmingDev/ESP32-BLE-Gamepad and insert it into the Arduino IDE, or download the same library here in the project.

Download the Gamepad.ino file, open it with the Arduino IDE and flash the ESP32 with this sketch.

Make the connections from the diagram by removing all existing gamepad connections.

I used the joystick mode switch to turn the controller on and off.

An LED can be connected to PIN19 of the ESP32 to signal the status of the Bluetooth connection.

<img src="https://github.com/Ferrazzi/PXN-0082-Arcade-Joystick/blob/main/Scheme.png" align="center" alt="fzCUSTOM" border="0">
