#define BLYNK_TEMPLATE_ID "TMPL3KYWDChmK"
#define BLYNK_TEMPLATE_NAME "Battery Intelligence Engine"
#define BLYNK_AUTH_TOKEN  "h0POqwSo5mthlO_aB3b9IWY9e21IDVZ5"

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// --- System Configurations & Thresholds ---
const float V_OVERVOLTAGE = 4.25;
const float V_UNDERVOLTAGE = 3.00;
const float IMBALANCE_CRITICAL = 10.0; // Percentage
const float IMBALANCE_MINOR = 5.0;

// Pin Mappings
const int CELL_PINS[4] = {32, 33, 34, 35};
const int RELAY_PIN = 4;
const int BUZZER_PIN = 5;
const int FAULT_LED = 18;

// Enumerations for State Management
enum BatteryHealth { HEALTHY, MINOR_IMBALANCE, CRITICAL_IMBALANCE, PACK_FAILURE };
enum RuntimeState { NORMAL, DEGRADED, FAILSAFE, SHUTDOWN };
enum HmiScreen { CELL_DATA, ANALYTICS, FAULT_LOGS };

// --- System Structures ---
struct BatteryPack {
    float cellVoltages[4];
    float totalVoltage;
    float averageVoltage;
    float maxVoltage;
    float minVoltage;
    float imbalancePercent;
    int strongestCell;
    int weakestCell;
    BatteryHealth health;
};

// Global Architecture Subsystems
BatteryPack bmsPack;
RuntimeState currentRuntime = NORMAL;
HmiScreen currentScreen = CELL_DATA;

// Non-blocking Timing Anchors
unsigned long lastHmiUpdate = 0;
unsigned long lastScreenRotate = 0;
unsigned long lastTelemetryUpdate = 0;
unsigned long lastRelayAction = 0;

// Configuration placeholders for Blynk
char auth[] = "h0POqwSo5mthlO_aB3b9IWY9e21IDVZ5";
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

LiquidCrystal_I2C lcd(0x27, 20, 4);

// ==========================================
// 1. Adaptive Battery Intelligence Engine
// ==========================================
void runBatteryIntelligenceEngine() {
    float sum = 0;
    bmsPack.maxVoltage = 0.0;
    bmsPack.minVoltage = 5.0;
    
    for (int i = 0; i < 4; i++) {
        // Read raw ADC and apply a simple moving average/scaling algorithm
        int rawAdc = analogRead(CELL_PINS[i]);
        // Mapping 0-4095 to 0.0V - 4.5V for simulation scaling
        float mappedVolt = (rawAdc / 4095.0) * 4.5; 
        bmsPack.cellVoltages[i] = mappedVolt;
        sum += mappedVolt;
        
        if (mappedVolt > bmsPack.maxVoltage) { bmsPack.maxVoltage = mappedVolt; bmsPack.strongestCell = i + 1; }
        if (mappedVolt < bmsPack.minVoltage) { bmsPack.minVoltage = mappedVolt; bmsPack.weakestCell = i + 1; }
    }
    
    bmsPack.totalVoltage = sum;
    bmsPack.averageVoltage = sum / 4.0;
    
    // Formula for Cell Imbalance: ((Vmax - Vmin) / Vavg) * 100
    if (bmsPack.averageVoltage > 0) {
        bmsPack.imbalancePercent = ((bmsPack.maxVoltage - bmsPack.minVoltage) / bmsPack.averageVoltage) * 100.0;
    } else {
        bmsPack.imbalancePercent = 0;
    }

    // Health Classification
    if (bmsPack.imbalancePercent >= IMBALANCE_CRITICAL || 
        bmsPack.minVoltage < V_UNDERVOLTAGE || 
        bmsPack.maxVoltage > V_OVERVOLTAGE) { 
        bmsPack.health = PACK_FAILURE;
    } else if (bmsPack.imbalancePercent >= IMBALANCE_MINOR) {
        bmsPack.health = CRITICAL_IMBALANCE;
    } else {
        bmsPack.health = HEALTHY;
    }
} // <-- Fixed: Closing brace restored here to fix structural compiler error

// ==========================================
// 2. Fault-Tolerant Runtime Subsystem
// ==========================================
void runFaultTolerantKernel() {
    bool sensorAnomaly = false;
    
    for (int i = 0; i < 4; i++) {
        // Sensor Disconnection / Frozen ADC Check
        if (bmsPack.cellVoltages[i] < 0.1 || bmsPack.cellVoltages[i] > 4.49) {
            sensorAnomaly = true;
        }
    }

    // Dynamic State Transitions
    if (sensorAnomaly) {
        currentRuntime = DEGRADED;
    } else if (bmsPack.health == PACK_FAILURE) {
        currentRuntime = FAILSAFE;
    } else {
        currentRuntime = NORMAL;
    }
}

