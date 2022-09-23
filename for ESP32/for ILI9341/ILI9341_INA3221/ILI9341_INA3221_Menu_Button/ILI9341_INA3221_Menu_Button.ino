#include "SPI.h"
#include "Adafruit_ILI9341.h"
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
#define BUTTON_PIN 4 // GPIO4 pin connected to button

float current1;
float voltage1;
float power1;
float battery1;
float energy1;
float tempC; 
float tempF;
float currentarray1[10];
float voltagearray1[10];
float powerarray1[10];
int clicknum;
int lastState = HIGH;
int currentState;
int batt = 1;
int temp = 1;
int cred = 1;
int tech = 1;
int numloop = 0;

Beastdevices_INA3221 ina3221(INA3221_ADDR40_GND); 

OneWire oneWire(SENSOR_PIN);
DallasTemperature DS18B20(&oneWire);
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

void setup() {
  Serial.begin(115200);
  // initialise the screen
  tft.begin();
  DS18B20.begin();
  tft.fillScreen(ILI9341_WHITE);
  tft.setRotation(0);
  tft.drawRGBBitmap(0, 0, Plan_Bee_Logo_ALIVE, BEE_WIDTH,BEE_HEIGHT);
  
  // initialise INA3221
  ina3221.begin();
  ina3221.reset();
  ina3221.setShuntRes(100, 100, 100);
}

void loop() {
  currentState = digitalRead(BUTTON_PIN);
  int index = clicknum%5;
  ifClick();
  Serial.println(currentState);
  switch(index){
    case 0:
      showbattery();
      break;
    case 1:
      showtemp();
      break;
    case 2:
      credits();
      break;
    case 3:
      techsupport();
      break;
  }
  lastState = currentState;
}

void ifClick(void){
 if (lastState == LOW && currentState == HIGH){
    clicknum++;
    cred = 1;
    tech = 1;
    batt = 1;
    temp = 1;
    }
}

// show the battery value
void showbattery(void) {
  if (batt ==1){
    tft.fillScreen(ILI9341_BLACK);
    batt = 0;
  }
  float current[3];
  float voltage[3];

  int index = numloop%10;
  currentarray1[index] = ina3221.getCurrent(INA3221_CH1);
  voltagearray1[index] = ina3221.getVoltage(INA3221_CH1);
  numloop++;
  if (numloop<10){
      current1 = avgvalue(currentarray1,numloop);
      voltage1 = avgvalue(voltagearray1,numloop);
      power1 = avgvalue(powerarr(currentarray1,voltagearray1),numloop);
      energy1 = energy(powerarr(currentarray1,voltagearray1));
      battery1 = battpercent(avgvalue(voltagearray1,numloop));
  }
  else{
      current1 = avgvalue(currentarray1,10);
      voltage1 = avgvalue(voltagearray1,10);
      power1 = avgvalue(powerarr(currentarray1,voltagearray1),10);
      energy1 = energy(powerarr(currentarray1,voltagearray1));
      battery1 = battpercent(avgvalue(voltagearray1,10));
  }
  if (numloop%10 == 0){
    refreshbatt();
    tft.setRotation(0);
    offsettext(0,3);
    tft.println("Battery");
    
    offsettext(40,2);
    tft.print("Current:");
    tft.print(current1);
    tft.println("A");
    
    offsettext(70,2);
    tft.print("Voltage:");
    tft.print(voltage1);
    tft.println("V");
    
    offsettext(100,2);
    tft.print("Power:");
    tft.print(power1);
    tft.println("W");
  
    offsettext(130,2);
    tft.print("Energy:");
    tft.print(energy1);
    tft.println("Ws");
  
    offsettext(160,2);
    tft.print("Battery Percent:");
    tft.print(battery1);
    tft.println("%");
  }
}

