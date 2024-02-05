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

AT+RESET Restarts the module.
AT+RENEW Restores the default factory settings.
AT+IMME? Query the start mode

AT+HELP?  <------------------------------------------------------------------
https://github.com/danasf/hm10-android-arduino/blob/master/HM-10/datasheet.pdf
AT+HELP?
****************************************************************Command             Description			           *
* ---------------------------------------------------------------- *
*              Check if the command terminal work normally  *
* AT+R        Software reboot				   *
* AT+VERS?   AT+VERSION          Get firmware, bluetooth, HCI and LMP version *
* AT+HELP     List all the commands		           *
* AT+NAME             Get/Set local device name                    *
* AT+PIN               pin code for pairing                 *
* AT+BAUD             Get/d rate		                   *
* AT+LADDR            Get local bluetoth address		   *
* AT+ADDR             Get local bluetooth address		   *
* AT+DEFAULT          Restore factory default			   *
* AT+RENEW            Restore factory default			   *
* AT+STATE            Get current state				   *
* AT+PWRM             Get/Set power on mode(low power) 		   *
* AT+POWE             Get/Set RF transmit power 		   *
* AT+SLEEP            Sleep mode 		                   *
* AT+ROLE             Get/Set current role.	                   *
* AT+PARI             Get/Set UART parity bit.                 *
* AT+STOP             Get/Set UART stop bit.                *
* AT+INQ              Search slave model                       * 
      AT+SHOW             Show the searched slave model.           * 
      AT+CONN             Connect the index slave model.           * 
      AT+IMME             System wait for command when power on.	  * 
      AT+START            System start working.			   *
* AT+UUID             Get/Set system SERVER_UUID .            	*
    AT+CHAR             Get/Set system CHAR_UUID .            	  * 
    -------------------------------------------------------------* 
    Note: (M) = The command support master mode only.
*/
/*
10:43:36.357 -> >AT+IMME?
10:43:36.475 -> OK+Get:1
10:43:43.746 -> >AT+ROLE?
10:43:43.853 -> OK+Get:0
10:43:46.975 -> >AT+ROLE1
10:43:47.104 -> OK+Set:1
10:43:50.477 -> >AT+RESET
10:43:50.591 -> OK+RESET
10:43:59.002 -> >AT+DISC?
10:43:59.118 -> OK+DISCSOK+DIS0:A6D73C040381OK+RSSI:-066
10:43:59.820 -> OK+NAME:L3250 Series
10:43:59.887 -> OK+DIS0:D43639A63BC4OK+RSSI:-062
10:43:59.923 -> OK+NAME:HMSoft
10:44:00.612 -> OK+DIS0:F6AC4B792FA9OK+RSSI:-087
10:44:00.665 -> OK+NAME:Mi Smart Band 6
10:44:02.621 -> OK+DISCE
10:44:39.425 -> >AT+COND43639A63BC4
10:44:39.538 -> OK+CONNAOK+CONNF
*/
/*
AT+CLEAR
AT+CONNL
AT+NOTI1
AT+NOTP1
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
  Serial.println(" ");
  Serial.print("Speed ");
  Serial.print(Baud);
  Serial.println(" .");

  Serial.print("Sketch:   ");   Serial.println(__FILE__);
  Serial.print("Uploaded: ");   Serial.println(__DATE__);
  bluetooth.begin(BTBaud);
  Serial.println("Bluetooth available.");


      // connect to the remote Bluetooth module
    /*
    Serial.print("Sending AT+RENEW");
    bluetooth.print("AT+RENEW" );
    Serial.println (" OK. wait 3000");
    
    delay(2000);
    */
    /*
    Serial.print("Sending AT+IMME1");
    bluetooth.print("AT+IMME1" );
    Serial.println (" OK");

    delay(1000);
    */
    /*
    Serial.print("Sending AT+ROLE1");
    bluetooth.print("AT+ROLE1" );
    Serial.println (" OK");
    
    delay(1000);
    
    Serial.print("Sending AT+RESET");
    bluetooth.print("AT+RESET" );
    Serial.println (" OK. wait 3000");
    
    delay(3000);
    */
    /*
    Serial.print("Sending AT+ROLE?");
    bluetooth.print("AT+ROLE?" );
    Serial.println (" OK");
    
    delay(1000);
    */
    //Serial.print("Sending AT+IMME?");
    //bluetooth.print("AT+IMME?" );
    //Serial.println (" OK");
    
    //delay(1000);
    /*
    Serial.println("Sending AT+DISC?");
    bluetooth.print("AT+DISC?");
    Serial.println (" OK  wait 200");
    
    delay(200);
*/
    //Serial.println("Sending AT+CON3CA308B4E3B5");
    //bluetooth.print("AT+CON3CA308B4E3B5" );
    /*
    Serial.println("Sending AT+COND43639A63BC4");
    bluetooth.print("AT+COND43639A63BC4");
    Serial.println (" OK  wait 3000");
    
    delay(3000);
    
    Serial.print("Sending AT+IMME0");
    bluetooth.print("AT+IMME0" );
    Serial.println (" OK");

    delay(1000);
  */
  Serial.println (" End of seq OK");
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
