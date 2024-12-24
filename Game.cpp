#include <Game.h>

Game::Game(Adafruit_ST7735& display)
  : tft(display), paddleY(128 - PADDLE_HEIGHT - 10), paddleWidth(20),
    ballSpeedX(1), ballSpeedY(1), playerLives(PLAYER_MAX_LIVES), playerScore(0), maxScore(0),
    isGameStarted(false), startTime(0), endTime(0) {
  
  brickColors[0] = ST7735_GREEN;
  brickColors[1] = ST7735_ORANGE;
  brickColors[2] = ST77XX_RED;
}

void Game::setup() {
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);
  tft.setFont();
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);

  analogSetAttenuation(ADC_11db);
  pinMode(VRX_PIN, INPUT);
  pinMode(SW_PIN, INPUT_PULLUP);

  initializeBricks();
  drawBricks();
  drawPaddle();
  initBall();
}

void Game::loop() {
  delay(30);
  int valueX = analogRead(VRX_PIN);
  int swValue = digitalRead(SW_PIN);

  if (swValue == 0) {
    isGameStarted = true;
    if (playerLives == 3) {
      startTime = millis();
    }
  }

  if (valueX < 1800) {
    movePaddle(MOVE_LEFT);
  } else if (valueX > 1900) {
    movePaddle(MOVE_RIGHT);
  }

  if (playerLives == 0) {
    showEndScreen(false);
    while (true);
  }

  if (!isGameStarted) {
    return;
  }

  moveBall();

  if (checkBrickCollision()) {
    playerScore++;
  }

  if (playerScore == maxScore) {
    showEndScreen(true);
    while (true);
  }
}

void Game::initializeBricks() {
  int brickThickness = NUM_ROWS;
  for (int row = 0; row < NUM_ROWS; row++) {
    for (int col = 0; col < NUM_COLS; col++) {
      bricks[row][col] = brickThickness;
      maxScore += brickThickness;
    }
    brickThickness--;
  }
}

void Game::drawBricks() {
  for (int row = 0; row < NUM_ROWS; row++) {
    for (int col = 0; col < NUM_COLS; col++) {
      int brickThickness = bricks[row][col];
      int x = col * (BRICK_WIDTH + 2) + 10;
      int y = row * (BRICK_HEIGHT + 2) + 10;
      if (brickThickness > 0) {
        tft.fillRect(x, y, BRICK_WIDTH, BRICK_HEIGHT, brickColors[brickThickness - 1]);
      } else {
        tft.fillRect(x, y, BRICK_WIDTH, BRICK_HEIGHT, ST7735_BLACK);
      }
    }
  }
}

void Game::drawPaddle() {
  int paddleX = (160 - paddleWidth) / 2;
  
  lastPaddleStep.x = paddleX;
  
  tft.fillRect(paddleX, paddleY, paddleWidth, PADDLE_HEIGHT, ST7735_MAGENTA);
}

void Game::drawBall(int x, int y, int color) {
  tft.fillRect(x, y, BALL_SIZE, BALL_SIZE, color);
}

void Game::initBall() {
  lastBallStep.x = random(20, 141);
  lastBallStep.y = random(50, 81);
  ballSpeedX = 3;
  ballSpeedY = ballSpeedX;
  isGameStarted = false;
  drawBall(lastBallStep.x, lastBallStep.y, ST77XX_CYAN);
}

void Game::moveBall() {
  drawBall(lastBallStep.x, lastBallStep.y, ST7735_BLACK);

  int newBallX = lastBallStep.x;
  int newBallY = lastBallStep.y;

  newBallX += ballSpeedX;
  newBallY += ballSpeedY;

  if (newBallX - BALL_SIZE <= 0 || newBallX + BALL_SIZE >= 159) {
    ballSpeedX = -ballSpeedX;
  }

  if (newBallY - BALL_SIZE <= 0) {
    ballSpeedY = -ballSpeedY;
  }

  if (newBallY + BALL_SIZE >= paddleY && newBallY + BALL_SIZE <= paddleY + PADDLE_HEIGHT &&
      newBallX >= lastPaddleStep.x && newBallX <= lastPaddleStep.x + paddleWidth) {
    ballSpeedY = -ballSpeedY;
    newBallY = paddleY - BALL_SIZE;
  }

  if (newBallY + BALL_SIZE > 127) {
    playerLives--;
    initBall();
    return;
  }

  if (lastBallStep.x != newBallX || lastBallStep.y != newBallY) {
    drawBall(lastBallStep.x, lastBallStep.y, ST7735_BLACK);
    drawBall(newBallX, newBallY, ST77XX_CYAN);
  }
  lastBallStep.x = newBallX;
  lastBallStep.y = newBallY;
}

