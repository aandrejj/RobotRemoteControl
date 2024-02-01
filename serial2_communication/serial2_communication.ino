// whizzbizz.com - Arnoud van Delden, 1st february 2021
//
// Configures a BT module connected to pins 8~12 (with optional hardware level-shifter in socket)
// For HC-05 press small button on pcb on power up, HM-10 and HM19 should automatically enter AT-mode
//
#include "SoftwareSerial.h"

// Outcomment line below for HM-10, HM-19 etc
//#define HIGHSPEED   // Most modules are only 9600, although you can reconfigure this
#define EN_PIN_HIGH   // You can use this for HC-05 so you don't have to hold the small button on power-up to get to AT-mode

//#ifdef HIGHSPEED
//#define Baud 38400   // Serial monitor
//#define BTBaud 38400 // There is only one speed for configuring HC-05, and that is 38400.
//#else
  #define Baud 9600    // Serial monitor
  #define BTBaud 9600  // HM-10, HM-19 etc
//#endif

#define STATE 8
#define BLUETOOTH_RX 9  // Bluetooth RX -> Arduino D9
#define BLUETOOTH_TX 10 // Bluetooth TX -> Arduino D10
#define GND 11
#define Vcc 12
#define ENABLE 13

//SoftwareSerial bluetooth(BLUETOOTH_TX, BLUETOOTH_RX);

void setup() {
  // Setup BT module
  //pinMode(BLUETOOTH_TX, INPUT);
  //pinMode(BLUETOOTH_RX, OUTPUT);  
  //pinMode(STATE, INPUT);
  //pinMode(GND, OUTPUT);
  //pinMode(Vcc, OUTPUT);
  //pinMode(ENABLE, OUTPUT);
  digitalWrite(GND, LOW);       // Ground for BLE Module
  digitalWrite(Vcc, HIGH);      // Vcc for BLE Module
//#ifdef EN_PIN_HIGH  
//  digitalWrite(ENABLE, HIGH);   // Used to force AT-mode for HC-05. More flexible is to press the button on the pcb
//#endif

  // Start serial communications.
  // The baud rate must be the same for both the serial and the bluetooth.
  Serial.begin(Baud, SERIAL_8N1);
  Serial2.begin(BTBaud);
  Serial.println("Bluetooth available.");
}

char c;

void loop() {
  // From BT(Serial2)-->Serial
  while (Serial2.available()) {
    c = Serial2.read();
    Serial.print(c); 
  }
  
  // From Serial-->BT(Serial2)
  if (Serial.available()) {
    while (Serial.available()) {
      c = Serial.read();
      Serial.print(c);
      Serial2.write(c);
    }
  }
  delay(100);
}
