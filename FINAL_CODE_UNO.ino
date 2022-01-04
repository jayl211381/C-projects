// This code is the property of NAVAIR and uses the ARDUINO UNO BOARD
// 11/24/2021, By: Robbie Butler, Jay Li, Robert Troutman and Aaron Strever
// Serial communication to Mega using Rx and Tx serial ports

/*The circuit:
** SD card attached to SPI bus as follows:
** MOSI - pin 11
** MISO - pin 12
** CLK - pin 13
** CS - pin 10 Uno (53 on Mega)
*/ 

#include "SD.h"
#include"SPI.h"
#include <DS3232RTC.h>
#include <Streaming.h> 
#include "Wire.h"
#define DS1307_ADDRESS 0x68

byte zero = 0x00;                                    //workaround for issue #527
                                                     //the hardware CS pin (10 on most Arduino boards,
                                                     // 53 on the Mega) must be left as an output or the SD
                                                     // library functions will not work.
const int CSpin = 53;
String dataString ="";                               // holds the data to be written to the SD card
String str;
String str_full;
char time_flag[16];
File sensorData;

#include <LiquidCrystal.h>
//LCD pin to Arduino
const int pin_RS = 8;
const int pin_EN = 9; 
const int pin_d4 = 4; 
const int pin_d5 = 5; 
const int pin_d6 = 6; 
const int pin_d7 = 7; 
const int pin_BL = 10; 
LiquidCrystal lcd( pin_RS,  pin_EN,  pin_d4,  pin_d5,  pin_d6,  pin_d7);

// Declare global start time
byte second1 =      00; //0-59
byte minute1 =      00; //0-59
byte hour1 =        12; //0-23
byte weekDay1 =     4;  //1-7
byte monthDay1 =    30; //1-31
byte month1 =       11; //1-12
byte year1  =       21; //0-99

void setDateTime(){

  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero);                       //stop Oscillator
  Wire.write(decToBcd(second1));
  Wire.write(decToBcd(minute1));
  Wire.write(decToBcd(hour1));
  Wire.write(decToBcd(weekDay1));
  Wire.write(decToBcd(monthDay1));
  Wire.write(decToBcd(month1));
  Wire.write(decToBcd(year1));

  Wire.write(zero);                       //start 

  Wire.endTransmission();

}
byte decToBcd(byte val){
// Convert normal decimal numbers to binary coded decimal
  return ( (val/10*16) + (val%10) );
}

byte bcdToDec(byte val)  {
// Convert binary coded decimal to normal decimal numbers
  return ( (val/16*10) + (val%16) );
}

void digitalClockDisplay()
{
    // Reset the register pointer
    Wire.beginTransmission(DS1307_ADDRESS);
    Wire.write(zero);
    Wire.endTransmission();
  
    Wire.requestFrom(DS1307_ADDRESS, 7);
    
    int second = bcdToDec(Wire.read());
    int minute = bcdToDec(Wire.read());
    int hour = bcdToDec(Wire.read() & 0b111111); //24 hour time
    int weekDay = bcdToDec(Wire.read());         //0-6 -> sunday - Saturday
    int monthDay = bcdToDec(Wire.read());
    int month = bcdToDec(Wire.read());
    int year = bcdToDec(Wire.read());

    // To debug, print to serial monitor
    /*Serial.print(month);
    Serial.print("/");
    Serial.print(monthDay);
    Serial.print("/");
    Serial.print(year);
    Serial.print(" ");
    Serial.print(hour);
    Serial.print(":");
    Serial.print(minute);
    Serial.print(":");
    Serial.println(second);*/
    
    // utility function for digital clock display: prints preceding colon and leading 0
    str = String(hour);
    if(hour < 10){
    str = "0" + str;
    }
    if(minute < 10){
    str = str + ':' + '0' + String(minute);
    }
    else{
    str = str + ':' + String(minute);
    }
    if(second < 10){
      str = str + ':' + '0' + String(second);
    }
    else{
    str = str + ':' + String(second);
    }
    str = str + ' ' + String(monthDay) + ' ' + String(month);
    // full time to be sent out to the mega
    str_full = str + ' ' + String(year);
    int str_len = str_full.length() + 1;
    
    dataString = str;
    lcd.setCursor(0,1);
    lcd.print(dataString);
    lcd.setCursor(14,0);
    lcd.print(String(year));
}

