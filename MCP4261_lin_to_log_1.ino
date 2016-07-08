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


#include <Wire.h>                 
#include <LiquidCrystal_I2C.h>    
LiquidCrystal_I2C lcd(0x27, 16, 2);

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
  

lcd.begin();  
lcd.backlight(); 
  lcd.createChar(0, grad);


lcd.setCursor(1, 0);   
lcd.print("MCP4261-502E/P"); 
lcd.setCursor(3, 1);   
lcd.print("lin to log"); 
delay(5000);           
lcd.clear();

digitalWrite(shutdownPin,HIGH); //Turn off shutdown
}


void loop() {

 
lcd.setCursor(0, 0);
lcd.print("Rotate from 0 to");
lcd.setCursor(0, 1);
lcd.print("100% => value...");
delay(lung);
lcd.clear();

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
afisare0(procent);

digitalPotWrite(wiper1writeAddr, level1); // Set wiper 0 to 255
afisare1(procent);
delay(scurt);
  }
delay(lung);

lcd.clear();
lcd.setCursor(0, 0);
lcd.print("Use 32 steps");
delay(lung);
lcd.clear();

for (int stepi = 0; stepi <= 32; stepi++) 
  {
    int procent1 = stepi * 3.125;
    if (procent1 < 63)  // for x = 0 to 63% => y = a*x;
    {
    level = a * procent1;
    }
    else               // for x = 63 to 100% => y = y1 + b*x;
    {
     level = a1 + b1 * procent1;
    }  
level1 = map(level, 0, 100, 0, 255);      // convert 100% in 256 steps  
//digitalPotWrite(wiper0writeAddr, level1); // Set wiper 0 to 255
//afisare0(procent1);
lcd.setCursor(0, 0);
lcd.print("Step no.");
if (stepi < 10) lcd.print(" ");
lcd.print(stepi);
lcd.print(" / 32");

digitalPotWrite(wiper1writeAddr, level1); // Set wiper 0 to 255
afisare1(procent1);
delay(2*scurt);
  }
delay(lung);

lcd.clear();
lcd.setCursor(0, 0);
lcd.print("Use 16 steps");
delay(lung);
lcd.clear();

for (int stepi = 0; stepi <= 16; stepi++) 
  {
    int procent1 = stepi * 6.25;
    if (procent1 < 63)  // for x = 0 to 63% => y = a*x;
    {
    level = a * procent1;
    }
    else               // for x = 63 to 100% => y = y1 + b*x;
    {
    //level = y1 + b * (procent - x1);
    level = a1 + b1 * procent1;
    }  
level1 = map(level, 0, 100, 0, 255);      // convert 100% in 256 steps  
//digitalPotWrite(wiper0writeAddr, level1); // Set wiper 0 to 255
//afisare0(procent1);
lcd.setCursor(0, 0);
lcd.print("Step no.");
if (stepi < 10) lcd.print(" ");
lcd.print(stepi);
lcd.print(" / 16");

digitalPotWrite(wiper1writeAddr, level1); // Set wiper 0 to 255
afisare1(procent1);
delay(2*scurt);
  }
delay(lung);

lcd.clear();
for (int stepi = 0; stepi <= 10; stepi++) 
  {
    int procent1 = stepi * 10;
    if (procent1 < 63)  // for x = 0 to 63% => y = a*x;
    {
    level = a * procent1;
    }
    else               // for x = 63 to 100% => y = y1 + b*x;
    {
    //level = y1 + b * (procent - x1);
    level = a1 + b1 * procent1;
    }  
level1 = map(level, 0, 100, 0, 255);      // convert 100% in 256 steps  
//digitalPotWrite(wiper0writeAddr, level1); // Set wiper 0 to 255
//afisare0(procent1);
lcd.setCursor(0, 0);
lcd.print("Step no.");
//if (stepi < 10) lcd.print(" ");
lcd.print(stepi);
lcd.print(" / 10");

digitalPotWrite(wiper1writeAddr, level1); // Set wiper 0 to 255
afisare1(procent1);
delay(2*scurt);
  }
delay(lung);

//lcd.clear();
lcd.clear();
for (int stepi = 0; stepi <= 8; stepi++) 
  {
    int procent1 = stepi * 12.5;
    if (procent1 < 63)  // for x = 0 to 63% => y = a*x;
    {
    level = a * procent1;
    }
    else               // for x = 63 to 100% => y = y1 + b*x;
    {
    //level = y1 + b * (procent - x1);
    level = a1 + b1 * procent1;
    }  
level1 = map(level, 0, 100, 0, 255);      // convert 100% in 256 steps  
//digitalPotWrite(wiper0writeAddr, level1); // Set wiper 0 to 255
//afisare0(procent1);
lcd.setCursor(0, 0);
lcd.print("Step no.");
//if (stepi < 10) lcd.print(" ");
lcd.print(stepi);
lcd.print(" / 8");

digitalPotWrite(wiper1writeAddr, level1); // Set wiper 0 to 255
afisare1(procent1);
delay(2*scurt);
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


void afisare0(byte lev) {
    lcd.setCursor(0, 0);
//    lcd.print("&1=");
    lcd.write(0b11100000);  // alpha symbol, see http://mil.ufl.edu/4744/docs/lcdmanual/characterset.html
    lcd.print("1=");
if (lev < 100) lcd.print(" ");
if (lev < 10) lcd.print(" ");
    lcd.print(lev);
    lcd.print("% ");
 //  lcd.write(0b11011111);  // degree symbol, see http://mil.ufl.edu/4744/docs/lcdmanual/characterset.html
//  lcd.write(byte(0));    
    
//    lcd.setCursor(8, 0);
    RawVoltage0 = analogRead(PotWiperVoltage0);
    Voltage0 = (RawVoltage0 * 100.0 )/ 1024.0;
if (Voltage0 < 100) lcd.print(" ");
if (Voltage0 < 10) lcd.print(" ");
    lcd.print(Voltage0,1);
    lcd.print("%U");    
}

void afisare1(byte lev) {
    lcd.setCursor(0, 1);
//    lcd.print("&1=");
    lcd.write(0b11100000);  // alpha symbol, see http://mil.ufl.edu/4744/docs/lcdmanual/characterset.html
    lcd.print("2=");
if (lev < 100) lcd.print(" ");
if (lev < 10) lcd.print(" ");
    lcd.print(lev);
    lcd.print("% ");
 //  lcd.write(0b11011111);  // degree symbol, see http://mil.ufl.edu/4744/docs/lcdmanual/characterset.html
//  lcd.write(byte(0));    
    
//    lcd.setCursor(8, 0);
    RawVoltage1 = analogRead(PotWiperVoltage1);
    Voltage1 = (RawVoltage1 * 100.0 )/ 1024.0;
if (Voltage1 < 100) lcd.print(" ");
if (Voltage1 < 10) lcd.print(" ");
    lcd.print(Voltage1,1);
    lcd.print("%U");    
}
