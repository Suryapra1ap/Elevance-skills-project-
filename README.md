<div align="center">
  <h1>🔋 Adaptive Multi-Cell Battery Intelligence Engine & Safety Kernel</h1>
  <p><strong>An Automotive-Grade, Event-Driven Battery Management System (BMS) Architecture for ESP32 Systems</strong></p>

  <img src="https://img.shields.io/badge/Platform-ESP32-blue?style=for-the-badge&logo=espressif" alt="ESP32 Platform">
  <img src="https://img.shields.io/badge/Simulator-Wokwi-green?style=for-the-badge" alt="Wokwi Simulator">
  <img src="https://img.shields.io/badge/IoT-Blynk%20Cloud-rebeccapurple?style=for-the-badge&logo=blynk" alt="Blynk IoT">
  <img src="https://img.shields.io/badge/Architecture-Event--Driven%20%2F%20Non--Blocking-orange?style=for-the-badge" alt="Architecture">
</div>

<hr />

<h2>📋 Project Overview</h2>
<p>
  This repository contains a production-grade firmware architecture implementing a <strong>4-Cell Lithium-Ion Battery Intelligence Engine</strong> alongside an <strong>Event-Driven Safety Protection Kernel</strong>. Engineered entirely on a non-blocking <code>millis()</code> design paradigm, the system targets high reliability, deterministic fault handling, asynchronous telemetry distribution, and low-latency safety mitigations without single-point software stalls.
</p>

<h3>Core Mathematical Core</h3>
<p>Individual cell imbalance analytics are processed using localized variation models against live pack means:</p>

$$
\text{Imbalance (\%)} = \frac{V_{\max} - V_{\min}}{V_{\text{avg}}} \times 100
$$

<hr />
<h2>🛠 Architecture Subsystems</h2>

<h3>1. Adaptive Multi-Cell Battery Intelligence Engine</h3>
<ul>
  <li><strong>Continuous Real-Time Analytics:</strong> Individually polls and converts ADC matrices into distinct real voltages.</li>
  <li><strong>Statistical Processing:</strong> Continuously computes total pack potential, moving arithmetic averages, historical absolute extremes, and precise geometric cell differences.</li>
  <li><strong>Categorization State Machine:</strong> Instantly clusters pack status across <code>Healthy</code>, <code>Minor Imbalance</code>, <code>Critical Imbalance</code>, and <code>Pack Failure</code> blocks.</li>
</ul>

<h3>2. Event-Driven Safety Protection Kernel</h3>
<ul>
  <li><strong>Non-blocking Execution Matrix:</strong> Rejects standard block processing loops (no <code>delay()</code> allowed), securing microsecond responsiveness under critical alert loads.</li>
  <li><strong>Anti-Relay Chatter Engine:</strong> Incorporates structural lockouts and time-based hysteretic paths to prevent rapid thermal failure of physical contactors.</li>
</ul>

<h3>3. Fault-Tolerant Embedded Runtime System</h3>
<ul>
  <li><strong>Deterministic Mode Transitioning:</strong> Smooth transitions across four operational tiers based on hardware integrity profiles:</li>
</ul>
<table border="1" cellpadding="5" style="border-collapse: collapse; width: 100%;">
  <thead>
    <tr style="background-color: #222; color: #fff;">
      <th>System Mode</th>
      <th>Triggering Event Criteria</th>
      <th>Applied Safety Action Mitigations</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><strong>NORMAL</strong></td>
      <td>All sensors functioning perfectly; parameters within normal thresholds.</td>
      <td>Full operations active; isolation relay remains closed.</td>
    </tr>
    <tr>
      <td><strong>DEGRADED</strong></td>
      <td>Minor sensor dropouts, outlier values, or transient read anomalies.</td>
      <td>Flag visual telemetry alerts; retain primary functionality safely.</td>
    </tr>
    <tr>
      <td><strong>FAILSAFE</strong></td>
      <td>Severe continuous imbalance or sustained over/under voltage detections.</td>
      <td>Instantly trip physical contactor relay; flash alert LEDs.</td>
    </tr>
    <tr>
      <td><strong>SHUTDOWN</strong></td>
      <td>Catastrophic ADC freeze or total feedback hardware loss.</td>
      <td>Permanent hardware cutoff; trigger high-frequency audible alarm.</td>
    </tr>
  </tbody>
</table>

<h3>4. Intelligent Cloud Telemetry Architecture (Blynk)</h3>
<ul>
  <li><strong>Anomaly-Driven Transmission:</strong> Prevents network saturation by deploying data vectors exclusively during state changes, severe delta steps, or dynamic threshold violations.</li>
  <li><strong>Asynchronous Safety Buffer:</strong> Network connectivity processes operate strictly decoupled from core loop evaluations, protecting the hardware safety tracking logic even during major network failures.</li>
</ul>

<hr />

<h2>🎛 HMI & Interface Design</h2>
<p>
  The local physical visualization structure is powered by an optimized, flicker-free liquid crystal runtime utilizing strategic frame-differential memory updates.
</p>
<ul>
  <li><strong>Rotational Diagnostic Sequencing:</strong> Periodically shifts across <code>Cell Statistics</code>, <code>Dynamic Diagnostics</code>, and <code>Active Fault Registers</code>.</li>
  <li><strong>High-Priority System Overrides:</strong> During critical faults, normal display workflows are overridden to output deterministic safety fault diagnostic vectors.</li>
</ul>

<hr />

<h2>🔌 Hardware Interfacing Profile (Wokwi-Ready)</h2>
<ul>
  <li><strong>Pins 32, 33, 34, 35:</strong> Connected to high-impedance voltage divider nodes (Simulated by Linear Potentiometers).</li>
  <li><strong>I2C Bus (GPIO 21 / 22):</strong> Drives the 20x4 Alphanumeric Diagnostic Frame Buffer.</li>
  <li><strong>GPIO 4:</strong> Logic-level output gate to the High-Power Safety Isolation Relay.</li>
  <li><strong>GPIO 5:</strong> Drives the audible alarm circuit.</li>
</ul>

<hr />

<div align="center">
  <small>Designed for advanced automotive technology verification frameworks and safety-critical embedded systems benchmarks.</small>
</div>
