/*
  Matthew McMillan @matthewmcmillan -  http://matthewcmcmillan.blogspot.com
  Digital Pot Control (MCP4251)
  
  This example controls a Microchip MCP4251 digital potentiometer.
  The MCP4251 has 2 potentiometer channels. Each channel's pins are labeled:
      A - connect this to voltage
      W - this is the pot's wiper, which changes when you set it
      B - connect this to ground.
  The MCP4251 also has Terminal Control Registers (TCON) which allow you to
  individually connect and disconnect the A,W,B terminals which can be useful
  for reducing power usage or motor controls.
  
  A value of 0 is no resistance
  A value of 128 is approximately half ohms
  A value of 255 is maxim of resistance
 
  The MCP4251 is SPI-compatible. To command it you send two bytes, 
  one with the memory address and one with the value to set at that address.
  The MCP4251 has few different memory addresses for wipers and tcon (Terminal Control)
     *Wiper 0 write
     *Wiper 0 read
     *Wiper 1 write
     *Wiper 1 read
     *TCON write
     *TCON read
 
 The circuit:
     * All A pins of MCP4251 connected to +5V
     * All B pins of MCP4251 connected to ground
     * W0 pin to A0
     * W1 pin to A1
     * VSS - to GND
     * VDD - to +5v
     * SHDN - to digital pin 7 (and a 4.7k pull down resistor)
     * CS - to digital pin 10  (SS pin) or 53 at Mega
     * SDI - to digital pin 11 (MOSI pin) or 51 at Mega
     * SDO - to digital pin 12 (MISO pin) or 50 at mega
     * CLK - to digital pin 13 (SCK pin) or 52 at Mega
 
 created 12 Mar 2014 
 
 Thanks to Heather Dewey-Hagborg and Tom Igoe for their original tutorials
adapted by Nicu FLORICA (aka niq_ro) - http://www.tehnic.go.ro 
& http://www.arduinotehniq.com/
& http://nicuflorica.blogspot.ro/
& http://arduinotehniq.blogspot.com/
*/

// Nokia 5110 LCD (PCD8544) from https://code.google.com/p/pcd8544/
/* niq_ro ( http://nicuflorica.blogspot.ro ) case for Nokia 5110 LCD (PCD8544) - LPH 7366:
 For module from China, you must connect like this:
* Pin 1 (RST) -> Arduino digital 6 (D6)
* Pin 2 (CE) -> Arduino digital 5 (D5)
* Pin 3 (DC) -> Arduino digital 4 (D4)
* Pin 4 (DIN) -> Arduino digital 3 (D3)
* Pin 5 (CLK) - Arduino digital 2 (D2)
* Pin 6 (Vcc) -> +5V thru adaptor module (see http://nicuflorica.blogspot.ro/2013/06/afisajul-folosit-la-telefoanele-nokia.html )
* Pin 7 (LIGHT) -> +5V thru 56-100 ohms resistor (for permanent lights) or... other pin control
* Pin 8 (GND) -> GND1 or GND2 
*/

// adapted sketch by niq_ro from http://nicuflorica.blogspot.ro
// version 3.0

#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
// Adafruit_PCD8544 display = Adafruit_PCD8544(SCLK, DIN, DC, CS, RST);
Adafruit_PCD8544 display = Adafruit_PCD8544(2, 3, 4, 5, 6);


// inslude the SPI library:
#include <SPI.h>

// set pin 10 as the slave select for the digital pot:
//const int slaveSelectPin = 10;    // Uno
const int slaveSelectPin = 53;    // Mega
const int shutdownPin = 7;
const int wiper0writeAddr = B00000000;
const int wiper1writeAddr = B00010000;
const int tconwriteAddr = B01000000;
const int tcon_0off_1on = B11110000;
const int tcon_0on_1off = B00001111;
const int tcon_0off_1off = B00000000;
const int tcon_0on_1on = B11111111;




int scurt = 250;
int lung = 3000;

int PotWiperVoltage0 = 0;
int RawVoltage0 = 0;
float Voltage0 = 0;
int PotWiperVoltage1 = 1;
int RawVoltage1 = 0;
float Voltage1 = 0;

// transform lin to log
float a = 0.2;     // for x = 0 to 63% => y = a*x;
float x1 = 63;
float y1 = 12.6;   // 
float b1 = 2.33;     // for x = 63 to 100% => y = a1 + b1*x;
float a1 = -133.33;


float level, level1, level2;

byte grad[8] = {
  B01110,
  B10001,
  B10001,
  B01110,
  B00000,
  B00000,
  B00000,
};


void setup() {
  // set the slaveSelectPin as an output:
  pinMode (slaveSelectPin, OUTPUT);
  // set the shutdownPin as an output:
  pinMode (shutdownPin, OUTPUT);
  // start with all the pots shutdown
  digitalWrite(shutdownPin,LOW);
  // initialize SPI:
  SPI.begin(); 
  
  display.begin();
  // init done

  // you can change the contrast around to adapt the display
  // for the best viewing!
//  display.setContrast(100);
 display.setContrast(60);
  display.clearDisplay();


  // Print a logo message to the LCD.
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(7,0);
  display.println("tehnic.go.ro");
  display.setCursor(18, 8);
  display.print("& niq_ro");
  display.setCursor(0, 22);
  display.print("MCP4261-502E/P");  
  display.setCursor(0, 30);
  display.print("lin to log pot");
  display.setCursor(5, 40);
  display.print("version ");
  display.setTextColor(WHITE, BLACK);
  display.print("2.0");
  display.display();
  delay (3000);
  display.clearDisplay(); 
digitalWrite(shutdownPin,HIGH); //Turn off shutdown
}


