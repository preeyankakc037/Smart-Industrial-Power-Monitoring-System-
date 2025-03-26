#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Initialize the LCDs
LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD 1
LiquidCrystal_I2C lcd1(0x26, 16, 2); // LCD 2

// Define sensor and device pins
const int pot1Pin = A0; // Potentiometer 1
const int pot2Pin = A1; // Potentiometer 2
const int ldr = A2;     // LDR
const int tempSensorPin = A3; // Temperature sensor
const int pirSensorPin = 8; // PIR sensor moved to pin 8
const int pirLedPin = 13; // LED controlled by PIR sensor moved to pin 13

// Define LEDs and buzzer pins
#define red 2
#define blue 3
#define green 4
#define buzzer 5
#define led 6 // LED controlled by LDR
#define tempLedPin 7 // Temperature-controlled LED

// Temperature thresholds
const float TEMP_HIGH_THRESHOLD = 50.0;  // High temperature above 50°C
const float TEMP_NORMAL_THRESHOLD_LOW = 40.0; // Normal temperature between 40°C and 50°C

void setup() {
  // Initialize LCDs
  lcd.init();
  lcd.backlight();
  lcd1.init();
  lcd1.backlight();

  // Startup message
  lcd.setCursor(0, 0);
  lcd.print("Smart Monitor");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(2000);
  lcd.clear();
  lcd1.clear();

  // Set LED and buzzer pins as output
  pinMode(red, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(led, OUTPUT); // LDR-controlled LED
  pinMode(tempLedPin, OUTPUT); // Temperature-controlled LED
  pinMode(pirLedPin, OUTPUT); // PIR-controlled LED
  pinMode(ldr, INPUT); // LDR input pin
  pinMode(pirSensorPin, INPUT); // PIR sensor input

  // Initialize Serial Monitor for debugging
  Serial.begin(9600);
}

void loop() {
  // Read sensor values
  int pot1Value = analogRead(pot1Pin);
  int pot2Value = analogRead(pot2Pin);
  int ldrValue = analogRead(ldr);
  int tempValue = analogRead(tempSensorPin);
  int pirValue = digitalRead(pirSensorPin); // Read PIR sensor state

  // Convert potentiometer readings to voltage
  float voltage1 = pot1Value * (14.0 / 1023.0); // Adjust range if necessary
  float voltage2 = pot2Value * (14.0 / 1023.0); // Adjust range if necessary

  // Convert temperature sensor value to °C
  float temperature = (tempValue * 5.0 / 1023.0) * 100;

  // --- LCD 1: Voltage and Day/Night Mode ---
  lcd.setCursor(0, 0);
  if (ldrValue < 500) { // Night mode threshold
    lcd.print("Night Mode       ");
    digitalWrite(led, HIGH); // Turn on LED during night
  } else {
    lcd.print("Day Mode         ");
    digitalWrite(led, LOW); // Turn off LED during day
  }

  lcd.setCursor(0, 1);
  if (voltage1 > 12 || voltage2 > 12) {
    lcd.print("High Volt ALERT  ");
    digitalWrite(red, HIGH);
    digitalWrite(green, LOW);
    digitalWrite(blue, LOW);
    digitalWrite(buzzer, HIGH);
  } else if (voltage1 > 5 && voltage2 > 5) {
    lcd.print("Voltage Normal   ");
    digitalWrite(green, HIGH);
    digitalWrite(red, LOW);
    digitalWrite(blue, LOW);
    digitalWrite(buzzer, LOW);
  } else {
    lcd.print("Low Volt ALERT   ");
    digitalWrite(blue, HIGH);
    digitalWrite(green, LOW);
    digitalWrite(red, LOW);
    digitalWrite(buzzer, HIGH);
  }

  // --- LCD 2: Temperature Status ---
  lcd1.setCursor(0, 0);
  lcd1.print("Temp Status:     ");

  lcd1.setCursor(0, 1);
  if (temperature > TEMP_HIGH_THRESHOLD) {
    lcd1.print("High Temp ALERT  ");
    digitalWrite(tempLedPin, HIGH); // Turn on LED for high temperature
  } else if (temperature >= TEMP_NORMAL_THRESHOLD_LOW && temperature <= TEMP_HIGH_THRESHOLD) {
    lcd1.print("Temp Normal      ");
    digitalWrite(tempLedPin, LOW); // Turn off LED for normal temperature
  } else {
    lcd1.print("Low Temp ALERT   ");
    digitalWrite(tempLedPin, HIGH); // Turn on LED for low temperature
  }

  // --- PIR Sensor LED Control ---
  if (pirValue == HIGH) { // Motion detected
    Serial.println("Motion Detected!");
    digitalWrite(pirLedPin, HIGH); // Turn on LED when motion detected
    delay(1000);
  } else {
    digitalWrite(pirLedPin, LOW); // Turn off LED when no motion
  }

  // Debugging via Serial Monitor
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");
  Serial.print("Voltage1: ");
  Serial.println(voltage1);
  Serial.print("Voltage2: ");
  Serial.println(voltage2);
  Serial.print("LDR Value: ");
  Serial.println(ldrValue);
  Serial.print("PIR Sensor: ");
  Serial.println(pirValue);

  delay(1000); // Update every second
}
