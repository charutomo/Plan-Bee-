#include <Wire.h>
#include <Beastdevices_INA3221.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DallasTemperature.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define SENSOR_PIN  21 // ESP32 pin GPIO21 connected to DS18B20 sensor's DQ pin

// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_MOSI  33
#define OLED_CLK   32
#define OLED_DC    26
#define OLED_CS    27
#define OLED_RESET 25
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

#define LOGO_HEIGHT   64
#define LOGO_WIDTH    128


#define SERIAL_SPEED      115200    // serial baud rate
#define PRINT_DEC_POINTS  3         // decimal points to print

// Set I2C address to 0x40 (A0 pin -> GND)
// VC connect to 3.3V, GND to GND, SCL to GPIO 22, SDA to GPIO 21 for esp32
Beastdevices_INA3221 ina3221(INA3221_ADDR40_GND);

OneWire oneWire(SENSOR_PIN);
DallasTemperature DS18B20(&oneWire);

float tempC; // temperature in Celsius
float tempF; // temperature in Fahrenheit

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
  Serial.begin(SERIAL_SPEED);
  DS18B20.begin();

  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

   for (int i = 1; i <= 4; ++i) {
        testdrawbitmap();
        }

   // Draw a small bitmap image
  textdisplay();

  //for (int i = 1; i <= 6; ++i) {
  //      lowbatt();
  //      }
        
  thanks();
  temperature();
  while (!Serial) {
      delay(1);
  }

  ina3221.begin();
  ina3221.reset();

  // Set shunt resistors to 10 mOhm for all channels
  ina3221.setShuntRes(100, 100, 100);
}

void loop() {
  float current[3];
  float voltage[3];

  current[0] = ina3221.getCurrent(INA3221_CH1);
  voltage[0] = ina3221.getVoltage(INA3221_CH1);
  float power1 = current[0] * voltage[0];
  float battpercent = ((voltage[0]-3.4)/(4.3-3.4))*100;

  current[1] = ina3221.getCurrent(INA3221_CH2);
  voltage[1] = ina3221.getVoltage(INA3221_CH2);
  float power2 = current[1] * voltage[1];

  current[2] = ina3221.getCurrent(INA3221_CH3);
  voltage[2] = ina3221.getVoltage(INA3221_CH3);
  float power3 = current[2] * voltage[2];

  Serial.print("Channel 1: ");
  Serial.print(current[0], PRINT_DEC_POINTS);
  Serial.print("A, ");
  Serial.print(voltage[0], PRINT_DEC_POINTS);
  Serial.print("V, ");
  Serial.print(power1);
  Serial.println("W");

  Serial.print("Channel 2: ");
  Serial.print(current[1], PRINT_DEC_POINTS);
  Serial.print("A, ");
  Serial.print(voltage[1], PRINT_DEC_POINTS);
  Serial.print("V, ");
  Serial.print(power2);
  Serial.println("W");
  Serial.print(battpercent);
  Serial.println("%");
  
  Serial.print("Channel 3: ");
  Serial.print(current[2], PRINT_DEC_POINTS);
  Serial.print("A, ");
  Serial.print(voltage[2], PRINT_DEC_POINTS);
  Serial.print("V, ");
  Serial.print(power3);
  Serial.println("W");
  delay(1000);

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("At Batt:");
  display.print(current[0], PRINT_DEC_POINTS);
  display.println("A, ");
  display.print(voltage[0], PRINT_DEC_POINTS);
  display.println("V, ");
  display.print(power1);
  display.println("W");
  display.display();
  delay(4000);

  if (battpercent<0) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("Please");
    display.println("Connect");
    display.println("To ");
    display.println("Battery.");
    display.display();
    delay(2000);
   } 
   
   else if(battpercent<=10){
      for (int i = 1; i <= 6; ++i) {
        lowbatt();
         }
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.println("Battery");
      display.println("Percent:");
      display.print(battpercent);
      display.println("%");
      display.display();
      delay(2000);

   }

   else{
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("Battery");
    display.println("Percent:");
    display.print(battpercent);
    display.println("%");
    display.display();
    delay(4000);
   }

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("B4 Batt:");
  display.print(current[1], PRINT_DEC_POINTS);
  display.println("A, ");
  display.print(voltage[1], PRINT_DEC_POINTS);
  display.println("V, ");
  display.print(power2);
  display.println("W");
  display.display();
  delay(4000);

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("B4 FWR:");
  display.print(current[2], PRINT_DEC_POINTS);
  display.println("A, ");
  display.print(voltage[2], PRINT_DEC_POINTS);
  display.println("V, ");
  display.print(power3);
  display.println("W");
  display.display();
  delay(4000);

  
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
  delay(500);
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
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Plan Bee");
  display.println("Group");
  display.println("Members:");
  display.display();
  delay(2000);
  
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Andy, ");
  display.println("Jia Woei,");
  display.println("Jinghui, ");
  display.display();
  delay(2000);
  
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Charissa");
  display.println("and");
  display.println("Chermaine");
  display.display();
  delay(2000);
  
  
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Thanks to");
  display.println("Tony and");
  display.println("Qi Jie");
  display.display();
  delay(2000);
  
}


void thanks(void){
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Thank You");
  display.println("Have a");
  display.println("Nice Day:)");
  display.display();
  delay(2000);
  }

void temperature(void){
  DS18B20.requestTemperatures();       // send the command to get temperatures
  tempC = DS18B20.getTempCByIndex(0);  // read temperature in °C
  tempF = tempC * 9 / 5 + 32;
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Temp:");
  display.print(tempC);
  display.println("°C");
  display.print(tempF);
  display.println("°F");
  display.display();
  delay(2000);
}
