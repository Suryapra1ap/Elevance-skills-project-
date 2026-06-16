#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define CELL1_PIN 34
#define CELL2_PIN 35
#define CELL3_PIN 32
#define CELL4_PIN 33

LiquidCrystal_I2C lcd(0x27, 16, 2);

float cell[4];

float readCellVoltage(int pin)
{
  int adc = analogRead(pin);
  return (adc * 4.2) / 4095.0;
}

String evaluateHealth(float imbalance, float minVoltage)
{
  if (minVoltage < 3.0)
    return "PACK FAILURE";

  if (imbalance > 15.0)
    return "PACK FAILURE";

  if (imbalance >= 7.0)
    return "CRITICAL";

  if (imbalance >= 3.0)
    return "MINOR";

  return "HEALTHY";
}

void setup()
{
  Serial.begin(115200);

  analogReadResolution(12);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Battery Engine");
  lcd.setCursor(0, 1);
  lcd.print("Initializing");
  delay(2000);

  lcd.clear();
}

void loop()
{
  float packVoltage = 0;

  cell[0] = readCellVoltage(CELL1_PIN);
  cell[1] = readCellVoltage(CELL2_PIN);
  cell[2] = readCellVoltage(CELL3_PIN);
  cell[3] = readCellVoltage(CELL4_PIN);

  float maxV = cell[0];
  float minV = cell[0];

  int strongest = 0;
  int weakest = 0;

  for (int i = 0; i < 4; i++)
  {
    packVoltage += cell[i];

    if (cell[i] > maxV)
    {
      maxV = cell[i];
      strongest = i;
    }

    if (cell[i] < minV)
    {
      minV = cell[i];
      weakest = i;
    }
  }

  float avgVoltage = packVoltage / 4.0;

  float imbalance =
    ((maxV - minV) / avgVoltage) * 100.0;

  String health =
    evaluateHealth(imbalance, minV);

  // Serial Output
  Serial.println("================================");
  for (int i = 0; i < 4; i++)
  {
    Serial.print("Cell ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(cell[i], 3);
    Serial.println(" V");
  }

  Serial.print("Pack Voltage: ");
  Serial.println(packVoltage, 3);

  Serial.print("Average Voltage: ");
  Serial.println(avgVoltage, 3);

  Serial.print("Strongest Cell: ");
  Serial.println(strongest + 1);

  Serial.print("Weakest Cell: ");
  Serial.println(weakest + 1);

  Serial.print("Imbalance: ");
  Serial.print(imbalance, 2);
  Serial.println("%");

  Serial.print("Status: ");
  Serial.println(health);

  // LCD Screen 1
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Pack:");
  lcd.print(packVoltage, 1);
  lcd.print("V");

  lcd.setCursor(0, 1);
  lcd.print("Avg:");
  lcd.print(avgVoltage, 1);
  lcd.print("V");

  delay(3000);

  // LCD Screen 2
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("S:");
  lcd.print(strongest + 1);

  lcd.print(" W:");
  lcd.print(weakest + 1);

  lcd.setCursor(0, 1);
  lcd.print("Imb:");
  lcd.print(imbalance, 1);
  lcd.print("%");

  delay(3000);

  // LCD Screen 3
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Battery State");

  lcd.setCursor(0, 1);
  lcd.print(health);

  delay(3000);
}