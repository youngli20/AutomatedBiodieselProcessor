// LIBRARY IMPORTS
#include "Adafruit_TCS34725.h"
#include <DallasTemperature.h>
#include <Keypad.h>
#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>
#include <NewPing.h>
#include <OneWire.h>
#include <Wire.h>
// END LIBRARY IMPORTS


// CONSTANTS
const int POWER_ON_TIME = millis();

const int LEVEL_SENSOR_MAX_DIST = 100;

const int KEYPAD_ROWS = 4;
const int KEYPAD_COLS = 4;

char KEYS[KEYPAD_ROWS][KEYPAD_COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
// END CONSTANTS


// PIN CONSTANTS
const int LEVEL_SENSOR_ONE_ECHO_PIN = 10;
const int LEVEL_SENSOR_ONE_TRIG_PIN = 11;
const int LEVEL_SENSOR_TWO_ECHO_PIN = 12;
const int LEVEL_SENSOR_TWO_TRIG_PIN = 13;

const int TEMP_SENSOR_ONE_PIN = 52;
const int TEMP_SENSOR_TWO_PIN = 53;

const int SDA_PIN = 20;
const int SCL_PIN = 21;

const int WASHER_DRYER_PUMP_PIN = 22; // relay one pin
const int REACTION_CHAMBER_PUMP_PIN   = 24; //RELAY_TWO_PIN
const int WASHER_DRYER_HEATING_PIN = 26; //Washer Dryer Heating Element  
const int REACTION_CHAMBER_HEATING_PIN  = 28; //Reaction Chamber Heating Element

const int WASHER_DRYER_DRAIN_PIN   = 31; //Drain for Washer Dryer
const int REACTOR_FILL_PIN   = 33; //Fills the reactor
const int REACTOR_DRAIN_PIN = 35; //Drain for the Reactor
const int DRAIN_REACTOR_TO_WD_PIN  = 37; //Drain from Reactor to the W/D
const int DRAIN_WASTER_WD_PIN  = 39; //Drain Waster from W/D (Color Sensor)
const int VALVE_SIX_PIN   = -1; //TBH
const int FEED_METHOXIDE_REACTOR_PIN = 43; //Feeds methoxide from carboy to Reactor
const int FEED_WATER_MISTER_PIN = 45; //Feeds water from carboy to mister ontop of W/D

/* OLD CONTANT NAMES IN CASE WE NEED THEM:
const int RELAY_ONE_PIN   = 22; //Washer Dryer Pump
const int RELAYd_TWO_PIN   = 24; //Reaction Chamber Pump
const int RELAYd_THREE_PIN = 26; //Washer Dryer Heating Element  
const int RELAYd_FOUR_PIN  = 28; //Reaction Chamber Heating Element

const int VALVEd_ONE_PIN   = 31; //Drain for Washer Dryer
const int VALVEd_TWO_PIN   = 33; //Fills the reactor
const int VALVEd_THREE_PIN = 35; //Drain for the Reactor
const int VALVEd_FOUR_PIN  = 37; //Drain from Reactor to the W/D
const int VALVEd_FIVE_PIN  = 39; //Drain Waster from W/D (Color Sensor)
const int VALVEd_SIX_PIN   = -1; //TBH
const int VALVEd_SEVEN_PIN = 43; //Feeds methoxide from carboy to Reactor
const int VALVE_dEIGHT_PIN = 45; //Feeds water from carboy to mister ontop of W/D
*/

byte KEYPAD_ROW_PINS[KEYPAD_ROWS]  = { 9, 8, 7, 6 };
byte KEYPAD_COL_PINS[KEYPAD_COLS]  = { 5, 4, 3, 2 };
// END PIN CONSTANTS


// SENSOR DECLARATIONS
Adafruit_TCS34725 color_sensor = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);

OneWire temp_sensor_one_onewire(TEMP_SENSOR_ONE_PIN);
OneWire temp_sensor_two_onewire(TEMP_SENSOR_TWO_PIN);
DallasTemperature temp_sensor_one(&temp_sensor_one_onewire);
DallasTemperature temp_sensor_two(&temp_sensor_two_onewire);

NewPing level_sensor_one(LEVEL_SENSOR_ONE_TRIG_PIN, LEVEL_SENSOR_ONE_ECHO_PIN, LEVEL_SENSOR_MAX_DIST);
NewPing level_sensor_two(LEVEL_SENSOR_TWO_TRIG_PIN, LEVEL_SENSOR_TWO_ECHO_PIN, LEVEL_SENSOR_MAX_DIST);

