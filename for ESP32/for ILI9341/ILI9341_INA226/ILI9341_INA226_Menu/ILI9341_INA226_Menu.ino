#include "SPI.h"
#include "Adafruit_ILI9341.h"
#include "Bee.h"
#include "AiEsp32RotaryEncoder.h"
#include "Arduino.h"
#include <Wire.h>
#include <INA226_WE.h>
#include <SPI.h>
#include <DallasTemperature.h>

#define TFT_CLK    32 
#define TFT_MOSI   33
#define TFT_MISO   12 
#define TFT_CS     27 
#define TFT_DC     26 
#define TFT_RST    25 
#define PRINT_DEC_POINTS  3  
#define SENSOR_PIN  0 
#define ROTARY_ENCODER_A_PIN 14
#define ROTARY_ENCODER_B_PIN 17
#define ROTARY_ENCODER_BUTTON_PIN 16
#define ROTARY_ENCODER_VCC_PIN - 1 
#define ROTARY_ENCODER_STEPS 4

float tempC; 
float tempF;
float shuntVoltage_mV = 0.0;
float loadVoltage_V = 0.0;
float busVoltage_V = 0.0;
float current_mA = 0.0;
float power_mW = 0.0; 
int menu_state = 0;
int value = 0;
int batt = 1;
int temp = 1;
int cred = 1;
int tech = 1;

OneWire oneWire(SENSOR_PIN);
DallasTemperature DS18B20(&oneWire);
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS);
INA226_WE ina226batt = INA226_WE(0x40); 

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
  rotaryEncoder.setAcceleration(250); 
  
  tft.begin();
  DS18B20.begin();
  Wire.begin();
  tft.fillScreen(ILI9341_BLACK);
  tft.setRotation(0);
  tft.drawRGBBitmap(0, 0, Plan_Bee_Logo_ALIVE, BEE_WIDTH,BEE_HEIGHT);
  delay(1000);
  ina226batt.init();
  ina226batt.setAverage(AVERAGE_128); 
  ina226batt.setConversionTime(CONV_TIME_8244); 
  ina226batt.setMeasureMode(TRIGGERED); 
  ina226batt.setCurrentRange(MA_800); 
  ina226batt.setCorrectionFactor(0.72); 
  menu();
}

void loop() {
  if (rotaryEncoder.encoderChanged() && menu_state < 5) {
    value = rotaryEncoder.readEncoder();
    if (value % 4 == 0) {
      menu_state = 1;
      batt = 1;
    } else if (value % 4 == 1) {
      menu_state = 2;
      temp = 1;
    } else if (value % 4 == 2) {
      menu_state = 3;
      cred = 1;
    }
    else if (value % 4 == 3) {
      menu_state = 4;
      tech = 1;
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
  4: main menu with selector on Tech Support

  7: Battery Info Page
  8: Temperature Page 
  9: Credits Page
  10: Tech Support Page
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
  if (batt ==1){
    refreshorig();
    batt = 0;
  }
  else {
    refreshbatt();
  }
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
  tft.println(current[0], PRINT_DEC_POINTS);
  offsetunits(40,2);
  tft.print("mA, ");
  offsettext(70,2);
  tft.print("Voltage:");
  tft.println(voltage[0], PRINT_DEC_POINTS);
  offsetunits(70,2);
  tft.print("V, ");
  offsettext(100,2);
  tft.print("Power: "); 
  tft.println(power0);
  offsetunits(100,2);
  tft.print("mW");
  offsettext(130,2);
  tft.println("Battery Percentage:");
  offsettext(160,2);
  tft.println(battpercent);
  offsetunits(160,2);
  tft.print("%");

}

// show the temperature
void showtemp(void) {
  if (temp ==1){
    refreshorig();
    temp = 0;
  }
  else{
    refreshtemp();
  }
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
  if (cred == 1){
    refreshorig();
    cred = 0;
  }
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

//show tech support information
void techsupport(void){
  if (tech == 1){
    refreshorig();
    tech = 0;
  }
  offsettext(0,3);
  tft.println("Tech Support");
  offsettext(40,2);
  tft.println("Email Address:");
  offsettext(70,2);
  tft.println("planbee9555@gmail.com");

  delay(1000);
}

void offsettext(int y, int font){
  /*
   * Function: offsetdegree
   * ----------------
   * Set cursor to (40,y) which indent the screen and set text size accordingly to the input font
   * 
   * y: y-axis value to be input for printing text
   * font: font size of the text to be printed on display
   */
  tft.setCursor(40, y);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(font);
}

void offsetdegrees(int y){
  /*
   * Function: offsetdegree
   * ----------------
   * Set cursor and indent the screen to show the degree symbol
   * 
   * y: y-axis value to be input for the degrees symbol
   */
  tft.setCursor(100, y);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(1);
  tft.println("o");
}

void offsetunits(int y, int font){
  /*
   * 
   */
   tft.setCursor(240, y);
   tft.setTextColor(ILI9341_YELLOW);
   tft.setTextSize(font);
   
}

void refreshorig(void){
  /*
   * Function: refreshtemp
   * ----------------
   * Fills a blackbox space to cover the menu screen for refreshing page 
   */
  tft.fillRect(40,40,200,120,ILI9341_BLACK);
}

void refreshbatt(void){
  /*
   * Function: refreshtemp
   * ----------------
   * Fills a blackbox space to cover the old battery values
   * for refreshing page on battery screen before showing new values
   */
  tft.fillRect(135,40,80,50,ILI9341_BLACK);
  tft.fillRect(110,100,100,20,ILI9341_BLACK);
  tft.fillRect(40,160,90,30,ILI9341_BLACK);
}

void refreshtemp(void){
  /*
   * Function: refreshtemp
   * ----------------
   * Fills a blackbox space to cover the old temperature values for refreshing page
   * on temperature screen before showing new values
   */
  tft.fillRect(40,30,60,80,ILI9341_BLACK);
}
