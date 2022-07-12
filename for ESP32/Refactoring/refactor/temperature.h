#include <DallasTemperature.h>

#define SENSOR_PIN  21 // ESP32 pin GPIO21 connected to DS18B20 sensor's DQ pin

OneWire oneWire(SENSOR_PIN);
DallasTemperature DS18B20(&oneWire);
float tempC; // temperature in Celsius
float tempF; // temperature in Fahrenheit

void temperature(void){
  DS18B20.requestTemperatures();       // send the command to get temperatures
  tempC = DS18B20.getTempCByIndex(0);  // read temperature in °C
  tempF = tempC * 9 / 5 + 32;
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Temp:");
  display.print(tempC);
  display.println("°C");
  display.print(tempF);
  display.println("°F");
  display.display();
  delay(2000);
}
