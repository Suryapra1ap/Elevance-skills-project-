#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define VOLTAGE_PIN 34
#define RELAY_PIN 26
#define BUZZER_PIN 27

LiquidCrystal_I2C lcd(0x27, 16, 2);

enum SystemState
{
  NORMAL,
  UNDER_VOLTAGE,
  OVER_VOLTAGE,
  SENSOR_FAULT,
  VOLTAGE_FLUCTUATION
};

SystemState currentState = NORMAL;

unsigned long sensorTimer = 0;
unsigned long lcdTimer = 0;
unsigned long recoveryTimer = 0;
unsigned long buzzerTimer = 0;

float voltage = 0;
float previousVoltage = 0;

bool relayState = true;
bool buzzerState = false;
bool recoveryRunning = false;

const float UV_LIMIT = 2.8;
const float UV_RECOVERY = 3.0;

const float OV_LIMIT = 4.2;
const float OV_RECOVERY = 4.1;

const float FLUCT_LIMIT = 0.5;

void setup()
{
  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(RELAY_PIN, HIGH);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0,0);
  lcd.print("Safety Kernel");
}

void loop()
{
  unsigned long now = millis();

  sensorTask(now);
  faultTask(now);
  recoveryTask(now);
  relayTask();
  buzzerTask(now);
  lcdTask(now);
}

void sensorTask(unsigned long now)
{
  if(now - sensorTimer >= 200)
  {
    sensorTimer = now;

    int adc = analogRead(VOLTAGE_PIN);

    previousVoltage = voltage;

    voltage = (adc / 4095.0) * 5.0;

    Serial.print("Voltage: ");
    Serial.println(voltage);
  }
}

void faultTask(unsigned long now)
{
  if(voltage < 0 || voltage > 5)
  {
    currentState = SENSOR_FAULT;
    recoveryRunning = false;
    return;
  }

  if(abs(voltage - previousVoltage) > FLUCT_LIMIT)
  {
    currentState = VOLTAGE_FLUCTUATION;
    recoveryRunning = false;
    return;
  }

  if(voltage > OV_LIMIT)
  {
    currentState = OVER_VOLTAGE;
    recoveryRunning = false;
    return;
  }

  if(voltage < UV_LIMIT)
  {
    currentState = UNDER_VOLTAGE;
    recoveryRunning = false;
    return;
  }

  if(currentState != NORMAL)
  {
    if(!recoveryRunning)
    {
      recoveryRunning = true;
      recoveryTimer = now;
    }
  }
}

void recoveryTask(unsigned long now)
{
  if(!recoveryRunning)
    return;

  bool safeRange =
    voltage > UV_RECOVERY &&
    voltage < OV_RECOVERY;

  if(safeRange)
  {
    if(now - recoveryTimer >= 5000)
    {
      currentState = NORMAL;
      recoveryRunning = false;
    }
  }
  else
  {
    recoveryTimer = now;
  }
}

void relayTask()
{
  if(currentState == NORMAL)
  {
    relayState = true;
  }
  else
  {
    relayState = false;
  }

  digitalWrite(RELAY_PIN, relayState);
}

void buzzerTask(unsigned long now)
{
  if(currentState == NORMAL)
  {
    digitalWrite(BUZZER_PIN, LOW);
    return;
  }

  if(now - buzzerTimer >= 500)
  {
    buzzerTimer = now;

    buzzerState = !buzzerState;

    digitalWrite(BUZZER_PIN, buzzerState);
  }
}

void lcdTask(unsigned long now)
{
  if(now - lcdTimer < 500)
    return;

  lcdTimer = now;

  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("V:");
  lcd.print(voltage,2);

  lcd.setCursor(0,1);

  switch(currentState)
  {
    case NORMAL:
      lcd.print("SYSTEM NORMAL");
      break;

    case UNDER_VOLTAGE:
      lcd.print("LOW VOLTAGE");
      break;

    case OVER_VOLTAGE:
      lcd.print("OVER VOLTAGE");
      break;

    case SENSOR_FAULT:
      lcd.print("SENSOR ERROR");
      break;

    case VOLTAGE_FLUCTUATION:
      lcd.print("UNSTABLE CELL");
      break;
  }
}