// ==========================================
// 3. Event-Driven Safety Protection Kernel (Optimized)
// ==========================================
void runSafetyProtectionKernel() {
    unsigned long now = millis();
    static RuntimeState lastRuntime = SHUTDOWN; 
    
    // Edge Detection Transition Check
    if (currentRuntime != lastRuntime) {
        switch (currentRuntime) {
            case NORMAL:
                digitalWrite(RELAY_PIN, HIGH); // Engage Pack
                digitalWrite(FAULT_LED, LOW);
                noTone(BUZZER_PIN);            // Shut off buzzer ONCE
                break;
                
            case DEGRADED:
                digitalWrite(RELAY_PIN, HIGH); // Retain operation with warnings
                noTone(BUZZER_PIN);            // Clear buzzer for non-critical warnings
                break;
                
            case FAILSAFE:
            case SHUTDOWN:
                // Anti-Relay Chatter Protection
                if (digitalRead(RELAY_PIN) == HIGH) {
                    digitalWrite(RELAY_PIN, LOW); // Cutoff Isolation Relay instantly
                    lastRelayAction = now;
                }
                tone(BUZZER_PIN, 1000); // Sound safety alert ONCE
                break;
        }
        lastRuntime = currentRuntime; // Sync state tracking memory
    }
    
    // CYCLIC ACTIONS: Code here runs continuously based on the active state
    if (currentRuntime == DEGRADED) {
        // Non-blocking slow flash for warning state
        if ((now / 500) % 2 == 0) digitalWrite(FAULT_LED, HIGH);
        else digitalWrite(FAULT_LED, LOW);
    } 
    else if (currentRuntime == FAILSAFE || currentRuntime == SHUTDOWN) {
        digitalWrite(FAULT_LED, HIGH); // Solid fault light indication
    }
}

// ==========================================
// 4. Intelligent Embedded HMI & Interface
// ==========================================
void updateHMI() {
    unsigned long now = millis();
    
    // Smooth Screen Rotation Pattern (Every 4 seconds)
    if (now - lastScreenRotate > 4000) {
        lastScreenRotate = now;
        currentScreen = static_cast<HmiScreen>((currentScreen + 1) % 3);
        lcd.clear(); // Clear execution strictly limited to screen change to eliminate flicker
    }
    
    // Critical Fault Priority Override
    if (currentRuntime == FAILSAFE || currentRuntime == SHUTDOWN) {
        lcd.setCursor(0, 0);
        lcd.print("!! SAFETY CRITICAL !!");
        lcd.setCursor(0, 1);
        lcd.print("RELAY: ISOLATED     ");
        lcd.setCursor(0, 2);
        lcd.print("FAULT DETECTED      ");
        return;
    }

    // Flicker-Free Smart Refresh Matrix (Updates every 300ms)
    if (now - lastHmiUpdate > 300) {
        lastHmiUpdate = now;
        switch (currentScreen) {
            case CELL_DATA:
                lcd.setCursor(0, 0); lcd.print("C1:" + String(bmsPack.cellVoltages[0],2) + "V C2:" + String(bmsPack.cellVoltages[1],2) + "V");
                lcd.setCursor(0, 1); lcd.print("C3:" + String(bmsPack.cellVoltages[2],2) + "V C4:" + String(bmsPack.cellVoltages[3],2) + "V");
                lcd.setCursor(0, 2); lcd.print("Pack V: " + String(bmsPack.totalVoltage, 2) + "V ");
                break;
            case ANALYTICS:
                lcd.setCursor(0, 0); lcd.print("Avg Volt: " + String(bmsPack.averageVoltage, 2) + "V ");
                lcd.setCursor(0, 1); lcd.print("Imb Max : " + String(bmsPack.imbalancePercent, 1) + "%  ");
                lcd.setCursor(0, 2); lcd.print("Wk:" + String(bmsPack.weakestCell) + " St:" + String(bmsPack.strongestCell));
                break;
            case FAULT_LOGS:
                lcd.setCursor(0, 0); lcd.print("SYS MODE: ");
                if(currentRuntime == NORMAL) lcd.print("NORMAL  ");
                else if(currentRuntime == DEGRADED) lcd.print("DEGRADED");
                break;
        }
    }
}

// ==========================================
// 5. Intelligent Cloud Telemetry Architecture
// ==========================================
void handleCloudTelemetry() {
    unsigned long now = millis();
    static RuntimeState lastSentState = NORMAL;
    static float lastSentImbalance = 0.0;
    
    // Conditional Anomaly-Driven Data Transmission
    bool stateChanged = (currentRuntime != lastSentState);
    bool significantDelta = (abs(bmsPack.imbalancePercent - lastSentImbalance) > 1.0);
    bool heartbeatWindow = (now - lastTelemetryUpdate > 15000); // Maximum 15 sec quiet update
    
    if (Blynk.connected() && (stateChanged || significantDelta || heartbeatWindow)) {
        lastTelemetryUpdate = now;
        lastSentState = currentRuntime;
        lastSentImbalance = bmsPack.imbalancePercent;
        
        Blynk.virtualWrite(V1, bmsPack.totalVoltage);
        Blynk.virtualWrite(V2, bmsPack.imbalancePercent);
        Blynk.virtualWrite(V3, currentRuntime);
        Blynk.virtualWrite(V4, bmsPack.cellVoltages[0]);
        Blynk.virtualWrite(V5, bmsPack.cellVoltages[1]);
        Blynk.virtualWrite(V6, bmsPack.cellVoltages[2]);
        Blynk.virtualWrite(V7, bmsPack.cellVoltages[3]);
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(FAULT_LED, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    
    digitalWrite(RELAY_PIN, HIGH); // Default closed (Active Safe initialization)
    
    lcd.init();
    lcd.backlight();
    
    // Asynchronous-safe connectivity call
    Blynk.begin(auth, ssid, pass);
}

void loop() {
    // Completely non-blocking structural execution loop
    runBatteryIntelligenceEngine();
    runFaultTolerantKernel();
    runSafetyProtectionKernel();
    updateHMI();
    
    // Run Blynk system framework if connection is sustained
    if (Blynk.connected()) {
        Blynk.run();
    }
    handleCloudTelemetry();
}