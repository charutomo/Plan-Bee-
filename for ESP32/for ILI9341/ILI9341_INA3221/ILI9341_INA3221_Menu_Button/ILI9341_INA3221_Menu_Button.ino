#include "SPI.h"
#include <TFT_eSPI.h>
#include "Arduino.h"
#include "Bee.h"
#include <Wire.h>
#include <Beastdevices_INA3221.h>
#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESP32Time.h>

#define TFT_SCLK 32
#define TFT_MOSI 33
#define TFT_MISO 12
#define TFT_CS   27
#define TFT_DC 26
#define TFT_RST 25
#define PRINT_DEC_POINTS  3  
#define SENSOR_PIN  0
#define BUTTON_PIN 4 // GPIO4 pin connected to button
//#define TFT_WIDTH 240
//#define TFT_HEIGHT  320

float current1;
float voltage1;
float power1;
float battery1;
float energy1;
float current2;
float voltage2;
float power2;
float energy3;
float current3;
float voltage3;
float power3;
float energy2;
float tempC; 
float tempF;
float currentarray1[10];
float voltagearray1[10];
float powerarray1[10];
float currentarray2[10];
float voltagearray2[10];
float powerarray2[10];
float currentarray3[10];
float voltagearray3[10];
float powerarray3[10];
int clicknum;
int lastState = HIGH;
int currentState;
int batt = 1;
int temp = 1;
int cred = 1;
int tech = 1;
int numloop = 0;
unsigned long drawTime = 0;
// Replace with your network credentials
const char* ssid     = "SSID";
const char* password = "Password";
char *dayarray[]= {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};

Beastdevices_INA3221 ina3221(INA3221_ADDR40_GND); 

OneWire oneWire(SENSOR_PIN);
DallasTemperature DS18B20(&oneWire);
TFT_eSPI tft = TFT_eSPI(); 
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
ESP32Time rtc(28800);

void setup() {
  Serial.begin(115200);
  
  // initialise the screen
  tft.begin();
  tft.fillScreen(ILI9341_WHITE);
  tft.setFreeFont(FF33);
  DS18B20.begin();

  //draw the bee logo
  int x = random(tft.width()  - BEE_WIDTH);
  int y = random(tft.height() - BEE_HEIGHT);
  //using the drawXBitmap function, bee_logo is in the header folder, Bee.h
  tft.drawXBitmap(x, y, bee_logo, BEE_WIDTH,BEE_HEIGHT,TFT_BLACK,TFT_WHITE); // monochrome, white background
  delay(1000);

  // initialise the NTPClient
  //datetimeConnect();
  //custom page ro show the name of the owner of the power bank and datetime
  showcustom();
  delay(2500);
  
  // initialise INA3221
  ina3221.begin();
  ina3221.reset();
  ina3221.setShuntRes(100, 100, 100);
  tft.fillScreen(ILI9341_BLACK);

}

void loop() {
  currentState = digitalRead(BUTTON_PIN);
  int index = clicknum%6;
  ifClick();
  Serial.println(currentState);
  switch(index){
    case 0:
      showbattery();
      break;
    case 1:
      showinput();
      break;
    case 2:
      showoutput();
      break;
    case 3:
      showtemp();
      break;
    case 4:
      credits();
      break;
    case 5:
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
    Serial.print(clicknum);
    }
}

void datetimeConnect(void){
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(28800); // SGT is GMT+8
}