void loop() {

display.clearDisplay();
display.display(); // show splashscreen 
  
  // first test with 0 to 100% rotation
display.setTextSize(1);
display.setTextColor(BLACK);
display.setCursor(10,0);
display.print("Simulate");
display.setCursor(0,8);
display.print("rotation from");
display.setCursor(10,16);
display.print("0 to 100%");
display.setCursor(30,24);
display.print("=>");
display.setCursor(0,32);
display.print("output value..");
display.display(); // show splashscreen
delay(2*lung);
display.clearDisplay();


for (int procent = 0; procent <= 100; procent++) 
  {
    if (procent < 63)  // for x = 0 to 63% => y = a*x;
    {
    level = a * procent;
    }
    else               // for x = 63 to 100% => y = y1 + b*x;
    {
    //level = y1 + b * (procent - x1);
    level = a1 + b1 * procent;
    }  
level1 = map(level, 0, 100, 0, 255);      // convert 100% in 256 steps  
digitalPotWrite(wiper0writeAddr, level1); // Set wiper 0 to 255
//afisare0(procent);
digitalPotWrite(wiper1writeAddr, level1); // Set wiper 0 to 255
//afisare1(procent);

display.clearDisplay();
display.setTextSize(1);
display.setCursor(0,0);
display.print("angle1:");
display.setTextSize(2);
display.setCursor(36,0);
if (procent < 100) display.print(" ");
if (procent < 10) display.print(" ");
    display.print(procent);
    display.print("%");
display.setTextSize(1);
display.setCursor(0,24);
    display.print("output value1:");
    RawVoltage0 = analogRead(PotWiperVoltage0);
    Voltage0 = (RawVoltage0 * 100.0 )/ 1024.0;
display.setTextSize(2);
display.setCursor(0,32);
if (Voltage0 < 100) display.print(" ");
if (Voltage0 < 10) display.print(" ");
    display.print(Voltage0,2);
    display.print("%");   
    display.display(); // show splashscreen 
delay(scurt);
  }
delay(lung);

for (int procent = 0; procent <= 100; procent++) 
  {
    if (procent < 63)  // for x = 0 to 63% => y = a*x;
    {
    level = a * procent;
    }
    else               // for x = 63 to 100% => y = y1 + b*x;
    {
    //level = y1 + b * (procent - x1);
    level = a1 + b1 * procent;
    }  
level1 = map(level, 0, 100, 0, 255);      // convert 100% in 256 steps  
digitalPotWrite(wiper0writeAddr, level1); // Set wiper 0 to 255
//afisare0(procent);
digitalPotWrite(wiper1writeAddr, level1); // Set wiper 0 to 255
//afisare1(procent);

display.clearDisplay();
display.setTextSize(1);
display.setCursor(0,0);
display.setTextColor(WHITE, BLACK);
display.print("angle");
display.setCursor(0,8);
display.print("1 & 2");
display.setTextSize(2);
display.setTextColor(BLACK);
display.setCursor(36,0);
if (procent < 100) display.print(" ");
if (procent < 10) display.print(" ");
    display.print(procent);
    display.print("%");
display.setTextSize(1);
display.setTextColor(WHITE, BLACK);
display.setCursor(0,20);
    display.print("o");
display.setCursor(0,28);
    display.print("u");
display.setCursor(0,36);
    display.print("t");
    RawVoltage0 = analogRead(PotWiperVoltage0);
    Voltage0 = (RawVoltage0 * 100.0 )/ 1024.0;
    RawVoltage1 = analogRead(PotWiperVoltage1);
    Voltage1 = (RawVoltage1 * 100.0 )/ 1024.0;
display.setTextSize(2);
display.setTextColor(BLACK);
display.setCursor(0,16);
if (Voltage0 < 100) display.print(" ");
if (Voltage0 < 10) display.print(" ");
    display.print(Voltage0,2);
    display.print("%");
display.setTextSize(2);
display.setCursor(0,32);
if (Voltage1 < 100) display.print(" ");
if (Voltage1 < 10) display.print(" ");
    display.print(Voltage1,2);
    display.print("%");
    
    display.display(); // show splashscreen 
delay(scurt);
  }
delay(lung);


}

// This function takes care of sending SPI data to the pot.
void digitalPotWrite(int address, int value) {
  // take the SS pin low to select the chip:
  digitalWrite(slaveSelectPin,LOW);
  //  send in the address and value via SPI:
  SPI.transfer(address);
  SPI.transfer(value);
  // take the SS pin high to de-select the chip:
  digitalWrite(slaveSelectPin,HIGH); 
}