void digitalClocksend()
// Send the time to the arduino mega
{
    // Reset the register pointer
    Wire.beginTransmission(DS1307_ADDRESS);
    Wire.write(zero);
    Wire.endTransmission();
  
    Wire.requestFrom(DS1307_ADDRESS, 7);
    
    int second = bcdToDec(Wire.read());
    int minute = bcdToDec(Wire.read());
    int hour = bcdToDec(Wire.read() & 0b111111); //24 hour time
    int weekDay = bcdToDec(Wire.read());         //0-6 -> sunday - Saturday
    int monthDay = bcdToDec(Wire.read());
    int month = bcdToDec(Wire.read());
    int year = bcdToDec(Wire.read());

    /*Serial.print(month);
    Serial.print("/");
    Serial.print(monthDay);
    Serial.print("/");
    Serial.print(year);
    Serial.print(" ");
    Serial.print(hour);
    Serial.print(":");
    Serial.print(minute);
    Serial.print(":");
    Serial.println(second);*/
    
    // utility function for digital clock display: prints preceding colon and leading 0
    str = String(hour);
    if(hour < 10){
    str = "0" + str;
    }
    if(minute < 10){
    str = str + ':' + '0' + String(minute);
    }
    else{
    str = str + ':' + String(minute);
    }
    if(second < 10){
      str = str + ':' + '0' + String(second);
    }
    else{
    str = str + ':' + String(second);
    }
    str = str + ' ' + String(monthDay) + ' ' + String(month);
    // full time to be sent out to the mega
    str_full = str + ' ' + String(year);
    int str_len = str_full.length() + 1;
    
    char Mymessage[18];
    str_full.toCharArray(Mymessage, 18);
    Serial.print(Mymessage);
    //Serial.write(Mymessage, 16);            //Write the serial data

    dataString = str;
    lcd.setCursor(0,1);
    lcd.print(dataString);
    lcd.setCursor(14,0);
    lcd.print(String(year));
}

void set_min()
//use up and down buttons to set minutes
{
  int x;
  x = analogRead (0);
    //Up
    if ((x < 400)) {
      if ( minute1 == 60){
        minute1 = -1;
      }
      minute1 = minute1 + 1;
      Wire.beginTransmission(DS1307_ADDRESS);
      Wire.write(zero); //stop Oscillator
    
      Wire.write(decToBcd(second1));
      Wire.write(decToBcd(minute1));
      Wire.write(decToBcd(hour1));
      Wire.write(decToBcd(weekDay1));
      Wire.write(decToBcd(monthDay1));
      Wire.write(decToBcd(month1));
      Wire.write(decToBcd(year1));
    
      Wire.write(zero); //start 
      Wire.endTransmission();
    }
    //Down
    else if ((x < 600)){
            if ( minute1 == 0){
        minute1 = 61;
      }
      minute1 = minute1 - 1;
      Wire.beginTransmission(DS1307_ADDRESS);
      Wire.write(zero); //stop Oscillator
    
      Wire.write(decToBcd(second1));
      Wire.write(decToBcd(minute1));
      Wire.write(decToBcd(hour1));
      Wire.write(decToBcd(weekDay1));
      Wire.write(decToBcd(monthDay1));
      Wire.write(decToBcd(month1));
      Wire.write(decToBcd(year1));
    
      Wire.write(zero); //start 
      Wire.endTransmission();
    }  
}

