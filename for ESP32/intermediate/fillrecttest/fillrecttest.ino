#include "SPI.h"
#include "Adafruit_ILI9341.h"

#define TFT_CLK    32
#define TFT_MOSI   33
#define TFT_MISO   12 
#define TFT_CS     27 
#define TFT_DC     26 
#define TFT_RST    25

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

void setup() {
  // put your setup code here, to run once:
  tft.begin();
  tft.fillScreen(ILI9341_WHITE);
  tft.fillRect(40,40,30,90, ILI9341_BLACK);
}

void loop() {
  // put your main code here, to run repeatedly:

}
