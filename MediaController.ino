#include <BleKeyboard.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

BleKeyboard bleKeyboard("ESP32 Spotify");

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  -1
);

// Pins
#define VRX_PIN 35
#define VRY_PIN 34
#define JOY_SW 32
#define BUTTON_PIN 33

unsigned long lastAction = 0;

// Trigger flags
bool rightTriggered = false;
bool leftTriggered = false;
bool upTriggered = false;
bool downTriggered = false;

void setup() {

  Serial.begin(115200);

  bleKeyboard.begin();

  pinMode(JOY_SW, INPUT_PULLUP);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  Wire.begin(21, 22);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  display.setRotation(2);

  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(WHITE);

  display.setCursor(10,10);
  display.println("Spotify");

  display.setTextSize(1);

  display.setCursor(20,40);
  display.println("Controller");

  display.display();

  delay(2000);
}

void loop() {

  if(!bleKeyboard.isConnected()) {

    display.clearDisplay();

    display.setTextSize(1);

    display.setCursor(0,20);
    display.println("Waiting BT...");

    display.display();

    delay(500);

    return;
  }

  int x = analogRead(VRX_PIN);
  int y = analogRead(VRY_PIN);

  bool joyPressed =
    !digitalRead(JOY_SW);

  bool buttonPressed =
    !digitalRead(BUTTON_PIN);

  // RIGHT -> NEXT
  if(x > 3500 && !rightTriggered) {

    bleKeyboard.write(KEY_MEDIA_NEXT_TRACK);

    showCommand("NEXT");

    rightTriggered = true;
  }

  if(x < 3000)
    rightTriggered = false;

  // LEFT -> PREVIOUS
  if(x < 500 && !leftTriggered) {

    bleKeyboard.write(KEY_MEDIA_PREVIOUS_TRACK);

    showCommand("PREV");

    leftTriggered = true;
  }

  if(x > 1000)
    leftTriggered = false;

  // UP -> VOLUME UP
  if(y > 3500) {

    bleKeyboard.write(KEY_MEDIA_VOLUME_UP);

    showCommand("VOL+");

    upTriggered = true;
  }

  if(y < 3000)
    upTriggered = false;

  // DOWN -> VOLUME DOWN
  if(y < 500) {

    bleKeyboard.write(KEY_MEDIA_VOLUME_DOWN);

    showCommand("VOL-");

    downTriggered = true;
  }

  if(y > 1000)
    downTriggered = false;

  // PLAY/PAUSE
  if(joyPressed && millis() - lastAction > 400) {

    bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE);

    showCommand("PLAY");
  }

  // MUTE
  if(buttonPressed &&
    millis() - lastAction > 400) {

    //bleKeyboard.write(KEY_MEDIA_MUTE);

    showCommand("MUTE");
  }

  delay(10);
}

void showCommand(String cmd) {

  display.clearDisplay();

  display.setTextSize(2);

  display.setCursor(10,20);

  display.println(cmd);

  display.display();

  lastAction = millis();
}