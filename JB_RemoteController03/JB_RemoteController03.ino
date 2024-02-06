#include "EasyTransfer.h"
#include "SoftwareSerial.h"

//create object
EasyTransfer ET1;   // send serial
EasyTransfer ET2;   // rec serial

#include "Wire.h"  // Comes with Arduino IDE
#include "LiquidCrystal_I2C.h"

#define BUTTON1 3
#define BUTTON2 4
#define BUTTON3 5
#define BUTTON4 33
#define BUTTON5 32

#define BLUETOOTH_SWITCH 35
#define DISPLAY_SWITCH 30

#define SWITCH1Up 20
#define SWITCH2Up 49
#define SWITCH3Up 50
#define SWITCH4Up 51
#define SWITCH5Up 52
#define SWITCH6Up 53

#define SWITCH1Down 54
#define SWITCH2Down 55
#define SWITCH3Down 56
#define SWITCH4Down 57
#define SWITCH5Down 58
#define SWITCH6Down 59

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


// Set the pins on the I2C chip used for LCD connections (Some LCD use Address 0x27 and others use 0x3F):
//LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, 1);  // Set the LCD I2C address (addr, en, rw, rs, d4, d5, d6, d7, backlight, polarity)
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 20 chars and 4 line display


bool button1;
bool button2;
bool button3;
bool button4;
bool button5;

bool bluetooth_On;
bool previous_Bluetooth_State;
bool bluetooth_initialized;
bool bluetooth_connecting;
bool bluetooth_connected;

bool showDataOnDisplay;

bool switch1Up;
bool switch2Up;
bool switch3Up;
bool switch4Up;
bool switch5Up;
bool switch6Up;

bool switch1Down;
bool switch2Down;
bool switch3Down;
bool switch4Down;
bool switch5Down;
bool switch6Down;


int rightJoystick_X;
int rightJoystick_Y;
int leftJoystick_X;
int leftJoystick_Y;

String count;

struct SEND_DATA_STRUCTURE{
  //put your variable definitions here for the data you want to send
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  
    bool menuDown;      
    bool Select; 
    bool menuUp;  
    bool toggleBottom;  
    bool toggleTop; 
    int mode;  

    int16_t thumb_fingertip;
    int16_t thumb_knuckle_left;
    int16_t thumb_knuckle_right;

    int16_t index_finger_fingertip;
    int16_t index_finger_knuckle_left;
    int16_t index_finger_knuckle_right;

    int16_t middle_finger_fingertip;
    int16_t middle_finger_knuckle_left;
    int16_t middle_finger_knuckle_right;
    
    int16_t ring_finger_fingertip;
    int16_t ring_finger_knuckle_left;
    int16_t ring_finger_knuckle_right;

    int16_t pinky_fingertip;
    int16_t pinky_knuckle_left;
    int16_t pinky_knuckle_right;

};

struct RECEIVE_DATA_STRUCTURE_REMOTE{
  //put your variable definitions here for the data you want to receive
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  int16_t mode;
  int16_t count;
};

SEND_DATA_STRUCTURE mydata_send;
RECEIVE_DATA_STRUCTURE_REMOTE mydata_remote;

char c;

int state; // BT state
int previous_state;

unsigned long previousMillis = 0;
const long interval = 100;

unsigned long previousDispMillis = 0;
const long Dispinterval = 10;

SoftwareSerial bluetooth(BLUETOOTH_TX, BLUETOOTH_RX);

