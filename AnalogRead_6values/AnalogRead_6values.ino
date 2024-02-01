//#include "Mouse.h"
//#include "Keyboard.h"

#define DOF 6
#define TX 0 // translation X
#define TY 1 // translation Y
#define TZ 2 // translation Z
#define RX 3 // rotation X
#define RY 4 // rotation Y
#define RZ 5 // rotation Z

#define SELECTED_APP 1 // 0 for Blender, 1 for Fusion 360 (index of appSetting[] )

#define MAX_APP 5 // number of apps. Currently no function to switch between apps.
#define MAX_MOTION_SET 5 // number of motion set (rotation, translation, zoom, ..) for each app
#define MAX_KEYS 5 // number of simultaneously pressed keys and buttons

#define INVALID -1 // invalid flag / terminator


/// hardware settings
#define TOTAL_DELAY 30 // motion sending cycle
#define INT_DELAY 1 // delay between key and button operations
#define DEAD_THRES 0 // threshold to ignore small motion
#define SPEED_PARAM 300 // larger is slower

// ports of analog input for joysticks
int port[DOF] = {A0, A1, A2, A3, A4, A5};

#define abs(x) ((x)<0?(-x):(x))

int origin[DOF]; // initial sensor values

void setup() {
Serial.begin(115200);
  Serial.println();
  Serial.println("Analog Read");
  Serial.println(" A0, A1, A2, A3, A4, A5");
  //Mouse.begin();
  //Keyboard.begin();
  delay(300);
  for(int i = 0; i < DOF; i++) {
    Serial.print( analogRead(port[i]));
    Serial.print(", ");
  }
  Serial.println(".");
}


void loop() {
  // read sensor value and subtract original position
  Serial.println("");
  Serial.println(" A0,  A1,  A2,  A3,  A4,  A5");
  for(int i = 0; i < DOF; i++) {
    Serial.print( analogRead(port[i]));
    Serial.print(", ");
  }
  Serial.println(".");
  delay(300);
}
