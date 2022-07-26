#include "SPI.h"
#include "Adafruit_ILI9341.h"
#include "Bee.h"
#include "AiEsp32RotaryEncoder.h"
#include "Arduino.h"
#include <Wire.h>
#include <INA226_WE.h>
#include <SPI.h>
#include <DallasTemperature.h>

/*For connecting ESP32 to INA226:
 VCC --> 3V3
 GND --> GND
 SDA --> PIN 21
 SCL --> PIN 22
 ALE --> PIN 18 
 VBUS and IN- --> negative terminal / VCC of load
 IN+ --> Positive terminal of power supply
 Negative terminal of power supply --> GND of ESP32 and load */
 
/* For ILI9341: 
 VCC --> 3v3
 GND --> GND 
 LED --> 3v3 */
#define TFT_CLK    32 
#define TFT_MOSI   33
#define TFT_MISO   12 
#define TFT_CS     27 
#define TFT_DC     26 
#define TFT_RST    25 

#define PRINT_DEC_POINTS  3  

#define SENSOR_PIN  0 // ESP32 pin GPIO0 connected to DS18B20 sensor's DQ pin

OneWire oneWire(SENSOR_PIN);
DallasTemperature DS18B20(&oneWire);

float tempC; // temperature in Celsius
float tempF; // temperature in Fahrenheit

//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
//If using the breakout, change pins as desired
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

//for INA226
INA226_WE ina226batt = INA226_WE(0x40); //battery channel
float shuntVoltage_mV = 0.0;
float loadVoltage_V = 0.0;
float busVoltage_V = 0.0;
float current_mA = 0.0;
float power_mW = 0.0; 

//encoder
#define ROTARY_ENCODER_A_PIN 14
#define ROTARY_ENCODER_B_PIN 17
#define ROTARY_ENCODER_BUTTON_PIN 16
#define ROTARY_ENCODER_VCC_PIN - 1 /* 27 put -1 of Rotary encoder Vcc is connected directly to 3,3V; else you can use declared output pin for powering rotary encoder */
#define ROTARY_ENCODER_STEPS 4

int menu_state = 0;
int value = 0;

//instead of changing here, rather change numbers above
AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS);

void IRAM_ATTR readEncoderISR() {
  rotaryEncoder.readEncoder_ISR();
}

void setup() {
  //initialize rotary encoder
  rotaryEncoder.begin();
  rotaryEncoder.setup(readEncoderISR);
  //set boundaries and if values should cycle or not
  //in this example we will set possible values between 0 and 1000;
  bool circleValues = false;
  rotaryEncoder.setBoundaries(0, 1000, circleValues); //minValue, maxValue, circleValues true|false (when max go to min and vice versa)

  /*Rotary acceleration introduced 25.2.2021.
   * in case range to select is huge, for example - select a value between 0 and 1000 and we want 785
   * without accelerateion you need long time to get to that number
   * Using acceleration, faster you turn, faster will the value raise.
   * For fine tuning slow down.
   */
  //rotaryEncoder.disableAcceleration(); //acceleration is now enabled by default - disable if you dont need it
  rotaryEncoder.setAcceleration(250); //or set the value - larger number = more accelearation; 0 or 1 means disabled acceleration
  
  tft.begin();
  DS18B20.begin();
  Wire.begin();
  tft.fillScreen(ILI9341_BLACK);
  tft.setRotation(0);
  tft.drawRGBBitmap(0, 0, Plan_Bee_Logo_ALIVE, BEE_WIDTH,BEE_HEIGHT);
  delay(1000);
  
  ina226batt.init();
  
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
  ina226batt.setConversionTime(CONV_TIME_8244); //choose conversion time and uncomment for change of default
  
  /* Set measure mode
  POWER_DOWN - INA226 switched off
  TRIGGERED  - measurement on demand
  CONTINUOUS  - continuous measurements (default)
  */
  ina226batt.setMeasureMode(TRIGGERED); // choose mode and uncomment for change of default
  
  /* Set Current Range
    * Mode *   * Max Current *
     MA_400          400 mA
     MA_800          800 mA (default)
  */
  ina226batt.setCurrentRange(MA_800); // choose gain and uncomment for change of default
  
  /* If the current values delivered by the INA226 differ by a constant factor
     from values obtained with calibrated equipment you can define a correction factor.
     Correction factor = current delivered from calibrated equipment / current delivered by INA226
  */
  ina226batt.setCorrectionFactor(0.72); 

  menu();
}

void loop() {
  if (rotaryEncoder.encoderChanged() && menu_state < 5) {
    value = rotaryEncoder.readEncoder();
    if (value % 4 == 0) {
      menu_state = 1;
    } else if (value % 4 == 1) {
      menu_state = 2;
    } else if (value % 4 == 2) {
      menu_state = 3;
    }
    else if (value % 4 == 3) {
      menu_state = 4;
    }
    changemenu();
  }
  if (rotaryEncoder.isEncoderButtonClicked()) {
    if (menu_state >= 5){
      menu_state = 0;
    } else if (value % 4 == 0) { 
      menu_state = 7;
    } else if (value % 4 == 1) {
      menu_state = 8;
    } else if (value % 4 == 2) {
      menu_state = 9;
    } 
    else if (value % 4 == 3) {
      menu_state = 10;
    } 
    changemenu();
  }
  if (menu_state>5){
    changemenu();
    delay(1000);
  }
}