void showcustom(void){
  tft.fillScreen(ILI9341_BLACK);
  offsettext(40);
  tft.println("Hello Name!");  

  //using library ESP32Time
  rtc.setTime(30, 24, 15, 17, 1, 2042);
  offsettext(120);
  tft.print("Date - ");
  tft.print(rtc.getDate(true));
  
  offsettext(150);
  tft.print("Time Now - ");
  tft.print(rtc.getTime());


  //for NTPClient
//  while(!timeClient.update()) {
//    timeClient.forceUpdate();
//  }
//
//  offsettext(120);
//  tft.print("Date - ");
//  tft.print(timeClient.getDay());
//  tft.print(" / ");
//  tft.print(timeClient.getMonth());
//  tft.print(" / ");
//  tft.print(timeClient.getYear());
//  
//  offsettext(150);
//  tft.print("Day - ");
//  tft.print(dayarray[timeClient.getDayOfWeek()-1]);
//  
//  offsettext(180);
//  tft.print("Time Now - ");
//  tft.print(timeClient.getFormattedTime());
}

// show the battery value
void showbattery(void) {
  if (batt ==1){
    tft.fillScreen(ILI9341_BLACK);
    batt = 0;
  }
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
    tft.setRotation(0);
    offsettext(20);
    tft.println("Battery");
    
    offsettext(60);
    tft.print("Current:");
    tft.setTextPadding(100);
    tft.drawFloat(current1, 3, 150, 47);
    tft.setCursor(230, 60);
    tft.println("A");
    
    offsettext(100);
    tft.print("Voltage:");
    tft.setTextPadding(100);
    tft.drawFloat(voltage1, 3, 150, 83);
    tft.setCursor(230, 100);
    tft.print("V");
    
    offsettext(140);
    tft.print("Power:");
    tft.setTextPadding(100);
    tft.drawFloat(power1, 3, 150, 129);
    tft.setCursor(230, 140);
    tft.println("W");
  
    offsettext(180);
    tft.print("Energy:");
    //tft.print(energy1);
    tft.setTextPadding(100);
    tft.drawFloat(energy1, 3, 150, 167);
    tft.setCursor(230, 180);
    tft.println("Ws");
  
    offsettext(220);
    tft.print("Battery %:");
    tft.setTextPadding(100);
    tft.drawFloat(battery1, 2, 150, 207);
    tft.setCursor(230, 220);
    tft.println("%");
  }
}

// show the voltage, current, power and energy of input
void showinput(){
  if (batt ==1){
    tft.fillScreen(ILI9341_BLACK);
    batt = 0;
  }
  int index = numloop%10;
  currentarray2[index] = ina3221.getCurrent(INA3221_CH2);
  voltagearray2[index] = ina3221.getVoltage(INA3221_CH2);
  numloop++;
  if (numloop<10){
      current2 = avgvalue(currentarray2,numloop);
      voltage2 = avgvalue(voltagearray2,numloop);
      power2 = avgvalue(powerarr(currentarray2,voltagearray2),numloop);
      energy2 = energy(powerarr(currentarray2,voltagearray2));
  }
  else{
      current2 = avgvalue(currentarray2,10);
      voltage2 = avgvalue(voltagearray2,10);
      power2 = avgvalue(powerarr(currentarray2,voltagearray2),10);
      energy2 = energy(powerarr(currentarray2,voltagearray2));
  }
  if (numloop%10 == 0){
    tft.setRotation(0);
    offsettext(20);
    tft.println("Input Values");
    
    offsettext(60);
    tft.print("Current:");
    tft.setTextPadding(100);
    tft.drawFloat(current2, 3, 150, 47);
    tft.setCursor(230, 60);
    tft.println("A");
    
    offsettext(100);
    tft.print("Voltage:");
    tft.setTextPadding(100);
    tft.drawFloat(voltage2, 3, 150, 83);
    tft.setCursor(230, 100);
    tft.print("V");
    
    offsettext(140);
    tft.print("Power:");
    tft.setTextPadding(100);
    tft.drawFloat(power2, 3, 150, 129);
    tft.setCursor(230, 140);
    tft.println("W");
  
    offsettext(180);
    tft.print("Energy:");
    tft.setTextPadding(100);
    tft.drawFloat(energy2, 3, 150, 167);
    tft.setCursor(230, 180);
    tft.println("Ws");
  }
}

