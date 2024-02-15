#define ROTARY_ENCODER_PIN1 2
#define ROTARY_ENCODER_PIN2 3
#define ROTARY_ENCODER_KEY  4

#define LCD_ROWS 4
#define LCD_COLS 20

#define BUTTON1 5
#define BUTTON2 6
#define BUTTON3 7
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

// Configure keyboard keys (ASCII)
#define UP 56     // NUMPAD 8
#define DOWN 50   // NUMPAD 2
#define ENTER 53  // NUMPAD 5
#define BACK 55   // NUMPAD 7



bool rotary_key;
bool previous_rotary_key;
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

byte showForm;

//bool bt_State;

bool showDataOnDisplay;
bool menuIsShown;

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

String sendToBT;
