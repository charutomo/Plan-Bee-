// ILI9341 example with embedded color bitmaps in sketch.
// WILL NOT FIT ON ARDUINO UNO OR OTHER AVR BOARDS;
// uses large bitmap image stored in array!

// Options for converting images to the format used here include:
//   http://www.rinkydinkelectronics.com/t_imageconverter565.php
// or
//  GIMP (https://www.gimp.org/) as follows:
//    1. File -> Export As
//    2. In Export Image dialog, use 'C source code (*.c)' as filetype.
//    3. Press export to get the export options dialog.
//    4. Type the desired variable name into the 'prefixed name' box.
//    5. Uncheck 'GLIB types (guint8*)'
//    6. Check 'Save as RGB565 (16-bit)'
//    7. Press export to save your image.
//  Assuming 'image_name' was typed in the 'prefixed name' box of step 4,
//  you can have to include the c file, then using the image can be done with:
//    tft.drawRGBBitmap(0, 0, image_name.pixel_data, image_name.width, image_name.height);
//  See also https://forum.pjrc.com/threads/35575-Export-for-ILI9341_t3-with-GIMP

#include "SPI.h"
#include "Adafruit_ILI9341.h"
#include "Bee.h"
#include <Wire.h>
#include <Beastdevices_INA3221.h>
#include <SPI.h>
#include <DallasTemperature.h>


#define TFT_CLK    32
#define TFT_MOSI   33
#define TFT_MISO   12 
#define TFT_CS     27 
#define TFT_DC     26 
#define TFT_RST    25 

#define PRINT_DEC_POINTS  3  

#define SENSOR_PIN  35 // ESP32 pin GPIO35 connected to DS18B20 sensor's DQ pin

Beastdevices_INA3221 ina3221(INA3221_ADDR40_GND);

OneWire oneWire(SENSOR_PIN);
DallasTemperature DS18B20(&oneWire);

float tempC; // temperature in Celsius
float tempF; // temperature in Fahrenheit

//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
// If using the breakout, change pins as desired
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

void setup() {
  tft.begin();
  DS18B20.begin();
  tft.fillScreen(ILI9341_WHITE);
  tft.setRotation(0);
  Serial.begin(115200);
  tft.drawRGBBitmap(0, 0, Plan_Bee_Logo_ALIVE, BEE_WIDTH,BEE_HEIGHT);
  delay(500);
  textdisplay();
  temperature();

  delay(200);

  ina3221.begin();
  ina3221.reset();

  // Set shunt resistors to 10 mOhm for all channels
  ina3221.setShuntRes(100, 100, 100);
  
}

void loop(void) {
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

  if (battpercent<0) {
    
    tft.fillScreen(ILI9341_WHITE);
    tft.setCursor(0, 20);
    tft.setTextColor(ILI9341_BLACK);
    tft.println("Please connect to battery");
    tft.println();
    tft.println("Charging Power:");
    tft.print(current[1], PRINT_DEC_POINTS);
    tft.println("A, ");
    tft.print(voltage[1], PRINT_DEC_POINTS);
    tft.println("V, ");
    tft.print(power2);
    tft.println("W");
  }

  else if(battpercent<=10){
    for (int i = 1; i <= 4; ++i) {
      tft.drawRGBBitmap(0, 0, Plan_Bee_Logo_DEAD, BEE_WIDTH,BEE_HEIGHT);
      tft.fillScreen(ILI9341_WHITE);
    }
    tft.fillScreen(ILI9341_WHITE);
    tft.setCursor(0, 20);
    tft.setTextColor(ILI9341_BLACK);
    tft.println("Charging Power:");
    tft.print(current[1], PRINT_DEC_POINTS);
    tft.println("A, ");
    tft.print(voltage[1], PRINT_DEC_POINTS);
    tft.println("V, ");
    tft.print(power2);
    tft.println("W");
    tft.println();
    tft.println("Battery Percentage:");
    tft.print(battpercent);
    tft.println("%");
    tft.println();
    
  }

  else{
  tft.fillScreen(ILI9341_WHITE);
  tft.setCursor(0, 20);
  tft.setTextColor(ILI9341_BLACK);
  tft.println("Charging Power:");
  tft.print(current[1], PRINT_DEC_POINTS);
  tft.println("A, ");
  tft.print(voltage[1], PRINT_DEC_POINTS);
  tft.println("V, ");
  tft.print(power2);
  tft.println("W");
  tft.println();
  
  tft.println("Battery Percentage:");
  tft.print(battpercent);
  tft.println("%");
  tft.println();
  }
  delay(1000);

}

void textdisplay(void){
  tft.fillScreen(ILI9341_WHITE);
  tft.setCursor(0, 20);
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_YELLOW);
  tft.println("Plan Bee");
  tft.setTextColor(ILI9341_BLACK);
  tft.println("Group Members:");
  tft.println("Andy,Jia Woei,Jinghui,");
  tft.println("Chermaine and Charissa");
  tft.println();
  tft.println("With thanks to Tony and Qi");
  tft.println("Jie");
  delay(1000);
  
}

void temperature(void){
  tft.fillScreen(ILI9341_WHITE);
  tft.setCursor(0, 20);
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_BLACK);
  DS18B20.requestTemperatures();       // send the command to get temperatures
  tempC = DS18B20.getTempCByIndex(0);  // read temperature in °C
  tempF = tempC * 9 / 5 + 32;
  tft.println("Temperature:");
  tft.print(tempC);
  tft.println("°C");
  tft.print(tempF);
  tft.println("°F");
  delay(1000);
}
