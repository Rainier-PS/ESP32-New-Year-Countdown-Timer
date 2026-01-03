# ESP32-New-Year-Countdown-Timer

An ESP32-based New Year countdown timer that synchronizes time using NTP, displays the current time and countdown on a 16×2 LCD, and activates a buzzer and LED when the target time is reached. The project also includes a test mode for quickly verifying functionality without waiting for the actual New Year.

## Features

- **Wi-Fi time synchronization (NTP)** for accurate countdown timing  
- **16×2 LCD display**  
  - Current local time with tenths of a second  
  - Countdown to the target time in seconds  
- **Buzzer alerts**  
  - Beep interval increases as the countdown approaches zero  
- **LED indicator**  
  - Pulses in sync with buzzer beeps  
  - Stays ON after countdown completion  
- **Test mode**  
  - Triggered via a GPIO pin  
  - Starts a 5-second countdown for testing and demos  
- **Final event display**  
  - Shows “HAPPY NEW YEAR!”  
  - Long buzzer tone when countdown reaches zero  

## Hardware Requirements

- ESP32 development board  
- 16×2 LCD (HD44780 compatible, 4-bit mode)  
- Buzzer (active or passive)  
- LED with current-limiting resistor  
- Push button or jumper wire (for test mode)  
- Stable Wi-Fi connection  
- Breadboard and jumper wires  

## Pin Configuration

### LCD Connections

| LCD Pin | ESP32 GPIO |
|-------|------------|
| RS    | 13 |
| EN    | 33 |
| D4    | 14 |
| D5    | 27 |
| D6    | 26 |
| D7    | 25 |

### Other Connections

| Component | ESP32 GPIO |
|----------|------------|
| Buzzer   | 32 |
| LED      | 2 |
| Test Pin | 34 |

Note: GPIO 34 is input-only and requires an external 10kΩ pull-down resistor to GND to prevent false triggers.

## Software Requirements

- Arduino IDE with ESP32 board support installed  
- Libraries used:  
  - `LiquidCrystal`  
  - `WiFi.h`  
  - `time.h`  

## Setup Instructions

1. Clone or download this repository into your Arduino projects folder.  
2. Open the `.ino` file in Arduino IDE.  
3. Install required libraries if they are not already available.  
4. Edit the following variables in the code:  
   - `ssid` and `password`: your Wi-Fi credentials.  
   - `gmtOffset_sec`: set your local time zone offset (used for display only; internal timing remains UTC). 
5. Connect the hardware according to the pin configuration.  
6. Upload the code to your ESP32.  
7. On boot, the ESP32 will connect to Wi-Fi, synchronize time via NTP, and start the countdown display.

## Build Guide

For a beginner-friendly, step-by-step tutorial with pictures and detailed explanations, check out the full build guide on Instructables:  
👉 [ESP32 New Year Countdown Timer on Instructables](https://www.instructables.com/ESP32-WiFi-New-Year-Countdown-Timer-Beginner-Frien/)

## Target Time

The default target is set to:

- **January 1, 2027 – 00:00:00 (local time, GMT+7)**
  
You can change the target date and time by modifying the `targetTime` value inside the `setTarget()` function.

The target time is stored as a Unix timestamp (UTC).

> Note: The provided timestamp 1798736400 corresponds to Midnight Jan 1, 2027, for GMT+7. If you are in a different timezone, you must update both the gmtOffset_sec and the targetTime timestamp.

## Test Mode

Test mode allows you to simulate the final countdown without waiting for the real target time.

- Pull **GPIO 34 HIGH** to activate test mode  
- A 10-second countdown begins
- The LCD shows a test indicator  
- Buzzer and LED behavior match the real countdown  

This is useful for debugging, demonstrations, and verifying hardware connections.

## Notes

- Time synchronization waits until a valid NTP timestamp is received before starting.  
- Beep frequency increases as the countdown approaches zero
- Once the final event is triggered, it will not repeat unless the device is reset.  

## License

This project is licensed under the MIT License.
