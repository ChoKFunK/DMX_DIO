#include <EEPROM.h>

int button = 2;         // Pin d'entrée, le 2 supporte l'interruption matérielle
int active = 0;         // La sortie ACTIVE LOW actuelle
int stored = 0;         // La sortie ACTIVE LOW en eeprom
int blinks = 0;         // nombre de blinks du LED interne
long blink_on = 100;    // durée en millisecondes de l'allumage d'un blink
long blink_off = 100;   // durée en millisecondes de l'exinction d'un blink
long blink_time = 0;    // utilisé pour marquer le début d'un blink

long time = 0;          // utilisé pour le debounce et l'eeprom
long debounce = 100;    // durée en millisecondes du debounce
long writenow = 10000;  // durée en millisecondes avant écriture en eeprom
const byte numPins = 8; // nombre de sorties (pas de array.length dans arduino apparament)
int state;              // etat d'une sortie - HIGH or LOW

/* pins de sortie - pas de 2 ni 13 */
byte pins[] = {5, 6, 7, 8, 9, 10, 11, 12};

void setup() {
  // Serial.begin(9600);
  active = EEPROMReadInt(0);
  stored = active;
  // Serial.print("Read the following int at the eeprom address 0: " + active);
  // Serial.println(EEPROMReadInt(0), HEX);
  //clearMonitor();   // pour le serial
  /* we setup all led pins as OUTPUT */
  for (int i = 0; i < numPins; i++) {
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], HIGH);
  }

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(button, INPUT);

  /* use pin 2 which has interrupt 0 on Arduino UNO */
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
  if (blinks > 0) {
    if (millis() - blink_time <= blink_on) {
      digitalWrite(LED_BUILTIN, HIGH);
    }
    else {
      digitalWrite(LED_BUILTIN, LOW);
      if (millis() - blink_time > blink_on + blink_off) {
        blinks--;
        blink_time = millis();
      }
    }
  }

  if (millis() - time > writenow) {
    time = millis();
    Serial.println("write test begin: " );
    Serial.println(time);
    if (stored != active) {
      stored = active;
      EEPROMWriteInt(0, active);
      startblink(3);
      Serial.println("write complete: " );
    }
    Serial.println("write test end: " );
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
  startblink(1);
}


void startblink(int number) {
  if (blinks == 0) {  // ne pas démarrer un blink s'il y en a en cours...
    blinks = number;
    blink_time = millis();
  }
}

//This function will write a 2 byte integer to the eeprom at the specified address and address + 1
void EEPROMWriteInt(int p_address, int p_value) {
  byte lowByte = ((p_value >> 0) & 0xFF);
  byte highByte = ((p_value >> 8) & 0xFF);

  EEPROM.write(p_address, lowByte);
  //EEPROM.write(p_address + 1, highByte);
}

//This function will read a 2 byte integer from the eeprom at the specified address and address + 1
unsigned int EEPROMReadInt(int p_address) {
  byte lowByte = EEPROM.read(p_address);
  byte highByte =0; // EEPROM.read(p_address + 1);
  return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);
}



