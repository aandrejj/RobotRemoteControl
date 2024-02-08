/* Encoder Library - Basic Example
 * http://www.pjrc.com/teensy/td_libs_Encoder.html
 *
 * This example code is in the public domain.
 */

#include <Encoder.h>  //  Encoder Library,  https://github.com/PaulStoffregen/Encoder ,  http://www.pjrc.com/teensy/td_libs_Encoder.html
#include "EasyTransfer.h"
#include "SoftwareSerial.h"
#include "JB_RemoteController.h"
#include "RemoteController_dataStructures.h"

//create object
EasyTransfer ET1;   // send serial
EasyTransfer ET2;   // rec serial

#include "Wire.h"  // Comes with Arduino IDE
#include "LiquidCrystal_I2C.h"
#include "ItemToggle.h"
#include "ItemCommand.h"
#include "ItemInput.h"
#include "ItemSubMenu.h"
#include "ItemList.h"
#include "LcdMenu.h"
#include "utils/commandProccesors.h"



boolean NL = true;
bool bt_State = false;

bool newDataReceived;

// Set the pins on the I2C chip used for LCD connections (Some LCD use Address 0x27 and others use 0x3F):
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 20 chars and 4 line display

// Change these two numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability
Encoder myEnc(ROTARY_ENCODER_PIN1, ROTARY_ENCODER_PIN2);
//   avoid using pins with LEDs attached
long oldPosition  = -999;
long oldEncoderPosition=-999;
long newEncoderPosition = 0;

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

// Configure keyboard keys (ASCII)
#define UP 56     // NUMPAD 8
#define DOWN 50   // NUMPAD 2
#define ENTER 53  // NUMPAD 5
#define BACK 55   // NUMPAD 7

// Declare the call back function
void toggleBacklight(uint16_t isOn);
void hide_menu();
void back_menu();
void start_BT_pair();
extern MenuItem* settingsMenu[];

// Initialize the main menu items
MAIN_MENU(
    ITEM_BASIC("Start service"),
    ITEM_COMMAND("Connect to BT", start_BT_pair),
    ITEM_TOGGLE("Backlight", toggleBacklight),
    ITEM_SUBMENU("Settings", settingsMenu),
    ITEM_BASIC("Blink SOS"),
    ITEM_COMMAND("Hide Menu", hide_menu)
);
/**
 * Create submenu and precise its parent
 */
SUB_MENU(settingsMenu, mainMenu,
    ITEM_BASIC("Backlight"),
    ITEM_BASIC("Contrast"),
    ITEM_COMMAND("..Back", back_menu)
);
// Construct the LcdMenu
LcdMenu menu(LCD_ROWS, LCD_COLS);


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

    BT_to_serial_prepare();

    //Serial.println("Bluetooth available.");
  }
  //previous_Bluetooth_State = bluetooth_On;

  menuIsShown = false;
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
  
  pinMode(ROTARY_ENCODER_KEY, INPUT);
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

  //pinMode(3, INPUT); // BT state

  //digitalWrite(2, LOW); // turn off LED
  previous_state = 0;
  delay(1000);
  lcd.clear();

  menuIsShown = true;
  // Initialize LcdMenu with the menu items
    menu.setupLcdWithMenu(0x27, mainMenu);
}
//----------------------------end of setup()------------------------------------
//----------------------------BT_to_serial_prepare-----------------------------------------
void BT_to_serial_prepare() {

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
//----------------------------end of BT_to_serial_prepare----------------------------------
void scroll_text_on_display() {
  Serial.println("scroll_text_on_display: lcdString3 = '"+String(lcdString3)+"'");

  lcdString0 = lcdString1;
  lcd.setCursor(0,0);
  lcd.print(lcdString0 );

  lcdString1 = lcdString2;
  lcd.setCursor(0,1);
  lcd.print(lcdString1 );
  
  lcdString2 = lcdString3;
  lcd.setCursor(0,2);
  lcd.print(lcdString2 );
  
  lcdString3="";
  lcd.setCursor(0,3);
  lcd.print(lcdString3 );
}
//--------------------------------pair_async-----------------------------------------------
void pair_async(unsigned long currentMillis)
{
  //Serial.println("pair_async:start");
  // From BT-->Serial
  while (bluetooth.available()) {
    c = bluetooth.read();
    //Serial.print("Rec'"+String(c)+"'");
    if (c!=10 & c!=13 ) {  
      lcdString3 = lcdString3 + String(c); 
      Serial.print(c);
      lcd.print(c);
    } else {
      scroll_text_on_display();
    }
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
             //lcd.print(c);
        }
        
        // Echo the user input to the main window. 
        // If there is a new line print the ">" character.
        if (NL) { 
            Serial.print("\r\n>");  
            NL = false; 
            //lcd.println();
            scroll_text_on_display();
          }
        
        Serial.print(c);
        lcd.print(c);
        
        if (c==10) { NL = true; }
      #else
        Serial.print(c);
        lcd.print(c);
        bluetooth.write(c);
      #endif
      
    }
  }
  //delay(100);
  bt_State = Bt_state_checker(currentMillis, previous_state, state);
}
//-----------------------------end of pair_async-------------------------------------------
//----------------------pair()--------------------------------------------------
void pair() {
  Serial.println("pair: started");
    state = digitalRead(STATE);
    Serial.println("pair: state =" + String(state));
    while(state == 0) {
        lcd.setCursor(0,3);
        lcd.print("Waiting to PairBT");
        //lcd.setCursor(0,3);
        //lcd.print("               ");
        previous_state = state;
        state = digitalRead(STATE);
        lcd.print(", "+String(state));
    }

    delay(500);  // wait before sending data
    Serial.println("pair: End. State@end = "+String(state));
}
//-----------------end of pair-------------------------------------

