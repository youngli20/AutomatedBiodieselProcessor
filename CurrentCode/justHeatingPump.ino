​// LIBRARY IMPORTS
//#include "Adafruit_TCS34725.h"
//#include <DallasTemperature.h>
//#include <Keypad.h>
//#include <LiquidCrystal.h>
//#include <LiquidCrystal_I2C.h>
//#include <NewPing.h>
//#include <OneWire.h>
//#include <Wire.h>
// END LIBRARY IMPORTS


//// CONSTANTS
//const int POWER_ON_TIME = millis();
//
//const int LEVEL_SENSOR_MAX_DIST = 100;
//// END CONSTANTS
//
////what we want
//
const int REACTION_CHAMBER_PUMP_PIN   = 24; //RELAY_TWO_PIN
const int REACTION_CHAMBER_HEATING_PIN  = 28; //Reaction Chamber Heating Element
//const int VALVEd_SEVEN_PIN = 43; //Feeds methoxide from carboy to Reactor
//const int VALVEd_FIVE_PIN  = 39; //Drain Waster from W/D (Color Sensor)
//
//// GLOBAL VARIABLES
//// 0: no input received yet
//// 1: select pin
//// 2: select high or low
//int input_state = 0;
//
//// 0: auto mode
//// 1: override mode
//// 2: jump to step
//int operation_mode = 0;
//
//int current_step = 1;
//
//// =1 if the default lcd display
//// needs to be overwritten to show
//// error or other info
//int lcd_interrupt = 0;
//
//// only 3 because at most we need
//// a two digit number for either
//// step numbers or pin numbers
//char input[2] = {'x', 'x'};
//
//// which digit we are inputting next
//int input_idx = 0;
//// END GLOBAL VARIABLES
//
//// timer var
unsigned long timeSinceStarted;
boolean heatOff = false;
boolean heatOn = false;

void setup ()
{
  Serial.begin(9600);
  
  pinMode(REACTION_CHAMBER_PUMP_PIN,   OUTPUT);
  pinMode(REACTION_CHAMBER_HEATING_PIN,  OUTPUT);
//  pinMode(VALVEd_SEVEN_PIN, OUTPUT);
//  pinMode(VALVEd_FIVE_PIN,  OUTPUT);

  timeSinceStarted = millis();
}

void loop ()
{
  unsigned long timePassed = millis() - timeSinceStarted;
  if(!heatOn) {
      Serial.println("turning on washer heater");
    turnOnWasherHeater();
    heatOn = true;
  }
  //timeSinceStarted = millis();
  if(timePassed >= 5000 && !heatOff) {
    Serial.println("Turning off heater...");
    turnOffWasherHeater();
    heatOff = true;
  }
}

/**
 * turns on the reaction chamber heating
 */
void turnOnWasherHeater() {
  Serial.println("inside turnOnWasherHeater() method...");
  digitalWrite(REACTION_CHAMBER_HEATING_PIN, HIGH);
  digitalWrite(REACTION_CHAMBER_PUMP_PIN, LOW);
}

/**
 * turns off the reaction chamber heating
 */
void turnOffWasherHeater() {
  Serial.println("inside turnOffWasherHeater() ");
  digitalWrite(REACTION_CHAMBER_HEATING_PIN, LOW);
  //digitalWrite(REACTION_CHAMBER_PUMP_PIN, HIGH);
}
      
//
////Transfer WVO to reactor step:7
//void TransferToReactor()
//{
//  current_step = 1;
//  digitalWrite(REACTOR_DRAIN_PIN, LOW);
//  digitalWrite(WASHER_DRYER_DRAIN_PIN, HIGH);
//  digitalWrite(FEED_METHOXIDE_REACTOR_PIN, HIGH);
//  digitalWrite(REACTION_CHAMBER_PUMP_PIN,HIGH);
//  //decide how long the pump needs to be run, either delay or level sensor
//  digitalWrite(REACTION_CHAMBER_PUMP_PIN,LOW);
//  digitalWrite(WASHER_DRYER_DRAIN_PIN, LOW);
//}
//
////STEP EIGHT IN PSEUDOCODE DOC
//void HeatReactor()
//{
//  current_step = 2;
//  digitalWrite(REACTION_CHAMBER_HEATING_PIN,HIGH);
//  
//}
//
//
// //Pseudoecode step:10
//void TransferToWD()
//{
//  digitalWrite(REACTOR_FILL_PIN, LOW);
//  digitalWrite(DRAIN_REACTOR_TO_WD_PIN, HIGH);
//  digitalWrite(WASHER_DRYER_PUMP_PIN, HIGH);
//  //need to decide how to leave pump on untill all of the liquid from the reactor has been pumped out
//  digitalWrite(WASHER_DRYER_PUMP_PIN, LOW);
//}
//
​