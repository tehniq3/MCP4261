/*
  Matthew McMillan
  @matthewmcmillan
  http://matthewcmcmillan.blogspot.com
  Created 12 Mar 2014
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
int lung = 2500;

int PotWiperVoltage0 = 0;
int RawVoltage0 = 0;
float Voltage0 = 0;
int PotWiperVoltage1 = 1;
int RawVoltage1 = 0;
float Voltage1 = 0;




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

lcd.setCursor(1, 0);   
lcd.print("MCP4261-502E/P"); 
lcd.setCursor(1, 1);   
lcd.print("test by niq_ro"); 
delay(3000);           
lcd.clear();

digitalWrite(shutdownPin,HIGH); //Turn off shutdown
digitalPotWrite(wiper0writeAddr, 0); // Set wiper 0 to 200
afisare0(0);
digitalPotWrite(wiper1writeAddr, 0); // Set wiper 1 to 200
afisare1(0);
delay(lung);

digitalPotWrite(wiper0writeAddr, 0); // Set wiper 0 to 200
afisare0(0);
digitalPotWrite(wiper1writeAddr, 255); // Set wiper 1 to 200
afisare1(255);
delay(lung);

digitalPotWrite(wiper0writeAddr, 255); // Set wiper 0 to 200
afisare0(255);
digitalPotWrite(wiper1writeAddr, 255); // Set wiper 1 to 200
afisare1(255);
delay(lung);

digitalPotWrite(wiper0writeAddr, 255); // Set wiper 0 to 200
afisare0(255);
digitalPotWrite(wiper1writeAddr, 0); // Set wiper 1 to 200
afisare1(0);
delay(lung);

}


void loop() {

  digitalWrite(shutdownPin,HIGH); //Turn off shutdown
/*
  digitalPotWrite(wiper0writeAddr, 200); // Set wiper 0 to 200
  digitalPotWrite(wiper1writeAddr, 200); // Set wiper 1 to 200
  digitalPotWrite(tconwriteAddr, tcon_0off_1on); // Disconnect wiper 0 with TCON register
  digitalPotWrite(tconwriteAddr, tcon_0off_1off); // Disconnect both wipers with TCON register
  digitalPotWrite(wiper0writeAddr, 255); //Set wiper 0 to 255
  digitalPotWrite(tconwriteAddr, tcon_0on_1on); // Connect both wipers with TCON register
*/

  for (int level = 0; level <= 51; level++) 
  {
digitalPotWrite(wiper0writeAddr, 5*level); // Set wiper 0 to 200
afisare0(5*level);

digitalPotWrite(wiper1writeAddr, 5*level); // Set wiper 1 to 200
afisare1(5*level);
delay(scurt);
  }

delay(lung);
for (int level = 51; level >= 0; level--) 
  {
digitalPotWrite(wiper0writeAddr, 5*level); // Set wiper 0 to 200
afisare0(5*level);

digitalPotWrite(wiper1writeAddr, 5*level); // Set wiper 1 to 200
afisare1(5*level);
delay(scurt);
  }

delay(lung);
for (int level = 51; level >= 0; level--) 
  {
digitalPotWrite(wiper0writeAddr, 5*level); // Set wiper 0 to 200
afisare0(5*level);

digitalPotWrite(wiper1writeAddr, 255-5*level); // Set wiper 1 to 200
afisare1(255-5*level);
delay(scurt);
  }

for (int level = 51; level >= 0; level--) 
  {
digitalPotWrite(wiper0writeAddr, 255-5*level); // Set wiper 0 to 200
afisare0(255-5*level);

digitalPotWrite(wiper1writeAddr, 5*level); // Set wiper 1 to 200
afisare1(5*level);
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


void afisare0(byte lev) {
    lcd.setCursor(0, 0);
    lcd.print("st1=");
if (lev < 100) lcd.print(" ");
if (lev < 10) lcd.print(" ");
    lcd.print(lev);
    lcd.setCursor(8, 0);
    RawVoltage0 = analogRead(PotWiperVoltage0);
    Voltage0 = (RawVoltage0 * 5.0 )/ 1024.0;
    lcd.print(Voltage0,3);
    lcd.print("/5V");    
}

void afisare1(byte lev) {
    lcd.setCursor(0, 1);
    lcd.print("st2=");
if (lev < 100) lcd.print(" ");
if (lev < 10) lcd.print(" ");
    lcd.print(lev);
    lcd.setCursor(8, 1);
    RawVoltage1 = analogRead(PotWiperVoltage1);
    Voltage1 = (RawVoltage1 * 5.0 )/ 1024.0;
    lcd.print(Voltage1,3);
    lcd.print("/5V");    
}