//--------------------------------------setup()--------------------------------------
void setup() {

  lcd.begin(20,4);   // Initialize the lcd for 20 chars 4 lines, turn on backlight

  //Serial.begin( 57600);
  Serial.begin(Baud, SERIAL_8N1);
  Serial.println(" ");
  Serial.print("Sketch:   ");   Serial.println(__FILE__);
  Serial.print("Uploaded: ");   Serial.println(__DATE__);

  bluetooth_initialized = false;
  previous_Bluetooth_State = false;
  bluetooth_connecting = false;
  bluetooth_connected = false;

  bluetooth_On = digitalRead(BLUETOOTH_SWITCH);
  showDataOnDisplay = digitalRead(DISPLAY_SWITCH);
  Serial.print("bluetooth_On = "+ String(bluetooth_On));
  Serial.println(" showDataOnDisplay = "+ String(showDataOnDisplay));
   
  if(bluetooth_On) {
    //Serial.println("Bluetooth initialisation....");
    BtConnect();
    //Serial.println("Bluetooth available.");
  }
  //previous_Bluetooth_State = bluetooth_On;

  // NOTE: Cursor Position: (CHAR, LINE) starts at 0  
  lcd.backlight();
  lcd.setCursor(0,0);
       lcd.print("  Remote Controller ");
  Serial.println("  Remote Controller ");
  lcd.setCursor(0,1);
       lcd.print(" andrejovaRobotika. ");
  Serial.println(" andrejovaRobotika. ");
  lcd.setCursor(0,2);
       lcd.print("     blogspot.sk    ");
  Serial.println("     blogspot.sk    ");
  
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(BUTTON3, INPUT_PULLUP);
  pinMode(BUTTON4, INPUT_PULLUP);
  pinMode(BUTTON5, INPUT_PULLUP);
  //pinMode(52, INPUT_PULLUP);
  
  pinMode(BLUETOOTH_SWITCH, INPUT_PULLUP);
  pinMode(DISPLAY_SWITCH, INPUT_PULLUP);
  
  pinMode(SWITCH1Up, INPUT_PULLUP);
  pinMode(SWITCH2Up, INPUT_PULLUP);
  pinMode(SWITCH3Up, INPUT_PULLUP);
  pinMode(SWITCH4Up, INPUT_PULLUP);
  pinMode(SWITCH5Up, INPUT_PULLUP);
  pinMode(SWITCH6Up, INPUT_PULLUP);

  pinMode(SWITCH1Down, INPUT_PULLUP);
  pinMode(SWITCH2Down, INPUT_PULLUP);
  pinMode(SWITCH3Down, INPUT_PULLUP);
  pinMode(SWITCH4Down, INPUT_PULLUP);
  pinMode(SWITCH5Down, INPUT_PULLUP);
  pinMode(SWITCH6Down, INPUT_PULLUP);

  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);

  pinMode(3, INPUT); // BT state

  digitalWrite(2, LOW); // turn off LED
  previous_state = 0;
  delay(1000);
  lcd.clear();
}
//----------------------------end of setup()------------------------------------
//----------------------------BtConnect-----------------------------------------
void BtConnect() {
    Serial.println("Bluetooth initialization....");

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
    
    //Serial2.begin(Baud);
    bluetooth.begin(BTBaud);
    //ET1.begin(details(mydata_send), &Serial2);
    //ET2.begin(details(mydata_remote), &Serial2);
    ET1.begin(details(mydata_send), &bluetooth);
    ET2.begin(details(mydata_remote), &bluetooth);
    bluetooth_initialized = true;
    Serial.println("Bluetooth available.");
    previous_Bluetooth_State = bluetooth_On;
    
}
//----------------------------end of BtConnect----------------------------------

//----------------------pair()--------------------------------------------------
void pair() {
    state = digitalRead(STATE);
    while(state == 0) {
        lcd.setCursor(0,3);
        lcd.print("Waiting to Pair BT  ");
        //lcd.setCursor(0,3);
        //lcd.print("               ");
        previous_state = state;
        state = digitalRead(STATE);    
    }

    delay(500);  // wait before sending data
}
//-----------------end of pair-------------------------------------

