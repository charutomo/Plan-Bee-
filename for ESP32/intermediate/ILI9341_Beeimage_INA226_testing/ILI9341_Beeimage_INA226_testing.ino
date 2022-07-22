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
#include <INA226_WE.h>
#include <SPI.h>
#include <DallasTemperature.h>
#define I2C_ADDRESS0 0x40 //battery channel: measure battery percentage
#define 12C_ADDRESS1 0X41 //charging channel: measure charging voltage and current

/*For connecting ESP32 to INA226:
 * VCC --> 3V3
 * GND --> GND
 * SDA --> PIN 21
 * SCL --> PIN 22
 * ALE --> PIN 25
 * VBUS and IN- --> negative terminal / VCC of load
 * IN+ --> Positive terminal of power supply
 */
 
/* For ILI9341: 
 * VCC --> 5V
 * GND --> GND 
 * LED --> 3V3
 */
#define TFT_CLK    32 //pin to SCK of ILI9341
#define TFT_MOSI   33
#define TFT_MISO   12 
#define TFT_CS     27 
#define TFT_DC     26 
#define TFT_RST    17 //originally 25

#define PRINT_DEC_POINTS  3  

#define SENSOR_PIN  35 // ESP32 pin GPIO35 connected to DS18B20 sensor's DQ pin

OneWire oneWire(SENSOR_PIN);
DallasTemperature DS18B20(&oneWire);

float tempC; // temperature in Celsius
float tempF; // temperature in Fahrenheit

//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
// If using the breakout, change pins as desired
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

//for INA226
INA226_WE ina226batt = INA226_WE(I2C_ADDRESS0); //battery channel
INA226_WE ina226charging = INA226_WE(I2C_ADDRESS1); //charging channel

float shuntVoltage_mV = 0.0;
float loadVoltage_V = 0.0;
float busVoltage_V = 0.0;
float current_mA = 0.0;
float power_mW = 0.0; 

void setup() {
  tft.begin();
  DS18B20.begin();
  Wire.begin();
  tft.fillScreen(ILI9341_WHITE);
  tft.setRotation(0);
  //Serial.begin(115200);
  tft.drawRGBBitmap(0, 0, Plan_Bee_Logo_ALIVE, BEE_WIDTH,BEE_HEIGHT);
  delay(1000);
  textdisplay();
  temperature();

  delay(200);

  ina226batt.init();
  ina226charging.init();
  
  /* Set Number of measurements for shunt and bus voltage which shall be averaged
  * Mode *     * Number of samples *
  AVERAGE_1            1 (default)
  AVERAGE_4            4
  AVERAGE_16           8
  AVERAGE_64          64
  AVERAGE_128        128
  AVERAGE_256        256
  AVERAGE_512        512
  AVERAGE_1024      1024
  */
  ina226batt.setAverage(AVERAGE_128); // choose mode and uncomment for change of default
  ina226charging.setAverage(AVERAGE_128);
  
  /* Set conversion time in microseconds
     One set of shunt and bus voltage conversion will take: 
     number of samples to be averaged x conversion time x 2
     
     * Mode *         * conversion time *
     CONV_TIME_140          140 µs
     CONV_TIME_204          204 µs
     CONV_TIME_332          332 µs
     CONV_TIME_588          588 µs
     CONV_TIME_1100         1.1 ms (default)
     CONV_TIME_2116       2.116 ms
     CONV_TIME_4156       4.156 ms
     CONV_TIME_8244       8.244 ms  
  */
  ina226batt.setConversionTime(CONV_TIME_1100); //choose conversion time and uncomment for change of default
  ina226charging.setConversionTime(CONV_TIME_1100);
  
  /* Set measure mode
  POWER_DOWN - INA226 switched off
  TRIGGERED  - measurement on demand
  CONTINUOUS  - continuous measurements (default)
  */
  ina226batt.setMeasureMode(TRIGGERED); // choose mode and uncomment for change of default
  ina226charging.setMeasureMode(TRIGGERED);
  
  /* Set Current Range
    * Mode *   * Max Current *
     MA_400          400 mA
     MA_800          800 mA (default)
  */
  ina226batt.setCurrentRange(MA_800); // choose gain and uncomment for change of default
  ina226charging.setCurrentRange(MA_800);
  
  /* If the current values delivered by the INA226 differ by a constant factor
     from values obtained with calibrated equipment you can define a correction factor.
     Correction factor = current delivered from calibrated equipment / current delivered by INA226
  */
  ina226batt.setCorrectionFactor(0.73); //arduino uno: 0.903, esp32: 0.73
  ina226charging.setCorrectionFactor(0.73);
  
  //tft.println("INA226 Current Sensor Example Sketch - Continuous"); //can change to triggered
   
  //ina226batt.waitUntilConversionCompleted(); //comment for triggered mode
  //ina226charging.waitUntilConversionCompleted();
  
}

void loop(void) {
  float current[3];
  float voltage[3];

  ina226batt.startSingleMeasurement();
  ina226batt.readAndClearFlags();
  ina226charging.startSingleMeasurement();
  ina226charging.readAndClearFlags();
  
  current[0] = ina226batt.getCurrent_mA(); //battery channel
  tft.print("Current[mA]: "); tft.println(current[0]);
  voltage[0] = ina226batt.getBusVoltage_V() + 0.13; //0.13V is added to caliberate the value for accuracy
  tft.print("Voltage [V]: "); tft.println(voltage[0]);
  
  float power0 = ina226batt.getBusPower();
  tft.print("Power [mW]: "); tft.println(power0);
  float battpercent = ((voltage[0]-3.4)/(4.3-3.4))*100;
  tft.println("Battery Percentage:");
  tft.print(battpercent);
  tft.println("%");
  tft.println();

  delay(5000);
  
  current[1] = ina226charging.getCurrent_mA(); //charging channel
  tft.print("Charging Current[mA]: "); tft.println(current[1]);
  voltage[1] = ina226charging.getBusVoltage_V(); //need to check if caliberation is needed
  tft.print("Charging Voltage [V]: "); tft.println(voltage[1]);
  
  float power1 = ina226charging.getBusPower();
  tft.print("Power [mW]: "); tft.println(power1);
  float battpercent = ((voltage[0]-3.4)/(4.3-3.4))*100;
  tft.println("Battery Percentage:");
  tft.print(battpercent);
  tft.println("%");
  tft.println();
  
  if (battpercent<0) {
    
    tft.fillScreen(ILI9341_WHITE);
    tft.setCursor(0, 20);
    tft.setTextColor(ILI9341_BLACK);
    tft.println("Please connect to battery");
    tft.println();
    tft.println("Charging Power:");
    tft.print(current[1], PRINT_DEC_POINTS);
    tft.println("mA, ");
    tft.print(voltage[1], PRINT_DEC_POINTS);
    tft.println("V, ");
    tft.print(power1);
    tft.println("mW"); 
    
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
    tft.println("mA, ");
    tft.print(voltage[1], PRINT_DEC_POINTS);
    tft.println("V, ");
    tft.print(power1);
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
  tft.println("mA, ");
  tft.print(voltage[1], PRINT_DEC_POINTS);
  tft.println("V, ");
  tft.print(power1);
  tft.println("mW");
  tft.println();

  tft.println("Battery Percentage:");
  tft.print(battpercent);
  tft.println("%");
  tft.println();
  }
  
  delay(5000);
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
  delay(3000);
  
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
