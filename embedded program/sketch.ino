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

// Cloud Override Flag (Task 6 Remote Kill-Switch)
bool remoteShutdownActive = false; 

// Task 6: Executive Analytics Variables
String operatorAdvisory = "SYSTEM READY";
int riskIndex = 0; // 0=Nominal, 1=Low Risk, 2=Medium Risk, 3=Critical Hazard

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
// Blynk Cloud Incoming Command Handler
// ==========================================
// V9 is configured as a Switch widget (0 = System Active, 1 = Remote Shutdown Force-Trip)
BLYNK_WRITE(V9) {
    int switchState = param.asInt();
    if (switchState == 1) {
        remoteShutdownActive = true;
        Serial.println("WARN: Remote Emergency Shutdown Command Received from Blynk cloud!");
    } else {
        remoteShutdownActive = false;
        Serial.println("INFO: Remote Emergency Shutdown Released.");
    }
}

// ==========================================
// 1. Adaptive Battery Intelligence Engine
// ==========================================
void runBatteryIntelligenceEngine() {
    float sum = 0;
    bmsPack.maxVoltage = 0.0;
    bmsPack.minVoltage = 5.0;
    
    for (int i = 0; i < 4; i++) {
        int rawAdc = analogRead(CELL_PINS[i]);
        float mappedVolt = (rawAdc / 4095.0) * 4.5; 
        bmsPack.cellVoltages[i] = mappedVolt;
        sum += mappedVolt;
        
        if (mappedVolt > bmsPack.maxVoltage) { bmsPack.maxVoltage = mappedVolt; bmsPack.strongestCell = i + 1; }
        if (mappedVolt < bmsPack.minVoltage) { bmsPack.minVoltage = mappedVolt; bmsPack.weakestCell = i + 1; }
    }
    
    bmsPack.totalVoltage = sum;
    bmsPack.averageVoltage = sum / 4.0;
    
    if (bmsPack.averageVoltage > 0) {
        bmsPack.imbalancePercent = ((bmsPack.maxVoltage - bmsPack.minVoltage) / bmsPack.averageVoltage) * 100.0;
    } else {
        bmsPack.imbalancePercent = 0;
    }

    if (bmsPack.imbalancePercent >= IMBALANCE_CRITICAL || 
        bmsPack.minVoltage < V_UNDERVOLTAGE || 
        bmsPack.maxVoltage > V_OVERVOLTAGE) { 
        bmsPack.health = PACK_FAILURE;
    } else if (bmsPack.imbalancePercent >= IMBALANCE_MINOR) {
        bmsPack.health = CRITICAL_IMBALANCE;
    } else {
        bmsPack.health = HEALTHY;
    }
}

// ==========================================
// 2. Fault-Tolerant Runtime Subsystem
// ==========================================
void runFaultTolerantKernel() {
    // Priority 1: Cloud Emergency Shutdown Command
    if (remoteShutdownActive) {
        currentRuntime = SHUTDOWN;
        return; 
    }

    // Priority 2: Sensor Wire Anomalies
    bool sensorAnomaly = false;
    for (int i = 0; i < 4; i++) {
        if (bmsPack.cellVoltages[i] < 0.1 || bmsPack.cellVoltages[i] > 4.49) {
            sensorAnomaly = true;
        }
    }

    if (sensorAnomaly) {
        currentRuntime = DEGRADED;
    } else if (bmsPack.health == PACK_FAILURE) {
        currentRuntime = FAILSAFE;
    } else {
        currentRuntime = NORMAL;
    }
}

// ==========================================
// 3. Event-Driven Safety Protection Kernel
// ==========================================
void runSafetyProtectionKernel() {
    unsigned long now = millis();
    static RuntimeState lastRuntime = SHUTDOWN; 
    
    if (currentRuntime != lastRuntime) {
        switch (currentRuntime) {
            case NORMAL:
                digitalWrite(RELAY_PIN, HIGH);
                digitalWrite(FAULT_LED, LOW);
                noTone(BUZZER_PIN);
                break;
                
            case DEGRADED:
                digitalWrite(RELAY_PIN, HIGH);
                noTone(BUZZER_PIN);
                break;
                
            case FAILSAFE:
            case SHUTDOWN:
                if (digitalRead(RELAY_PIN) == HIGH) {
                    digitalWrite(RELAY_PIN, LOW); // Force isolation instantly
                    lastRelayAction = now;
                }
                tone(BUZZER_PIN, 1000);
                break;
        }
        lastRuntime = currentRuntime;
    }
    
    if (currentRuntime == DEGRADED) {
        if ((now / 500) % 2 == 0) digitalWrite(FAULT_LED, HIGH);
        else digitalWrite(FAULT_LED, LOW);
    } 
    else if (currentRuntime == FAILSAFE || currentRuntime == SHUTDOWN) {
        digitalWrite(FAULT_LED, HIGH);
    }
}

