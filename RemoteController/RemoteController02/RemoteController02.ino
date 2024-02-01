// 'Flitzer' with Bluetooth remote joystick shield
// ArduinoBlue compatible, uses Adafruit AdaFruit MotorShield
//
// Arnoud van Delden - February-March 2021
//

#include "Servo.h"
//#include "Adafruit_MotorShield.h"
#include "SoftwareSerial.h"
#include "ArduinoBlue.h"

//#define DEBUG
#define Baudrate 38400 // Serial monitor during DEBUG mode...

// Various defines...
#define EncoderIRQPin 2
#define ServoPin 9
#define HornPin 12
#define minAngle 55
#define maxAngle 125
#define MaxSpeed 255
#define FlasherL 0
#define FlasherR 14
#define REARLIGHT 100  // Default read light brightness, with headroom for brake light (=255)

// Distance and velocity measurement...
#define IPCM 14.606    // Nr impulses per cm with 50mm wheels...
#define Interval 1000  // Sample-/Measure interval in ms
volatile unsigned long tachoPulses;
unsigned long starttime;
const float f = 36000/IPCM; // Geschw. m/h = f x Impulse / Interval
unsigned long tachoSample;
float velocity, velocityAvg, distance;
unsigned long tachoTimer;
char valueStr[15];
char displayStr[50]; 

// Software Serial for BT-module. Keep D9 and D10 free on the otorshield for servos!
#define RX 4
#define TX 5
#define GND 6
#define Vcc 7
//#define BTBaud 38400 // Configured my MT-19 BLE to 38400 baud...
#define BTBaud 9600 // Configured my MT-19 BLE to 9600 baud...
SoftwareSerial BT(TX, RX);
ArduinoBlue remoteBLE(BT);

Servo ServoMotor; // Steering servo...

//Adafruit_MotorShield MShield = Adafruit_MotorShield(0x60);
//Adafruit_DCMotor *Motor = MShield.getMotor(1); // Main motor
//Adafruit_DCMotor *Light = MShield.getMotor(2); // Headlights...
//Adafruit_DCMotor *RedRearLight = MShield.getMotor(3); // Rear and brake light...
//Adafruit_DCMotor *ReverseLight = MShield.getMotor(4); // Reverse light..

int error = 0; 
byte controller_type = 0;
int motorspeed = 0;
int lastspeed = 0;
int steerAngle = 90; // Start with default steer angle...
unsigned long lasttime;
unsigned long brakeOffTimer;
boolean FlasherLeft = false, FlasherRight = false, FlasherOn = false, RumbleBrake = false,
        WarningLights = false, BrakeLights = false, autoParking = false, lightsOn = false;
boolean showSpeed = true;

// Vars for ArduinoBlue...
int throttleAB, steeringAB, buttonAB; // Sliders are not used (yet)...

void count() { // Interrupt routine of encoder motor...
  tachoPulses++;
}

