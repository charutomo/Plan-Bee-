#include "SPI.h"
#include "Adafruit_ILI9341.h"
#include "AiEsp32RotaryEncoder.h"
#include "Arduino.h"
#include <Wire.h>
#include <Beastdevices_INA3221.h>
#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define TFT_CLK 32
#define TFT_MOSI 33
#define TFT_MISO 12
#define TFT_CS 27
#define TFT_DC 26
#define TFT_RST 25

#define PRINT_DEC_POINTS  3  

#define SENSOR_PIN  0

#define ROTARY_ENCODER_A_PIN 14
#define ROTARY_ENCODER_B_PIN 17
#define ROTARY_ENCODER_BUTTON_PIN 16
#define ROTARY_ENCODER_VCC_PIN - 1 /* 27 put -1 of Rotary encoder Vcc is connected directly to 3,3V; else you can use declared output pin for powering rotary encoder */

#define ROTARY_ENCODER_STEPS 4

int menu_state = 0;
int value = 0;

Beastdevices_INA3221 ina3221(INA3221_ADDR40_GND); 

OneWire oneWire(SENSOR_PIN);
DallasTemperature DS18B20(&oneWire);

float tempC; // temperature in Celsius
float tempF; // temperature in Fahrenheit

//instead of changing here, rather change numbers above
AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS);

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

void IRAM_ATTR readEncoderISR() {
  rotaryEncoder.readEncoder_ISR();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  //we must initialize rotary encoder
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

  // initalise the screen
  tft.begin();
  DS18B20.begin();
  tft.fillScreen(ILI9341_WHITE);
  tft.setRotation(0);
  
  ina3221.begin();
  ina3221.reset();
  ina3221.setShuntRes(100, 100, 100);
  // start the main menu
  menu();
}

void loop() {
  if (rotaryEncoder.encoderChanged() && menu_state < 5) {
    value = rotaryEncoder.readEncoder();
    if (value % 3 == 0) {
      menu_state = 1;
    } else if (value % 3 == 1) {
      menu_state = 2;
    } else if (value % 3 == 2) {
      menu_state = 3;
    }
    changemenu();
  }
  if (rotaryEncoder.isEncoderButtonClicked()) {
    if (menu_state >= 5){
      menu_state = 0;
    } else if (value % 3 == 0) {
      menu_state = 7;
    } else if (value % 3 == 1) {
      menu_state = 8;
    } else if (value % 3 == 2) {
      menu_state = 9;
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
  case 7:
    showbattery();
    break;
  case 8:
    showtemp();
    break;
  case 9:
    credits();
    break;
  }
}

// menu with no selector
void menu() {
  tft.fillScreen(ILI9341_WHITE);
  tft.setRotation(0);
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(70, 40);
  tft.println("Battery Info");
  tft.setCursor(70, 70);
  tft.println("Temperature");
  tft.setCursor(70, 100);
  tft.println("Credits");
  tft.setCursor(70, 130);
}

// menu with selector on 1
void menu1() {
  tft.fillRect(40,40,30,100, ILI9341_WHITE);
  tft.setCursor(40, 40);
  tft.print("->");
}

// menu with selector on 2
void menu2() {
  tft.fillRect(40,40,30,100, ILI9341_WHITE);
  tft.setCursor(40, 70);
  tft.print("->");
}

// menu with selector on 3
void menu3() {
  tft.fillRect(40,40,30,100, ILI9341_WHITE);
  tft.setCursor(40, 100);
  tft.print("->");
}

// show the battery value
void showbattery(void) {
  tft.fillScreen(ILI9341_WHITE);
  tft.setTextSize(3);
  tft.setCursor(0,0);
  tft.setTextColor(ILI9341_BLACK);
  tft.println("Battery");
  tft.setTextSize(2);
  tft.println();

  float current[3];
  float voltage[3];

  current[0] = ina3221.getCurrent(INA3221_CH1);
  voltage[0] = ina3221.getVoltage(INA3221_CH1);
  float power1 = current[0] * voltage[0];
  float battpercent = ((voltage[0]-3.4)/(4.3-3.4))*100;

  current[1] = ina3221.getCurrent(INA3221_CH2);
  voltage[1] = ina3221.getVoltage(INA3221_CH2);
  float power2 = current[1] * voltage[1];
  tft.println("Charging Power:");
  tft.print(current[0], PRINT_DEC_POINTS);
  tft.println("A, ");
  tft.print(voltage[0], PRINT_DEC_POINTS);
  tft.println("V, ");
  tft.print(power1);
  tft.println("W");
  tft.println();
  
  tft.println("Battery Percentage:");
  tft.print(battpercent);
  tft.println("%");
  tft.println();
}

// show the temperature
void showtemp(void) {
  tft.fillScreen(ILI9341_WHITE);
  tft.setTextSize(3);
  tft.setCursor(0,0);
  tft.setTextColor(ILI9341_BLACK);
  tft.println("Temperature");
  tft.setTextSize(1);
  tft.println();
  DS18B20.requestTemperatures();       
  tempC = DS18B20.getTempCByIndex(0);  
  tempF = tempC * 9 / 5 + 32;
  tft.setCursor(65,50);
  tft.println("o");
  tft.setTextSize(2);
  tft.print(tempC);
  tft.println(" C");
  tft.setCursor(65,80);
  tft.setTextSize(1);
  tft.println("o");
  tft.setTextSize(2);
  tft.print(tempF);
  tft.println(" F");
}

// show the credits
void credits(void) {
  tft.fillScreen(ILI9341_WHITE);
  tft.setTextSize(3);
  tft.setCursor(0,0);
  tft.setTextColor(ILI9341_BLACK);
  tft.println("Credits");
  tft.setTextSize(2);
  tft.println();
  tft.println("Group Members:");
  tft.println("Andy,Jia Woei,Jinghui,");
  tft.println("Chermaine and Charissa");
  tft.println();
  tft.println("With thanks to Tony and Qi");
  tft.println("Jie");
  
}
