#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

#define TFT_CS     10
#define TFT_RST    8
#define TFT_DC     9

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Очни параметри
int eyeLX = 40, eyeRX = 88, eyeY = 64;
#define EYE_RADIUS 22
#define IRIS_RADIUS 8
#define PUPIL_RADIUS 3

#define BUTTON_HAPPY     2
#define BUTTON_ANGRY     3
#define BUTTON_SURPRISED 4
#define BUTTON_DREAMY    5
#define BUTTON_CONFUSED  6
#define BUTTON_NEUTRAL   7

String currentMood = "neutral";

// Анимации
unsigned long lastBlinkTime = 0;
unsigned long lastEyeMoveTime = 0;
bool isBlinking = false;
int blinkStep = 0;
int irisOffsetX = 0, irisOffsetY = 0;

void setup() {
  tft.initR(INITR_144GREENTAB);
  tft.fillScreen(ST77XX_BLACK);
  randomSeed(analogRead(A0));

  pinMode(BUTTON_HAPPY, INPUT_PULLUP);
  pinMode(BUTTON_ANGRY, INPUT_PULLUP);
  pinMode(BUTTON_SURPRISED, INPUT_PULLUP);
  pinMode(BUTTON_DREAMY, INPUT_PULLUP);
  pinMode(BUTTON_CONFUSED, INPUT_PULLUP);
  pinMode(BUTTON_NEUTRAL, INPUT_PULLUP);

  drawFace(currentMood);
}

void loop() {
  if (digitalRead(BUTTON_HAPPY) == LOW) {
    currentMood = "happy";
    drawFace(currentMood);
    delay(300);
  } 
  else if (digitalRead(BUTTON_ANGRY) == LOW) {
    currentMood = "angry";
    drawFace(currentMood);
    delay(300);
  } 
  else if (digitalRead(BUTTON_SURPRISED) == LOW) {
    currentMood = "surprised";
    drawFace(currentMood);
    delay(300);
  } 
  else if (digitalRead(BUTTON_DREAMY) == LOW) {
    currentMood = "dreamy";
    drawFace(currentMood);
    delay(300);
  } 
  else if (digitalRead(BUTTON_CONFUSED) == LOW) {
    currentMood = "confused";
    drawFace(currentMood);
    delay(300);
  } 
  else if (digitalRead(BUTTON_NEUTRAL) == LOW) {
    currentMood = "neutral";
    drawFace(currentMood);
    delay(300);
  }

  updateAnimation();
}

// --------------------- Анимации -------------------------

void updateAnimation() {
  unsigned long now = millis();

  // Мигане на всеки 4 сек
  if (now - lastBlinkTime > 4000 && !isBlinking) {
    isBlinking = true;
    blinkStep = 0;
    lastBlinkTime = now;
  }

  if (isBlinking) blinkEyes();

  // Движение на ирисите
  if (now - lastEyeMoveTime > 1500) {
    irisOffsetX = random(-2, 3);
    irisOffsetY = random(-2, 3);
    drawFace(currentMood);
    lastEyeMoveTime = now;
  }
}

void blinkEyes() {
  if (blinkStep == 0) {
    // Затваряне
    tft.fillRect(eyeLX - EYE_RADIUS, eyeY - EYE_RADIUS, EYE_RADIUS * 2, EYE_RADIUS * 2, ST77XX_BLACK);
    tft.fillRect(eyeRX - EYE_RADIUS, eyeY - EYE_RADIUS, EYE_RADIUS * 2, EYE_RADIUS * 2, ST77XX_BLACK);
    blinkStep = 1;
    delay(100);
  } else if (blinkStep == 1) {
    // Отваряне
    drawFace(currentMood);
    isBlinking = false;
  }
}

// --------------------- Рисуване -------------------------

void drawFace(String mood) {
  tft.fillScreen(ST77XX_BLACK);
  drawEye(eyeLX, eyeY, mood);
  drawEye(eyeRX, eyeY, mood);
  drawEyebrows(mood);
  drawMouth(mood);
  drawAccessories(mood);
}