unsigned long milisOfLastStateChanged;
unsigned long maxTimeOfNoChangeMillis = 1500;
unsigned long  currentStateDuration;
bool LedIsBlinking = true;
bool BtLedIsSteadyOn = false;

//-----------------------Bt_state_checker----------------------------------------------
bool Bt_state_checker(unsigned long currentMillis, bool previousState, bool newState) {
  if(previousState!=newState) {
    milisOfLastStateChanged = currentMillis;
    LedIsBlinking = true;
    BtLedIsSteadyOn = false;
  } else {
    currentStateDuration = currentMillis - milisOfLastStateChanged;
    if(currentStateDuration > maxTimeOfNoChangeMillis) {
      LedIsBlinking = false;
      if(newState) {
        BtLedIsSteadyOn = true;
      } else {
        BtLedIsSteadyOn = false;
      }
    } else {
      LedIsBlinking = true;
      BtLedIsSteadyOn = false;
    }
  }
  return BtLedIsSteadyOn;
}
//-----------------------Bt_state_checker----------------------------------------------

//------------------BtWriteEvent-------------------------------------

void BtWriteEvent(unsigned long currentMillis) {

    if (Serial.available()) {
      Serial.print("bluetooth_On = "+ String(bluetooth_On));
      Serial.println(" showDataOnDisplay = "+ String(showDataOnDisplay));
    }
   
    if(bluetooth_On) {
      // check to see if BT is paired
      state = digitalRead(STATE);

      bt_State = Bt_state_checker(currentMillis, previous_state, state);

      previous_state = state;

      if (!bt_State) {
          if(showDataOnDisplay && (!menuIsShown)) {
            //lcd.setCursor(0,3);
            //lcd.print(" BT connecting.. ");
          }
          //Serial.println("BT connecting...");
        //pair();
      }
      else {
          if(showDataOnDisplay && (!menuIsShown)) {
            lcd.setCursor(0,3);
            lcd.print(" BT Paired to Robot ");
          }
          Serial.println("BT Paired to Robot");
      }
    } else {
      if (showDataOnDisplay && (!menuIsShown)) {
        lcd.setCursor(0,3);
        lcd.print(" BT:"+String(bluetooth_On)+  ", Displ:"+String(showDataOnDisplay));
      }
    }// end of if bluetooth_On

    if(bluetooth_On){
      ET1.sendData();
    }
    
    if(showDataOnDisplay && (!menuIsShown)){
      if (!bluetooth_connecting) {
          lcd.setCursor(0,0);
          lcd.print("LX:"+String(leftJoystick_X)+", RX:"+String(rightJoystick_X)+"  ");
          lcd.setCursor(0,1);
          lcd.print("LY:"+String(leftJoystick_Y)+",  RY:"+String(rightJoystick_Y)+"  ");
          lcd.setCursor(0,2);
          lcd.print("Btns:"+String(button1)+" "+String(button2)+" "+String(button3)+" "+String(button4)+" "+String(button5)+" "+String(rotary_key));
      }
    }
}
//------------------end of BtWriteEvnet-------------------------------------

