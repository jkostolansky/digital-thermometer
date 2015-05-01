/*
  Digital Thermometer
  
  Reads temperature from a Dallas temperature sensor DS18B20
  and displays it on a 4-digit 7-segment display 5461BS
 
  by Juraj Kostolansky

 */

#include <OneWire.h>
#include <DallasTemperature.h>
#include <SevSeg.h>

// pinout
#define ONE_WIRE_BUS   3
#define SEG_A         19
#define SEG_B          9
#define SEG_C         11
#define SEG_D         17
#define SEG_E         16
#define SEG_F         18
#define SEG_G         12
#define SEG_DP        10
#define SEG_CA1       14
#define SEG_CA2        6
#define SEG_CA3        5
#define SEG_CA4       15

// settings
#define SEG_TYPE COMMON_ANODE
#define SEG_DIGITS 4
// resolution: 9 -> .5°C, 10 -> .25°C, 11 -> .125°C, 12 -> .0625°C
#define SIG_RESOLUTION 12
#define SEG_BRIGHTNESS 100

// One Wire and Dallas sensor setup
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensors(&oneWire);
DeviceAddress tempDeviceAddress;

// SevSeg library setup
SevSeg screen;

// global variables
unsigned int temp_delay;
float temp;
int tempDigits;
unsigned long tempRequestEndTime;
char screenString[10];

void setup() {
  // start up the display
  screen.Begin(SEG_TYPE, SEG_DIGITS,
               SEG_CA1, SEG_CA2, SEG_CA3, SEG_CA4,
               SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G, SEG_DP);
  screen.SetBrightness(SEG_BRIGHTNESS);
  
  // start up the sensor
  sensors.begin();
  sensors.getAddress(tempDeviceAddress, 0);
  sensors.setWaitForConversion(false);
  sensors.setResolution(SIG_RESOLUTION);
  
  // request first temperature
  sensors.requestTemperatures();
  tempRequestEndTime = millis() + sensors.millisToWaitForConversion(SIG_RESOLUTION);
  // and wait for first temp to prevent false startup reading
  delay(sensors.millisToWaitForConversion(SIG_RESOLUTION));  
}

void loop() {
  // get temperature
  if(tempRequestEndTime < millis()) {
    temp = sensors.getTempC(tempDeviceAddress);
    sensors.requestTemperatures();
    tempRequestEndTime = millis() + sensors.millisToWaitForConversion(SIG_RESOLUTION);
  }  
  // round temp and convert to int (digits only)
  tempDigits = round(temp * 10.0);  
  // convert temperature to string
  if(temp >= 0) {
    sprintf(screenString, "%4d", tempDigits); 
  } else {
    // workaround for "-.5 or leading zeros" problem
    screenString[0] = '-';
    sprintf(&screenString[1], "%3d", tempDigits * -1);
  }  
  // and display it (one decimal place)
  screen.DisplayString(screenString, 4);
}
