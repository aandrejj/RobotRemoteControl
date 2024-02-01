// Gamepad Shield

// nastavení čísel propojovacích pinů
#define tlacitkoA 2
#define tlacitkoB 3
#define tlacitkoC 4
#define tlacitkoD 5
#define tlacitkoE 6
#define tlacitkoF 7
#define tlacitkoK 8
#define joystickX A0
#define joystickY A1

void setup() {
  // komunikace po sériové lince rychlostí 9600 baud
  Serial.begin(9600);
  // inicializace všech vstupů
  pinMode(tlacitkoA, INPUT);
  pinMode(tlacitkoB, INPUT);
  pinMode(tlacitkoC, INPUT);
  pinMode(tlacitkoD, INPUT);
  pinMode(tlacitkoE, INPUT);
  pinMode(tlacitkoF, INPUT);
  pinMode(tlacitkoK, INPUT);
  pinMode(joystickX, INPUT);
  pinMode(joystickY, INPUT);
}

void loop() {
  // proměnná, podle které se řídí délka pauzy
  int stiskPauza = 0;
  // kontrola pro každé tlačítko, kdy při
  // jeho stisknutí vypíšeme hlášku
  // a nastavíme pauzu
  if (!digitalRead(tlacitkoA)) {
    Serial.println("Stisknuto tlacitko A.");
    stiskPauza = 1;
  }
  if (!digitalRead(tlacitkoB)) {
    Serial.println("Stisknuto tlacitko B.");
    stiskPauza = 1;
  }
  if (!digitalRead(tlacitkoC)) {
    Serial.println("Stisknuto tlacitko C.");
    stiskPauza = 1;
  }
  if (!digitalRead(tlacitkoD)) {
    Serial.println("Stisknuto tlacitko D.");
    stiskPauza = 1;
  }
  if (!digitalRead(tlacitkoE)) {
    Serial.println("Stisknuto tlacitko E.");
    stiskPauza = 1;
  }
  if (!digitalRead(tlacitkoF)) {
    Serial.println("Stisknuto tlacitko F.");
    stiskPauza = 1;
  }
  if (!digitalRead(tlacitkoK)) {
    Serial.println("Stisknuto tlacitko K.");
    stiskPauza = 1;
  }
  // načtení stavu analogových pinů do proměnných
  int osaX = analogRead(joystickX);
  int osaY = analogRead(joystickY);
  // přepočet z celého rozsahu na rozsah -25 až 25,
  // pro větší rozsah nulové pozice je zvolen
  // přepočet na lichý počet čísel a odečtení poloviny
  int souradniceX = map(osaX, 0, 1023, 0, 51) - 24;
  int souradniceY = map(osaY, 0, 1023, 0, 51) - 25;
  // vytištění informací o přepočtené souřadnici joysticku
  Serial.print("X: ");
  Serial.print(souradniceX);
  Serial.print(" | Y: ");
  Serial.print(souradniceY);
  Serial.println();
  // pokud bylo stisknuto alespoň jedno tlačítko,
  // nastav delší pauzu, v opačném případě jen 100 ms
  if (stiskPauza) {
    delay(200);
  } else {
    delay(100);
  }
}
