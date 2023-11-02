#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

#define version 1.0.0

// Define the pins for the buttons and LED
const int BUTTON_SPIDER_1 = 4; // Button for spider 1
const int BUTTON_SPIDER_2 = 5; // Button for spider 2
const int BUTTON_SPIDER_3 = 6; // Button for spider 3
const int BUTTON_SPIDER_4 = 7; // Button for spider 4

const int LED_SPIDER_1 = 8; //
const int LED_SPIDER_2 = 9; //
const int LED_SPIDER_3 = 10; //
const int LED_SPIDER_4 = 11; //
const int GODIS_PIN = 13;      // Pin for the LED
const int LIGHTNING_PIN = 14;  // Connected to gate of mosfet for 12V light

const int BUSY_PIN = 12;      // Pin for player busy 

// Use pins 2 and 3 to communicate with DFPlayer Mini
static const uint8_t PIN_MP3_TX = 2; // Connects to module's RX
static const uint8_t PIN_MP3_RX = 3; // Connects to module's TX
SoftwareSerial softwareSerial(PIN_MP3_RX, PIN_MP3_TX);

// Create the Player object
DFRobotDFPlayerMini player;

// Variables to track button states and last press times
bool lastButton1State = HIGH;
bool lastButton2State = HIGH;
bool lastButton3State = HIGH;
bool lastButton4State = HIGH;
unsigned long lastButton1DebounceTime = 0;
unsigned long lastButton2DebounceTime = 0;
unsigned long lastButton3DebounceTime = 0;
unsigned long lastButton4DebounceTime = 0;
const unsigned long debounceDelay = 100; // Debounce time (milliseconds)

enum States{
  INIT,
  WAIT_FOR_SPIDER_1,
  WAIT_FOR_SPIDER_2,
  WAIT_FOR_SPIDER_3,
  WAIT_FOR_SPIDER_4,
  WAIT_FOR_RESET,
  GAME_OVER,
  UNKOWN
};

States gameState = INIT;

void setup() {
  // Init USB serial port for debugging
  Serial.begin(9600);

  // Initialize button pins
  pinMode(BUTTON_SPIDER_1, INPUT_PULLUP);
  pinMode(BUTTON_SPIDER_2, INPUT_PULLUP);
  pinMode(BUTTON_SPIDER_3, INPUT_PULLUP);
  pinMode(BUTTON_SPIDER_4, INPUT_PULLUP);

  // Initialize LED pin
  pinMode(GODIS_PIN, OUTPUT);
  pinMode(LED_SPIDER_1, OUTPUT);
  pinMode(LED_SPIDER_2, OUTPUT);
  pinMode(LED_SPIDER_3, OUTPUT);
  pinMode(LED_SPIDER_4, OUTPUT);
  pinMode(LIGHTNING_PIN, OUTPUT);

  digitalWrite(LED_SPIDER_1, HIGH);
  digitalWrite(LIGHTNING_PIN, LOW);
  digitalWrite(GODIS_PIN, LOW);

  // Init serial port for DFPlayer Mini
  softwareSerial.begin(9600);
  delay(500);
  Serial.println("see if software serial is ok...");

  // Start communication with DFPlayer Mini
  if (player.begin(softwareSerial)) {
    Serial.println("OK");

    // Set volume to maximum (0 to 30).
    player.volume(30);

    // Play the "0001.mp3" in the "mp3" folder on the SD card
    player.playMp3Folder(1);
    delay(200);
    while(digitalRead(BUSY_PIN) == LOW)
    {
      scaryBlink_1();
    }

  } else {
    Serial.println("Connecting to DFPlayer Mini failed!");
  }
}

void scaryBlink_1()
{
  digitalWrite(LIGHTNING_PIN, HIGH);
  delay(random(10,70));
  digitalWrite(LIGHTNING_PIN, LOW);
  delay(random(300,1500));
}

void gameOverBlink()
{
  digitalWrite(LIGHTNING_PIN, HIGH); 
  delay(300);
  digitalWrite(LIGHTNING_PIN, LOW); 
  delay(100);
}

