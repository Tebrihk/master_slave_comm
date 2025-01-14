#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>

RF24 radio(53, 48);  
RF24Network network(radio); 
int bulbGreen = 4;
int bulbBlue = 5;
int bulbRed = 7;
int button = 6; 

const uint16_t this_node = 00;   
const uint16_t other_node = 01;  

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
  unsigned long counter;
};

void setup(void) {
  Serial.begin(9600);
  while (!Serial) {}
  Serial.println(F("SLAVE"));

  if (!radio.begin()) {
    Serial.println(F("Radio hardware not responding!"));
    while (1) {}
  }
  
  radio.setChannel(90);
  network.begin(this_node);

  pinMode(bulbGreen, OUTPUT);  
  pinMode(bulbBlue, OUTPUT);   
  pinMode(bulbRed, OUTPUT); 
  pinMode(button, INPUT_PULLUP);   
}

void loop(void) {
  network.update();  
  if (digitalRead(button) == LOW && (millis() - last_button_pressed > debounce_delay)) {
    last_button_pressed = millis(); 
    payload_t payload = { millis(), packets_sent++ };
    RF24NetworkHeader header(other_node);
    bool ok = network.write(header, &payload, sizeof(payload));

    if (ok) {
      Serial.println(F("acknowledga"));
      greenBlink = false;
      digitalWrite(bulbGreen, LOW);  
      ONgreen = false;
    } else {
      Serial.println(F("Fail")); 
    }
  }

  while (network.available()) {  
    payload_t payload;
    RF24NetworkHeader header; 
    network.read(header, &payload, sizeof(payload));
    Serial.print(" calling");
    greenBlink = true;
  }
  if (greenBlink) {
    if ((millis() - last_action_time) >= 500 && !ONgreen) {
      digitalWrite(bulbGreen, HIGH); 
      ONgreen = true;
    } else if ((millis() - last_action_time) >= 1000 && ONgreen) {
      digitalWrite(bulbGreen, LOW); 
      ONgreen = false;
      last_action_time = millis();  
    }
  }
}
