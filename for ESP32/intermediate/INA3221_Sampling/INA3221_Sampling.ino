#include <Wire.h>
#include <Beastdevices_INA3221.h>

#define SERIAL_SPEED      115200    // serial baud rate
#define PRINT_DEC_POINTS  3         // decimal points to print

// Set I2C address to 0x41 (A0 pin -> VCC)
Beastdevices_INA3221 ina3221(INA3221_ADDR40_GND);

float currentarray1[10];
float voltagearray1[10];
int numloop = 0;

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
  if (numloop<=10){
  float current[3];
  float voltage[3];

  current[0] = ina3221.getCurrent(INA3221_CH1);
  currentarray1[numloop] = ina3221.getCurrent(INA3221_CH1);
  voltage[0] = ina3221.getVoltage(INA3221_CH1);
  voltagearray1[numloop] = ina3221.getVoltage(INA3221_CH1);

  /*current[1] = ina3221.getCurrent(INA3221_CH2);
  voltage[1] = ina3221.getVoltage(INA3221_CH2);

  current[2] = ina3221.getCurrent(INA3221_CH3);
  voltage[2] = ina3221.getVoltage(INA3221_CH3);*/

  Serial.print("Channel 1: ");
  Serial.print(current[0], PRINT_DEC_POINTS);
  Serial.print("A, ");
  Serial.print(voltage[0], PRINT_DEC_POINTS);
  Serial.println("V");
  Serial.println();

  /*Serial.print("Channel 2: ");
  Serial.print(current[1], PRINT_DEC_POINTS);
  Serial.print("A, ");
  Serial.print(voltage[1], PRINT_DEC_POINTS);
  Serial.println("V");

  Serial.print("Channel 3: ");
  Serial.print(current[2], PRINT_DEC_POINTS);
  Serial.print("A, ");
  Serial.print(voltage[2], PRINT_DEC_POINTS);
  Serial.println("V");
  Serial.println();*/

  for (int i = 0; i < 10; i = i + 1) {
    Serial.print(currentarray1[i]);
    Serial.print(", ");
  }
  
  Serial.println();
  
  for (int i = 0; i < 10; i = i + 1) {
    Serial.print(voltagearray1[i]);
    Serial.print(", ");
  }
  Serial.println();

  avgvalue(currentarray1);
  avgvalue(voltagearray1);
  numloop+=1;
  }
  delay(100);
}

void avgvalue(float arr1[10]){
  float totalval = 0;
  for (int i = 0; i < 10; i++){
    totalval += arr1[i];
  }
  float avgval = totalval/10;
  Serial.println(avgval);
  Serial.println();
}