LiquidCrystal_I2C lcd(0x3f, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

Keypad keypad(makeKeymap(KEYS), KEYPAD_ROW_PINS, KEYPAD_COL_PINS, KEYPAD_ROWS, KEYPAD_COLS);
// END SENSOR DECLARATIONS

// GLOBAL VARIABLES
// 0: no input received yet
// 1: select pin
// 2: select high or low
int input_state = 0;

// 0: auto mode
// 1: override mode
// 2: jump to step
int operation_mode = 0;

int current_step = 1;

// =1 if the default lcd display
// needs to be overwritten to show
// error or other info
int lcd_interrupt = 0;

// only 3 because at most we need
// a two digit number for either
// step numbers or pin numbers
char input[2] = {'x', 'x'};

// which digit we are inputting next
int input_idx = 0;
// END GLOBAL VARIABLES



void setup ()
{
  Serial.begin(9600);
  
  // End Keypad Initialization
  pinMode(WASHER_DRYER_PUMP_PIN,   OUTPUT);
  pinMode(REACTION_CHAMBER_PUMP_PIN,   OUTPUT);
  pinMode(WASHER_DRYER_HEATING_PIN, OUTPUT);
  pinMode(REACTION_CHAMBER_HEATING_PIN,  OUTPUT);
  pinMode(WASHER_DRYER_DRAIN_PIN,   OUTPUT);
  pinMode(REACTOR_FILL_PIN,   OUTPUT);
  pinMode(REACTOR_DRAIN_PIN, OUTPUT);
  pinMode(DRAIN_REACTOR_TO_WD_PIN,  OUTPUT);
  pinMode(DRAIN_WASTER_WD_PIN,  OUTPUT);
  pinMode(VALVE_SIX_PIN,   OUTPUT);
  pinMode(FEED_METHOXIDE_REACTOR_PIN, OUTPUT);
  pinMode(FEED_WATER_MISTER_PIN, OUTPUT);
  
  // Start LCD
  lcd.begin(16, 2);
  lcd.home();
  lcd.print("Initialized");
  
  // Add interrupt to keypad
  keypad.addEventListener(keypadEvent);
  
  // Start Temperature Sensors
  temp_sensor_one.begin();
  temp_sensor_two.begin();
  
  // Start Level Sensors
  level_sensor_one.begin();
  level_sensor_two.begin();
}

void loop ()
{
  switch (current_step) {
    case 1:
      TransferToReactor();
      break;
    case 2:
      break;
    case 3:
      break;
    case 4:
      break;
    case 5:
      break;
    default:
      // error i guess
      break;
  }
  
  // Update Sensors
  
  // Error Checks
  
  // Update LCD
  if (lcd_interrupt == 0)
  {
    lcd.clear();
    lcd.home();
    lcd.print("Default Stuff");
  }
}

/**
 * Function Name: keyPadEvent
 * Description: Updates input array to record input. 
 * Unknown: I don't know what the purpose of this method is because we don't use input anywhere.
 * Parameters: key
 * Return: None
 */
void keypadEvent (KeypadEvent key)
{
  switch (keypad.getState()) {
    case RELEASE:
      // auto mode doesn't take inputs
      if (operation_mode == 0)
        break;
      
      lcd_interrupt = 1;
      lcd.clear();
      lcd.home();
      
      if (operation_mode == 1)
      {
        lcd.print("Choose a Pin. Press # when done.");
        lcd.setCursor(0,2);
      }
      else if (operation_mode == 2)
      {
        lcd.print("Choose a Step. Press # when done.");
        lcd.setCursor(0,2);
      }
    
      if (key == '1')
        input[input_idx] = '1';
        
      else if (key == '2')
        input[input_idx] = '2';
        
      else if (key == '3')
        input[input_idx] = '3';
        
      else if (key == '4')
        input[input_idx] = '4';
        
      else if (key == '5')
        input[input_idx] = '5';
        
      else if (key == '6')
        input[input_idx] = '6';
        
      else if (key == '7')
        input[input_idx] = '7';
        
      else if (key == '8')
        input[input_idx] = '8';
        
      else if (key == '9')
        input[input_idx] = '9';
        
      else if (key == '0')
        input[input_idx] = '0';
        
      else if (key == 'A')
        input[input_idx] = 'A';
        
      else if (key == 'B')
        input[input_idx] = 'B';
        
      else if (key == 'C')
        input[input_idx] = 'C';
        
      else if (key == 'D')
        input[input_idx] = 'D';
        
      else if (key == '*')
        input[input_idx] = '*';
        
      else if (key == '#')
      
      if (input_idx == 1)
      {
        if (operation_mode == 1)
          // 
        else if (operation_mode == 2)
          // change operation step
      }
      else
      {
        input_idx++;
      }
      
      break;
    case HOLD:
      if (key == 'A')
      {
        // switch to auto mode
        operation_mode = 0;
      }
      else if (key == 'B')
      {
        // switch to override mode
        operation_mode = 1;
      }
      else if (key == 'C')
      {
        // switch to jump to step mode
        operation_mode = 2;
      }
      
      input_idx = 0;
      break;
  }
}

/**
 * Function Name: getTemp
 * Description: Sets temp_one and temp_two to the temperature of ___?
 * Unknown: ___ is current state of biodeisel processor?
 * Parameters: None
 * Return: None
 */
void getTemp ()
{
  temp_sensor_one.requestTemperatures();
  temp_sensor_two.requestTemperatures();
  
  int temp_one = temp_sensor_one.getTempCByIndex(0);
  int temp_two = temp_sensor_two.getTempCByIndex(0);
}

/**
 * Function Name: getLevel
 * Description: _____
 * Unknown: _____
 * Parameters: None
 * Return: None
 */
void getLevel ()
{
  unsigned int uS_one = level_sensor_one.ping();
  unsigned int uS_two = level_sensor_one.ping();

  int level_one = uS_one / US_ROUNDTRIP_CM;
  int level_two = uS_two / US_ROUNDTRIP_CM;
}


void getColor ()
{
  uint16_t r, g, b, c, color_temp, lux;
  
  color_sensor.getRawData(&r, &g, &b, &c);
  color_temp = color_sensor.calculateColorTemperature(r, g, b);
  lux = color_sensor.calculateLux(r, g, b);
}


//Transfer WVO to reactor step:7
void TransferToReactor()
{
  current_step = 1;
  digitalWrite(REACTOR_DRAIN_PIN, LOW);
  digitalWrite(WASHER_DRYER_DRAIN_PIN, HIGH);
  digitalWrite(FEED_METHOXIDE_REACTOR_PIN, HIGH);
  digitalWrite(REACTION_CHAMBER_PUMP_PIN,HIGH);
  //decide how long the pump needs to be run, either delay or level sensor
  digitalWrite(REACTION_CHAMBER_PUMP_PIN,LOW);
  digitalWrite(WASHER_DRYER_DRAIN_PIN, LOW);
}

//STEP EIGHT IN PSEUDOCODE DOC
void HeatReactor()
{
  current_step = 2;
  digitalWrite(REACTION_CHAMBER_HEATING_PIN,HIGH);
  
  if(temp_one >= 50){   //Move to step 3 once temp is 50c
  	current_step = 3;
  }

}

void Reaction()
{
  current_step = 3;
  digitalWrite(REACTOR_DRAIN_PIN, HIGH);
  digitalWrite(WASHER_DRYER_PUMP_PIN, HIGH);
  digitalWrite(REACTOR_FILL_PIN, HIGH);
  
  //need a delay to mix the methoxide and wvo
  
  digitalWrite(WASHER_DRYER_PUMP_PIN, LOW);
  digitalWrite(REACTOR_DRAIN_PIN, LOW);
}

 //Pseudoecode step:10
void TransferToWD()
{
  digitalWrite(REACTOR_FILL_PIN, LOW);
	digitalWrite(DRAIN_REACTOR_TO_WD_PIN, HIGH);
  digitalWrite(WASHER_DRYER_PUMP_PIN, HIGH);
  //need to decide how to leave pump on untill all of the liquid from the reactor has been pumped out
  digitalWrite(WASHER_DRYER_PUMP_PIN, LOW);
}

//Pseudocode step 11
void BDSeperation()
{
	digitalWrite(DRAIN_WASTER_WD_PIN, HIGH);
  //drian Glycerol untill the light sesor detects biodeisel. 
  digitalWrite(DRAIN_WASTER_WD_PIN, LOW);
}

//pseudocode step 12,13, & 14
void WashBD()
{
	digitalWrite(WASHER_DRYER_HEATING_PIN, HIGH);
  digitalWrite(VALVE_SIX_PIN, HIGH);
  digitalWrite(FEED_WATER_MISTER_PIN,HIGH); //feed misters
  //TURN ON BUBBLER
  //STOP WATER WHEN ENOUGH IS ADDED
  digitalWrite(VALVE_SIX_PIN, LOW);
  digitalWrite(FEED_WATER_MISTER_PIN,LOW); //close misters
  //WAIT FOR IT TO SETTLE
  
}
//Pseudocode step 15
void WaterSeperation()
{
	digitalWrite(DRAIN_WASTER_WD_PIN, HIGH);
  //drian WATER untill the light sesor detects biodeisel. 
  digitalWrite(DRAIN_WASTER_WD_PIN, LOW);
}

//DRY BD STEP 16
void DryBD()
{
	digitalWrite(REACTION_CHAMBER_PUMP_PIN, HIGH);
  //time
  digitalWrite(REACTION_CHAMBER_PUMP_PIN, LOW);
}

