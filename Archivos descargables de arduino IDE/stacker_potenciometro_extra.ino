#include <Arduino.h>
#include <LedControl.h>

#define DIN_PIN 11
#define CLK_PIN 13
#define CS_PIN 10

#define BUTTON_PIN 2
#define POT_PIN A0

#define MATRIX_COUNT 1
#define WIDTH 8
#define HEIGHT 8

LedControl lc = LedControl(DIN_PIN, CLK_PIN, CS_PIN, MATRIX_COUNT);

unsigned char tower[HEIGHT];

int currentLevel;
int blockX;
int blockWidth;
int direction;

unsigned long lastMoveTime;
unsigned long lastButtonTime;

int gameOver;
int win;

void resetGame(void);
void moveBlock(void);
void placeBlock(void);
void drawGame(void);
void clearAll(void);
void setPixel(int x, int y, int state);
unsigned char makeBlock(int x, int width);
int countBits(unsigned char value);
int firstBit(unsigned char value);

void setup(void){
    int i;

    pinMode(BUTTON_PIN, INPUT_PULLUP);
    for (i = 0; i < MATRIX_COUNT; i++){
        lc.shutdown(i, false);
        lc.setIntensity(i, 15);
        lc.clearDisplay(i);
    }
    resetGame();
    drawGame();
}

void loop(void){
    int potValue;
    int speedDelay;

    if (gameOver || win){
        delay(1000);
        resetGame();
        drawGame();
        return;
    }
    potValue = analogRead(POT_PIN);
    speedDelay = map(potValue, 0, 1023, 500, 80);
    if (millis() - lastMoveTime >= speedDelay){
        lastMoveTime = millis();
        moveBlock();
        drawGame();
    }

    if (digitalRead(BUTTON_PIN) == LOW){
        if (millis() - lastButtonTime > 250){
            lastButtonTime = millis();
            placeBlock();
            drawGame();
        }
    }
}

void resetGame(void){
    int i;

    for (i = 0; i < HEIGHT; i++){
        tower[i] = 0;
    }
    currentLevel = 0;
    blockX = 0;
    blockWidth = 3;
    direction = 1;
    lastMoveTime = 0;
    lastButtonTime = 0;
    gameOver = 0;
    win = 0;
    clearAll();
}

void moveBlock(void){
    blockX = blockX + direction;
    if (blockX <= 0){
        blockX = 0;
        direction = 1;
    }
    if (blockX + blockWidth >= WIDTH){
        blockX = WIDTH - blockWidth;
        direction = -1;
    }
}

void placeBlock(void){
    unsigned char currentBlock;
    unsigned char resultBlock;

    currentBlock = makeBlock(blockX, blockWidth);
    if (currentLevel == 0){
        resultBlock = currentBlock;
    }
    else{
        resultBlock = currentBlock & tower[currentLevel - 1];
    }

    if (resultBlock == 0){
        gameOver = 1;
        return;
    }
    tower[currentLevel] = resultBlock;
    blockWidth = countBits(resultBlock);
    blockX = firstBit(resultBlock);
    direction = 1;
    currentLevel++;
    if (currentLevel >= HEIGHT){
        win = 1;
    }
}

void drawGame(void){
    int y;
    int x;
    unsigned char rowData;

    clearAll();
    for (y = 0; y < HEIGHT; y++){
        rowData = tower[y];
        if (y == currentLevel && !gameOver && !win){
            rowData = makeBlock(blockX, blockWidth);
        }
        for (x = 0; x < WIDTH; x++){
            if (rowData & (1 << x)){
                setPixel(x, y, 1);
            }
        }
    }
}

void setPixel(int x, int y, int state){
  int physicalRow;
  int physicalCol;
  physicalRow = 7 - x;
  physicalCol = y;
  lc.setLed(0, physicalRow, physicalCol, state);
}

void clearAll(void){
    lc.clearDisplay(0);
}

unsigned char makeBlock(int x, int width){
    unsigned char block;
    int i;

    block = 0;
    for (i = 0; i < width; i++){
        block |= (1 << (x + i));
    }
    return block;
}

int countBits(unsigned char value){
    int i;
    int count;

    count = 0;
    for (i = 0; i < WIDTH; i++){
        if (value & (1 << i)){
            count++;
        }
    }

    return count;
}

int firstBit(unsigned char value){
    int i;

    for (i = 0; i < WIDTH; i++){
        if (value & (1 << i)){
            return i;
        }
    }
    return 0;
}