//------------------BtWriteEvent-------------------------------------
void BtWriteEvent() {
    if (Serial.available()) {
      Serial.print("bluetooth_On = "+ String(bluetooth_On));
      Serial.println(" showDataOnDisplay = "+ String(showDataOnDisplay));
    }
   
    if(bluetooth_On) {
      // check to see if BT is paired
      previous_state = state;
      state = digitalRead(STATE);

      if (state == 0) {
        pair();
      }
      else {
        if (previous_state==0) {
          lcd.setCursor(0,3);
          lcd.print(" BT Paired to Robot ");
        }
      }
    } else {
      if (showDataOnDisplay) {
        lcd.setCursor(0,3);
        lcd.print(" BT:"+String(bluetooth_On)+  ", Displ:"+String(showDataOnDisplay));
      }
    }// end of if bluetooth_On

    button1 =  digitalRead(BUTTON1);
    button2 =  digitalRead(BUTTON2);
    button3 =  digitalRead(BUTTON3);
    button4 =  digitalRead(BUTTON4);
    button5 =  digitalRead(BUTTON5);

    if (button1 == 0) {
      mydata_send.menuDown = 1;
    } else {
      mydata_send.menuDown = 0;
    }

    if (button2 == 0) {
      mydata_send.Select = 1;
    } else {
      mydata_send.Select = 0;
    }

    if (button3 == 0) {
      mydata_send.menuUp = 1;
    } else {
      mydata_send.menuUp = 0;
    }

    if (button4 == 0) {
      mydata_send.toggleBottom = 1;
    }else {
      mydata_send.toggleBottom = 0;
    }

    if (button5 == 0) {
      mydata_send.toggleTop = 1;
    } else {
      mydata_send.toggleTop = 0;
    }

    leftJoystick_X = analogRead(A0);
    leftJoystick_Y = analogRead(A1);
    rightJoystick_X = analogRead(A2);
    rightJoystick_Y = analogRead(A3);

    mydata_send.index_finger_fingertip = rightJoystick_X;
    mydata_send.index_finger_knuckle_left = rightJoystick_Y;
    mydata_send.index_finger_knuckle_right = leftJoystick_X;
    mydata_send.pinky_knuckle_right = leftJoystick_Y;

    if(bluetooth_On){
      ET1.sendData();
    }
    
    if(showDataOnDisplay){
      lcd.setCursor(0,0);
      lcd.print("LX:"+String(leftJoystick_X)+", RX:"+String(rightJoystick_X)+"  ");
      lcd.setCursor(0,1);
      lcd.print("LY:"+String(leftJoystick_Y)+",  RY:"+String(rightJoystick_Y)+"  ");
      lcd.setCursor(0,2);
      lcd.print("Buttons:"+String(button1)+" "+String(button2)+" "+String(button3)+" "+String(button4)+" "+String(button5));
    }
}
//------------------end of BtWriteEvnet-------------------------------------

//------------------BtReadEvent-------------------------------------------
void BtReadEvent() {
    if(bluetooth_On) {
      if(ET2.receiveData()){
        count = String(mydata_remote.count);
        lcd.setCursor(0,3);
        lcd.print(count);

        if (mydata_remote.mode == 0) {
          lcd.setCursor(0,0);
          lcd.print("Mode 0 - Kin Test   ");
          lcd.setCursor(0,1);
          lcd.print("                    ");
        }
        else if (mydata_remote.mode == 1) {
          lcd.setCursor(0,0);
          lcd.print("Mode 1 - Walk #1    ");
          lcd.setCursor(0,1);
          lcd.print("                    ");
        }
        else if (mydata_remote.mode == 2) {
          lcd.setCursor(0,0);
          lcd.print("Mode 2 -            ");
          lcd.setCursor(0,1);
          lcd.print("                    ");
        }
        else if (mydata_remote.mode == 3) {
          lcd.setCursor(0,0);
          lcd.print("Mode 3 -            ");
          lcd.setCursor(0,1);
          lcd.print("                    ");
        }
        else if (mydata_remote.mode == 4) {
          lcd.setCursor(0,0);
          lcd.print("Mode 4 -            ");
          lcd.setCursor(0,1);
          lcd.print("                    ");
        }
        else if (mydata_remote.mode == 5) {
          lcd.setCursor(0,0);
          lcd.print("Mode 5 -            ");
          lcd.setCursor(0,1);
          lcd.print("                    ");
        }
      } //end of if ET.receivedData()
    }// end of if bluetooth_On          
}
//------------------end of BtReadEvent-------------------------------------------

//-------------------------loop------------------------------------------------
void loop() {
  bluetooth_On = digitalRead(BLUETOOTH_SWITCH);
  showDataOnDisplay = digitalRead(DISPLAY_SWITCH);
  button3 =  digitalRead(BUTTON3);

  if((!previous_Bluetooth_State) && (bluetooth_On)) {
    BtConnect();
  }
  
  if(bluetooth_On && bluetooth_initialized) {
    if(button3) {
      if(showDataOnDisplay) {
        lcd.setCursor(0,3);
        lcd.print("Bluetooth: Connecting...");
        bluetooth_connecting = true;
      }
    }
    if (bluetooth_connecting) {
      
    }
  }

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {  // start timed event for read and send
    previousMillis = currentMillis;
    BtWriteEvent();
  } // end of timed event send

  if (currentMillis - previousDispMillis >= Dispinterval) {  // start timed event for read
    previousDispMillis = currentMillis;  
    BtReadEvent();
  }  // end of second timed event
}
//-----------------end of loop-----------------------------------------