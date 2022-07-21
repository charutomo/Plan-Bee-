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

#if defined(ESP8266)
#define ROTARY_ENCODER_A_PIN D6
#define ROTARY_ENCODER_B_PIN D5
#define ROTARY_ENCODER_BUTTON_PIN D7
#else
#define ROTARY_ENCODER_A_PIN 14
#define ROTARY_ENCODER_B_PIN 17
#define ROTARY_ENCODER_BUTTON_PIN 16
#endif
#define ROTARY_ENCODER_VCC_PIN -1 /* 27 put -1 of Rotary encoder Vcc is connected directly to 3,3V; else you can use declared output pin for powering rotary encoder */

//depending on your encoder - try 1,2 or 4 to get expected behaviour
//#define ROTARY_ENCODER_STEPS 1
//#define ROTARY_ENCODER_STEPS 2
#define ROTARY_ENCODER_STEPS 4

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

void rotary_loop() {
  //dont print anything unless value changed
  if (rotaryEncoder.encoderChanged())
  {
    Serial.print("Value: ");
    Serial.println(rotaryEncoder.readEncoder());
  }
  if (rotaryEncoder.isEncoderButtonClicked())
  {
    rotary_onButtonClick();
  }
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
  tft.begin();
  tft.fillScreen(ILI9341_WHITE);
  tft.setRotation(0);
  menu();
}

void loop() {
  // put your main code here, to run repeatedly:
  //rotary_loop();
  
  if (rotaryEncoder.encoderChanged()) {
    int value = rotaryEncoder.readEncoder();
    Serial.println(value);
  if (value%3==0){
      menu1();
      if (rotaryEncoder.isEncoderButtonClicked()){
        showbattery();
      }
  }
  else if (value%3==1){
      menu2();
      if (rotaryEncoder.isEncoderButtonClicked()){
        showtemp();
      }
  }
  else{
      menu3();
      if (rotaryEncoder.isEncoderButtonClicked()){
        credits();
      }
  }

  }
}

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

void menu1() {
  tft.fillScreen(ILI9341_WHITE);
  tft.setRotation(0);
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(40,40);
  tft.print("->");
  tft.setCursor(70,40);
  tft.println("Battery Info");
  tft.setCursor(70,70);
  tft.println("Temperature");
  tft.setCursor(70,100);
  tft.println("Credits");
  tft.setCursor(70,130);
}


void menu2() {
  tft.fillScreen(ILI9341_WHITE);
  tft.setRotation(0);
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(70,40);
  tft.println("Battery Info");
  tft.setCursor(40,70);
  tft.print("->");
  tft.setCursor(70,70);
  tft.println("Temperature");
  tft.setCursor(70,100);
  tft.println("Credits");
  tft.setCursor(70,130);
}

void menu3() {
  tft.fillScreen(ILI9341_WHITE);
  tft.setRotation(0);
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(70,40);
  tft.println("Battery Info");
  tft.setCursor(70,70);
  tft.println("Temperature");
  tft.setCursor(40,100);
  tft.print("->");
  tft.setCursor(70,100);
  tft.println("Credits");
  tft.setCursor(70,130);
}

void showbattery(void){
  tft.fillScreen(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_BLACK);
  tft.println("Battery");
}

void showtemp(void){
  tft.fillScreen(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_BLACK);
  tft.println("Temperature");
}

void credits(void){
  tft.fillScreen(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_BLACK);
  tft.println("Credits");
}