void button1_activity(int button1State)
{
  // Check if the buttons have changed state
  if (button1State != lastButton1State) {
    lastButton1DebounceTime = millis();
  }

  // If a button state has been stable for a certain time, take action
  if ((millis() - lastButton1DebounceTime) > debounceDelay) {
    if (button1State == LOW) {
      digitalWrite(LED_SPIDER_1, LOW);
      scaryBlink_1();
      player.playMp3Folder(3);
      delay(200);
      while(digitalRead(BUSY_PIN) == LOW)
      {
        scaryBlink_1();
      }
      gameState = WAIT_FOR_SPIDER_2;
      digitalWrite(LED_SPIDER_2, HIGH);
    }
  }
}

void button2_activity(int button2State)
{
  if (button2State != lastButton2State) {
    lastButton2DebounceTime = millis();
  }
  if ((millis() - lastButton2DebounceTime) > debounceDelay) {
    if (button2State == LOW) {
      digitalWrite(LED_SPIDER_2, LOW);
      scaryBlink_1();
      player.playMp3Folder(4);
      delay(500);
      while(digitalRead(BUSY_PIN) == LOW)
      {
        scaryBlink_1();
      }
      gameState = WAIT_FOR_SPIDER_3;
      digitalWrite(LED_SPIDER_3, HIGH);
    }
  }
}

void button3_activity(int button3State)
{
  if (button3State != lastButton3State) {
    lastButton3DebounceTime = millis();
  }
  if ((millis() - lastButton3DebounceTime) > debounceDelay) {
    if (button3State == LOW) {
      digitalWrite(LED_SPIDER_3, LOW);
      scaryBlink_1();
      player.playMp3Folder(1);
      delay(500);
      while(digitalRead(BUSY_PIN) == LOW)
      {
        scaryBlink_1();
      }
      gameState = WAIT_FOR_SPIDER_4;
      digitalWrite(LED_SPIDER_4, HIGH);
    }
  }
}

void button4_activity(int button4State)
{
  if (button4State != lastButton4State) {
    lastButton4DebounceTime = millis();
  }
  if ((millis() - lastButton4DebounceTime) > debounceDelay) {
    if (button4State == LOW) {
      digitalWrite(LED_SPIDER_4, LOW);
      digitalWrite(GODIS_PIN, HIGH);
      player.loop(6);
      delay(500);
      gameOverBlink();
      gameState = GAME_OVER;
    }
  }
}

void button_reset_activity(int button1State)
{
  if (button1State != lastButton1State) {
    lastButton4DebounceTime = millis();
  }
  if ((millis() - lastButton4DebounceTime) > debounceDelay) {
    if (button1State == LOW) {
      digitalWrite(LED_SPIDER_4, LOW);
      digitalWrite(GODIS_PIN, HIGH);
      player.loop(6);
      delay(500);
      gameOverBlink();
      gameState = GAME_OVER;
    }
  }
}

void readButtons(void)
{
  int button1State = digitalRead(BUTTON_SPIDER_1);
  int button2State = digitalRead(BUTTON_SPIDER_2);
  int button3State = digitalRead(BUTTON_SPIDER_3);
  int button4State = digitalRead(BUTTON_SPIDER_4);
  // Read the state of the buttons
  switch (gameState)
  {
    case INIT:
      Serial.println("INIT");
      player.loop(6);
      gameState = WAIT_FOR_SPIDER_1;
      digitalWrite(LED_SPIDER_1, HIGH);
      
      break;
    case WAIT_FOR_SPIDER_1:
      Serial.println("WAIT_FOR_SPIDER_1");
      button1_activity(button1State);
      lastButton1State = button1State;
      break;
    case WAIT_FOR_SPIDER_2:
      Serial.println("WAIT_FOR_SPIDER_2");
      button2_activity(button2State);
      lastButton2State = button2State;
      break;
    case WAIT_FOR_SPIDER_3:
      Serial.println("WAIT_FOR_SPIDER_3");
      button3_activity(button3State);
      lastButton3State = button3State;
      break;
    case WAIT_FOR_SPIDER_4:
      Serial.println("WAIT_FOR_SPIDER_4");
      button4_activity(button4State);
      lastButton4State = button4State;
      break;
    case WAIT_FOR_RESET:
      Serial.println("WAIT_FOR_RESET");
      button_reset_activity(button1State);
      gameOverBlink();
      lastButton1State = button1State;
      break;
    case GAME_OVER:
      Serial.println("GAME_OVER");
      player.loop(6);
      gameState = WAIT_FOR_RESET;
      break;
  }
}

void loop() {
  readButtons();
}
