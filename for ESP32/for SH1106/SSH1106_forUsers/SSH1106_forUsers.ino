#include <SPI.h>
#include <Wire.h>
#include <Beastdevices_INA3221.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>
#include <DallasTemperature.h>


#define OLED_SDA 21
#define OLED_SCL 22

Adafruit_SH1106 display(21, 22);

#define LOGO_HEIGHT 64 
#define LOGO_WIDTH  128 

#define PRINT_DEC_POINTS  3   

#define SENSOR_PIN  21 // ESP32 pin GPIO21 connected to DS18B20 sensor's DQ pin
#define BUTTON_PIN 4 // GPIO4 pin connected to button

Beastdevices_INA3221 ina3221(INA3221_ADDR40_GND);

OneWire oneWire(SENSOR_PIN);
DallasTemperature DS18B20(&oneWire);

float tempC; // temperature in Celsius
float tempF; // temperature in Fahrenheit
float current1;
float voltage1;
float power1;
float battery1;
float energy1;
float currentarray1[10];
float voltagearray1[10];
float powerarray1[10];
int numloop = 0;
int clicknum;
int lastState = HIGH;
int currentState;    

static const unsigned char PROGMEM logo_bmp[] =
  {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x8f, 0xfe, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xe7, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xc0, 0x0f, 0xff, 0xff, 0xfb, 0xff, 0xff, 0xf9, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xfc, 0x7f, 0xfe, 0x1f, 0xff, 0xf7, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xf7, 0xff, 0xff, 0xf8, 0x7f, 0xef, 0xff, 0xfc, 0xff, 0xbf, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xef, 0xff, 0xe7, 0xc7, 0xc7, 0xdf, 0xff, 0xf9, 0xff, 0xdf, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xdf, 0xff, 0xc7, 0x80, 0xfc, 0x5f, 0xff, 0xf3, 0xff, 0xef, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xdf, 0xff, 0xfe, 0x00, 0x1f, 0xc7, 0xff, 0xe7, 0xff, 0xef, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0x9d, 0xff, 0xfc, 0x0f, 0x07, 0xf8, 0x7f, 0xcf, 0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xde, 0x7f, 0xf0, 0x1f, 0xe3, 0xff, 0x8f, 0x9f, 0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xdf, 0x1f, 0xe0, 0x3f, 0xf9, 0xff, 0xe1, 0x3f, 0xff, 0xf0, 0x00, 0x7f, 0xff, 0xff, 
  0xff, 0xff, 0xcf, 0xe3, 0xc0, 0xf8, 0xfd, 0xc7, 0xfc, 0x7f, 0xff, 0xe7, 0xff, 0xc7, 0xff, 0xff, 
  0xff, 0xff, 0xef, 0xff, 0x81, 0xf0, 0x7d, 0x80, 0xff, 0x8f, 0xff, 0xef, 0xff, 0xf9, 0xff, 0xff, 
  0xff, 0xff, 0xf7, 0xff, 0x03, 0xf0, 0xfd, 0x00, 0x7f, 0xf3, 0xff, 0xdf, 0xff, 0xff, 0x7f, 0xff, 
  0xff, 0xff, 0xfb, 0xfe, 0x07, 0xff, 0xfa, 0x04, 0x3f, 0xfc, 0x7f, 0x3f, 0xff, 0xff, 0x9f, 0xff, 
  0xff, 0xff, 0xfd, 0xf8, 0x0f, 0x7f, 0xe0, 0x0f, 0xbf, 0xff, 0x1c, 0xff, 0xff, 0xff, 0xef, 0xff, 
  0xff, 0xff, 0xfe, 0xf0, 0x3e, 0x87, 0x80, 0x3f, 0x7f, 0xff, 0xe3, 0xff, 0xff, 0x9f, 0xf7, 0xff, 
  0xff, 0xff, 0xff, 0x78, 0x79, 0xf8, 0x60, 0x7c, 0xff, 0xff, 0xf9, 0xff, 0xfe, 0x7f, 0xf3, 0xff, 
  0xff, 0xff, 0xff, 0xbe, 0xf3, 0xff, 0x80, 0xf9, 0xff, 0xef, 0xfe, 0x7f, 0xf9, 0xff, 0xfb, 0xff, 
  0xff, 0xff, 0xff, 0xcf, 0x8f, 0xfe, 0x03, 0xf7, 0xff, 0x81, 0xff, 0x9f, 0xe7, 0xff, 0xfb, 0xff, 
  0xff, 0xff, 0xff, 0xf7, 0xff, 0xfe, 0x07, 0xef, 0xfe, 0x00, 0x7f, 0xe7, 0x8f, 0xff, 0xfb, 0xff, 
  0xff, 0xff, 0xff, 0xf9, 0xff, 0xf8, 0x0f, 0x9f, 0xf8, 0x04, 0x3f, 0xf9, 0x3f, 0xff, 0xfb, 0xff, 
  0xff, 0xff, 0xff, 0xfe, 0xff, 0xf8, 0x3f, 0x3f, 0xe0, 0x1f, 0x9f, 0xfc, 0xff, 0xff, 0xfb, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0x3f, 0xf8, 0x7e, 0x3f, 0xc0, 0x7f, 0xbf, 0xff, 0x3f, 0xff, 0xf7, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xcf, 0xf8, 0xfe, 0x3e, 0x00, 0xff, 0x3f, 0x9f, 0xdf, 0xff, 0xef, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xf7, 0xfc, 0x3f, 0x9c, 0x03, 0xdf, 0x7e, 0x03, 0xe7, 0xff, 0x9f, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xf9, 0xff, 0xcf, 0xc0, 0x0f, 0x3e, 0xfc, 0x01, 0xf3, 0xfe, 0x7f, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xf1, 0x20, 0x38, 0x7d, 0xf8, 0x18, 0xcc, 0x03, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0x9f, 0xfe, 0xe0, 0xf0, 0xf9, 0xf0, 0x3e, 0x02, 0x7f, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xe7, 0xff, 0xe3, 0xf8, 0xfb, 0xc0, 0xfe, 0x00, 0x3f, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xf3, 0x07, 0xf7, 0x81, 0xfe, 0x00, 0x1f, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0xfe, 0x43, 0xef, 0x07, 0xfe, 0x1e, 0x4f, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xcf, 0xff, 0xe3, 0xce, 0x0f, 0xfc, 0x7e, 0xe7, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf9, 0xff, 0xf3, 0xd8, 0x1f, 0xfc, 0xf9, 0xf7, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xfc, 0xb0, 0x3e, 0x7d, 0xf3, 0xfb, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xcf, 0xff, 0x60, 0x7c, 0x7f, 0xe7, 0xf9, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf9, 0xff, 0xe0, 0xf8, 0xff, 0xdf, 0xfd, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0xf1, 0xe0, 0xff, 0x3f, 0xfd, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe3, 0xfc, 0xc0, 0xfe, 0x7f, 0xfd, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x7f, 0x81, 0xf9, 0xff, 0xfd, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc7, 0xf9, 0xf3, 0xff, 0xfb, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x3c, 0xcf, 0xff, 0xf3, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x1f, 0xff, 0x81, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0xfe, 0x7f, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x8f, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf3, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

  static const unsigned char PROGMEM logo2_bmp[] ={
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xc0, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xfc, 0x7f, 0xfe, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xf7, 0xff, 0xff, 0xf8, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xef, 0xff, 0xe7, 0xc7, 0xc7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xdf, 0xff, 0xc7, 0x80, 0xfc, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xdf, 0xff, 0xfe, 0x00, 0x1f, 0xc7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0x9d, 0xff, 0xfc, 0x0f, 0x07, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xde, 0x7f, 0xf0, 0x1f, 0xe3, 0xff, 0x9f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xdf, 0x1f, 0xe0, 0x3f, 0xf9, 0xff, 0xe3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xcf, 0xe3, 0xc0, 0xf8, 0xfd, 0xc7, 0xfc, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xef, 0xff, 0x81, 0xf0, 0x7d, 0x80, 0xff, 0x9f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xf7, 0xff, 0x03, 0xf0, 0xfd, 0x00, 0x7f, 0xf3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xfb, 0xfe, 0x07, 0xff, 0xfa, 0x06, 0x3f, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xfd, 0xf8, 0x0f, 0x7f, 0xe0, 0x0f, 0xbf, 0xff, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xfe, 0xf0, 0x3e, 0x87, 0x80, 0x3f, 0x7f, 0xff, 0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0x78, 0x79, 0xf8, 0x60, 0x7c, 0xff, 0xff, 0xf9, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xbe, 0xf3, 0xff, 0x80, 0xf9, 0xff, 0xef, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xcf, 0x8f, 0xfe, 0x03, 0xf7, 0xff, 0x81, 0xff, 0x9f, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xf7, 0xff, 0xfe, 0x07, 0xef, 0xfe, 0x00, 0x7f, 0xef, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xf9, 0xff, 0xf8, 0x0f, 0x9f, 0xf8, 0x04, 0x3f, 0xfb, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xfe, 0xff, 0xf8, 0x3f, 0x3f, 0xe0, 0x1f, 0x9f, 0xfc, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0x3f, 0xf8, 0x7e, 0x3f, 0xc0, 0x7f, 0xbf, 0xff, 0x7f, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xcf, 0xf8, 0xfe, 0x3e, 0x00, 0xff, 0x3f, 0x9f, 0xdf, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xf7, 0xfc, 0x3f, 0x9c, 0x03, 0xdf, 0x7e, 0x03, 0xe7, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xf9, 0xff, 0xcf, 0xc0, 0x0f, 0x3e, 0xfc, 0x01, 0xfb, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xf1, 0x20, 0x38, 0x7d, 0xf8, 0x18, 0xcd, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0x9f, 0xfe, 0xe0, 0xf0, 0xf9, 0xf0, 0x3e, 0x02, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xe7, 0xff, 0xe3, 0xf8, 0xfb, 0xc0, 0xfe, 0x00, 0x3f, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xf3, 0x07, 0xf7, 0x81, 0xfe, 0x00, 0x1f, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0xfe, 0x43, 0xef, 0x07, 0xfe, 0x1e, 0x4f, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xcf, 0xff, 0xe3, 0xce, 0x0f, 0xfc, 0x7e, 0xe7, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf9, 0xff, 0xf3, 0xd8, 0x1f, 0xfc, 0xf9, 0xf7, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xfc, 0xb0, 0x3e, 0x7d, 0xf3, 0xfb, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xcf, 0xff, 0x60, 0x7c, 0x7f, 0xe7, 0xf9, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf9, 0xff, 0xe1, 0xf8, 0xff, 0xdf, 0xfd, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0xf1, 0xe0, 0xff, 0x3f, 0xfd, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe3, 0xfc, 0xc0, 0xfe, 0x7f, 0xfd, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x7f, 0x81, 0xf9, 0xff, 0xfd, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc7, 0xf9, 0xf3, 0xff, 0xfb, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x3c, 0xcf, 0xff, 0xf3, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x1f, 0xff, 0x81, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0xfe, 0x7f, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x8f, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf3, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

static const unsigned char PROGMEM logo3_bmp[] ={
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,  
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,  
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,  
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,  
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xcf, 0xff, 0xc7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,  
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xf9, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,  
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfd, 0xff, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff,  
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfb, 0xff, 0xff, 0xff, 0x9f, 0xff, 0xff, 0xff, 0xff, 0xff,  
   0xff, 0xff, 0xff, 0xc0, 0x0f, 0xff, 0xf7, 0xff, 0xff, 0x7f, 0xef, 0xff, 0xff, 0xff, 0xff, 0xff,  
   0xff, 0xff, 0xfc, 0x7f, 0xfe, 0x1f, 0xf7, 0xff, 0xfe, 0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff,  
   0xff, 0xff, 0xf7, 0xff, 0xff, 0xf8, 0x77, 0xff, 0xfd, 0xff, 0xf3, 0xff, 0xff, 0xff, 0xff, 0xff,  
   0xff, 0xff, 0xef, 0xff, 0xe7, 0xc7, 0xc3, 0xff, 0xfb, 0xff, 0xfb, 0xff, 0xff, 0xff, 0xff, 0xff,  
   0xff, 0xff, 0xdf, 0xff, 0xc7, 0x80, 0xfc, 0x3f, 0xe7, 0xff, 0xfb, 0xff, 0xff, 0xff, 0xff, 0xff,  
   0xff, 0xff, 0xdf, 0xff, 0xfe, 0x00, 0x1f, 0xc7, 0xcf, 0xff, 0xfb, 0xff, 0xff, 0xff, 0xff, 0xff,  
   0xff, 0xff, 0x9d, 0xff, 0xfc, 0x0f, 0x07, 0xf8, 0x1f, 0xff, 0xfb, 0xff, 0xff, 0xff, 0xff, 0xff,  
   0xff, 0xff, 0xde, 0x7f, 0xf0, 0x1f, 0xe3, 0xff, 0x8f, 0xff, 0xfb, 0xff, 0xff, 0xff, 0xff, 0xff,  
   0xff, 0xff, 0xdf, 0x1f, 0xe0, 0x3f, 0xf9, 0xff, 0xe1, 0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff,  
   0xff, 0xff, 0xcf, 0xe3, 0xc0, 0xf8, 0xfd, 0xc7, 0xfc, 0x7f, 0xef, 0xf8, 0x00, 0x7f, 0xff, 0xff,  
   0xff, 0xff, 0xef, 0xff, 0x81, 0xf0, 0x7d, 0x80, 0xff, 0x8f, 0x9f, 0x8f, 0xff, 0xcf, 0xff, 0xff,  
   0xff, 0xff, 0xf7, 0xff, 0x03, 0xf0, 0xfd, 0x00, 0x7f, 0xf0, 0x7e, 0x7f, 0xff, 0xf9, 0xff, 0xff,  
   0xff, 0xff, 0xfb, 0xfe, 0x07, 0xff, 0xfa, 0x04, 0x3f, 0xfc, 0xfd, 0xff, 0xff, 0xfe, 0x7f, 0xff,  
   0xff, 0xff, 0xfd, 0xf8, 0x0f, 0x7f, 0xe0, 0x0f, 0xbf, 0xff, 0x3b, 0xff, 0xff, 0xff, 0x9f, 0xff,  
   0xff, 0xff, 0xfe, 0xf0, 0x3e, 0x87, 0x80, 0x3f, 0x7f, 0xff, 0xe7, 0xff, 0xff, 0xff, 0xcf, 0xff,  
   0xff, 0xff, 0xff, 0x78, 0x79, 0xf8, 0x60, 0x7c, 0xff, 0xff, 0xf9, 0xff, 0xff, 0x3f, 0xe7, 0xff,  
   0xff, 0xff, 0xff, 0xbe, 0xf3, 0xff, 0x80, 0xf9, 0xff, 0xef, 0xfe, 0x7f, 0xfe, 0x7f, 0xf7, 0xff,  
   0xff, 0xff, 0xff, 0xcf, 0x8f, 0xfe, 0x03, 0xf7, 0xff, 0x81, 0xff, 0x9f, 0xf9, 0xff, 0xfb, 0xff,  
   0xff, 0xff, 0xff, 0xf7, 0xff, 0xfe, 0x07, 0xef, 0xfe, 0x00, 0x7f, 0xe7, 0xe7, 0xff, 0xfb, 0xff,  
   0xff, 0xff, 0xff, 0xf9, 0xff, 0xf8, 0x0f, 0x9f, 0xf8, 0x04, 0x3f, 0xf9, 0x9f, 0xff, 0xfb, 0xff,  
   0xff, 0xff, 0xff, 0xfe, 0xff, 0xf8, 0x3f, 0x3f, 0xe0, 0x1f, 0x9f, 0xfc, 0x7f, 0xff, 0xfb, 0xff,  
   0xff, 0xff, 0xff, 0xff, 0x3f, 0xf8, 0x7e, 0x3f, 0xc0, 0x7f, 0xbf, 0xff, 0x3f, 0xff, 0xf7, 0xff,  
   0xff, 0xff, 0xff, 0xff, 0xdf, 0xf8, 0xfe, 0x3e, 0x00, 0xff, 0x3f, 0x9f, 0xdf, 0xff, 0xf7, 0xff,  
   0xff, 0xff, 0xff, 0xff, 0xf7, 0xfc, 0x3f, 0x9c, 0x03, 0xdf, 0x7e, 0x03, 0xe7, 0xff, 0xcf, 0xff,  
   0xff, 0xff, 0xff, 0xff, 0xf9, 0xff, 0xcf, 0xc0, 0x0f, 0x3e, 0xfc, 0x01, 0xf3, 0xff, 0xbf, 0xff,  
   0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xf1, 0x20, 0x38, 0x7d, 0xf8, 0x18, 0xcc, 0xfc, 0x7f, 0xff,  
   0xff, 0xff, 0xff, 0xff, 0xff, 0x9f, 0xfe, 0xe0, 0xf0, 0xf9, 0xf0, 0x3e, 0x02, 0x07, 0xff, 0xff,  
   0xff, 0xff, 0xff, 0xff, 0xff, 0xe7, 0xff, 0xe3, 0xf8, 0xfb, 0xc0, 0xfe, 0x00, 0x3f, 0xff, 0xff,  
   0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xf3, 0x07, 0xf7, 0x81, 0xfe, 0x00, 0x1f, 0xff, 0xff,  
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0xfe, 0x43, 0xe7, 0x07, 0xfe, 0x1e, 0x4f, 0xff, 0xff,  
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xcf, 0xff, 0xe3, 0xce, 0x0f, 0xfc, 0x7e, 0xe7, 0xff, 0xff,  
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf9, 0xff, 0xf3, 0xd8, 0x1f, 0xfc, 0xf9, 0xf7, 0xff, 0xff,  
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xfc, 0xb0, 0x3e, 0x7d, 0xf3, 0xfb, 0xff, 0xff,  
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff,0xff, 0xcf, 0xff, 0x60, 0x7c, 0x7f, 0xe7, 0xf9, 0xff, 0xff,  
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf9, 0xff, 0xe0, 0xf8, 0xff, 0xdf, 0xfd, 0xff, 0xff,  
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0xf1, 0xe0, 0xff, 0x3f, 0xfd, 0xff, 0xff,  
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe3, 0xfc, 0xc0, 0xfe, 0x7f, 0xfd, 0xff, 0xff,  
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x7f, 0x81, 0xf9, 0xff, 0xfd, 0xff, 0xff,  
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc7, 0xf9, 0xf3, 0xff, 0xfb, 0xff, 0xff,  
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x3c, 0xcf, 0xff, 0xf3, 0xff, 0xff,  
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x1f, 0xff, 0x81, 0xff, 0xff,  
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0xfe, 0x7f, 0xff,  
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x8f, 0xff,  
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf3, 0xff,  
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xff,  
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,  
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,  
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,  
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,  
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,  
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,  
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,  
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,  
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,  
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,  
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

static const unsigned char PROGMEM logodead_bmp[] ={
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x8f, 0xfe, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xe7, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xc0, 0x0f, 0xff, 0xff, 0xfb, 0xff, 0xff, 0xf9, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xfc, 0x7f, 0xfe, 0x1f, 0xff, 0xf7, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xf7, 0xff, 0x9f, 0xf8, 0x7f, 0xef, 0xff, 0xfc, 0xff, 0xbf, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xef, 0xff, 0x93, 0xc7, 0xc7, 0xdf, 0xff, 0xf9, 0xff, 0xdf, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xdf, 0xfc, 0x0f, 0x80, 0xfc, 0x5f, 0xff, 0xf3, 0xff, 0xef, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xdf, 0xff, 0x9e, 0x00, 0x1f, 0xc7, 0xff, 0xe7, 0xff, 0xef, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0x9d, 0xff, 0x9c, 0x0f, 0x07, 0xf8, 0x7f, 0xcf, 0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xde, 0x7f, 0xf0, 0x1f, 0xe3, 0xff, 0x8f, 0x9f, 0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xdf, 0x1f, 0xe0, 0x3f, 0xf9, 0xff, 0xe1, 0x3f, 0xff, 0xf0, 0x00, 0x7f, 0xff, 0xff, 
  0xff, 0xff, 0xcf, 0xe3, 0xc0, 0xf8, 0xfd, 0xc7, 0xfc, 0x7f, 0xff, 0xe7, 0xff, 0xc7, 0xff, 0xff, 
  0xff, 0xff, 0xef, 0xff, 0x81, 0xf0, 0x7d, 0x80, 0xff, 0x8f, 0xff, 0xef, 0xff, 0xf9, 0xff, 0xff, 
  0xff, 0xff, 0xf7, 0xff, 0x03, 0xf0, 0xfd, 0x00, 0x7f, 0xf3, 0xff, 0xdf, 0xff, 0xff, 0x7f, 0xff, 
  0xff, 0xff, 0xfb, 0xfe, 0x07, 0xff, 0xfa, 0x04, 0x3f, 0xfc, 0x7f, 0x3f, 0xff, 0xff, 0x9f, 0xff, 
  0xff, 0xff, 0xfd, 0xf8, 0x0f, 0x7f, 0xe0, 0x0f, 0xbf, 0xff, 0x1c, 0xff, 0xff, 0xff, 0xef, 0xff, 
  0xff, 0xff, 0xfe, 0xf0, 0x3e, 0x87, 0x80, 0x3f, 0x7f, 0xff, 0xe3, 0xff, 0xff, 0x9f, 0xf7, 0xff, 
  0xff, 0xff, 0xff, 0x78, 0x79, 0xf8, 0x60, 0x7c, 0xff, 0xff, 0xf9, 0xff, 0xfe, 0x7f, 0xf3, 0xff, 
  0xff, 0xff, 0xff, 0xbe, 0xf3, 0xff, 0x80, 0xf9, 0xff, 0xef, 0xfe, 0x7f, 0xf9, 0xff, 0xfb, 0xff, 
  0xff, 0xff, 0xff, 0xcf, 0x8f, 0xfe, 0x03, 0xf7, 0xff, 0x81, 0xff, 0x9f, 0xe7, 0xff, 0xfb, 0xff, 
  0xff, 0xff, 0xff, 0xf7, 0xff, 0xfe, 0x07, 0xef, 0xfe, 0x00, 0x7f, 0xe7, 0x8f, 0xff, 0xfb, 0xff, 
  0xff, 0xff, 0xff, 0xf9, 0xff, 0xf8, 0x0f, 0x9f, 0xf8, 0x04, 0x3f, 0xf9, 0x3f, 0xff, 0xfb, 0xff, 
  0xff, 0xff, 0xff, 0xfe, 0xff, 0xf8, 0x3f, 0x3f, 0xe0, 0x1f, 0x9f, 0xfc, 0xff, 0xff, 0xfb, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0x3f, 0xf8, 0x7e, 0x3f, 0xc0, 0x7f, 0xbf, 0xff, 0x3f, 0xff, 0xf7, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xdf, 0xf8, 0xfe, 0x3e, 0x00, 0xff, 0x3f, 0x9f, 0xdf, 0xff, 0xef, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xf7, 0xfc, 0x3f, 0x9c, 0x03, 0xdf, 0x7e, 0x03, 0xe7, 0xff, 0x9f, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xf9, 0xff, 0xcf, 0xc0, 0x0f, 0x3e, 0xfc, 0x01, 0xf3, 0xfe, 0x7f, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xf1, 0x20, 0x38, 0x7d, 0xf8, 0x18, 0xcc, 0x03, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0x9f, 0xfe, 0xe0, 0xf0, 0xf9, 0xf0, 0x3e, 0x02, 0x7f, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xe7, 0xff, 0xe3, 0xf8, 0xfb, 0xc0, 0xfe, 0x00, 0x3f, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xf3, 0x07, 0xf7, 0x81, 0xfe, 0x00, 0x1f, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0xfe, 0x43, 0xe7, 0x07, 0xfe, 0x1e, 0x4f, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xcf, 0xff, 0xe3, 0xce, 0x0f, 0xfc, 0x7e, 0xe7, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf9, 0xff, 0xf3, 0xd8, 0x1f, 0xfc, 0xf9, 0xf7, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xfc, 0xb0, 0x3e, 0x7d, 0xf3, 0xfb, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xcf, 0xff, 0x60, 0x7c, 0x7f, 0xe7, 0xf9, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf9, 0xff, 0xe0, 0xf8, 0xff, 0xdf, 0xfd, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0xf1, 0xe0, 0xff, 0x3f, 0xfd, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe3, 0xfc, 0xc0, 0xfe, 0x7f, 0xfd, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x7f, 0x81, 0xf9, 0xff, 0xfd, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc7, 0xf9, 0xf3, 0xff, 0xfb, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x3c, 0xcf, 0xff, 0xf3, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x1f, 0xff, 0x81, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0xfe, 0x7f, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x8f, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf3, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};


void setup() {
  Serial.begin(115200);
  DS18B20.begin();
  pinMode(BUTTON_PIN, INPUT);
  display.begin(SH1106_SWITCHCAPVCC, 0x3C);
  for (int i = 1; i <= 4; ++i) {
        testdrawbitmap();
        }

   // Draw a small bitmap image
  ina3221.begin();
  ina3221.reset();
  ina3221.setShuntRes(100, 100, 100);
}

void loop() {
  currentState = digitalRead(BUTTON_PIN);
  Serial.print(currentState);
  int index = clicknum%4;
  ifClick();
  switch(index){
    /*
    case 0:
      custom();
      break;
    */
    case 0:
      showbatt();
      break;
    case 1:
      temperature();
      break;
    case 2:
      textdisplay();
      break;
    case 3:
      tech(); 
      break;
  }
  lastState = currentState;
}

void lowbatt(void){
  display.clearDisplay();
  display.drawBitmap(
  (display.width()  - LOGO_WIDTH ) / 2,
  (display.height() - LOGO_HEIGHT) / 2,
  logodead_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
  delay(500);
  display.clearDisplay();
  display.display();
  delay(200);
}


void testdrawbitmap(void) {
  display.clearDisplay();
  display.drawBitmap(
    (display.width()  - LOGO_WIDTH ) / 2,
    (display.height() - LOGO_HEIGHT) / 2,
    logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
  delay(200);
  display.clearDisplay();

  display.drawBitmap(
    (display.width()  - LOGO_WIDTH ) / 2,
    (display.height() - LOGO_HEIGHT) / 2,
    logo2_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
  delay(500);
  display.clearDisplay();
}


void textdisplay(void){
    refresh();
    display.println("Plan Bee");
    display.println("Thanks to");
    display.println("Tony & QJ");
    display.display();
    delay(1000);
}


void temperature(void){
  DS18B20.requestTemperatures();       // send the command to get temperatures
  tempC = DS18B20.getTempCByIndex(0);  // read temperature in °C
  tempF = tempC * 9 / 5 + 32;
  refresh();
  display.println("Temp:");
  display.print(tempC);
  display.println("°C");
  display.print(tempF);
  display.println("°F");
  display.display();
  delay(1000);
}

void showbatt(void){
  float current[3];
  float voltage[3];

  int index = numloop%10;
  currentarray1[index] = ina3221.getCurrent(INA3221_CH1);
  voltagearray1[index] = ina3221.getVoltage(INA3221_CH1);
  numloop++;
  if (numloop<10){
    power1 = avgvalue(powerarr(currentarray1,voltagearray1),numloop);
    energy1 += energy(powerarr(currentarray1,voltagearray1));
    battery1 = battpercent(avgvalue(voltagearray1,numloop));
  }
  else{
    power1 = avgvalue(powerarr(currentarray1,voltagearray1),10);
    energy1 += energy(powerarr(currentarray1,voltagearray1));
    battery1 = battpercent(avgvalue(voltagearray1,10));
  }

  if (battery1<0) {
    refresh();
    display.println("Please");
    display.println("Connect");
    display.println("To ");
    display.print("Battery.");
    display.display();
    delay(1000);
   } 
   
   else if(battery1<=10 && battery1>=0){
      for (int i = 1; i <= 2; ++i) {
        lowbatt();
         }
      refresh();
      display.println("Battery");
      display.print(battery1);
      display.println("%");
      display.print(power1);
      display.println("W");
      display.print(energy1);
      display.println("Ws");
      display.display();
      delay(1000);
   }

   else{
      refresh();
      display.println("Battery");
      display.print(battery1);
      display.println("%");
      display.print(power1);
      display.println("W");
      display.print(energy1);
      display.println("Ws");
      display.display();
      delay(1000);
   }
}

void tech(void){
  refresh();
  display.println("Contact us");
  display.println("at planbee9555@gmail.com");
  display.display();
}

void custom(void){
  refresh();
  display.println("Name");
  display.println("Company");
  display.display();
}

void ifClick(void){
    if (lastState == LOW && currentState == HIGH){
    clicknum++;
  }
}

void refresh(void){
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.setTextColor(WHITE);
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
   *  avgenergy = sum of 10 respective points * 0.0001 s/point
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