void driveCar() { // Remote controlled car...
  throttleAB = abs(remoteBLE.getThrottle());
  steeringAB = abs(remoteBLE.getSteering());
  if (steeringAB>99) steeringAB = 99;
  buttonAB = remoteBLE.getButton();
      
  // Servo steering...
  steerAngle = map(steeringAB, 0, 100, minAngle, maxAngle);
  
  #ifdef DEBUG
  if (steerAngle != 90) {
    Serial.print(" steerAngle: ");
    Serial.println(steerAngle, DEC);
  }
  #endif
  ServoMotor.write(steerAngle);

  // Process and set motor speed...
  motorspeed = throttleAB - 50;
  if (motorspeed>0) {
    // Forward...
    //motorspeed = abs(motorspeed);
    motorspeed = map(motorspeed, 0, 50, 0, 255);
    //Motor->setSpeed(motorspeed);
    //Motor->run(FORWARD);
    //ReverseLight->setSpeed(0);
#ifdef DEBUG¨
    if (motorspeed>0) {
      Serial.print("Forward motorspeed: ");
      Serial.println(motorspeed, DEC);
    }
#endif

  } else {
    // Backward/reverse...
    motorspeed = map(motorspeed, 0, -50, 0, 255);
    //Motor->run(BACKWARD);
    //Motor->setSpeed(motorspeed);
    //if (motorspeed>5)
    //  //ReverseLight->setSpeed(255);
    //else // Switch off reverse light if not moving...
    //  //ReverseLight->setSpeed(0);
  #ifdef DEBUG
    if (motorspeed>0) {
      Serial.print("Reverse motorspeed: ");
      Serial.println(motorspeed, DEC);
    }
#endif   
  }
  if (motorspeed) {
    delay(100); // Delay for button debouncing
    
    // Car horn...
    if (buttonAB==4) { // Button 4 was pressed...
      tone(HornPin, 200, 500);
      delay(1000);
      tone(HornPin, 200, 500);
      delay(500);
      noTone(HornPin);
#ifdef DEBUG
      Serial.println("Horn sounded");
#endif
    }
    
    // Toggle head and rear lights...
    if (buttonAB==1) { // Button 1 was pressed...
      if (lightsOn) {
        //Light->setSpeed(0);
        //RedRearLight->setSpeed(0);
        lightsOn=false;
      } else {
        //Light->setSpeed(255);
        //RedRearLight->setSpeed(REARLIGHT);
        lightsOn=true;
      }
#ifdef DEBUG
      Serial.println("Headlights toggle");
#endif
      // Toggle buttons/functions are one-shot, so wait until button is released
      while (remoteBLE.getButton()>0) delay(100);
    }
  
    // Toggle warning lights...
    if (buttonAB==2) { // Button 2 was pressed...
      if (WarningLights)
        WarningLights = false;
      else
        WarningLights = true;
#ifdef DEBUG
    Serial.println("WarningLights toggle");
#endif
      // Toggle buttons/functions are one-shot, so wait until button is released
      while (remoteBLE.getButton()>0) delay(100);
    }
  
    // Reset speed and distance...
    if (buttonAB==3) { // Button 3 was pressed...
      tachoPulses = tachoSample = 0;
      distance = velocity = velocityAvg = 0.0;
      Serial.println("Reset speed and distance");
#ifdef DEBUG
      Serial.println("Reset speed and distance");
#endif
    }

    // showSpeed...
    if (buttonAB==5) { // Button 5 was pressed...
      if (showSpeed) { // End display mode...
        remoteBLE.sendMessage("VALUES");
      } else {
        remoteBLE.sendMessage("CLR");
        delay(100);
        remoteBLE.sendMessage("LCDPRINT0,0:Speed        m/h");
        delay(100);
        remoteBLE.sendMessage("LCDPRINT1,1:Dist         cm");
        delay(100);
      }
      showSpeed = !showSpeed;
#ifdef DEBUG
      Serial.println("Show velocity and speed");
#endif
      // Toggle buttons/functions are one-shot, so wait until button is released
      while (remoteBLE.getButton()>0) delay(100);
    }  
  }

  //delay(50); // Grace...
}

void setup() {
#ifdef DEBUG
  Serial.begin(Baudrate); // Start serial monitor
#endif

  // Setup speed/distance tracking...
  pinMode(EncoderIRQPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(EncoderIRQPin), count, CHANGE);
  starttime = millis(); // Millisecs since program start...

  // Initialize car horn buzzer and signal we're alive...
  pinMode(HornPin, OUTPUT);
  tone(HornPin, 800, 250);
  delay(500);
  tone(HornPin, 800, 250);
  delay(250);
  noTone(HornPin);
      
  // Init BLE module over Software Serial...
  BT.begin(BTBaud);
  delay(1000);
  
  //MShield.begin();              // Motor Shield init...
  Wire.setClock(400000);        // Set I²C-Frequency to 400 kHz...
  ServoMotor.attach(ServoPin);  // Servo with default angle...
  ServoMotor.write(steerAngle); // Default middle...
      
  //ReverseLight->setSpeed(0); 
  //ReverseLight->run(FORWARD);
  //RedRearLight->setSpeed(0); 
  //RedRearLight->run(BACKWARD);
  //Light->setSpeed(0); 
  //Light->run(FORWARD);

  //MShield.setPin(FlasherL, false);
  //MShield.setPin(FlasherR, false);
    
  tachoPulses = 0;
  motorspeed = 0;
  //Motor->run(FORWARD); // Default=Forward...
  //Motor->setSpeed(motorspeed);
  
  delay(1000);
}

