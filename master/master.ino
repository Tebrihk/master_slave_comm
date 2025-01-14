#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>

RF24 radio(7, 8);  
RF24Network network(radio);  
int bulbGreen = 4;
int bulbBlue = 5;
int bulbRed = 3;
int button = 6; 

const uint16_t this_node = 01;   
const uint16_t other_node = 00;  

const unsigned long interval = 2000;  

unsigned long last_sent;     
unsigned long packets_sent; 
unsigned long last_action_time = 0;
unsigned long last_button_pressed = 0;
const unsigned long debounce_delay = 200;

bool greenBlink = false;
bool redBlink = false;
bool blueBlink = false;
bool ONgreen = false; 
bool ONred = false; 

struct payload_t {  
  unsigned long ms;
  String counter;
};

void setup(void) {
  Serial.begin(9600);
  pinMode(bulbGreen, OUTPUT);
  pinMode(bulbBlue, OUTPUT);
  pinMode(bulbRed, OUTPUT);
  pinMode(button, INPUT_PULLUP);
  while (!Serial) {
  }
  Serial.println(F("MASTER"));

  if (!radio.begin()) {
    Serial.println(F("Radio hardware not responding!"));
    while (1) {
    }
  }
  radio.setChannel(90);
  network.begin(this_node);
}

void loop() {
  network.update();  
  if (digitalRead(button) == LOW && (millis() - last_button_pressed > debounce_delay)) {
    last_button_pressed = millis();
    digitalWrite(bulbBlue, LOW);

    payload_t payload = { 100, "green" };
    RF24NetworkHeader header(other_node);
    bool ok = network.write(header, &payload, sizeof(payload));
    if (ok) {
      Serial.println(F("CALL sent"));
      greenBlink = true;
      digitalWrite(bulbRed, LOW);
      redBlink = false;   
    } 
    else {
      Serial.println(F("CALL failed to send"));
      redBlink = true;   
    }  
    last_action_time = millis();  
  }

  if (greenBlink && (millis() - last_action_time) > 500 && !ONgreen) {
    digitalWrite(bulbGreen, HIGH);
    ONgreen = !ONgreen;
  } 
  if (greenBlink && (millis() - last_action_time) > 1000 && ONgreen) {
    digitalWrite(bulbGreen, LOW);
    ONgreen = !ONgreen;
    last_action_time = millis();
  }

  if (redBlink && (millis() - last_action_time) > 500 && !ONred) {
    digitalWrite(bulbRed, HIGH);
    ONred = !ONred;
  } 
  if (redBlink && (millis() - last_action_time) > 1000 && ONred) {
    digitalWrite(bulbRed, LOW);
    ONred = !ONred;
    last_action_time = millis();
  }

  while (network.available()) {
    RF24NetworkHeader header;
    payload_t response;
    network.read(header, &response, sizeof(response));
    greenBlink = true;
    if (greenBlink) {
      greenBlink = false;
      digitalWrite(bulbGreen, LOW);
    }
  }
}