void set_sec()
{
  int x;
  x = analogRead (0);
    //Up
    if ((x < 400)) {
      if (second1 == 60){
        second1 = -1;
      }
      second1 = second1 + 1;
      Wire.beginTransmission(DS1307_ADDRESS);
      Wire.write(zero); //stop Oscillator
    
      Wire.write(decToBcd(second1));
      Wire.write(decToBcd(minute1));
      Wire.write(decToBcd(hour1));
      Wire.write(decToBcd(weekDay1));
      Wire.write(decToBcd(monthDay1));
      Wire.write(decToBcd(month1));
      Wire.write(decToBcd(year1));
    
      Wire.write(zero); //start 
      Wire.endTransmission();
    }
    //Down
    else if ((x < 600)){
            if (second1 == 0){
        second1 = 60;
      }
      second1 = second1 - 1;
      Wire.beginTransmission(DS1307_ADDRESS);
      Wire.write(zero); //stop Oscillator
    
      Wire.write(decToBcd(second1));
      Wire.write(decToBcd(minute1));
      Wire.write(decToBcd(hour1));
      Wire.write(decToBcd(weekDay1));
      Wire.write(decToBcd(monthDay1));
      Wire.write(decToBcd(month1));
      Wire.write(decToBcd(year1));
    
      Wire.write(zero); //start 
      Wire.endTransmission();
    }  
}

void set_24()
{
  int x;
  x = analogRead (0);
    //Up
    if ((x < 400)) {
      lcd.print ("Up");
      if (hour1 == 24){
        hour1 = -1;
      }
      hour1 = hour1 + 1;
      Wire.beginTransmission(DS1307_ADDRESS);
      Wire.write(zero); //stop Oscillator
    
      Wire.write(decToBcd(second1));
      Wire.write(decToBcd(minute1));
      Wire.write(decToBcd(hour1));
      Wire.write(decToBcd(weekDay1));
      Wire.write(decToBcd(monthDay1));
      Wire.write(decToBcd(month1));
      Wire.write(decToBcd(year1));
    
      Wire.write(zero); //start 
      Wire.endTransmission();
    }
    //Down
    else if ((x < 600)){
      lcd.print ("Down  ");
            if (hour1 == 0){
        hour1 = 25;
      }
      hour1 = hour1 - 1;
      Wire.beginTransmission(DS1307_ADDRESS);
      Wire.write(zero); //stop Oscillator
    
      Wire.write(decToBcd(second1));
      Wire.write(decToBcd(minute1));
      Wire.write(decToBcd(hour1));
      Wire.write(decToBcd(weekDay1));
      Wire.write(decToBcd(monthDay1));
      Wire.write(decToBcd(month1));
      Wire.write(decToBcd(year1));
    
      Wire.write(zero); //start 
      Wire.endTransmission();
    }  
}

void set_day()
{
  int x;
  x = analogRead (0);
    //Up
    if ((x < 400)) {
      lcd.print ("Up");
      if (monthDay1 == 31){
        monthDay1 = -1;
      }
      monthDay1 = monthDay1 + 1;
      Wire.beginTransmission(DS1307_ADDRESS);
      Wire.write(zero); //stop Oscillator
    
      Wire.write(decToBcd(second1));
      Wire.write(decToBcd(minute1));
      Wire.write(decToBcd(hour1));
      Wire.write(decToBcd(weekDay1));
      Wire.write(decToBcd(monthDay1));
      Wire.write(decToBcd(month1));
      Wire.write(decToBcd(year1));
    
      Wire.write(zero); //start 
      Wire.endTransmission();
    }
    //Down
    else if ((x < 600)){
      lcd.print ("Down  ");
            if (monthDay1 == 0){
        monthDay1 = 31;
      }
      monthDay1 = monthDay1 - 1;
      Wire.beginTransmission(DS1307_ADDRESS);
      Wire.write(zero); //stop Oscillator
    
      Wire.write(decToBcd(second1));
      Wire.write(decToBcd(minute1));
      Wire.write(decToBcd(hour1));
      Wire.write(decToBcd(weekDay1));
      Wire.write(decToBcd(monthDay1));
      Wire.write(decToBcd(month1));
      Wire.write(decToBcd(year1));
    
      Wire.write(zero); //start 
      Wire.endTransmission();
    }  
}

