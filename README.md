# IoT Automated Smart Dryer 🌤️👗

An automated, intelligent clothes drying system designed for smart home integration. The system continuously monitors environmental factors to optimize outdoor natural drying while protecting fabrics from unexpected rain and adverse weather conditions.

Developed as part of the Computer Engineering curriculum at the Technical University of Cluj-Napoca.

---

## 📌 Features

* **Dual Mode Operation**: 
    * **Automated Mode**: Uses an algorithm to compute a drying efficiency index and safely deploys or retracts the drying rack based on live sensor data.
    * **Manual Mode**: Full override capability using an infrared remote control for loading clothes or specific manual adjustments.
* **Intelligent Emergency Retraction**: Triggers an instantaneous hardware sequence to pull the rack inside and close the window upon detecting the first drops of rain.
* **Fabric & Energy Protection**: Harnesses eco-friendly natural wind and solar drying to prolong textile lifespan and lower home electricity bills by eliminating traditional electrical dryers.

---

## 🛠️ Hardware Architecture

The system utilizes an external 5V/1.5A power isolation circuit, separate from the microcontroller, integrated with a **1000µF decoupling capacitor (C1)** connected in parallel to suppress transient voltage drops during simultaneous motor actuations.

### Components Used:
* **Microcontroller**: Arduino Uno R4 WiFi
* **Sensors**:
    * `DHT22 (AM2302)`: Digital Temperature and Humidity Sensor (monitored via digital pin with a 10kΩ pull-up resistor).
    * `Rain Sensor Module`: Equipped with an `LM393` voltage comparator for dual digital and analog liquid detection.
    * `HX1838`: Infrared Receiver module operating at 38kHz for remote command capture.
* **Actuators**:
    * 2x `MG90S` Metal Gear Micro Servos (driven via hardware PWM for automated structural execution).

---

## 💻 Software Implementation

The logic is built around three distinct finite states: `MOD_AUTOMAT`, `MOD_MANUAL`, and `MOD_SETARE`. 

### The Decision Core (Drying Index)
In automated mode, the controller samples the sensors every 5 seconds and computes a custom linear metric to gauge environmental dryness:

**dryIndex = (Temperature * 3) - Humidity**

The rack deploys outdoors **only if**:
1. The calculated `dryIndex` is strictly non-negative (>= 0).
2. The rain sensor's analog reading stays above the dry threshold (> 400).
3. The digital rain indicator is flagged `HIGH` (no water detected).

If any of these conditions fail, the emergency loop immediately rolls back the mechanical structural positions to protect the clothes.

---

## 🎮 Remote Control Mapping

The system processes raw structural hex frames decoded via the `IRremote` library:

| IR Command Hex | Remote Key | System Functionality |
| :--- | :--- | :--- |
| `0xE916FF00` | `*` (Star) | Switch to **Automated Mode** (Safely homing elevator servos first). |
| `0xE619FF00` | `0` | Switch to **Manual Mode**. |
| `0xF20DFF00` | `#` (Grid) | Switch to **Calibration Mode** (Homing all structural components). |
| `0xE718FF00` | `UP` | Manual Mode: Raise the elevator assembly. |
| `0xAD52FF00` | `DOWN` | Manual Mode: Lower the elevator assembly for clothes mounting. |
| `0xF708FF00` | `LEFT` | Manual Mode: Extend rack outside / Window open sequence. |
| `0xA55AFF00` | `RIGHT` | Manual Mode: Retract rack inside / Window close sequence. |
| `0xE31CFF00` | `OK` | Calibration Mode: Emergency hard-brake stop for all servos. |

---

## 📈 Identified Challenges & Future Scope

* **Mechanical Load Limits**: The `MG90S` micro-servos lack the structural torque needed for real-world heavy, wet textile loads. Future iterations will migrate to high-torque bipolar stepper motors or heavy-duty linear actuators.
* **Sensor Filtering**: Low-cost rain sensor modules are prone to interference from analog signal drift and external debris. Integrating software-defined hysteresis loops and physical wire-mesh bird guards will prevent false triggers.
* **Power Redundancy**: Implementing a secondary rechargeable battery backup system is planned to ensure proper automated emergency retraction even during complete home power outages.

---

## 👥 Authors
* **Prode Gabriel-Ilie** - *Computer Engineering Student, Year II*
