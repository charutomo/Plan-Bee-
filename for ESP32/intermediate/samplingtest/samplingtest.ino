#include <Wire.h>
#include <Beastdevices_INA3221.h>

#define SERIAL_SPEED      115200    // serial baud rate
#define PRINT_DEC_POINTS  3         // decimal points to print

// Set I2C address to 0x41 (A0 pin -> VCC)
Beastdevices_INA3221 ina3221(INA3221_ADDR40_GND);

float currentarray1[20];
float voltagearray1[20];
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
  if (numloop<20){
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

  Serial.print("Current Array:");
  for (int i = 0; i < 20; i = i + 1) {
    Serial.print(currentarray1[i]);
    Serial.print(", ");
  }

  
  Serial.println();
  Serial.print("Voltage Array:");
  for (int i = 0; i < 20; i = i + 1) {
    Serial.print(voltagearray1[i]);
    Serial.print(", ");
  }
  Serial.println();
  numloop+=1;
  delay(10);
  }
  else if(numloop == 20){
    Serial.print("Average Current:");
    avgvalue(currentarray1);
    Serial.print("Average Voltage:");
    avgvalue(voltagearray1);
    numloop=0;
    for (int i=0; i<20; i++){
      currentarray1[i]=0;
      voltagearray1[i]=0;
    delay(500);
    Serial.println("New Loop");
  }
  
  }
  
}

void avgvalue(float arr1[20]){
  float totalval = 0;
  for (int i = 0; i < 20; i++){
    totalval += arr1[i];
  }
  float avgval = totalval/20;
  Serial.println(avgval);
  Serial.println();
}