void set_month()
{
  int x;
  x = analogRead (0);
    //Up
    if ((x < 400)) {
      lcd.print ("Up");
      if (month1 == 12){
        month1 = -1;
      }
      month1 = month1 + 1;
      Wire.beginTransmission(DS1307_ADDRESS);
      Wire.write(zero); //stop Oscillator
    
      Wire.write(decToBcd(second1));
      Wire.write(decToBcd(minute1));
      Wire.write(decToBcd(hour1));
      Wire.write(decToBcd(weekDay1));
      Wire.write(decToBcd(monthDay1));
      Wire.write(decToBcd(month1));
      Wire.write(decToBcd(year1));
    
      Wire.write(zero); //start 
      Wire.endTransmission();
    }
    //Down
    else if ((x < 600)){
      lcd.print ("Down  ");
            if (month1 == 0){
        month1 = 13;
      }
      month1 = month1 - 1;
      Wire.beginTransmission(DS1307_ADDRESS);
      Wire.write(zero); //stop Oscillator
    
      Wire.write(decToBcd(second1));
      Wire.write(decToBcd(minute1));
      Wire.write(decToBcd(hour1));
      Wire.write(decToBcd(weekDay1));
      Wire.write(decToBcd(monthDay1));
      Wire.write(decToBcd(month1));
      Wire.write(decToBcd(year1));
    
      Wire.write(zero); //start 
      Wire.endTransmission();
    }  
}

void setup()
{
// Open serial communications
setSyncProvider(RTC.get);
time_t t = now();
RTC.set(now());  
setSyncProvider(RTC.get);   // the function to get the time from the RTC
    if(timeStatus() != timeSet)
        Serial.println("Unable to sync with the RTC");
    else
        Serial.println("RTC has set the system time");
pinMode(CSpin, OUTPUT);
pinMode(2,INPUT);

//Set initial time 
Wire.begin();
setDateTime(); //MUST CONFIGURE IN FUNCTION
//

// LCD setup
lcd.begin(16, 2);
lcd.setCursor(0,0);
lcd.print("Current Time:");
lcd.setCursor(0,1);
lcd.print("");
}

void send_time(){
  // Check for the flag from mega
  if (digitalRead(2) == 1){
    Serial.println("pin2 is high");
    Serial.begin(9600);
    digitalClocksend();
    Serial.end();
  }
}

void loop(){
send_time();
lcd.setCursor(0,0);
lcd.print("Current Time:");
// Reads Time from RTC and displays it on the LCD
digitalClockDisplay();

int x;
x = analogRead(0);
lcd.setCursor(0,1);
// Press Left to exit, only right loops
if ((x < 1000) && (x > 800)){
  int b_state = 1;
  // keep track of cursor location
  int cs_location = 0;
  lcd.setCursor(0,0); 
  lcd.print ("Set New Time: ");
  
  while (b_state = 1) {
    lcd.setCursor(cs_location,1); 
    lcd.print("  ");
    delay(50);
    digitalClockDisplay();
    delay(50);
    
    x = analogRead (0);
    
    //Right
    if ((x < 60)) {
      cs_location += 3;
      delay(50);
      if (cs_location > 12){
        break;
      }
    }
    // if up or down, set time 24 for hour 
    else if ((x < 400)){
      if(cs_location == 0){
        // if on hour location
        set_24();
      }
      else if((cs_location == 3)){
        set_min();
      }
      else if((cs_location == 6)){
        set_sec();
      }
      else if((cs_location == 9)){
        set_day();
      }
      else if((cs_location == 12)){
        set_month();
      }
      
    }
    else if ((x < 600)){
      if(cs_location == 0){
        // if on hour location
        set_24();
      }
      else if((cs_location == 3)){
        set_min();
      }
      else if((cs_location == 6)){
        set_sec();
      }
      else if((cs_location == 9)){
        set_day();
      }
      else if((cs_location == 12)){
        set_month();
      }
      
    }
    //Left
    else if ((x < 800)){
      cs_location -= 3;
      delay(50);
      if (cs_location < 0){
        break;
      }
    }
    // press select twice to stop
    else if ((x < 1000)){
      break;
    }
}
}
}
