<div align="center">
  <h1>🔋 Adaptive Multi-Cell Battery Intelligence Engine & Safety Kernel</h1>
  <p><strong>An Automotive-Grade, Event-Driven Battery Management System (BMS) Architecture for ESP32 Systems</strong></p>

  <img src="https://img.shields.io/badge/Platform-ESP32-blue?style=for-the-badge&logo=espressif" alt="ESP32 Platform">
  <img src="https://img.shields.io/badge/Simulator-Wokwi-green?style=for-the-badge" alt="Wokwi Simulator">
  <img src="https://img.shields.io/badge/IoT-Blynk%20Cloud-rebeccapurple?style=for-the-badge&logo=blynk" alt="Blynk IoT">
  <img src="https://img.shields.io/badge/Architecture-Event--Driven%20%2F%20Non--Blocking-orange?style=for-the-badge" alt="Architecture">
</div>

<hr />

---

## 📺 Live System Demonstration

### Assembled Hardware Profile (Wokwi Simulation)
![Wokwi BMS Circuit Diagram](https://github.com/Suryapra1ap/Elevance-skills-project-/blob/main/diagram.png)

### Real-Time IoT Cloud Telemetry Video
> [!TIP]
> **[Click Here to Watch the Full BMS Operational Video Demonstration](https://link-to-your-video.com)**
> *Demonstrating real-time overvoltage protection trips, multi-cell imbalance alarms, and automated fail-safe execution.*

---

## 🧮 Mathematical Core

To evaluate system health, the battery intelligence engine processes localized variation models against live pack means. Cell imbalance analytics are processed using localized variation models against live pack means according to the following structural expression:



$$
\text{Imbalance \%)} = \frac{V_{\max} - V_{\min}}{V_{\text{avg}}} \times 100
$$


This index drives the fault-tolerant state machine to identify cells suffering from accelerated degradation or micro-shorting before runtime failure occurs.

---

## 🏗️ Firmware Architecture Subsystems

The core application code is decoupled into six dedicated, independent kernel modules executing deterministically without blocking telemetry cycles:

### 1. Multi-Cell Battery Intelligence Engine
* Continuously processes analog-to-digital conversions (ADC) from 4 independent battery cells using a calibrated attenuation voltage model.
* Dynamically extracts dynamic extrema data ($V_{max}$, $V_{min}$), pinpoints cell structural tracking indices (`strongestCell`, `weakestCell`), and checks for minor/critical health states.

### 2. Fault-Tolerant Runtime Kernel
* Provides an algorithmic layer protecting against hardware sensor anomalies.
* If an ADC pin registers an open-circuit float ($<0.1\text{V}$) or an out-of-bounds rail fault ($>4.49\text{V}$), the system enters a **DEGRADED** operational mode, safeguarding the asset against corrupt telemetry while preserving basic logging functionality.

### 3. Event-Driven Safety Protection Kernel
* Operates as a low-latency hardware abstraction layer controlling the safety isolation relay, alarm buzzer, and fault LED indicator arrays.
* Executes immediate safety isolation within milliseconds of threshold violation while utilizing non-blocking asynchronous state changes to keep the network interface active.

### 4. Intelligent Embedded HMI Subsystem
* Drives a localized LiquidCrystal I2C display (20x4) using an automatic page-rotation engine.
* Cycles through deep-level diagnostic views (Live Cell Potentials, Pack Statistics, and Mode Flags) every 4 seconds without using execution-freezing `delay()` blocks.

### 5. Asynchronous Cloud Telemetry Architecture
* Handles network connectivity and state sync over the Blynk IoT Framework.
* Implements a smooth telemetry transmission engine throttling transmissions to 1500ms intervals, avoiding data-rate caps while maintaining high system responsiveness.

### 6. Executive Diagnostic & Advisory Engine
* Translates real-time structural calculations into actionable human-readable logs and risk indices.
* Computes real-time failure reasons (e.g., specific cell overcharge or deep discharge alerts) and serves them as explicit string instructions to terminal nodes.

---

## 📊 Blynk IoT Enterprise Dashboard

The cloud telemetry layout uses strict semantic typing and severity-based state coloring driven entirely by specialized device `Enums`.

### Cloud Datastream Structure

| Virtual Pin | Datastream Name | Data Type | Operational Functionality |
| :---: | :--- | :---: | :--- |
| **V0** | `total pack voltage` | Double | Combined voltage across the 4-cell series pack |
| **V1** | `cell 1` | Double | Real-time voltage metric for Cell Module 1 |
| **V2** | `cell 2` | Integer | Truncated tracking component for Cell Module 2 |
| **V3** | `cell 3` | Double | Real-time voltage metric for Cell Module 3 |
| **V4** | `cell 4` | Double | Real-time voltage metric for Cell Module 4 |
| **V5** | `cell imbalance` | Double | Calculated structural variation metric (%) |
| **V6** | `runtime state` | Enum | System tracking state matching device firmware indices |
| **V7** | `Operator Advisory` | String | Live contextual instructions forwarded from the advisory layer |
| **V8** | `Risk Index` | Enum | Severity index running automated UI color shifting |

<img width="1441" height="402" alt="image" src="https://github.com/user-attachments/assets/b9f7ec06-3db7-4309-a19f-6f21ce56a5b1" />


### 🛠️ Executive State Vector Mappings (Enums)

The system relies on strict semantic typing where firmware-level C++ enumerations are transmitted directly as integer values to populate matching specialized `Enum` datastreams on the cloud dashboard.

#### 1. Runtime State Vector Mappings (Virtual Pin V6)
Tracks the high-level operating loop execution state of the embedded micro-kernel. Configure your Blynk V6 Datastream options with the following parameters:

| Incoming Value (INT) | Firmware Enum State | Dashboard Display String | Status Color Code | Engineering Definition |
| :---: | :--- | :--- | :---: | :--- |
| **0** | `NORMAL` | `System Nominal` | 🟩 **GREEN** | Pack healthy; active tracking loops running normally. |
| **1** | `DEGRADED` | `Degraded Operations` | 🟨 **YELLOW** | Active hardware sensor anomaly or wire float detected. |
| **2** | `FAILSAFE` | `Failsafe Protective Trip` | 🟧 **ORANGE** | Out-of-bounds limit hit; isolation relay opened. |
| **3** | `SHUTDOWN` | `Emergency Shutdown` | 🟥 **RED** | Critical multi-system failure; manual recovery required. |

#### 2. Risk Index Vector Mappings (Virtual Pin V8)
Feeds the executive analytics dashboard engine, serving localized hazard threat levels for the physical battery asset. Configure your Blynk V8 Datastream options with the following parameters:

| Incoming Value (INT) | Dashboard Outcome Text | UI Color Code | Asset Threat Status | Actionable Operational Meaning |
| :---: | :--- | :---: | :--- | :--- |
| **0** | `Nominal operation` | 🟩 **GREEN** | Safe State | Cell delta under bounds; asset performing within factory spec. |
| **1** | `Degraded warning state` | 🟨 **YELLOW** | Minor Hazard | Low-level cell voltage divergence; passive balancing active. |
| **2** | `Sensor anomaly hazard` | 🟧 **ORANGE** | Medium Hazard | Open circuit or rail voltage failure discovered on a cell pin. |
| **3** | `Safety Isolation  | 🟥 **RED** | Severe Hazard | Overcharge/deep discharge verified; hardware isolation active. |


---

## 🔌 Hardware Profile & Pin Configurations

| ESP32 Pin | Peripheral Interface | Configuration Type | Logic Level / Protocol |
| :---: | :--- | :---: | :--- |
| **GPIO 32** | Cell 1 Potentiometer | Input (ADC1_CH4) | Analog ($0.0\text{V} - 3.3\text{V}$) |
| **GPIO 33** | Cell 2 Potentiometer | Input (ADC1_CH5) | Analog ($0.0\text{V} - 3.3\text{V}$) |
| **GPIO 34** | Cell 3 Potentiometer | Input (ADC1_CH6) | Analog ($0.0\text{V} - 3.3\text{V}$) |
| **GPIO 35** | Cell 4 Potentiometer | Input (ADC1_CH7) | Analog ($0.0\text{V} - 3.3\text{V}$) |
| **GPIO 4** | Safety Isolation Relay | Output | Active High Digital |
| **GPIO 5** | Auditory Alarm Buzzer | Output | PWM / Tone Frequency Generation |
| **GPIO 18** | Master Fault Status LED | Output | Active High Digital |
| **GPIO 21** | LiquidCrystal I2C SDA | Input/Output | $I^2C$ Data Line |
| **GPIO 22** | LiquidCrystal I2C SCL | Output | $I^2C$ Clock Line |

---

## 🚀 Deployment Instructions

### 1. Cloud Pre-Configuration
1. Log into your **Blynk.Console** account.
2. Navigate to **Developer Zone** $\rightarrow$ **Templates** and create a template named `Battery Intelligence Engine`.
3. Set up the 9 datastreams exactly as defined in the [Cloud Datastream Structure](#cloud-datastream-structure) table. Make sure **V6** and **V8** are typed as **Enum** fields.

### 2. Firmware Compilation & Flashing
1. Open the source code inside your embedded environment workspace (e.g., VS Code + PlatformIO or Arduino IDE).
2. Install the required target libraries via your library manager:
   * `Blynk` (by Volodymyr Shymanskyy)
   * `LiquidCrystal_I2C` (by Frank de Brabander)
3. Replace the configuration tokens with your explicit credentials at the top of the sketch:
   ```cpp
   #define BLYNK_TEMPLATE_ID   "YOUR_TEMPLATE_ID"
   #define BLYNK_TEMPLATE_NAME "YOUR_TEMPLATE_NAME"
   #define BLYNK_AUTH_TOKEN    "YOUR_AUTH_TOKEN"
   ```
4. Compile the binaries and flash the payload to your physical target ESP32 board, or copy the code directly into your Wokwi simulation instance.