//------------------BtReadEvent-------------------------------------------
bool BtReadEvent() {
  bool _newDataReceived = false;
    if(bluetooth_On) {
      if(ET2.receiveData()){
        _newDataReceived = true;
      } //end of if ET.receivedData()
    }// end of if bluetooth_On          
    return _newDataReceived;
}
//------------------end of BtReadEvent-------------------------------------------

//-------------------------loop------------------------------------------------
void loop() {

  unsigned long currentMillis = millis();
  bluetooth_On = digitalRead(BLUETOOTH_SWITCH);
  showDataOnDisplay = digitalRead(DISPLAY_SWITCH);
  button3 =  digitalRead(BUTTON3);

  if((!previous_Bluetooth_State) && (bluetooth_On)) {
    BT_to_serial_prepare();
  }
  
  if(bluetooth_On && bluetooth_initialized) {
    if(!button3) {
      start_BT_pair();
    }

    if(bluetooth_connecting) {

    } else {
        if(showDataOnDisplay && (!menuIsShown)) {
        lcd.setCursor(0,3);
        lcd.print("Ready to connect");
        Serial.println("BT Ready to connect");
    }

   }


    if (bluetooth_connecting) {
      pair_async(currentMillis);
    }
  }

  if (currentMillis - previousMillis >= interval) {  // start timed event for read and send
    previousMillis = currentMillis;
    ReadHwData();
    BtWriteEvent(currentMillis);
  } // end of timed event send

  if (currentMillis - previousDispMillis >= Dispinterval) {  // start timed event for read
    previousDispMillis = currentMillis;  
    newDataReceived = BtReadEvent();
    if(newDataReceived) {
      ShowDataOnDisplay();
    }
  }  // end of second timed event

  //rotary encoder handling
  if(menuIsShown) {
    long newPosition = myEnc.read();
    if (newPosition != oldPosition) {
      oldPosition = newPosition;
      //Serial.println(newPosition);
      newEncoderPosition = (newPosition/4);
      if(showDataOnDisplay && (!menuIsShown)) {
        lcd.setCursor(16,3);
        lcd.print(String(newEncoderPosition));
      }

      if(oldEncoderPosition != newEncoderPosition) {
        if(newEncoderPosition < oldEncoderPosition) {
          menu.up();
        }
        if(newEncoderPosition > oldEncoderPosition) {
          menu.down();
        }
        oldEncoderPosition = newEncoderPosition;
      }//if(oldEncoderPosition != newEncoderPosition)
    }//if (newPosition != oldPosition)
  }//if(menuIsShown)

  rotary_key = digitalRead(ROTARY_ENCODER_KEY);
  if(previous_rotary_key && (!rotary_key)) {
    if(menuIsShown) {
      Serial.println("loop: Executing menu.enter()...");
      menu.enter();
      //menu.hide();
      //menu.back();
      //menuIsShown = false;
    } else {
      menuIsShown = true;
      Serial.println("loop: Executing menu.show");
      menu.show();
    }
  }
  previous_rotary_key = rotary_key;
}
//-----------------end of loop-----------------------------------------
//-----------------end of loop-----------------------------------------
//-----------------end of loop-----------------------------------------
//-----------------end of loop-----------------------------------------
//-----------------end of loop-----------------------------------------
//-----------------end of loop-----------------------------------------
//-----------------end of loop-----------------------------------------
//-----------------end of loop-----------------------------------------
//-----------------end of loop-----------------------------------------
/**
 * Define callback
 */
void toggleBacklight(uint16_t isOn) { 
  Serial.println("toggleBacklight isOn="+String(isOn));
  menu.setBacklight(isOn);
  
}

void hide_menu() {
  Serial.println("hide menu");
  menu.hide();
  menuIsShown = false;
}
void back_menu() {
  Serial.println("back_menu");
  menu.back();
}
void start_BT_pair(){
  Serial.println("start_BT_pair"); 
  hide_menu();
  if(showDataOnDisplay && (!menuIsShown)) {
    lcd.clear();
    Serial.println("BT Connecting...");
    bluetooth_connecting = true;
    lcdString0=""; lcdString1=""; lcdString2=""; lcdString3="";
    lcd.setCursor(0,0);
    lcd.print("BT manual connecting");
  }

}



//--------------ReadHwData-------------------------------------------
void ReadHwData() {
    rotary_key = digitalRead(ROTARY_ENCODER_KEY);
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


}
//--------------end of ReadHwData------------------------------------

//---------------------ShowDataOnDisplay----------------------------------------
void ShowDataOnDisplay() {
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
}
//----------------------end of ShowDataOnDisplay--------------------------------


