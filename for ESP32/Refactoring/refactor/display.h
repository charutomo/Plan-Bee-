#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Beastdevices_INA3221.h>

// Set I2C address to 0x40 (A0 pin -> GND)
// VC connect to 3.3V, GND to GND, SCL to GPIO 22, SDA to GPIO 21 for esp32
Beastdevices_INA3221 ina3221(INA3221_ADDR40_GND);

#define PRINT_DEC_POINTS  3

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

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

void general(void){
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
}

void displaytime(int i){
  display.display();
  delay(i);
}

void lowbatt(void){
  display.clearDisplay();
  display.drawBitmap(
  (display.width()  - LOGO_WIDTH ) / 2,
  (display.height() - LOGO_HEIGHT) / 2,
  logodead_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
  displaytime(500);
  display.clearDisplay();
  displaytime(500);
}

void testdrawbitmap(void) {
  display.clearDisplay();
  display.drawBitmap(
    (display.width()  - LOGO_WIDTH ) / 2,
    (display.height() - LOGO_HEIGHT) / 2,
    logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
  displaytime(200);
  display.clearDisplay();
  display.drawBitmap(
    (display.width()  - LOGO_WIDTH ) / 2,
    (display.height() - LOGO_HEIGHT) / 2,
    logo2_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
  displaytime(500);
  display.clearDisplay();
}


void textdisplay(void){
  general();
  display.println("Plan Bee");
  display.println("Group");
  display.println("Members:");
  displaytime(2000);
  general();
  display.println("Andy, ");
  display.println("Jia Woei,");
  display.println("Jinghui, ");
  displaytime(2000);
  general();
  display.println("Charissa");
  display.println("and");
  display.println("Chermaine");
  displaytime(2000);
  general();
  display.println("Thanks to");
  display.println("Tony and");
  display.println("Qi Jie");
  displaytime(2000);
}

void thanks(void){
  general();
  display.println("Thank You");
  display.println("Have a");
  display.println("Nice Day:)");
  displaytime(2000);
  }