void drawEye(int cx, int cy, String mood) {
  int irisX = cx + irisOffsetX, irisY = cy + irisOffsetY;
  int eyelidOffset = 0;

  if (mood == "happy")      { irisX += 3; irisY += 3; eyelidOffset = -2; }
  else if (mood == "angry") { irisX -= 4; irisY -= 2; }
  else if (mood == "surprised") { irisY -= 3; }
  else if (mood == "dreamy") { irisX += 5; irisY -= 3; }
  else if (mood == "confused") { irisX -= 4; irisY += 4; }

  tft.fillCircle(cx, cy, EYE_RADIUS, ST77XX_WHITE);
  tft.fillCircle(irisX, irisY, IRIS_RADIUS, ST77XX_BLUE);
  tft.fillCircle(irisX, irisY, PUPIL_RADIUS, ST77XX_BLACK);
  tft.fillCircle(irisX - 2, irisY - 2, 1, ST77XX_WHITE); // отблясък
}

void drawEyebrows(String mood) {
  int h = eyeY - EYE_RADIUS - 10;
  drawCurvedBrow(eyeLX, h, getBrowType(mood, true));
  drawCurvedBrow(eyeRX, h, getBrowType(mood, false));
}

String getBrowType(String mood, bool left) {
  if (mood == "happy") return "up";
  if (mood == "angry") return left ? "downL" : "downR";
  if (mood == "dreamy") return left ? "tiltL" : "tiltR";
  if (mood == "confused") return "zigzag";
  return "flat";
}

void drawCurvedBrow(int cx, int cy, String type) {
  int o = 12;
  if (type == "up")        tft.drawLine(cx - o, cy + 2, cx + o, cy, ST77XX_WHITE);
  else if (type == "downL")tft.drawLine(cx - o, cy, cx + o, cy + 3, ST77XX_WHITE);
  else if (type == "downR")tft.drawLine(cx - o, cy + 3, cx + o, cy, ST77XX_WHITE);
  else if (type == "tiltL")tft.drawLine(cx - o, cy, cx + o, cy - 2, ST77XX_WHITE);
  else if (type == "tiltR")tft.drawLine(cx - o, cy - 2, cx + o, cy, ST77XX_WHITE);
  else if (type == "zigzag") {
    tft.drawLine(cx - o, cy, cx, cy + 3, ST77XX_WHITE);
    tft.drawLine(cx, cy + 3, cx + o, cy, ST77XX_WHITE);
  }
  else tft.drawLine(cx - o, cy, cx + o, cy, ST77XX_WHITE); // flat
}

void drawMouth(String mood) {
  int mx = 64, my = 110, w = 30;

  if (mood == "happy") {
    tft.drawLine(mx - w / 2, my, mx, my + 5, ST77XX_RED);
    tft.drawLine(mx, my + 5, mx + w / 2, my, ST77XX_RED);
  } 
  else if (mood == "angry") {
    tft.drawLine(mx - w / 2, my + 5, mx + w / 2, my - 5, ST77XX_RED);
  } 
  else if (mood == "surprised") {
    tft.drawCircle(mx, my, 6, ST77XX_WHITE);
  } 
  else if (mood == "dreamy") {
    tft.drawLine(mx - w / 4, my + 2, mx + w / 2, my, ST77XX_MAGENTA);
  } 
  else if (mood == "confused") {
    tft.drawLine(mx - 8, my, mx - 2, my + 5, ST77XX_WHITE);
    tft.drawLine(mx - 2, my + 5, mx + 4, my, ST77XX_WHITE);
    tft.drawLine(mx + 4, my, mx + 8, my + 4, ST77XX_WHITE);
  } 
  else {
    tft.drawLine(mx - 10, my, mx + 10, my, ST77XX_WHITE);
  }
}

void drawAccessories(String mood) {
  if (mood == "surprised") {
    tft.fillCircle(110, 30, 4, ST77XX_CYAN); // пот
  } else if (mood == "angry") {
    tft.drawLine(20, 20, 24, 24, ST77XX_RED);
    tft.drawLine(24, 20, 20, 24, ST77XX_RED); // жилка
  } else if (mood == "confused") {
    tft.setCursor(100, 10);
    tft.setTextColor(ST77XX_YELLOW);
    tft.setTextSize(2);
    tft.print("?");
  }
}
