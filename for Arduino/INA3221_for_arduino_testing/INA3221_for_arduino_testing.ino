#include <Wire.h>
#include <Beastdevices_INA3221.h>

#define SERIAL_SPEED      115200    // serial baud rate
#define PRINT_DEC_POINTS  3         // decimal points to print

// Set I2C address to 0x40 (A0 pin -> GND)
Beastdevices_INA3221 ina3221(INA3221_ADDR40_GND);

void setup() {
  Serial.begin(SERIAL_SPEED);
  while (!Serial) {
      delay(1);
  }

  ina3221.begin();
  ina3221.reset();

  // Set shunt resistors to 10 mOhm for all channels
  ina3221.setShuntRes(100, 100, 100);
}

void loop() {
  float current[3];
  float voltage[3];

  current[0] = ina3221.getCurrent(INA3221_CH1);
  voltage[0] = ina3221.getVoltage(INA3221_CH1);
  float power1 = current[0] * voltage[0];

  current[1] = ina3221.getCurrent(INA3221_CH2);
  voltage[1] = ina3221.getVoltage(INA3221_CH2);
  float power2 = current[1] * voltage[1];

  current[2] = ina3221.getCurrent(INA3221_CH3);
  voltage[2] = ina3221.getVoltage(INA3221_CH3);
  float power3 = current[2] * voltage[2];

  Serial.print("Channel 1: ");
  Serial.print(current[0], PRINT_DEC_POINTS);
  Serial.print("A, ");
  Serial.print(voltage[0], PRINT_DEC_POINTS);
  Serial.print("V, ");
  Serial.print(power1);
  Serial.println("W");

  Serial.print("Channel 2: ");
  Serial.print(current[1], PRINT_DEC_POINTS);
  Serial.print("A, ");
  Serial.print(voltage[1], PRINT_DEC_POINTS);
  Serial.print("V, ");
  Serial.print(power2);
  Serial.println("W");

  Serial.print("Channel 3: ");
  Serial.print(current[2], PRINT_DEC_POINTS);
  Serial.print("A, ");
  Serial.print(voltage[2], PRINT_DEC_POINTS);
  Serial.print("V, ");
  Serial.print(power3);
  Serial.println("W");
  delay(1000);
  }
