#include "beelogo.h"
#include "display.h"
#include "temperature.h"

void setup() {
  Serial.begin(115200);
  DS18B20.begin();

  for (int i = 1; i <= 4; ++i) {
        testdrawbitmap();
        }

  textdisplay();
  thanks();
  temperature();

  ina3221.begin();
  ina3221.reset();
  ina3221.setShuntRes(100, 100, 100); // Set shunt resistors to 100 mOhm for all channels
}

void loop() {
  float current[3];
  float voltage[3];
  
  current[0] = ina3221.getCurrent(INA3221_CH1);
  voltage[0] = ina3221.getVoltage(INA3221_CH1);
  float power1 = current[0] * voltage[0];
  float battpercent = ((voltage[0]-3.4)/(4.3-3.4))*100;
  
  current[1] = ina3221.getCurrent(INA3221_CH2);
  voltage[1] = ina3221.getVoltage(INA3221_CH2);
  float power2 = current[1] * voltage[1];
  
  current[2] = ina3221.getCurrent(INA3221_CH3);
  voltage[2] = ina3221.getVoltage(INA3221_CH3);
  float power3 = current[2] * voltage[2];

  general();
  display.println("At Batt:");
  display.print(current[0], PRINT_DEC_POINTS);
  display.println("A, ");
  display.print(voltage[0], PRINT_DEC_POINTS);
  display.println("V, ");
  display.print(power1);
  display.println("W");
  displaytime(4000);
  
  if (battpercent<0) {
    general();
    display.println("Please");
    display.println("Connect");
    display.println("To ");
    display.println("Battery.");
    displaytime(4000);
   } else if(battpercent<=10){
      for (int i = 1; i <= 6; ++i) {
        lowbatt();
         }
      general();
      display.println("Battery");
      display.println("Percent:");
      display.print(battpercent);
      display.println("%");
      displaytime(2000);
  
   }else{
    general();
    display.println("Battery");
    display.println("Percent:");
    display.print(battpercent);
    display.println("%");
    displaytime(4000);
   }
  
  general();
  display.println("B4 Batt:");
  display.print(current[1], PRINT_DEC_POINTS);
  display.println("A, ");
  display.print(voltage[1], PRINT_DEC_POINTS);
  display.println("V, ");
  display.print(power2);
  display.println("W");
  displaytime(4000);
  
  general();
  display.println("B4 FWR:");
  display.print(current[2], PRINT_DEC_POINTS);
  display.println("A, ");
  display.print(voltage[2], PRINT_DEC_POINTS);
  display.println("V, ");
  display.print(power3);
  display.println("W");
  displaytime(4000);

}