void Game::movePaddle(Direction currentDirection) {
  int newX = lastPaddleStep.x;
  if(currentDirection == MOVE_LEFT) {
    newX -= PADDLE_MOVE_SPEED;
    if (newX < 0) newX = 0;
  } else if (currentDirection == MOVE_RIGHT) {
    newX += PADDLE_MOVE_SPEED;
    if (newX > 159 - paddleWidth) newX = 159 - paddleWidth;
  }

  if(lastPaddleStep.x != newX) {
    tft.fillRect(lastPaddleStep.x, paddleY, paddleWidth, PADDLE_HEIGHT, ST7735_BLACK);
  }
  
  tft.fillRect(newX, paddleY, paddleWidth, PADDLE_HEIGHT, ST7735_MAGENTA);
  lastPaddleStep.x = newX;
}

bool Game::checkBrickCollision(){
  int ballX = lastBallStep.x;
  int ballY = lastBallStep.y;

  for (int row = 0; row < NUM_ROWS; row++) {
    for (int col = 0; col < NUM_COLS; col++) {
      int brick_thickness = bricks[row][col];
      if (brick_thickness > 0) {
        int brickX = col * (BRICK_WIDTH + 2) + 10;
        int brickY = row * (BRICK_HEIGHT + 2) + 10;

        if (ballX + BALL_SIZE >= brickX && ballX - BALL_SIZE <= brickX + BRICK_WIDTH &&
            ballY + BALL_SIZE >= brickY && ballY - BALL_SIZE <= brickY + BRICK_HEIGHT) {
          brick_thickness--;
          if (brick_thickness == 0) {
            tft.fillRect(brickX, brickY, BRICK_WIDTH, BRICK_HEIGHT, ST7735_BLACK);
          } else {
            tft.fillRect(brickX, brickY, BRICK_WIDTH, BRICK_HEIGHT, brickColors[brick_thickness-1]);
          }
          ballSpeedY = -ballSpeedY;
          bricks[row][col] = brick_thickness;
          return true;
        }
      }
    }
  }
  return false;
}

void Game::showEndScreen(bool isWin) {
  endTime = millis();
  tft.fillScreen(ST77XX_BLACK);

  uint16_t color = isWin ? ST77XX_GREEN : ST77XX_RED;
  const char* message = isWin ? "You Win!" : "Game Over!";

  tft.setTextColor(color);
  tft.setTextSize(2);
  int textWidth = strlen(message) * 6 * 2;
  int textHeight = 8 * 2; 
  int xPosition = (160 - textWidth) / 2;
  int yPosition = (128 - textHeight) / 2;

  tft.setCursor(xPosition, yPosition);
  tft.print(message);

  tft.setTextSize(1);
  yPosition += textHeight + 10;
  tft.setCursor(xPosition, yPosition);

  printPlayerScore();

  tft.setTextSize(1);
  tft.setCursor(xPosition, yPosition + 10);

  printPlayerTime();
}

void Game::printPlayerScore() {
  double percentage = maxScore > 0 ? (double(playerScore) * 100.0) / double(maxScore) : 0.0;
  char percentageStr[10];
  dtostrf(percentage, 5, 2, percentageStr);

  tft.print("Score: ");
  tft.print(percentageStr);
  tft.print("%");
}

void Game::printPlayerTime() {
  unsigned long elapsedTime = (endTime - startTime) / 1000;
  int minutes = elapsedTime / 60;
  int seconds = elapsedTime % 60;
  
  tft.print("Time:  ");
  if (minutes < 10) {
    tft.print("0");
  }
  tft.print(minutes);
  tft.print(":");
  if (seconds < 10) {
    tft.print("0");
  }
  tft.print(seconds);
  tft.print("min");
}
