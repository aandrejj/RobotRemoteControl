#include "EasyTransfer.h"
#include "SoftwareSerial.h"
#include <Math.h>

//create object
EasyTransfer ET1;   // send serial
EasyTransfer ET2;   // rec serial

// knee calcs
#define DIGITLENGTH 330L    // length of each top/bottom leg
#define KNEEROD 180L       // length of push rod
#define KNEEROD2 94L        // other side of push rod triangle
#define KNEEACTANGLE 15L   // angle between actuator and upp leg joint

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


double legLength;           // required overall leg length
double kneeAngle;           // the actual angle of the knee between top and bottom sections
double kneeAngle2;          // angle between bottom of leg and actuator
double kneeAngle2a;          // angle between bottom of leg and actuator
double kneeAngle3;          // other angle
double kneeAngle3a;          // other angle
double kneeAngle4;          // other angle.
double kneeAngle4a;          // other angle
double kneeActuator;        // calculated length of actuator from joint

int axis1;
int axis2;
int axis3;
int axis4;
int axis5;
int axis6;

int mode;
int count;

int menuFlag = 0;        

struct RECEIVE_DATA_STRUCTURE{
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

struct SEND_DATA_STRUCTURE{
  //put your variable definitions here for the data you want to receive
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  int16_t mode;
  int16_t count;
};

SEND_DATA_STRUCTURE mydata_send;
RECEIVE_DATA_STRUCTURE mydata_remote;

unsigned long previousMillis = 0;
const long interval = 100;

long previousSafetyMillis;

SoftwareSerial bluetooth(BLUETOOTH_TX, BLUETOOTH_RX);

void setup() {

  //Serial.begin(9600);
  //Serial2.begin(9600);
  Serial.begin(Baud, SERIAL_8N1);
  Serial.println(" ");
  Serial.print("Sketch:   ");   Serial.println(__FILE__);
  Serial.print("Uploaded: ");   Serial.println(__DATE__);



  BT_to_serial_prepare();
  //ET1.begin(details(mydata_send), &Serial2);
  //ET2.begin(details(mydata_remote), &Serial2);


}
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
    //bluetooth_initialized = true;
    Serial.println("Bluetooth available.");
    //previous_Bluetooth_State = bluetooth_On;
    
}
//----------------------------end of BT_to_serial_prepare----------------------------------

void loop() {

   unsigned long currentMillis = millis();
       if (currentMillis - previousMillis >= interval) {  // start timed event for read and send
            previousMillis = currentMillis;

            if(ET2.receiveData()){                                        // main data receive
                previousSafetyMillis = currentMillis; 

                mydata_send.mode = mode;
                mydata_send.count = count;

                ET1.sendData();                                           // send data back to remote       
                
                Serial.println( "LX:"+String(mydata_remote.index_finger_knuckle_right)+
                              ", LY:"+String(mydata_remote.pinky_knuckle_right)+
                              ", RX:"+String(mydata_remote.index_finger_fingertip)+
                              ", RY:"+String(mydata_remote.index_finger_knuckle_left));
            } // end of receive data

            else if(currentMillis - previousSafetyMillis > 200) {         // safeties
                Serial.println("no data");
            }

            count = count+1;                                              // update count for remote monitoring
  
            if (mydata_remote.menuUp == 1 && menuFlag == 0) {             // menu select handling & debounce
              menuFlag = 1;
              mode = mode+1;
              mode = constrain(mode,0,5);
            }            
            else if (mydata_remote.menuDown == 1 && menuFlag == 0) {
              menuFlag = 1;
              mode = mode-1;
              mode = constrain(mode,0,5);
            }
            else if (mydata_remote.menuDown == 0 && mydata_remote.menuUp == 0){
              menuFlag = 0;
            }
            
    
            /*
            // initial leg length calcs
            legLength = map(mydata_remote.RT,0,1023,380,650);             // map control data to mm

            kneeAngle = acos ( (sq(DIGITLENGTH) + sq(DIGITLENGTH) - sq(legLength)) / (2 * DIGITLENGTH * DIGITLENGTH) );          
            kneeAngle = (kneeAngle * 4068) / 71;                          // convert radians to degrees
            kneeAngle2a = kneeAngle - KNEEACTANGLE;                       // take away known angle
            kneeAngle2 = (kneeAngle2a * 71) / 4068;                        // convert degrees to radians

            kneeAngle3 = asin ( (KNEEROD2 * sin(kneeAngle2)/KNEEROD));
            kneeAngle3a = (kneeAngle3 * 4068 / 71);                        // convert radians to degrees

            kneeAngle4a = 180 - kneeAngle2a - kneeAngle3a;                  // we now know all four angles
            kneeAngle4 = (kneeAngle4a * 71) / 4068;                        // convert degrees to radians

            kneeActuator = (((sin(kneeAngle4)) * KNEEROD)) / sin(kneeAngle2);

            Serial.print(legLength);
            Serial.print(" , ");
            Serial.println(kneeActuator);      
            */

       }  // end of timed event

}