// show the temperature
void showtemp(void) {
  if (temp ==1){
    tft.fillScreen(ILI9341_BLACK);;
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
}

// show the credits
void credits(void) {
  if (cred == 1){
    tft.fillScreen(ILI9341_BLACK);
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
}

//show tech support information
void techsupport(void){
  if (tech == 1){
    tft.fillScreen(ILI9341_BLACK);
    tech = 0;
  }
  offsettext(0,3);
  tft.println("Tech Support");
  offsettext(40,2);
  tft.println("Email Address:");
  offsettext(70,2);
  tft.println("planbee9555@gmail.com");
}

void offsettext(int y, int font){
  /*
   * Function: offsettext
   * ----------------
   * Set cursor to (40,y) which indent the screen and set text size accordingly to the input font
   * 
   * y: y-axis value to be input for printing text
   * font: font size of the text to be printed on display
   * 
   */
  tft.setCursor(40, y);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(font);
}

void offsetdegrees(int y){
  /*
   * Function: offsetdegrees
   * ----------------
   * Set cursor and indent the screen to show the degree symbol
   * 
   * y: y-axis value to be input for the degrees symbol
   * 
   */
  tft.setCursor(100, y);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(1);
  tft.println("o");
}

void refreshorig(void){
  /*
   * Function: refreshorig
   * ----------------
   * Fills a blackbox space to cover the menu screen for refreshing page 
   */
  tft.fillRect(40,40,200,120,ILI9341_BLACK);
}

void refreshbatt(void){
  /*
   * Function: refreshbatt
   * ----------------
   * Fills a blackbox space to cover the old battery values
   * for refreshing page on battery screen before showing new values
   * 
   */
  tft.fillRect(135,40,80,50,ILI9341_BLACK);
  tft.fillRect(110,100,80,20,ILI9341_BLACK);
  tft.fillRect(120,130,80,20,ILI9341_BLACK);
  tft.fillRect(230,160,80,20,ILI9341_BLACK);
}

void refreshtemp(void){
  /*
   * Function: refreshtemp
   * ----------------
   * Fills a blackbox space to cover the old temperature values for refreshing page
   * on temperature screen before showing new values
   * 
   */
  tft.fillRect(40,30,60,80,ILI9341_BLACK);
}

float avgvalue(float arr1[10], int numloop){
    /*
     * Function: avgvalue
     * ----------------
     * Calculate the average value of a array containing floats
     *  avgval = sum of the numloop number of values in the array/numloop
     *  
     * arr1[10]: Array to be taken average
     * numloop: The number loops that have been iterated
     * 
     * returns:
     *  avgvalue 
     *  
     */
    float totalval = 0;
    for (int i = 0; i < numloop; i++){
      totalval += arr1[i];
    }
    float avgval = totalval/numloop;
    return avgval;
}

float* powerarr(float carr1[10], float varr1[10]){
  /*
   * Function: powerarr
   * ----------------
   * Forms a power array by multiply the current and voltage at each datapoint collected by INA3221
   *  powerarray1[index] = currentarray[index]* voltage[index]
   * 
   * carr1[10]: Current Array that collected 10 datapoints from INA3221
   * varr1[10]: Voltage Array that collected 10 datapoints from INA3221
   * 
   * returns:
   * powerarray1 
   * 
   */
  for (int i = 0; i <10; i++){
    powerarray1[i] = carr1[i]*varr1[i];
  }
  return powerarray1;
}


float energy(float powerarr[10]){
  /*
   * Function: energy
   * ----------------
   * computes the energy generated through the 10 sample points collected
   *  avgenergy = sum of 10 respective points * s/point
   *  
   *  powerarr[10]: power array collected by powerarr function
   *  
   *  returns:
   *  Average Energy via Riemann Sum or Total Area under the rectangular graph at each point for Power-Time Graph
   *  
   */
  float avgenergy = 0;
  for (int i = 0; i <10; i++){
    avgenergy+= powerarr[i]*1;
  }
  return avgenergy;
}

float battpercent(float avgvoltage){
  /*  
   *   Function: battpercent
   *   ---------------------
   *   compute the battery percentage in terms of voltage difference at the battery
   *    equation are as follows battery percent = ((avgvoltage-3.4)/ (4.2 - 3.4))*100%
   *    
   *   avgvoltage: average voltage value after sampling taken by INA3221
   *   
   *   returns: batterypercent (battery percentage of power bank)
   *   
   */
  if (avgvoltage<= 4.2 & avgvoltage >= 3.4){
    float batterypercent = ((avgvoltage-3.4)/ (4.2 - 3.4))*100;
    return batterypercent;
  }
}