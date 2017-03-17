#include "Flasher.cpp"; // Le fichier de la classe Flasher
#include <EEPROM.h>

int button = 2;         // Pin d'entrée, le 2 supporte l'interruption matérielle
int active = 0;         // La sortie ACTIVE LOW actuelle
int stored = 0;         // La sortie ACTIVE LOW en eeprom
long time = 0;          // utilisé pour le debounce et l'eeprom
long debounce = 50;     // durée en millisecondes du debounce
long writenow = 20000;  // durée en millisecondes avant écriture en eeprom
const byte numPins = 8; // nombre de sorties (pas de array.length dans arduino apparament)
int state;              // etat d'une sortie - HIGH or LOW

//pins de sortie - pas de 2 ni 13
byte pins[] = {5, 6, 7, 8, 9, 10, 11, 12};

// Initialisation du flasher: Flasher(int pin, long on, long off) - long: durée en ms
Flasher led(13, 50, 100);

void setup() {
  active = EEPROMReadInt(0);
  stored = active;
  /* we setup all led pins as OUTPUT */
  for (int i = 0; i < numPins; i++) {
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], HIGH);
  }
  
  pinMode(button, INPUT);

  /*  -use pin 2 which has interrupt 0 on Arduino UNO,
      -la fonction count() est appelé à chaque fois que pin 2 est LOW 
  */
  attachInterrupt(0, count, LOW);
}

void loop() {
  for (int i = 0; i < numPins; i++) {
    state = HIGH;
    if (i == active) {
      state = LOW;
    }
    digitalWrite(pins[i], state);
  }

  // Verifier s'il y a un update à faire pour l'oblet led
  led.Update();
  
  if (millis() - time > writenow) {
    time = millis();
    if (stored != active) {
      stored = active;
      EEPROMWriteInt(0, active);
      led.Blink(3);   // Commander 3 blinks à la led
    }
  }
}

/* fonction callback de l'interruption */
void count() {
  // we debounce the button and increase position
  if (millis() - time > debounce)  active++;
  if (active == numPins) {
    active = 0; //ACTIVE LOW
  }
  time = millis();
  led.Blink(1);   // Commander 1 blink à la led
}

//This function will write a 2 byte integer to the eeprom at the specified address and address + 1
void EEPROMWriteInt(int p_address, int p_value) {
  byte lowByte = ((p_value >> 0) & 0xFF);
  byte highByte = ((p_value >> 8) & 0xFF);

  EEPROM.write(p_address, lowByte);
  //EEPROM.write(p_address + 1, highByte); Disabled car seul l'octet bas est considéré (8 sorties)
}

//This function will read a 2 byte integer from the eeprom at the specified address and address + 1
unsigned int EEPROMReadInt(int p_address) {
  byte lowByte = EEPROM.read(p_address);
  byte highByte = 0; // EEPROM.read(p_address + 1); Disabled car seul l'octet bas est considéré (8 sorties)
  return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);
}



