#include <Key.h>
#include <Keypad.h>

//YWROBOT
//Compatible with the Arduino IDE 1.0
//Library version:1.1
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include "Adafruit_MAX31855.h"

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

// Example creating a thermocouple instance with software SPI on any three
// digital IO pins.
#define MAXCS   10


// initialize the Thermocouple
Adafruit_MAX31855 thermocouple(MAXCS);

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
    {'1','2','3'},
    {'4','5','6'},
    {'7','8','9'},
    {'.','0','#'}
};

byte rowPins[ROWS] = {2, 3, 4, 5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {6, 7, 8}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

//initalize all functions
void menu();
void exitMenu();
void set_temp_menu();
void set_ramp_menu();
void set_profile_menu();
void reset_min_max();

void setup()
{
  lcd.init();                      // initialize the lcd 
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(1,0);
  lcd.print("Lab Thermometer");
  lcd.setCursor(2,1);
  lcd.print("and thermostat");
   lcd.setCursor(0,2);
  lcd.print("K type thermocouple");
   lcd.setCursor(0,3);
  lcd.print("Charles Fletcher");

  keypad.addEventListener(keypadEvent); // Add an event listener for this keypad

  delay(1000);
    lcd.clear();
}

double internal;
double probe;
float set_temp = 0;
float ramp_speed = 1;
double min_temp = 1000;
double max_temp = -200;
unsigned long time_now = 0;
unsigned long time_previous = 0;
bool Is_menu = false;
bool Is_second_menu = false;
char set_temp_string[5] = {'0','0','.','0','0'};
char ramp_speed_string = '9';
char profile_type = '1';

void loop()
{

  char key = keypad.getKey();
  time_now =  millis();
  if (((time_now-time_previous)>=100)&& !Is_menu) {
    time_previous = time_now;
    
    internal = thermocouple.readInternal();
    probe = thermocouple.readCelsius();
    
    if (min_temp > probe)
    {
      min_temp = probe;
    }
    if (max_temp < probe)
    {
      max_temp = probe;
    }
    
    lcd.setCursor(0,0);
    lcd.print("Probe:");
    lcd.setCursor(0,1);
    lcd.print("Set Temp:");
    lcd.setCursor(0,2);
    lcd.print("Min:");
    lcd.setCursor(0,3);
    lcd.print("Max:");
    
    lcd.setCursor(14,0);
    lcd.print(probe);
    lcd.setCursor(14,1);
    lcd.print(set_temp);
    lcd.setCursor(14,2);
    lcd.print(min_temp);
    lcd.setCursor(14,3);
    lcd.print(max_temp);

  }
  
}

void keypadEvent(KeypadEvent key){
    switch (keypad.getState()){
    case PRESSED:
        if (key == '#') {
          if (!Is_menu){
            menu();
          }
          else
          {
            exitMenu();
          }
        }
        break;

    case HOLD:
        if (key == '.') {
            reset_min_max();
        }
        break;
    }
}

void menu() {
  Is_menu = true;
  lcd.clear();

  while (Is_menu)
  {
    lcd.setCursor(0,0);
    lcd.print("1. Enter Set Temp");
    lcd.setCursor(0,1);
    lcd.print("2. Enter Ramp Speed");
    lcd.setCursor(0,2);
    lcd.print("3. Set Temp Profile");
    char key = keypad.getKey();
    if (key)
    {
      switch(key){
      case '1':
        set_temp_menu();
        break;
      case '2':
        set_ramp_menu();
        break;
      case '3':
        //set_profile_menu();
        break;
      }
    }
  }
}
void exitMenu()
{
  if (Is_second_menu)
  {
    Is_second_menu = false;
  }
  else
  {
    Is_menu = false;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Set Temp:      ");
    lcd.print(set_temp_string);
    set_temp = atof(set_temp_string);
    lcd.setCursor(0,1);
    lcd.print("Ramp Speed (0-9):  ");
    lcd.print(ramp_speed_string);
    ramp_speed = atof(ramp_speed_string)/9.0;
    lcd.setCursor(0,2);
    lcd.print("Temp Profile:      ");
    lcd.print(profile_type);
    delay(2000);
    lcd.clear();
  }
}

void reset_min_max() {
  min_temp = 1100;
  max_temp = -200;
}

void set_temp_menu() {
  int n = 0;
  Is_second_menu = true;
  set_temp_string[0] = '0';
  set_temp_string[1] = '0';
  set_temp_string[2] = '0';
  set_temp_string[3] = '0';
  set_temp_string[4] = '0';
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Enter Temperature");
  lcd.setCursor(0,1);
  lcd.print("  Set Point:");
  lcd.setCursor(2,2);
  
  while (Is_second_menu)
  {
    char key = keypad.getKey();
    if ((key)&&(n<5))
    {
      if (key != '#')
      {
        set_temp_string[n] = key;
        n++;
        lcd.print(key);
      }
    }
  }
  lcd.clear();
}

void set_ramp_menu() {
  int n = 0;
  Is_second_menu = true;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Enter Ramp Speed:");
  lcd.setCursor(0,1);
  lcd.print(" 0 (off) to 9 (max)");
    
  while (Is_second_menu)
  {
    char key = keypad.getKey();
    if (key)
    {
      if (key != '#')
      {
        ramp_speed_string = key;
        n++;
        lcd.setCursor(2,2);
        lcd.print(key);
      }
    }
  }
  lcd.clear();
}

void set_profile_menu() {
  int n = 0;
  Is_second_menu = true;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("1.Ramp and Hold");
  lcd.setCursor(0,1);
  lcd.print("2.Ramp,Hold for time");
    
  while (Is_second_menu)
  {
    char key = keypad.getKey();
    if (key)
    {
      if (key == '1')
      {
        profile_type = key;
        lcd.setCursor(2,2);
        lcd.print(key);
      }
      else if (key == '2')
      {
        profile_type = key;
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Enter Hold Time:");
      }
    }
  }
  lcd.clear();
}
  


