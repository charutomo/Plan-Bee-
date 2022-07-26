#include "SPI.h"
#include "Adafruit_ILI9341.h"
#include "AiEsp32RotaryEncoder.h"
#include "Arduino.h"
#include "Bee.h"
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
float currentarray1[10];
float voltagearray1[10];
float powerarray1[10];
int numloop = 0;

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
  tft.drawRGBBitmap(0, 0, Plan_Bee_Logo_ALIVE, BEE_WIDTH,BEE_HEIGHT);
  
  ina3221.begin();
  ina3221.reset();
  ina3221.setShuntRes(100, 100, 100);
  // start the main menu
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
  offsettext(40,2);
  tft.println();
  if (numloop<10){
  float current[3];
  float voltage[3];

  Serial.println(numloop);
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
  for (int i = 0; i < 10; i = i + 1) {
    Serial.print(currentarray1[i]);
    Serial.print(", ");
  }
  
  Serial.println();
  Serial.print("Voltage Array:");
  for (int i = 0; i < 10; i = i + 1) {
    Serial.print(voltagearray1[i]);
    Serial.print(", ");
  }
  Serial.println();
  numloop+=1;
  delay(0.001);
  }
  else if(numloop == 10){
    tft.fillScreen(ILI9341_BLACK);
    tft.setRotation(0);
    offsettext(0,3);
    tft.println("Battery");
    offsettext(40,2);
    Serial.print("Current:");
    Serial.println(avgvalue(currentarray1,numloop));
    tft.print("Current:");
    tft.print(avgvalue(currentarray1,numloop));
    tft.println("A");
    offsettext(70,2);
    Serial.print("Voltage:");
    Serial.println(avgvalue(voltagearray1,numloop));
    tft.print("Voltage:");
    tft.print(avgvalue(voltagearray1,numloop));
    tft.println("V");
    offsettext(100,2);
    Serial.print("Power:");
    Serial.println(avgvalue(powerarr(currentarray1,voltagearray1),numloop));
    tft.print("Power:");
    tft.print(avgvalue(powerarr(currentarray1,voltagearray1),numloop));
    tft.println("W");
    offsettext(130,2);
    Serial.print("Energy:");
    Serial.println(energy(powerarr(currentarray1,voltagearray1)));
    tft.print("Energy:");
    tft.print(energy(powerarr(currentarray1,voltagearray1)));
    tft.println("Ws");
    offsettext(160,2);
    tft.print("Battery Percent:");
    tft.print(battpercent(avgvalue(voltagearray1,numloop)));
    tft.println("%");
    numloop=0;
    for (int i=0; i<10; i++){
      currentarray1[i]=0;
      voltagearray1[i]=0;
    delay(1000);
    Serial.println("New Loop");
  }
  
  }
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
}

void techsupport(void){
  tft.fillScreen(ILI9341_BLACK);
  offsettext(0,3);
  tft.println("Tech Support");
  offsettext(40,2);
  tft.println("Email Address:");
  offsettext(70,2);
  tft.println("planbee9555@gmail.com");
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

float avgvalue(float arr1[10], int numloop){
    float totalval = 0;
    for (int i = 0; i < numloop; i++){
      totalval += arr1[i];
    }
    float avgval = totalval/numloop;
    return avgval;
}

float* powerarr(float carr1[10], float varr[10]){
  for (int i = 0; i <10; i++){
    powerarray1[i] = carr1[i]*varr[i];
  }
  return powerarray1;
}


float energy(float powerarr[10]){
  float avgenergy = 0;
  for (int i = 0; i <10; i++){
    avgenergy+= powerarr[i]*0.005;
  }
  return avgenergy;
}

float battpercent(float avgvoltage){
  if (avgvoltage<= 4.3 & avgvoltage >= 3.4){
    float batterypercent = ((avgvoltage-3.4)/ (4.3 - 3.4))*100;
    return batterypercent;
  }
}
