#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>

RF24 radio(9, 8);
RF24Network network(radio);

const uint16_t reception_node = 02;
const uint16_t master_node = 00;
const uint16_t office_node = 01;

const unsigned long debounce_delay = 200;

unsigned long last_action_time_red = 0, last_action_time_green = 0, last_action_time_blue = 0;
unsigned long last_button_pressed = 0;
unsigned long packets_sent = 0;

int bulbGreen = 4;
int bulbBlue = 3;
int bulbRed = 5;
int button1 = 6;
int button2 = 2;

bool greenBlink = false;
bool redBlink = false;
bool blueBlink = false;
bool ONgreen = false;
bool ONred = false;
bool ONblue = false;

struct payload_t {
  unsigned long ms;
  unsigned long counter;
};

void setup() {
  Serial.begin(9600);
  while (!Serial) {}

  Serial.println(F("RECEPTIONIST"));

  if (!radio.begin()) {
    Serial.println(F("Radio hardware not responding!"));
    while (1) {}
  }
  
  radio.setChannel(90);
  network.begin(reception_node);

  pinMode(bulbGreen, OUTPUT);
  pinMode(bulbBlue, OUTPUT);
  pinMode(bulbRed, OUTPUT);
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
}

void loop() {
  network.update();

  // Green LED Blink 
  if (greenBlink) {
    if ((millis() - last_action_time_green) > 500 && !ONgreen) {
      digitalWrite(bulbGreen, HIGH);
      ONgreen = true;
    }
    if ((millis() - last_action_time_green) > 1000 && ONgreen) {
      digitalWrite(bulbGreen, LOW);
      ONgreen = false;
      last_action_time_green = millis();
    }
  }

  // Red LED Blink 
  if (redBlink) {
    if ((millis() - last_action_time_red) > 500 && !ONred) {
      digitalWrite(bulbRed, HIGH);
      ONred = true;
    }
    if ((millis() - last_action_time_red) > 1000 && ONred) {
      digitalWrite(bulbRed, LOW);
      ONred = false;
      last_action_time_red = millis();
    }
  }

  // Blue LED Blink
  if (blueBlink) {
    if ((millis() - last_action_time_blue) > 500 && !ONblue) {
      digitalWrite(bulbBlue, HIGH);
      ONblue = true;
    }
    if ((millis() - last_action_time_blue) > 1000 && ONblue) {
      digitalWrite(bulbBlue, LOW);
      ONblue = false;
      last_action_time_blue = millis();
    }
  }

  // acknowledging director call
  if (digitalRead(button1) == LOW && (millis() - last_button_pressed > debounce_delay) && greenBlink == true) {
    last_button_pressed = millis();
    payload_t payload1 = { millis(), packets_sent++ };
    RF24NetworkHeader header1(master_node);
    bool success = network.write(header1, &payload1, sizeof(payload1));
    if (success) {
      Serial.println(F("Acknowledge"));
      greenBlink = false;
    } else {
      Serial.println(F("Fail to acknowledge"));
      redBlink = true;
    }
  }

// acknowledging office call
  if (digitalRead(button2) == LOW && (millis() - last_button_pressed > debounce_delay) && blueBlink == true) {
    last_button_pressed = millis();
    payload_t payload4 = { millis(), packets_sent++ };
    RF24NetworkHeader header4(office_node);
    bool success4 = network.write(header4, &payload4, sizeof(payload4));
    if (success4) {
      Serial.println(F("Acknowledge"));
      blueBlink = false;
    } else {
      Serial.println(F("Fail to acknowledge"));
      redBlink = true;
    }
  }


  // Receive from director
  while (network.available()) {
    RF24NetworkHeader header;
    payload_t payload;
    if (network.read(header, &payload, sizeof(payload))) {
      if (header.from_node == office_node) {
        Serial.println("Office calling");
        blueBlink = true;
      } else if (header.from_node == master_node) {
        Serial.println("Director calling");
        greenBlink = true;
      }
    }
  }
}
