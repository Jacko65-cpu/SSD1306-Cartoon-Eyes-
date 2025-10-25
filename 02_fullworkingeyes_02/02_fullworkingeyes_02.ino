/************************************************************
 *                                                          *
 *      ♠  Jacko's Eyes OLED Animation Sketch ♠             *
 *                                                          *
 *      Version: 1.0                                        *
 *      Date: 25-10-2025                                    *
 *                 mayby not perfect but it's cool          *
 *  Description:                                            *
 *      This sketch animates cartoon eyes on an SSD1306     *
 *      OLED. Features include:                             *
 *        - Random open/close eyes                          *
 *        - Scanner pupil movement                          *
 *        - Idle pupil tremble                              *
 *        - Super-fast but visible 3-second blink           *
 *        - fast blink every 3 sec                          *
 *  Notes:                                                  *
 *      - Better idees are welcome                          *
 *      - Eye parameters and blink speed can be adjusted    *
 *      - I got help from ai to make it complete            *
 *                                                          *
 ************************************************************/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

// ===== SSD1306 DISPLAY INITIALIZATION =====
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ===== EYE PARAMETERS =====
int eyeWidth = 50;
int eyeHeight = 30;
int eyeY = 16;
int leftEyeX = 26;
int rightEyeX = 102;
int pupilSize = 18;

// ===== TIMER FOR SUPER-FAST VISIBLE BLINK =====
unsigned long lastQuickBlink = 0;       
unsigned long quickBlinkInterval = 3000; // every 3 seconds

// ===== DRAW SINGLE EYE =====
void drawEye(int x, int y, int w, int h, int pupilOffsetX, int pupilOffsetY = 0, bool closed=false) {
  if (!closed) {
    display.fillEllipse(x, y + h / 2, w / 2, h / 2, SSD1306_WHITE);  
    display.drawEllipse(x, y + h / 2, w / 2, h / 2, SSD1306_WHITE);  
    display.fillCircle(x + pupilOffsetX, y + h / 2 + pupilOffsetY, pupilSize / 2, SSD1306_BLACK); 
  } else {
    display.drawLine(x - w/2, y + h/2, x + w/2, y + h/2, SSD1306_WHITE); 
  }
}

// ===== DRAW BOTH EYES =====
void drawEyes(int h, int pupilOffsetX, int pupilOffsetY = 0, bool leftClosed=false, bool rightClosed=false) {
  display.clearDisplay(); 
  drawEye(leftEyeX, eyeY, eyeWidth, h, pupilOffsetX, pupilOffsetY, leftClosed);
  drawEye(rightEyeX, eyeY, eyeWidth, h, pupilOffsetX, pupilOffsetY, rightClosed);
  display.display(); 
}

// ===== SMOOTH BLINK =====
void smoothBlink(int startH, int endH, bool leftOnly=false, bool rightOnly=false, int minDelay=5, int maxDelay=50) {
  int step = (startH < endH) ? 1 : -1;
  int steps = abs(endH - startH);

  for (int i=0; i<=steps; i++) {
    bool leftClosed = (!leftOnly && !rightOnly) ? false : leftOnly;
    bool rightClosed = (!leftOnly && !rightOnly) ? false : rightOnly;
    drawEyes(startH + i*step, 0, 0, leftClosed, rightClosed);
    delay(random(minDelay, maxDelay)); 
  }
  drawEyes(endH, 0, 0, leftOnly, rightOnly);
}

// ===== SCANNER EFFECT =====
void scannerEffect(int h, int duration) {
  int fullScanRange = (eyeWidth / 2 - pupilSize / 2);
  int scanMargin = 3; 
  int scanRange = fullScanRange - scanMargin;
  int pos = 0;
  int dir = 1;
  unsigned long start = millis();

  while (millis() - start < duration) {
    drawEyes(h, pos);
    pos += dir;
    if (pos >= scanRange || pos <= -scanRange) dir = -dir;
    delay(30);
  }
}

