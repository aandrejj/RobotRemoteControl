#include <EasyTransfer.h>

#define STATE 3
#define BLUETOOTH_RX 9  // Bluetooth RX -> Arduino D9
#define BLUETOOTH_TX 10 // Bluetooth TX -> Arduino D10
#define GND 11
#define Vcc 12
#define ENABLE 13


//create object
EasyTransfer ET1;   // send serial
EasyTransfer ET2;   // rec serial

#include <Wire.h>  // Comes with Arduino IDE
#include <LiquidCrystal.h>

// Set the pins on the I2C chip used for LCD connections (Some LCD use Address 0x27 and others use 0x3F):
//LiquidCrystal_I2C lcd(0x27, 2, 1);  // Set the LCD I2C address (addr, en, rw, rs, d4, d5, d6, d7, backlight, polarity)
#define lcdSvit 10
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

bool but1;
bool but2;
bool but3;
bool but4;
bool but5;

int axis1;
int axis2;
int axis3;
int axis4;
int axis5;
int axis6;

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
    int16_t RLR;
    int16_t RFB;
    int16_t RT;
    int16_t LLR;
    int16_t LFB;
    int16_t LT;
};

struct RECEIVE_DATA_STRUCTURE_REMOTE{
  //put your variable definitions here for the data you want to receive
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  int16_t mode;
  int16_t count;
};

SEND_DATA_STRUCTURE mydata_send;
RECEIVE_DATA_STRUCTURE_REMOTE mydata_remote;

int state; // BT state

unsigned long previousMillis = 0;
const long interval = 40;

unsigned long previousDispMillis = 0;
const long Dispinterval = 10;

void setup() {

  //lcd.begin(20,4);   // Initialize the lcd for 20 chars 4 lines, turn on backlight
  // zahájení komunikace s displejem,
  // 16 znaků, 2 řádky
  lcd.begin(16, 2);
  // nastavení pinu pro řízení osvětlení jako výstupu
  pinMode(lcdSvit, OUTPUT);
  
  // zapnutí osvětlení displeje
  digitalWrite(lcdSvit, HIGH);
  delay(100);
  // vypnutí osvětlení displeje
  //digitalWrite(lcdSvit, LOW);
  //delay(100);
  // zapnutí osvětlení displeje
  //digitalWrite(lcdSvit, HIGH);
  Serial.begin(57600);

  digitalWrite(lcdSvit, LOW);
  delay(100);
  //digitalWrite(lcdSvit, HIGH);
  //delay(100);
  
  Serial2.begin(57600);

  //digitalWrite(lcdSvit, LOW);
  //delay(100);
  digitalWrite(lcdSvit, HIGH);
  delay(100);

  ET1.begin(details(mydata_send), &Serial2);
  digitalWrite(lcdSvit, LOW);
  delay(100);
  //digitalWrite(lcdSvit, HIGH);
  //delay(100);
  ET2.begin(details(mydata_remote), &Serial2);

  digitalWrite(lcdSvit, LOW);
  delay(200);
  digitalWrite(lcdSvit, HIGH);
  delay(200);

  // NOTE: Cursor Position: (CHAR, LINE) starts at 0  
  lcd.setCursor(0,0);
  lcd.print("Remote  Controll");
  lcd.setCursor(0,1);
  lcd.print("v0.1            ");
  delay(300);

  pinMode(44, INPUT_PULLUP);
  pinMode(46, INPUT_PULLUP);
  pinMode(48, INPUT_PULLUP);
  pinMode(50, INPUT_PULLUP);
  pinMode(52, INPUT_PULLUP);

  


  //pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  pinMode(A6, INPUT);

  pinMode(STATE, INPUT); // BT state

  digitalWrite(2, LOW); // turn off LED

}

  void pair() {
    state = digitalRead(3);
    while(state == 0) {
        lcd.setCursor(0,1);
        lcd.print("Waiting to Pair BT  ");
        //lcd.setCursor(0,3);
        //lcd.print("               ");
        state = digitalRead(3);    
    }

    delay(500);  // wait before sending data
}

void loop() {


 unsigned long currentMillis = millis();
 if (currentMillis - previousMillis >= interval) {  // start timed event for read and send
    previousMillis = currentMillis;


// check to see if BT is paired
state = digitalRead(3);
if (state == 0) {
  pair();
}
else {
  lcd.setCursor(0,1);
  lcd.print("Paired to Robot!  ");
}


//but1 =  digitalRead(44);
//but2 =  digitalRead(46);
//but3 =  digitalRead(48);
//but4 =  digitalRead(50);
//but5 =  digitalRead(52);

but1 =  1;
but2 =  1;
but3 =  1;
but4 =  1;
but5 =  1;

// načtení údajů z analogového pinu A0 do proměnné
  int analogData = analogRead(0);
  // nastavení výpisu na první znak, první řádek
  lcd.setCursor(0, 0);
  // vytištění textu na displej
  lcd.print("Stlacene:       ");
  // nastavení výpisu na třináctý znak, první řádek
  lcd.setCursor(12, 0);
  // vytištění textu na displej
  lcd.print(analogData);
  // nastavení výpisu na třetí znak, druhý řádek
  lcd.setCursor(2, 1);
  // vytištění textu na displej pomocí zavolání funkce
  // s předáním aktuálně změřené hodnoty na vstupu

  String text;
  // postupná kontrola pomocí podmínek if,
  // pro každé tlačítko je uveden rozsah hodnot,
  // ve kterých je detekováno a poté nastaven text na výstup
  if  (analogData < 50  ) {
    text = "Vpravo(RIGHT)";
    but5 = 0;
  }
  if ((analogData > 700 ) && (analogData < 1024)) {
    text = "nic          ";
  }
  if ( (analogData > 95 ) && (analogData < 150) ) {
    text = "Hore (UP)";
    but3 = 0;
  }
  if ( (analogData > 250) && (analogData < 350) ) {
    text = "Dolu(DOWN)";
    but1 = 0;
  }
  if ( (analogData > 400) && (analogData < 500) ) {
    text = "Vlavo(LEFT)";
    but4 = 0;
  }
  if ( (analogData > 600) && (analogData < 750) ) {
    text = "Vyber(SELECT)";
    but2 = 0;
  }
  // vrácení textu jako výstup funkce
  lcd.print(text);
if (but1 == 0) {
  mydata_send.menuDown = 1;
}
else {
  mydata_send.menuDown = 0;
}

if (but2 == 0) {
  mydata_send.Select = 1;
}
else {
  mydata_send.Select = 0;
}

if (but3 == 0) {
  mydata_send.menuUp = 1;
}
else {
  mydata_send.menuUp = 0;
}

if (but4 == 0) {
  mydata_send.toggleBottom = 1;
}
else {
  mydata_send.toggleBottom = 0;
}

if (but5 == 0) {
  mydata_send.toggleTop = 1;
}
else {
  mydata_send.toggleTop = 0;
}

axis1 = analogRead(A1);
axis2 = analogRead(A2);
axis3 = analogRead(A3);
axis4 = analogRead(A4);
//axis5 = analogRead(A4);
//axis6 = analogRead(A5);

mydata_send.RLR = axis1;
mydata_send.RFB = axis2;
mydata_send.RT = 0;
mydata_send.LLR = axis3;
mydata_send.LFB = axis4;
mydata_send.LT = 0;

ET1.sendData();

 } // end of timed event send

if (currentMillis - previousDispMillis >= Dispinterval) {  // start timed event for read and send
  previousDispMillis = currentMillis;  

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
          

        }

        
}  // end of second timed event






}
