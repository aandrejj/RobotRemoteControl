// whizzbizz.com - Arnoud van Delden, 1st february 2021
//
// Configures a BT module connected to pins 8~12 (with optional hardware level-shifter in socket)
// For HC-05 press small button on pcb on power up, HM-10 and HM19 should automatically enter AT-mode
//   https://www.whizzbizz.com/en/bluetooth-communication
//
#include "SoftwareSerial.h"


#define IS_HM_10 //uncomment this if module is HM-10

// Outcomment line below for HM-10, HM-19 etc
//#define HIGHSPEED   // Most modules are only 9600, although you can reconfigure this
#define EN_PIN_HIGH   // You can use this for HC-05 so you don't have to hold the small button on power-up to get to AT-mode

#ifdef HIGHSPEED
  #define Baud 38400   // Serial monitor
  #define BTBaud 38400 // There is only one speed for configuring HC-05, and that is 38400.
#else
  #define Baud 9600    // Serial monitor
  #define BTBaud 9600  // HM-10, HM-19 etc
#endif

#define STATE 11
#define BLUETOOTH_RX 9  // Bluetooth RX -> Arduino D9
#define BLUETOOTH_TX 10 // Bluetooth TX -> Arduino D10
//#define GND 13
//#define Vcc 12
#define ENABLE 8

boolean NL = true;

/* ---------------------------------------------------------------
//HC 05
//Enable HIGHSPEED  38400
//AT+ROLE? -> ma byt 1

//AT+CMODE?  -> CMODE1
//AT+ADDR? -> ADDR:FCA8:FF:4411
//
// AT+NAME  
//AT+STATE?  -> INITIALIZED


//AT+UART?
//AT+UART:38400,0,0
https://www.whizzbizz.com/en/bluetooth-communication

//https://alselectro.wordpress.com/2014/10/21/bluetooth-hc05-how-to-pair-two-modules/

 postup pre HC-05
AT+CMODE=1 
AT+INIT
 --- zapnut druhy BT modul aby ho bolo mozne vyhladat. Potom na Maser modula dat dalsi command
AT+INQ   //Start searching:inquire surrounding bluetooth devices
        //A short list of devices found should appear, one of them will be the slave module

AT+UART?
AT+UART:38400,0,0

postup pre HM-10
HM-10
https://people.ece.cornell.edu/land/courses/ece4760/PIC32/uart/HM10/DSD%20TECH%20HM-10%20datasheet.pdf
HM-10

PIN = 000000

Prechod do At-rezimu: staci zapnut modul a nic k nemu nenaparovat - nepripojit (cez BT)
Pri zapinani netreba nic stlacat ani drzat stlacene, do rezimu AT vojde modul sam od seba

uzitocne linky:
https://www.martyncurrey.com/hm-10-bluetooth-4ble-modules/
https://www.martyncurrey.com/hm-10-bluetooth-4ble-modules/#arduinoToHM-10


https://www.martyncurrey.com/hm-10-bluetooth-4ble-modules/

AT -> na odpovedat OK

AT+NAME?
19:31:48.031 -> OK+Get:HMSoft

master
AT+ADDR?
19:32:19.739 -> OK+ADDR:=D43639A63BC4


SLAVE
AT+ADDR?
OK+ADDR:3CA308B4E3B5

AT+CHAR?
19:36:03.324 -> OK_Get:0xFFE1

AT+UUID?
19:36:29.159 -> OK+Get:0xFFE0

AT+ROLE?
19:37:53.160 -> OK+Get:0

AT+VERR?  NEFUNGUJE. NEODPOVEDA  POUZI  AT+VERS?
AT+VERS?
HMSoft V005

AT+NAME?

*/


//https://www.instructables.com/AT-command-mode-of-HC-05-Bluetooth-module/
//https://content.instructables.com/FKY/Z0UT/HX7OYY7I/FKYZ0UTHX7OYY7I.pdf


SoftwareSerial bluetooth(BLUETOOTH_TX, BLUETOOTH_RX);

void setup() {
  // Setup BT module
  pinMode(BLUETOOTH_TX, INPUT);
  pinMode(BLUETOOTH_RX, OUTPUT);  
  pinMode(STATE, INPUT);
  //pinMode(GND, OUTPUT);
  //pinMode(Vcc, OUTPUT);
  pinMode(ENABLE, OUTPUT);
  //digitalWrite(GND, LOW);       // Ground for BLE Module
  //digitalWrite(Vcc, HIGH);      // Vcc for BLE Module
#ifdef EN_PIN_HIGH  
  digitalWrite(ENABLE, HIGH);   // Used to force AT-mode for HC-05. More flexible is to press the button on the pcb
#endif

  // Start serial communications.
  // The baud rate must be the same for both the serial and the bluetooth.
  Serial.begin(Baud, SERIAL_8N1);
  bluetooth.begin(BTBaud);
  Serial.println("Bluetooth available.");
}

char c;

void loop() {
  // From BT-->Serial
  while (bluetooth.available()) {
    c = bluetooth.read();
    //Serial.print("Rec'"+String(c)+"'"); 
    Serial.print(c); 
  }
  
  // From Serial-->BT
  if (Serial.available()) {
    while (Serial.available()) {
      c = Serial.read();
      //Serial.print("Send'"+String(c)+"'");
      

      #ifdef IS_HM_10
      // do not send line end characters to the HM-10
        if (c!=10 & c!=13 ) 
        {  
             bluetooth.write(c);
        }
        
        // Echo the user input to the main window. 
        // If there is a new line print the ">" character.
        if (NL) { Serial.print("\r\n>");  NL = false; }
        
        Serial.print(c);
        
        if (c==10) { NL = true; }
      #else
      Serial.print(c);
        bluetooth.write(c);
      #endif
      
    }
  }
  delay(100);
}
