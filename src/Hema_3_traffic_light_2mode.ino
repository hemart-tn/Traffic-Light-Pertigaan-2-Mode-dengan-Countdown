#include <TM1637Display.h>

/* ================= PIN ================= */
// Jalur A
#define A_R 2
#define A_Y 3
#define A_G 4
TM1637Display tmA(A5, A4);

// Jalur B
#define B_R 5
#define B_Y 6
#define B_G 7
TM1637Display tmB(A3, A2);

// Jalur C
#define C_R 8
#define C_Y 9
#define C_G 10
TM1637Display tmC(A1, A0);

// Push Button
#define BTN_NIGHT 11

/* ================= WAKTU ================= */
#define T_RED_INIT 13
#define T_RED      26
#define T_YELLOW   3
#define T_GREEN    10

/* ================= STATE ================= */
enum State { RED, YELLOW, GREEN };

State stateA = RED;
State stateB = RED;
State stateC = YELLOW;

int timerA = T_RED;
int timerB = T_RED_INIT;
int timerC = T_YELLOW;

/* ================= GLOBAL ================= */
unsigned long lastTick = 0;
bool nightMode = false;
bool lastBtn = HIGH;

/* ================= DISPLAY ================= */
void showTime(TM1637Display &tm, int sec) {
  if (sec < 0) sec = 0;
  int mm = sec / 60;
  int ss = sec % 60;
  tm.showNumberDecEx(mm * 100 + ss, 0b01000000, true);
}

void setup() {
  pinMode(A_R, OUTPUT); pinMode(A_Y, OUTPUT); pinMode(A_G, OUTPUT);
  pinMode(B_R, OUTPUT); pinMode(B_Y, OUTPUT); pinMode(B_G, OUTPUT);
  pinMode(C_R, OUTPUT); pinMode(C_Y, OUTPUT); pinMode(C_G, OUTPUT);

  pinMode(BTN_NIGHT, INPUT_PULLUP);

  tmA.setBrightness(0x0f);
  tmB.setBrightness(0x0f);
  tmC.setBrightness(0x0f);

  lastTick = millis();
}

void loop() {

  /* ========= BACA TOMBOL ========= */
  bool nowBtn = digitalRead(BTN_NIGHT);

  // deteksi TEKAN (HIGH → LOW)
  if (lastBtn == HIGH && nowBtn == LOW) {
    delay(20); 
    nightMode = !nightMode;
  }

  lastBtn = nowBtn;

  /* ========= MODE MALAM ========= */
  if (nightMode) {
    static bool blink = false;
    static unsigned long lastBlink = 0;

    if (millis() - lastBlink >= 500) {
      lastBlink = millis();
      blink = !blink;
    }

    digitalWrite(A_R, LOW); digitalWrite(A_G, LOW);
    digitalWrite(B_R, LOW); digitalWrite(B_G, LOW);
    digitalWrite(C_R, LOW); digitalWrite(C_G, LOW);

    digitalWrite(A_Y, blink);
    digitalWrite(B_Y, blink);
    digitalWrite(C_Y, blink);

    tmA.clear();
    tmB.clear();
    tmC.clear();
    return;
  }

  /* ========= MODE NORMAL ========= */
  if (millis() - lastTick >= 1000) {
    lastTick += 1000;

    timerA--;
    timerB--;
    timerC--;

    digitalWrite(A_R, stateA == RED);
    digitalWrite(A_Y, stateA == YELLOW);
    digitalWrite(A_G, stateA == GREEN);
    showTime(tmA, stateA == YELLOW ? 0 : timerA);

    digitalWrite(B_R, stateB == RED);
    digitalWrite(B_Y, stateB == YELLOW);
    digitalWrite(B_G, stateB == GREEN);
    showTime(tmB, stateB == YELLOW ? 0 : timerB);

    digitalWrite(C_R, stateC == RED);
    digitalWrite(C_Y, stateC == YELLOW);
    digitalWrite(C_G, stateC == GREEN);
    showTime(tmC, stateC == YELLOW ? 0 : timerC);

    if (timerA <= 0) {
      if (stateA == RED) { stateA = YELLOW; timerA = T_YELLOW; }
      else if (stateA == YELLOW) { stateA = GREEN; timerA = T_GREEN; }
      else { stateA = RED; timerA = T_RED; }
    }

    if (timerB <= 0) {
      if (stateB == RED) { stateB = YELLOW; timerB = T_YELLOW; }
      else if (stateB == YELLOW) { stateB = GREEN; timerB = T_GREEN; }
      else { stateB = RED; timerB = T_RED; }
    }

    if (timerC <= 0) {
      if (stateC == YELLOW) { stateC = GREEN; timerC = T_GREEN; }
      else if (stateC == GREEN) { stateC = RED; timerC = T_RED; }
      else { stateC = YELLOW; timerC = T_YELLOW; }
    }
  }
}
