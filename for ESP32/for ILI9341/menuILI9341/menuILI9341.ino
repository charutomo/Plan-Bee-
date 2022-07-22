#include "SPI.h"
#include "Adafruit_ILI9341.h"
#include "AiEsp32RotaryEncoder.h"
#include "Arduino.h"

#define TFT_CLK    32
#define TFT_MOSI   33
#define TFT_MISO   12 
#define TFT_CS     27 
#define TFT_DC     26 
#define TFT_RST    25 

#define ROTARY_ENCODER_A_PIN 14
#define ROTARY_ENCODER_B_PIN 17
#define ROTARY_ENCODER_BUTTON_PIN 16
#define ROTARY_ENCODER_VCC_PIN -1 /* 27 put -1 of Rotary encoder Vcc is connected directly to 3,3V; else you can use declared output pin for powering rotary encoder */

#define ROTARY_ENCODER_STEPS 4

int menu_state = 0;

//instead of changing here, rather change numbers above
AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS);

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

void rotary_onButtonClick() {
  static unsigned long lastTimePressed = 0;
  //ignore multiple press in that time milliseconds
  if (millis() - lastTimePressed < 500)
  {
    return;
  }
  lastTimePressed = millis();
  Serial.print("button pressed ");
  Serial.print(millis());
  Serial.println(" milliseconds after restart");
}

void IRAM_ATTR readEncoderISR()
{
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
  tft.fillScreen(ILI9341_WHITE);
  tft.setRotation(0);

  // start the main menu
  menu();
}

void loop() {
  if (rotaryEncoder.encoderChanged()) {
    int value = rotaryEncoder.readEncoder();
    Serial.println(value);
  if (value%3==0){
      menu_state = 1;
      if (rotaryEncoder.isEncoderButtonClicked()){
        menu_state = 7;
      }
  }
  else if (value%3==1){
      menu_state = 2;
      if (rotaryEncoder.isEncoderButtonClicked()){
        menu_state = 8;
      }
  }
  else{
      menu_state = 3;
      if (rotaryEncoder.isEncoderButtonClicked()){
        menu_state = 9;
      }
  changemenu();
  }

  }
}

void changemenu(){
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
        break
      case 1:
        menu1();
        break
      case 2:
        menu2();
        break
      case 3:
        menu3();
        break
      case 7:
        showbattery();
        break
      case 8:
        showtemp();
        break
      case 9:
        credits();
        break
  }
}

// menu with no selector
void menu() {
  tft.fillScreen(ILI9341_WHITE);
  tft.setRotation(0);
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(70,40);
  tft.println("Battery Info");
  tft.setCursor(70,70);
  tft.println("Temperature");
  tft.setCursor(70,100);
  tft.println("Credits");
  tft.setCursor(70,130);
}

// menu with selector on 1
void menu1() {
  menu();
  tft.setCursor(40,40);
  tft.print("->");
}

// menu with selector on 2
void menu2() {
  menu();
  tft.setCursor(40,70);
  tft.print("->");
}

// menu with selector on 3
void menu3() {
  menu();
  tft.setCursor(40,100);
  tft.print("->");
}

// show the battery value
void showbattery(void){
  tft.fillScreen(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_BLACK);
  tft.println("Battery");
}

// show the temperature
void showtemp(void){
  tft.fillScreen(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_BLACK);
  tft.println("Temperature");
}

// show the credits
void credits(void){
  tft.fillScreen(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_BLACK);
  tft.println("Credits");
}