void changemenu() {
  /* Menu values
  0: main menu without any selector
  1: main menu with selector on Battery Info
  2: main menu with selector on Temperature
  3: main menu with selector on Credits

  7: Battery Info Page
  8: Temperature Page 
  9: Credits Page
  */
  switch (menu_state) {
  case 0:
    menu();
    break;
  case 1:
    menu1();
    break;
  case 2:
    menu2();
    break;
  case 3:
    menu3();
    break;
  case 4:
    menu4();
    break;
  case 7:
    showbattery();
    break;
  case 8:
   showtemp();
   break;
  case 9:
    credits();
    break;
  case 10:
    techsupport();
    break;
  }
}

// menu with no selector
void menu() {
  tft.fillScreen(ILI9341_BLACK);
  tft.setRotation(0);
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setCursor(70, 40);
  tft.println("Battery Info");
  tft.setCursor(70, 70);
  tft.println("Temperature");
  tft.setCursor(70, 100);
  tft.println("Credits");
  tft.setCursor(70, 130);
  tft.println("Tech Support");
 
}

// menu with selector on 1
void menu1() {
  tft.fillRect(40,40,30,130, ILI9341_BLACK);
  tft.setCursor(40, 40);
  tft.print("->");
}

// menu with selector on 2
void menu2() {
  tft.fillRect(40,40,30,130, ILI9341_BLACK);
  tft.setCursor(40, 70);
  tft.print("->");
}

// menu with selector on 3
void menu3() {
  tft.fillRect(40,40,30,130, ILI9341_BLACK);
  tft.setCursor(40, 100);
  tft.print("->");
}

// menu with selector on 4
void menu4(){
  tft.fillRect(40,40,30,130, ILI9341_BLACK);
  tft.setCursor(40, 130);
  tft.print("->");
}


// show the battery value
void showbattery(void) {
  tft.fillScreen(ILI9341_BLACK);
  float current[2]; //no. of channels
  float voltage[2];

  ina226batt.startSingleMeasurement();
  ina226batt.readAndClearFlags();
  current[0] = ina226batt.getCurrent_mA();
  voltage[0] = ina226batt.getBusVoltage_V();
  float power0 = ina226batt.getBusPower();
  float battpercent = ((voltage[0]-3.4)/(4.3-3.4))*100;
 
  offsettext(0,3);
  tft.println("Battery");
  offsettext(40,2);
   tft.print("Current:");
  tft.print(current[0], PRINT_DEC_POINTS);
  tft.println("mA, ");
  offsettext(70,2);
  tft.print("Voltage:");
  tft.print(voltage[0], PRINT_DEC_POINTS);
  tft.println("V, ");
  offsettext(100,2);
  tft.print("Power: "); 
  tft.print(power0);
  tft.println("mW");
  offsettext(130,2);
  tft.println("Battery Percentage:");
  offsettext(160,2);
  tft.print(battpercent);
  tft.println("%");

}

// show the temperature
void showtemp(void) {
  tft.fillScreen(ILI9341_BLACK);
  offsettext(0,3);
  tft.println("Temperature");
  tft.println();
  DS18B20.requestTemperatures();       
  tempC = DS18B20.getTempCByIndex(0);  
  tempF = tempC * 9 / 5 + 32;
  offsetdegrees(30);
  offsettext(40,2);
  tft.print(tempC);
  tft.println(" C");
  offsetdegrees(60);
  offsettext(70,2);
  tft.print(tempF);
  tft.println(" F");

  delay(1000);
}

// show the credits
void credits(void) {
  tft.fillScreen(ILI9341_BLACK);
  offsettext(0,3);
  tft.println("Credits");
  tft.println();
  offsettext(30,2);
  tft.println("Group Members:");
  offsettext(60,2);
  tft.println("Andy,Jia Woei,Jing Hui,");
  offsettext(90,2);
  tft.println("Chermaine and Charissa");
  tft.println();
  offsettext(120,2);
  tft.println("With thanks to Tony");
  offsettext(150,2);
  tft.println("and Qi Jie");

  delay(1000);
}

void techsupport(void){
  tft.fillScreen(ILI9341_BLACK);
  offsettext(0,3);
  tft.println("Tech Support");
  offsettext(40,2);
  tft.println("Email Address:");
  offsettext(70,2);
  tft.println("planbee9555@gmail.com");

  delay(1000);
}

void offsettext(int y, int font){
  tft.setCursor(40, y);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(font);
}

void offsetdegrees(int y){
  tft.setCursor(100, y);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(1);
  tft.println("o");
}