// ===== IDLE EYE MOVEMENT =====
void idleEyeMovement(int h, int duration) {
  int fullRangeX = (eyeWidth / 2 - pupilSize / 2) / 3;
  int fullRangeY = h / 6;
  unsigned long start = millis();
  int curX = 0, curY = 0;
  int targetX = 0, targetY = 0;

  while (millis() - start < duration) {
    if (abs(curX - targetX) < 1 && abs(curY - targetY) < 1) {
      targetX = random(-fullRangeX, fullRangeX);
      targetY = random(-fullRangeY, fullRangeY);
    }
    curX += (targetX - curX) * 0.2;
    curY += (targetY - curY) * 0.2;
    drawEyes(h, curX, curY);
    delay(80);
  }
}

// ===== PUPIL TREMBLE =====
void pupilTremble(int h, int duration) {
  unsigned long start = millis();
  float posX = 0;
  float posY = 0;
  float dirX = 1.0;
  float dirY = 1.0;

  while (millis() - start < duration) {
    posX += dirX * random(1, 3) * 0.2;
    posY += dirY * random(0, 2) * 0.2;

    float limitX = (eyeWidth / 2 - pupilSize / 2) / 4;
    float limitY = (h / 2 - pupilSize / 2) / 4;
    if (posX > limitX) { posX = limitX; dirX = -1; }
    if (posX < -limitX) { posX = -limitX; dirX = 1; }
    if (posY > limitY) { posY = limitY; dirY = -1; }
    if (posY < -limitY) { posY = limitY; dirY = 1; }

    drawEyes(h, (int)posX, (int)posY);
    delay(random(40, 90));
  }
}

// ===== SUPER-FAST BUT VISIBLE SINGLE BLINK =====
void singleBlink(int h) {
  drawEyes(h/4, 0);  // eyes close
  display.display();
  delay(50);          // very short but visible blink
  drawEyes(h, 0);    // eyes open
  display.display();
  delay(50);          // tiny pause to make it noticeable
}

// ===== SETUP =====
void setup() {
  randomSeed(analogRead(0));
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
}

// ===== MAIN LOOP =====
void loop() {
  unsigned long now = millis();

  // ==== SUPER-FAST 3-SECOND BLINK ====
  if (now - lastQuickBlink >= quickBlinkInterval) {
    singleBlink(eyeHeight); 
    lastQuickBlink = now;
  }

  // ==== RANDOM STATE SELECTION ====
  int state = random(0, 9);

  switch (state) {
    case 0: // Quick blink normally
      smoothBlink(eyeHeight, eyeHeight / 6);
      delay(random(50, 200));
      smoothBlink(eyeHeight / 6, eyeHeight);
      break;

    case 1: // Fully open eyes + idle movement + effects
      drawEyes(eyeHeight, 0);
      idleEyeMovement(eyeHeight, random(1000, 3000));

      if (random(0,2)==0) singleBlink(eyeHeight);

      pupilTremble(eyeHeight, random(800,2000));
      break;

    case 2: // Slow blink
      smoothBlink(eyeHeight, eyeHeight / 2);
      delay(random(150, 400));
      smoothBlink(eyeHeight / 2, eyeHeight);
      break;

    case 3: // Half open + scanner
      smoothBlink(eyeHeight, eyeHeight / 2);
      scannerEffect(eyeHeight / 2, random(1500, 3000));
      smoothBlink(eyeHeight / 2, eyeHeight);
      break;

    case 4: // Half open rest
      smoothBlink(eyeHeight, eyeHeight / 2);
      delay(random(1000, 2500));
      smoothBlink(eyeHeight / 2, eyeHeight);
      break;

    case 5: // Open eyes + idle pupil movement
      drawEyes(eyeHeight, 0);
      idleEyeMovement(eyeHeight, random(1500, 4000));
      break;

    case 6: // Open eyes + short scan
      drawEyes(eyeHeight, 0);
      scannerEffect(eyeHeight, random(1500, 2500));
      break;

    case 7: // Fully open + pupil tremble
      drawEyes(eyeHeight, 0);
      pupilTremble(eyeHeight, random(1000, 3000));
      break;

    case 8: // Fully open rest
      drawEyes(eyeHeight, 0);
      delay(random(500,1500));
      break;
  } // switch

  delay(random(200, 800)); 
}

