# IoT-Based Home Automation using ESP RainMaker (4-Channel Relay Module)

An ESP32-based 4-channel smart home automation system powered by **Espressif RainMaker**. This project allows real-time control of up to 4 electrical appliances using the official ESP RainMaker mobile app or physical wall switches, with two-way status synchronization and memory state retention.

---

## 📸 Circuit Diagram

![Circuit Diagram](circuit_diagram.png)

---

## ✨ Features

* **Dual Control Mode:** Control relays using either the ESP RainMaker mobile app or physical switches/push buttons.
* **State Memory (EEPROM):** Retains relay states during power outages and restores them when power is re-established.
* **BLE Provisioning:** Easy Wi-Fi pairing via Bluetooth Low Energy (BLE) using the ESP RainMaker app.
* **Hardware Debouncing:** Uses the `AceButton` library for reliable, jitter-free switch presses.
* **Multi-Function Reset:**
  * **Hold for 3+ seconds:** Reset Wi-Fi configuration.
  * **Hold for 10+ seconds:** Factory reset device.
* **Wi-Fi Status Indicator:** Visual status feedback via the built-in LED (GPIO 2).

---

## 📍 Pin Mapping

| Channel / Function | ESP32 GPIO | Connected Component | Active State |
| :--- | :--- | :--- | :--- |
| **Relay 1** | GPIO 23 | Relay IN1 | Active LOW |
| **Relay 2** | GPIO 19 | Relay IN2 | Active LOW |
| **Relay 3** | GPIO 18 | Relay IN3 | Active LOW |
| **Relay 4** | GPIO 5 | Relay IN4 | Active LOW |
| **Switch 1** | GPIO 13 | Manual Switch S1 | Active LOW (PULLUP) |
| **Switch 2** | GPIO 12 | Manual Switch S2 | Active LOW (PULLUP) |
| **Switch 3** | GPIO 14 | Manual Switch S3 | Active LOW (PULLUP) |
| **Switch 4** | GPIO 27 | Manual Switch S4 | Active LOW (PULLUP) |
| **Status LED** | GPIO 2 | Wi-Fi Connection LED | Active HIGH |
| **Reset Button** | GPIO 0 | Boot Button | Active LOW |

---

## 🛠️ Hardware Requirements

* **ESP32 DevKit V1**
* **4-Channel 5V Relay Module** (Active-Low Trigger)
* **4 x Toggle Switches / Push Buttons**
* **5V / 2A DC Power Supply**
* AC Loads (Bulbs, Fans, Appliances)
* Jumper Wires & Terminal Blocks

---

## 💻 Software & Libraries

Ensure you have the following installed in your **Arduino IDE**:

1. **ESP32 Board Package** (v2.0.0 or higher)
2. **AceButton** by *Brian T. Park* (Install via Library Manager)
3. **EEPROM** *(Included with ESP32 board package)*
4. **ESP RainMaker** *(Included with ESP32 board package)*

---

## 🚀 Setup & Installation Instructions

### 1. Arduino IDE Setup
1. Open Arduino IDE and go to **Tools > Board > ESP32 Arduino > ESP32 Dev Module**.
2. Go to **Tools > Partition Scheme** and set it to **RainMaker**.
3. Install the **AceButton** library (*Sketch > Include Library > Manage Libraries...*).

### 2. Flashing the Code
1. Open `ESP32_RainMaker_4Channel.ino`.
2. Select your ESP32 COM Port under **Tools > Port**.
3. Click **Upload**.

### 3. ESP RainMaker App Setup
1. Download the **ESP RainMaker** app from Google Play Store or Apple App Store.
2. Sign in or create a new Espressif account.
3. Tap **Add Device** (+).
4. Turn on Bluetooth on your phone and scan the QR code printed in the Serial Monitor (or pair via BLE using POP: `1234567`).
5. Connect your ESP32 to your Wi-Fi network.

---

## 📄 License

This project is open-source and available under the [MIT License](LICENSE).
