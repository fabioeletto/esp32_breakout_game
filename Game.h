#ifndef GAME_H
#define GAME_H

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <Pins.h>
#include <Game_Constants.h>

enum Direction {
  MOVE_RIGHT,
  MOVE_LEFT
};

struct Step {
  int x;
  int y;
};

class Game {
public:
  Game(Adafruit_ST7735& display);
  void setup();
  void loop();

private:
  Adafruit_ST7735& tft;

  int paddleY;
  int paddleWidth;
  int ballSpeedX;
  int ballSpeedY;
  int bricks[NUM_ROWS][NUM_COLS];
  int playerLives;
  int playerScore;
  int maxScore;
  bool isGameStarted;
  unsigned long startTime;
  unsigned long endTime;
  Step lastPaddleStep;
  Step lastBallStep;

  int brickColors[NUM_ROWS];

  void initializeBricks();
  void drawBricks();
  void drawPaddle();
  void drawBall(int x, int y, int color);
  void initBall();
  void moveBall();
  void movePaddle(Direction direction);
  bool checkBrickCollision();
  void showEndScreen(bool isWin);
  void printPlayerScore();
  void printPlayerTime();
};

#endif