void Flasher() {
  //if (FlasherOn) {
  //  //if (FlasherLeft)  MShield.setPin(FlasherL, true);
  //  //if (FlasherRight) MShield.setPin(FlasherR, true);
  //}
  //else {
  //  //if (FlasherLeft)  MShield.setPin(FlasherL, false);
  //  //if (FlasherRight) MShield.setPin(FlasherR, false);
  //}
  FlasherOn = !FlasherOn;
}

void loop() {

  // Do tacho stuff each interval, but only if showSpeed=true...
  if (showSpeed && (millis() - tachoTimer) > Interval) {
    tachoTimer = millis(); // Restart timer...
    distance = tachoPulses / IPCM;
    velocity = (tachoPulses-tachoSample) * f / Interval;  // Recalc to impulses m/h
    velocityAvg = tachoPulses * f / (millis()-starttime);
  
#ifdef DEBUG
    Serial.print("distance=");
    Serial.println(distance);
    dtostrf(distance, 3, 2, valueStr);  // 3 is mininum width, 2 is precision
    sprintf(displayStr, "distance %s cm", valueStr);
    Serial.println(displayStr);
    Serial.print("velocity=");
    Serial.println(velocity);
    dtostrf(velocity, 3, 2, valueStr);  // 3 is mininum width, 2 is precision
    sprintf(displayStr, "velocity %s m/h", valueStr);
    Serial.println(displayStr);        
    //Serial.print("velocityAvg=");
    //Serial.println(velocityAvg);
#endif
    
    tachoSample = tachoPulses; // Store current count...      
  }

  if (autoParking) { // auto parking...
    // To Do...
    autoParking = false; // End parking procedure...
  } else {
    // Normal loop...
    driveCar(); // Main...

    // Write distance and speed to remote display...

    if (WarningLights) { // Flashers on...
      FlasherRight = true;
      FlasherLeft = true;
      if ((millis()-lasttime) > 500) { // Flasher routine
        lasttime = millis();
        Flasher();
      }
    }
    else { // Flashers off...
      if (steerAngle < 89) { // Steering left
        FlasherLeft = true;
        FlasherRight = false;
        if ((millis()-lasttime) > 500) { // Flasher routine
          lasttime = millis();
          Flasher();
        }
      }
      else if (steerAngle > 91) { // Steering right
        FlasherRight = true;
        FlasherLeft = false;
        if ((millis()-lasttime) > 500) { // Flasher routine
          lasttime = millis();
          Flasher();
        }
      }
      else {
        FlasherRight = false;
        FlasherLeft = false;
        //MShield.setPin(FlasherL, false); // Left flasher off
        //MShield.setPin(FlasherR, false); // Right flasher off
        FlasherOn = false;
      }
    }

    // Brake light handling and auto-off...
    if (BrakeLights) {
      if (brakeOffTimer <= 0) brakeOffTimer = millis(); // Start timer...
      if ((millis()-brakeOffTimer) > 1000) { // Time-out brake lights...
        BrakeLights = false;
        brakeOffTimer = 0;
      } else {
        RedRearLight->setSpeed(255);         
      }
    } else {
      if (lightsOn)
        RedRearLight->setSpeed(REARLIGHT);
      else
        RedRearLight->setSpeed(0);
   }
 
  }
}
