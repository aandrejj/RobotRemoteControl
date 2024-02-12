/* Encoder Library - Basic Example
 * http://www.pjrc.com/teensy/td_libs_Encoder.html
 *
 * This example code is in the public domain.
 */
#define IS_HM_10 //uncomment this if module is HM-10

#include <Encoder.h>  //  Encoder Library,  https://github.com/PaulStoffregen/Encoder ,  http://www.pjrc.com/teensy/td_libs_Encoder.html
#include "EasyTransfer.h"
#include "SoftwareSerial.h"
#include "JB_RemoteController.h"
#include "RemoteController_dataStructures.h"
#include "MyLcd.h"
#include "MyLcd_SplashScreen.h"
#include "FormData.h"

//create object
EasyTransfer ET1;   // send serial
EasyTransfer ET2;   // rec serial

#include "Wire.h"  // Comes with Arduino IDE
#include <LiquidCrystal_I2C.h>
#include "ItemToggle.h"
#include "ItemCommand.h"
#include "ItemInput.h"
#include "ItemSubMenu.h"
#include "ItemList.h"
#include "LcdMenu.h"
#include "MyLcd.h"
#include "utils/commandProccesors.h"
#include "Menu.h"
#include "constants.h"


boolean NL = true;
bool bt_State = false;
bool previous_bt_state = false;
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
MyLcd myLcd;
MyLcd_SplashScreen myLcd_SplashScreen;


int state; // BT state
int previous_state;

unsigned long previousMillis = 0;
const long interval = 100;

unsigned long previousDispMillis = 0;
const long Dispinterval = 10;

SoftwareSerial bluetooth(BLUETOOTH_TX, BLUETOOTH_RX);


// Construct the LcdMenu
LcdMenu menu(LCD_ROWS, LCD_COLS);