// show the voltage, current, power and energy of output
void showoutput(){
  if (batt ==1){
    tft.fillScreen(ILI9341_BLACK);
    batt = 0;
  }
  int index = numloop%10;
  currentarray3[index] = ina3221.getCurrent(INA3221_CH3);
  voltagearray3[index] = ina3221.getVoltage(INA3221_CH3);
  numloop++;
  if (numloop<10){
      current3 = avgvalue(currentarray3,numloop);
      voltage3 = avgvalue(voltagearray3,numloop);
      power3 = avgvalue(powerarr(currentarray3,voltagearray3),numloop);
      energy3 = energy(powerarr(currentarray3,voltagearray3));
  }
  else{
      current3 = avgvalue(currentarray3,10);
      voltage3 = avgvalue(voltagearray3,10);
      power3 = avgvalue(powerarr(currentarray3,voltagearray3),10);
      energy3 = energy(powerarr(currentarray3,voltagearray3));
  }
  if (numloop%10 == 0){
    tft.setRotation(0);
    offsettext(20);
    tft.println("Output Values");
    
    offsettext(60);
    tft.print("Current:");
    tft.setTextPadding(100);
    tft.drawFloat(current3, 3, 150, 47);
    tft.setCursor(230, 60);
    tft.println("A");
    
    offsettext(100);
    tft.print("Voltage:");
    tft.setTextPadding(100);
    tft.drawFloat(voltage3, 3, 150, 83);
    tft.setCursor(230, 100);
    tft.print("V");
    
    offsettext(140);
    tft.print("Power:");
    tft.setTextPadding(100);
    tft.drawFloat(power3, 3, 150, 129);
    tft.setCursor(230, 140);
    tft.println("W");
  
    offsettext(180);
    tft.print("Energy:");
    tft.setTextPadding(100);
    tft.drawFloat(energy3, 3, 150, 167);
    tft.setCursor(230, 180);
    tft.println("Ws");
  }
}


// show the temperature
void showtemp(void) {
  if (temp ==1){
    tft.fillScreen(ILI9341_BLACK);
    temp = 0;
  }
  else{
    refreshtemp();
  }
  offsettext(20);
  tft.println("Temperature");
  tft.println();
  DS18B20.requestTemperatures();       
  tempC = DS18B20.getTempCByIndex(0);  
  tempF = tempC * 9 / 5 + 32;
  offsetdegrees(40);
  offsettext(60);
  tft.setTextPadding(100);
  tft.drawFloat(tempC, 2, 40, 47);
  //tft.print(tempC);
  tft.setCursor(130, 60);
  tft.println(" C");
  offsetdegrees(100);
  offsettext(120);
  tft.setTextPadding(100);
  tft.drawFloat(tempF, 2, 40, 107);
  tft.setCursor(130, 120);
  tft.println(" F");
  delay(200);
}

// show the credits
void credits(void) {
  if (cred == 1){
    tft.fillScreen(ILI9341_BLACK);
    cred = 0;
  }
  offsettext(20);
  tft.println("Credits");
  tft.println();
  offsettext(60);
  tft.println("Group Members:");
  offsettext(100);
  tft.println("Andy, Jia Woei, Jing Hui,");
  offsettext(140);
  tft.println("Chermaine and Charissa");
  tft.println();
  offsettext(180);
  tft.println("With thanks to Tony");
  offsettext(220);
  tft.println("and Qi Jie");
}

//show tech support information
void techsupport(void){
  if (tech == 1){
    tft.fillScreen(ILI9341_BLACK);
    tech = 0;
  }
  offsettext(20);
  tft.println("Tech Support");
  offsettext(60);
  tft.println("Email Address:");
  offsettext(100);
  tft.println("planbee9555@gmail.com");
}

void offsettext(int y){
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
  tft.setTextColor(TFT_YELLOW, ILI9341_BLACK);
  //tft.setTextSize(font);
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
  tft.setCursor(120, y);
  tft.setTextColor(TFT_YELLOW);
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
