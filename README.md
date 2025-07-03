# Arduino_Pick_And_Place_bot
# arduino-bt-car-servo-fix

An Arduino sketch for a 4WD Bluetooth-controlled car that solves the hardware timer conflict between the Adafruit Motor Shield (`AFMotor.h`) and the `Servo.h` library. This code implements a mode-switching system to safely toggle between DC motor and servo control, preventing shield overheating and damage.

---

## 🧐 The Problem: Hardware Timer Conflict

When using an Arduino Uno with an Adafruit Motor Shield V1 (or a common clone), a hardware conflict arises between controlling DC motors and a servo simultaneously.

* **`AFMotor.h` Library**: This library uses **Timer 1** and **Timer 2** on the Arduino to control the speed of the DC motors.
* **`Servo.h` Library**: This library also requires a hardware timer to send control signals. For servos connected to **pins 9 and 10**, it uses **Timer 1**.

When both libraries try to control **Timer 1** at the same time, it corrupts the signals being sent, leading to erratic behavior and the risk of permanent hardware damage.

---

## ⚠️ Important: The Risk of Hardware Damage (The "Burning" Issue)

A critical issue to be aware of is the real risk of **permanently damaging your hardware** if you attempt to run both the DC motors and the servo simultaneously without the mode-switching logic provided in this code.

**What Happens?**

The hardware timer conflict creates unstable electrical signals. This can lead to a condition within the motor driver chips (H-bridges) on the shield where they rapidly switch on and off or even short-circuit, generating immense heat.

**The Consequences:**

1.  **Burning Out the Motor Shield:** The most common result is that the motor driver chips will overheat very quickly. This can cause them to burn out, rendering the **motor shield** useless. You might notice a distinct smell or even see smoke from the shield.

2.  **Damaging the Arduino Uno:** Since the shield is stacked directly onto the **Arduino Uno**, the excessive heat and potential short circuits from the shield can feed back into the Arduino. This can damage the Arduino's input/output pins or even destroy the main microcontroller, "bricking" the entire board.

This project's code, with its mode-switching feature, is specifically designed to **prevent this damage** by ensuring the conflicting components are never active at the same time.

---

## ✅ The Solution: Mode Switching

This code solves the problem by ensuring that only one system uses Timer 1 at any given moment. This is achieved by creating two distinct operational modes:

1.  **Motor Mode**: The servo is programmatically detached using `myServo.detach()`. This releases Timer 1, allowing the `AFMotor` library to have exclusive, stable control over the DC motors. An onboard LED will be **ON** to indicate this mode.

2.  **Servo Mode**: All DC motors are stopped, and the servo is attached using `myServo.attach(10)`. This gives the `Servo` library exclusive control of Timer 1. The onboard LED will be **OFF** to indicate this mode.

A special command, **`X`**, is used to toggle between these two modes, providing a safe and reliable way to use all components without hardware conflicts.

---

## 🛠️ Hardware Requirements

* Arduino UNO
* Adafruit Motor Shield V1 (or a compatible clone)
* 4 x DC Geared Motors with Wheels
* 4WD Robot Car Chassis
* 1 x SG90 or MG90S Micro Servo
* 1 x HC-05 or HC-06 Bluetooth Module
* 1 x LED (any color)
* 1 x 220Ω Resistor (for the LED)
* Jumper Wires
* Power Source: 7.4V - 9V battery pack (e.g., 2x 18650 Li-ion cells or a 6xAA battery holder)

---

## 🔌 Wiring and Connections

1.  **Motor Shield**: Stack the Adafruit Motor Shield directly on top of the Arduino UNO.
2.  **Power**: Connect your battery pack to the blue power terminal on the motor shield.
3.  **DC Motors**: Connect the four DC motors to the `M1`, `M2`, `M3`, and `M4` terminals on the shield.
    * Front Right Motor -> `M1`
    * Back Right Motor -> `M2`
    * Front Left Motor -> `M3`
    * Back Left Motor -> `M4`
4.  **Servo Motor**: Connect the 3-pin servo connector to the `SERVO_1` header pins on the motor shield. This corresponds to Arduino pin D10.
5.  **LED Indicator**:
    * Connect the **longer leg** (anode) of the LED to **Pin 9** on the motor shield.
    * Connect the **shorter leg** (cathode) of the LED to one end of the 220Ω resistor.
    * Connect the other end of the resistor to a **GND** pin on the shield.
6.  **Bluetooth Module (HC-05/HC-06)**:
    * `VCC` -> `5V` on the motor shield.
    * `GND` -> `GND` on the motor shield.
    * `TXD` -> `RX` (Pin 0) on the Arduino.
    * `RXD` -> `TX` (Pin 1) on the Arduino.

> **⚠️ CRITICAL NOTE:** You **MUST DISCONNECT** the Bluetooth module's wires from pins 0 (RX) and 1 (TX) before you can upload the code to the Arduino. Reconnect them after the upload is complete.

---

## 💾 Software and Deployment

1.  **Install Arduino IDE**: Download it from the [official Arduino website](https://www.arduino.cc/en/software).
2.  **Install Library**: In the Arduino IDE, go to `Tools` > `Manage Libraries...`. Search for `Adafruit Motor Shield V1 Library` and install it. The `Servo.h` library is already included with the IDE.
3.  **Upload the Code**:
    * Disconnect the Bluetooth module from pins 0 and 1.
    * Connect your Arduino to your computer via USB.
    * Open the code in the Arduino IDE.
    * Go to `Tools` > `Board` and select "Arduino Uno".
    * Go to `Tools` > `Port` and select the correct COM port.
    * Click the "Upload" button.
    * Once you see "Done uploading," you can disconnect the USB and reconnect the Bluetooth module.

---

## 📱 How to Operate the Car

1.  **Get a Bluetooth App**: Download a Bluetooth serial controller app from the Google Play Store or Apple App Store. "Arduino Bluetooth Controller" or "Serial Bluetooth Terminal" are good options.
2.  **Pair Your Phone**: Turn on the robot. On your phone, go to Bluetooth settings and pair with the "HC-05" or "HC-06" device. The default password is usually `1234` or `0000`.
3.  **Configure the App**: Set up buttons in the app to send the following commands. Ensure the app is set to send a newline character (`\n`) after each command.

> ### ⚠️ Important App Configuration
>
> You must configure two types of buttons in your app:
>
> 1.  A **Toggle Button** that sends the single character **`X`**. This will be your button to switch between Motor Mode and Servo Mode.
> 2.  **Servo Control Buttons** (or a slider) that send commands starting with **`Z`**, followed by an angle. For example: `Z0`, `Z90`, `Z180`.

### Command List

* **Movement (Only in Motor Mode - LED ON)**
    * `F` - Move Forward
    * `B` - Move Backward
    * `L` - Turn Left
    * `R` - Turn Right
    * `S` - Stop Motors
* **Speed Control (Only in Motor Mode - LED ON)**
    * `1` - Speed 25%
    * `2` - Speed 50%
    * `3` - Speed 75%
    * `4` - Speed 100%
* **Mode Control**
    * `X` - **Toggle between Motor Mode and Servo Mode.**
* **Servo Control (Only in Servo Mode - LED OFF)**
    * `Z<angle>` - Sets the servo to a specific angle (e.g., `Z45`, `Z120`).
    * 

### Controler Link and Instructions 
* **https://play.google.com/store/apps/details?id=com.electro_tex.bluetoothcar
* **Don't forget to calibrate the controller
* **Change the servo value from x to z and values from 0-80
