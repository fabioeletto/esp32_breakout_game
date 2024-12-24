#include <Game.h>

Adafruit_ST7735 tft(TFT_CS, TFT_DC, TFT_SDA, TFT_SCK, TFT_RST);
Game game(tft);

void setup() {
  Serial.begin(9600);
  game.setup();
}

void loop() {
  game.loop();
}