// ==========================================
// 4. Intelligent Embedded HMI & Interface
// ==========================================
void updateHMI() {
    unsigned long now = millis();
    
    if (now - lastScreenRotate > 4000) {
        lastScreenRotate = now;
        currentScreen = static_cast<HmiScreen>((currentScreen + 1) % 3);
        lcd.clear();
    }
    
    if (currentRuntime == FAILSAFE || currentRuntime == SHUTDOWN) {
        lcd.setCursor(0, 0); 
        if (remoteShutdownActive) {
            lcd.print("! REMOTE SHUTDOWN ! ");
        } else {
            lcd.print("!! SAFETY CRITICAL !!");
        }
        lcd.setCursor(0, 1); lcd.print("RELAY: ISOLATED     ");
        lcd.setCursor(0, 2); lcd.print("FAULT DETECTED      ");
        return;
    }

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
// 6. Executive Diagnostic & Advisory Engine
// ==========================================
void runDiagnosticEngine() {
    if (remoteShutdownActive) {
        riskIndex = 3;
        operatorAdvisory = "REMOTE OVERRIDE: Emergency system isolation active from Cloud HMI.";
        return;
    }

    if (currentRuntime == NORMAL) {
        riskIndex = 0; 
        if (bmsPack.imbalancePercent > 1.0) {
            operatorAdvisory = "Optimizing: Passive cell balancing active.";
        } else {
            operatorAdvisory = "Nominal: Battery asset performing within spec.";
        }
    } 
    else if (currentRuntime == DEGRADED) {
        riskIndex = 1; 
        for (int i = 0; i < 4; i++) {
            if (bmsPack.cellVoltages[i] < 0.1) {
                operatorAdvisory = "Hardware Warning: Cell " + String(i + 1) + " open circuit detected. Verify harness.";
                riskIndex = 2;
            } else if (bmsPack.cellVoltages[i] > 4.49) {
                operatorAdvisory = "Sensor Warning: Cell " + String(i + 1) + " ADC rail fault. Check bounds.";
                riskIndex = 2;
            }
        }
    } 
    else if (currentRuntime == FAILSAFE) {
        riskIndex = 3; 
        if (bmsPack.maxVoltage > V_OVERVOLTAGE) {
            operatorAdvisory = "CRITICAL FAULT: Cell " + String(bmsPack.strongestCell) + " Overcharge (>4.25V)! Safety relay tripped.";
        } else if (bmsPack.minVoltage < V_UNDERVOLTAGE) {
            operatorAdvisory = "CRITICAL FAULT: Cell " + String(bmsPack.weakestCell) + " Deep Discharge (<3.00V)! Isolate pack immediately.";
        } else if (bmsPack.imbalancePercent >= IMBALANCE_CRITICAL) {
            operatorAdvisory = "CRITICAL FAULT: Pack Delta > 10% structural failure. Maintenance required.";
        } else {
            operatorAdvisory = "CRITICAL FAULT: Protective system automatic trip active.";
        }
    }
}

// ==========================================
// 5. Intelligent Cloud Telemetry Architecture
// ==========================================
void handleCloudTelemetry() {
    unsigned long now = millis();
    
    if (Blynk.connected() && (now - lastTelemetryUpdate > 1500)) {
        lastTelemetryUpdate = now;
        
        Blynk.virtualWrite(V0, bmsPack.totalVoltage);
        Blynk.virtualWrite(V1, bmsPack.cellVoltages[0]);
        Blynk.virtualWrite(V2, bmsPack.cellVoltages[1]);
        Blynk.virtualWrite(V3, bmsPack.cellVoltages[2]);
        Blynk.virtualWrite(V4, bmsPack.cellVoltages[3]);
        Blynk.virtualWrite(V5, bmsPack.imbalancePercent);
        Blynk.virtualWrite(V6, currentRuntime);
        Blynk.virtualWrite(V7, operatorAdvisory);
        Blynk.virtualWrite(V8, riskIndex);
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(FAULT_LED, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    
    digitalWrite(RELAY_PIN, HIGH);
    
    lcd.init();
    lcd.backlight();
    
    Blynk.begin(auth, ssid, pass);
}

void loop() {
    runBatteryIntelligenceEngine();
    runFaultTolerantKernel();
    runSafetyProtectionKernel();
    runDiagnosticEngine(); 
    updateHMI();
    
    if (Blynk.connected()) {
        Blynk.run();
    }
    handleCloudTelemetry();
}