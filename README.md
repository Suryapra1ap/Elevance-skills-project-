<h1 align="center">Event-Driven Safety Protection Kernel</h1>

<p align="center">
Automotive-Inspired Battery Safety Protection Controller using ESP32
</p>

<hr>

<h2>📌 Project Overview</h2>

<p>
This project implements an automotive-grade Battery Safety Protection Controller
using a fully non-blocking event-driven architecture.
</p>

<p>
The system continuously monitors battery voltage and detects:
</p>

<ul>
  <li>Undervoltage</li>
  <li>Overvoltage</li>
  <li>Sensor Faults</li>
  <li>Rapid Voltage Fluctuations</li>
</ul>

<p>
When a fault occurs, the controller:
</p>

<ul>
  <li>Disconnects the battery through a relay</li>
  <li>Activates an alarm buzzer</li>
  <li>Displays warning messages on LCD</li>
</ul>

<hr>

<h2>✨ Features</h2>

<h3>🔋 Undervoltage Protection</h3>

<pre>
Voltage &lt; 2.8V
</pre>

<h3>⚡ Overvoltage Protection</h3>

<pre>
Voltage &gt; 4.2V
</pre>

<h3>📡 Sensor Fault Detection</h3>

<ul>
  <li>Invalid ADC values</li>
  <li>Disconnected sensor</li>
  <li>Out-of-range voltage readings</li>
</ul>

<h3>📈 Voltage Fluctuation Detection</h3>

<pre>
Voltage Change &gt; 0.5V in 500ms
</pre>

<hr>

<h2>🛠 Hardware Components</h2>

<table>
<tr>
<th>Component</th>
<th>GPIO</th>
</tr>

<tr>
<td>Voltage Sensor (Potentiometer)</td>
<td>GPIO34</td>
</tr>

<tr>
<td>Relay Module</td>
<td>GPIO26</td>
</tr>

<tr>
<td>Buzzer</td>
<td>GPIO27</td>
</tr>

<tr>
<td>LCD SDA</td>
<td>GPIO21</td>
</tr>

<tr>
<td>LCD SCL</td>
<td>GPIO22</td>
</tr>

</table>

<hr>

<h2>🔌 Circuit Connections</h2>

<h3>Potentiometer</h3>

<pre>
Left Pin   → GND
Middle Pin → GPIO34
Right Pin  → 3.3V
</pre>

<h3>Relay Module</h3>

<pre>
IN   → GPIO26
VCC  → 5V
GND  → GND
</pre>

<h3>Buzzer</h3>

<pre>
+ → GPIO27
- → GND
</pre>

<h3>I2C LCD</h3>

<pre>
SDA → GPIO21
SCL → GPIO22
VCC → 5V
GND → GND
</pre>

<hr>

<h2>🏗 Software Architecture</h2>

<pre>
Sensor Task
     ↓
Fault Detection Task
     ↓
Recovery Manager
     ↓
Relay Control
     ↓
Buzzer Control
     ↓
LCD Update Task
</pre>

<p>
All tasks execute using:
</p>

<pre>
millis()
</pre>

<p>
No delay() function is used anywhere in the project.
</p>

<hr>

<h2>🔄 State Machine</h2>

<pre>
NORMAL
 ├── UNDER_VOLTAGE
 ├── OVER_VOLTAGE
 ├── SENSOR_FAULT
 └── VOLTAGE_FLUCTUATION
</pre>

<hr>

<h2>🛡 Recovery Logic</h2>

<p>
When a fault is detected, the relay is disconnected immediately.
</p>

<p>
Recovery occurs only if:
</p>

<ul>
  <li>Voltage returns to safe range</li>
  <li>System remains stable for 5 seconds</li>
</ul>

<hr>

<h2>🚀 Wokwi Simulation</h2>

<p>
Components Used:
</p>

<ul>
  <li>ESP32 DevKit V1</li>
  <li>Potentiometer</li>
  <li>Relay Module</li>
  <li>Buzzer</li>
  <li>16x2 I2C LCD</li>
</ul>

<hr>

<h2>📚 Technologies Used</h2>

<ul>
  <li>ESP32</li>
  <li>Arduino IDE</li>
  <li>Wokwi Simulator</li>
  <li>C++</li>
  <li>Event-Driven Programming</li>
  <li>Finite State Machine</li>
</ul>

<hr>

<h2>👨‍💻 Author</h2>

<p>
Surya Pratap
</p>

<p>
B.Tech Electronics & Communication Engineering
</p>