//--------------------------------------setup()--------------------------------------
void setup() {

   
  myLcd.begin(lcd);


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

  //menuIsShown = false;
  showForm = form_SplashScreen;
  // NOTE: Cursor Position: (CHAR, LINE) starts at 0  
  
  lcd.backlight();
  Serial.println("setup: after lcd.backlight..");
  //delay(2000);
  
  Serial.println("setup: Before myLcd.showSplashScreen()..");
  myLcd.showSplashScreen();
  Serial.println("setup: After myLcd.showSplashScreen()..");
  //delay(500);
  Init_PinModes();
  previous_state = 0;
  delay(1000);
  Serial.println("setup: before lcd.clear");
  lcd.clear();

  menuIsShown = true;
  showForm = form_Menu;
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
    pinMode(ENABLE, OUTPUT);
    #ifdef EN_PIN_HIGH  
      digitalWrite(ENABLE, HIGH);   // Used to force AT-mode for HC-05. More flexible is to press the button on the pcb
    #endif
    
    bluetooth.begin(BTBaud);
    ET1.begin(details(mydata_send), &bluetooth);
    ET2.begin(details(mydata_remote), &bluetooth);
    bluetooth_initialized = true;
    Serial.println("Bluetooth available.");
    previous_Bluetooth_State = bluetooth_On;
    
}
//----------------------------end of BT_to_serial_prepare----------------------------------
//--------------------------------bt_serial_async-----------------------------------------------
void bt_serial_async(unsigned long currentMillis)
{
  //Serial.println("bt_serial_async:start");
  // From BT-->Serial
  while (bluetooth.available()) {
    c = bluetooth.read();
    //Serial.print("Rec'"+String(c)+"'");
    //if (c!=10 & c!=13 ) {  
    if (c>31 && c<128 ) {
      myLcd.print(c);
      //lcdStringMain = lcdStringMain + String(c); 
      Serial.print(c);
      //lcd.print(c);
    } else {
      int asciiVal = (unsigned int) c;
      Serial.print("Ascii:");
      Serial.print(String(asciiVal));
      Serial.println(",");
      //Serial.print(c);
      myLcd.scroll_text_on_display();
    }
  }
  
  
  // From Serial-->BT
  if (Serial.available()) {
    String toCumulativeShow0="";
    //String toCumulativeShow1="";
    //String toCumulativeShow2="";
    while (Serial.available()) {
      c = Serial.read();
      // do not send line end characters to the HM-10
      if (c!=10 & c!=13 ) 
      {  
        bluetooth.write(c);
        //lcdStringMain = lcdStringMain +c;
        //myLcd.print(c);
        //lcd.print(c);
      }
      
      // Echo the user input to the main window. 
      // If there is a new line print the ">" character.
      if (NL) { 
        Serial.print("\r\n>");  
        NL = false; 
        //lcd.println();
        //lcdStringMain = lcdStringMain + toCumulativeShow0;
        myLcd.print(toCumulativeShow0);
        myLcd.scroll_text_on_display();
        toCumulativeShow0="";
        //lcdStringMain="";

      }
      
      Serial.print(c);
      //lcdStringMain = lcdStringMain +c;
      toCumulativeShow0 = toCumulativeShow0 + String(c);
      //myLcd.print(c);
      
      if (c==10) { NL = true; }
      
    } // end of while
    myLcd.println(toCumulativeShow0);
    toCumulativeShow0="";

  }
  //delay(100);
  // From command-->BT
  if(sendToBT.length()>0) {
    myLcd.print(sendToBT);
    for(int i=0; i<sendToBT.length(); i++) {
      char charToSend =sendToBT[i];
      bluetooth.write(charToSend);
      //lcdStringMain = lcdStringMain + String(charToSend);
      Serial.print(charToSend);
    }
    sendToBT="";
  }

  bt_State = Bt_state_checker(currentMillis, previous_state, state);
}
//-----------------------------end of bt_serial_async-------------------------------------------
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
      
      previous_bt_state = bt_State;
      bt_State = Bt_state_checker(currentMillis, previous_state, state);

      previous_state = state;

      if (!bt_State) {
        if(previous_bt_state!= bt_State) {
          Serial.println("BT connecting...");
          lcd.setCursor(0,3);
          lcd.print(" BT connecting.. ");
        }
      }
      else {
        if(previous_bt_state!= bt_State) {
          Serial.println("BT Paired to Robot");
          lcd.setCursor(0,3);
          lcd.print(" BT Paired to Robot ");
        }
      }
    } else {
      if(showForm == form_ShowMeasuredData) {
        lcd.setCursor(0,3);
        lcd.print(" BT:"+String(bluetooth_On)+  ", Displ:"+String(showDataOnDisplay));
      }
    }// end of if bluetooth_On

    if(bluetooth_On){
      ET1.sendData();
    }
    
    if(showForm == form_ShowMeasuredData){
      String btnsString = "Btns:"+String(button1)+" "+String(button2)+" "+String(button3)+" "+String(button4)+" "+String(button5)+" "+String(rotary_key);
      myLcd.showMeasuredDateScreen(leftJoystick_X, rightJoystick_X, leftJoystick_Y, rightJoystick_Y, btnsString, "");
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
  button1 =  digitalRead(BUTTON1);
  button3 =  digitalRead(BUTTON3);

  if((!previous_Bluetooth_State) && (bluetooth_On)) {
    BT_to_serial_prepare();
  }
    
  if(bluetooth_On && bluetooth_initialized) {
    if(showForm == form_Menu) {
      if(!button3) {
        start_BT_pair();
      }
    } else {
      if(!button1) {
        Serial.println("Button1: menu.back, menu.show");
        //menu.back();
        menu.show();
        showForm = form_Menu;
        menuIsShown = true;
      }
    }

    if(showForm == form_BluetoothConnecting){
      bt_serial_async(currentMillis);
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
      if(showForm == form_ShowMeasuredData){
        ShowDataOnDisplay();
      }
    }
  }  // end of second timed event

  //rotary encoder handling
  if(showForm == form_Menu) {
    long newPosition = myEnc.read();
    if (newPosition != oldPosition) {
      oldPosition = newPosition;
      //Serial.println(newPosition);
      newEncoderPosition = (newPosition/4);
      if(showForm == form_ShowMeasuredData) {
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
  }//if(showForm == form_Menu)

  rotary_key = digitalRead(ROTARY_ENCODER_KEY);
  if(previous_rotary_key && (!rotary_key)) {
    if(showForm == form_Menu) {
      Serial.println("loop: Executing menu.enter()...");
      menu.hide();
      menu.enter();
      //menu.back();
      //menuIsShown = false;
    } else {
      menuIsShown = true;
      Serial.println("loop: Executing menu.show");
      showForm = form_Menu;
      //menu.back();
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

void at_cmd() {
  Serial.println("at_cmd");
  execute_AT_command("AT");
}
void at_disc_cmd() {
  Serial.println("at_disc");
  execute_AT_command("AT+DISC?");
}
void at_name_cmd() {
  Serial.println("at_disc");
  execute_AT_command("AT+NAME?");
}

void at_addr_cmd() {
  Serial.println("at_c");
  execute_AT_command("AT+ADDR?");
}
void at_connl_cmd() {
  Serial.println("at_connl");
  execute_AT_command("AT+CONNL");
}
void at_con_adr1_cmd() {
  Serial.println("at_con_adr1");
  execute_AT_command("AT+CON3CA308B4E3B5");  
}

void hide_menu() {
  Serial.println("hide menu");
  menu.hide();
  showForm = form_None;
}
void back_menu() {
  Serial.println("back_menu");
  menu.back();
}

void show_measured_data() {
  Serial.println("show_measured_data"); 
  hide_menu();
  showForm = form_ShowMeasuredData;
  myLcd.ShowNewForm("","","","");
}

void start_BT_pair(){
  Serial.println("start_BT_pair"); 
  hide_menu();
  showForm = form_BluetoothConnecting;
  if(showForm == form_BluetoothConnecting) {
    myLcd.ShowNewForm("","","BT Connecting...","BT manual connecting");
  }
}

void execute_AT_command(String str){
  Serial.println("execute_AT_command '"+str+"'"); 
  hide_menu();
  showForm = form_BluetoothConnecting;
  sendToBT = str;
  myLcd.ShowNewForm("","","BT Connecting...",str);
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

//----------------------Init_PinModes------------------------------------
void Init_PinModes()
{
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

}
//--------------------end of Init_PinModes--------------------------------
