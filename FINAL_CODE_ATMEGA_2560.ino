//This code is the property of NAVAIR and is written to capture and record data of 102 inputs and store them on an SD card
// 11/30/2021 By: Robbie Butler, Jay Li, Robert Troutman and Aaron Strever

// connect 1st 74HC4067 S0~S3 (select lines) to Arduino D29~D26 respectively
// connect 2nd 74HC4067 S0~S3 (select lines) to Arduino D25~D22 respectively
// connect 3rd 74HC4067 S0~S3 (select lines) to Arduino D37~D34 respectively
// connect 4th 74HC4067 S0~S3 (select lines) to Arduino D33~D30 respectively
// connect 5th 74HC4067 S0~S3 (select lines) to Arduino D9~D6   respectively
// connect 6th 74HC4067 S0~S3 (select lines) to Arduino D13~D10 respectively
// connect 1st 74HC4067 S0~S3 (select lines) to Arduino D21~D18 respectively

// connect the 1st 74HC4067 signal pin to Arduino A0
// connect the 2nd 74HC4067 signal pin to Arduino A1
// connect the 3rd 74HC4067 signal pin to Arduino A2
// connect the 4th 74HC4067 signal pin to Arduino A3
// connect the 5th 74HC4067 signal pin to Arduino A4
// connect the 6th 74HC4067 signal pin to Arduino A5
// connect the 7th 74HC4067 signal pin to Arduino A6

// Get Time information from Uno and saves in LCD
// Note all libraries are found within arduino software already
#include <SPI.h>      //Includes SPI Library
#include <SD.h>       //Includes SD Library
#include <RTClib.h>   //Includes RTClib Library
#include <Wire.h>     //Includes Wire Library

//Declaring variables below
File root;
char t[32];
File sensorData;
char Mymessage[18];

// controlPins output table in array form
// see truth table on page 2 of TI 74HC4067 data sheet for the array below
byte controlPins[] =              //This sets up an array to send to each port to use the 74HC4067 truth table
                 {B00000000,      //Note 0 is low and 1 is high here
                  B10001000,
                  B01000100,
                  B11001100,
                  B00100010,
                  B10101010,
                  B01100110,
                  B11101110,
                  B00010001,
                  B10011001,
                  B01010101,
                  B11011101,
                  B00110011,
                  B10111011,
                  B01110111,
                  B11111111}; 

// holds incoming values from 74HC4067                  
byte muxValues[]  = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,}; //Stores the multiplexer values for the 1st mux
byte muxValues1[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,}; //Stores the multiplexer values for the 2nd mux
byte muxValues2[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,}; //Stores the multiplexer values for the 3rd mux
byte muxValues3[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,}; //Stores the multiplexer values for the 4th mux
byte muxValues4[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,}; //Stores the multiplexer values for the 5th mux
byte muxValues5[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,}; //Stores the multiplexer values for the 6th mux
byte muxValues6[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,}; //Stores the multiplexer values for the 7th mux

void setup()
{
  Serial.begin(9600);   //sets the baud rate to 9600 for serial communication 
  DDRA = B11111111;     // set PORTA (digital 22~29) to outputs
  DDRB = B11111111;     // set PORTB (digital 10~13) to outputs
  DDRC = B11111111;     // set PORTC (digital 30~37) to outputs
  DDRD = B11111111;     // set PORTD (digital 21~18) to outputs
  DDRH = B11111111;     // set PORTH (digital 6~9) to outputs
Serial.print("Initializing SD card...");
const int CSpin = 53;
pinMode(CSpin, OUTPUT);
pinMode(2, OUTPUT);

// see if the card is present and can be initialized:
if (!SD.begin(CSpin)) {
Serial.println("Card failed, or not present");
// don't do anything more:
return;
}
Serial.println("card initialized.");            //displays to the serial monitor that the SD card is initialized

digitalWrite(2,HIGH);                           //uses digital pin 2 as a flag from the Atmega to the Arduino to signal an event (sets digital pin 2 high)
Serial.readBytesUntil('\n', Mymessage, 18);     //tells the Atmega when to stop reading the searial data from the Uno
digitalWrite(2,LOW);                            //sets digital pin 2 low
}

void setPin(int outputPin)
// function to select pins on 74HC4067
{
  PORTA = controlPins[outputPin]; // sends the truth table commands to PORTA
  PORTB = controlPins[outputPin]; // sends the truth table commands to PORTB
  PORTC = controlPins[outputPin]; // sends the truth table commands to PORTC
  PORTD = controlPins[outputPin]; // sends the truth table commands to PORTD
  PORTH = controlPins[outputPin]; // sends the truth table commands to PORTH
}

void displayData()
// dumps captured data from array to serial monitor (used to identifty while debugging code)
{
  Serial.println();
  Serial.println("Values from multiplexer1:");   //displays the data captured from the 1st multiplexer to the serial monitor
  Serial.println("========================");
  for (int i = 0; i < 16; i++)
  {
    Serial.print("input I"); 
    Serial.print(i); 
    Serial.print(" = "); 
    Serial.println(muxValues[i]);
  }
  Serial.println("========================");
  
  Serial.println();
  Serial.println("Values from multiplexer2:");    //displays the data captured from the 2nd multiplexer to the serial monitor
  Serial.println("========================");
  for (int i = 16; i < 32; i++)
  {
    Serial.print("input I"); 
    Serial.print(i); 
    Serial.print(" = "); 
    Serial.println(muxValues1[i]);
  }
  Serial.println("========================");

  
  Serial.println();
  Serial.println("Values from multiplexer3:");    //displays the data captured from the 3rd multiplexer to the serial monitor
  Serial.println("========================");
  for (int i = 32; i < 48; i++)
  {
    Serial.print("input I"); 
    Serial.print(i); 
    Serial.print(" = "); 
    Serial.println(muxValues2[i]);
  }
  Serial.println("========================");     

  
  Serial.println();
  Serial.println("Values from multiplexer4:");    //displays the data captured from the 4th multiplexer to the serial monitor
  Serial.println("========================");
  for (int i = 48; i < 64; i++)
  {
    Serial.print("input I"); 
    Serial.print(i); 
    Serial.print(" = "); 
    Serial.println(muxValues3[i]);
  }
  Serial.println("========================");

  
  Serial.println();
  Serial.println("Values from multiplexer5:");    //displays the data captured from the 5th multiplexer to the serial monitor
  Serial.println("========================");
  for (int i = 64; i < 80; i++)
  {
    Serial.print("input I"); 
    Serial.print(i); 
    Serial.print(" = "); 
    Serial.println(muxValues4[i]);
  }
  Serial.println("========================");

  
  Serial.println();
  Serial.println("Values from multiplexer6:");    //displays the data captured from the 6th multiplexer to the serial monitor
  Serial.println("========================");
  for (int i = 80; i < 96; i++)
  {
    Serial.print("input I"); 
    Serial.print(i); 
    Serial.print(" = "); 
    Serial.println(muxValues5[i]);
  }
  Serial.println("========================");

  
  Serial.println();
  Serial.println("Values from multiplexer7:");    //displays the data captured from the 7th multiplexer to the serial monitor
  Serial.println("========================");
  for (int i = 96; i < 112; i++)
  {
    Serial.print("input I"); 
    Serial.print(i); 
    Serial.print(" = "); 
    Serial.println(muxValues6[i]);
  }
  Serial.println("========================");
}

String col;

void saveMux(String mux_id, int flag){  
  if(SD.exists("data.csv")){            //check the card is still there
  // now append new data file
    sensorData = SD.open("data.csv", FILE_WRITE);
  if (sensorData){
      String col = " ";
      int mux_c = mux_id.toInt();
      for (int c = 0; c < 112; c++)
     {
      //Serial.println(mux_id); 
      if (mux_c == c){
       if (flag == 1){
          sensorData.print(col);
          sensorData.print("HIGH"); 
          Serial.println(mux_id + "HIGH"); 
     
          digitalWrite(2,HIGH);
          
        Serial.readBytesUntil('\n', Mymessage, 18);    
        // Convert message from char to string
        String myString = String(Mymessage);
        sensorData.println(myString);                 // Print data to SD card
        Serial.println(myString);                     // Print data 
        sensorData.close();                           // close the file
        digitalWrite(2,LOW);
    }
              

       else if (flag == 0){
          sensorData.print(col);
          sensorData.print("LOW");
          Serial.println(mux_id + "LOW"); 

           digitalWrite(2,HIGH);
          
        Serial.readBytesUntil('\n', Mymessage, 18);   //Read the serial data and store in varSerial.println(Mymessage); 
        // Convert message from char to string
        String myString = String(Mymessage);
        sensorData.println(myString);                // print data to SD card
        Serial.println(myString);                    //Print data on Serial Monitor
        sensorData.close();                          // close the file
        digitalWrite(2,LOW);
       }
      }
        col = col + ',';
      }
    // , goes to next column, terminate by println
    sensorData.close();   
    }
    else{
    Serial.println("Error writing to file !");
    const int CSpin = 53;
    // see if the card is present and can be initialized:
  if (!SD.begin(CSpin)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
    }
  Serial.println("card initialized.");  //displays to the serial monitor that the SD card is initialized
  }
}
}
int flag[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int flag1[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int flag2[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int flag3[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int flag4[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int flag5[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

void loop()
{
  String mux_id;
  for (int i = 0; i < 16; i++)
  {
    setPin(i);
    //delay(3000);                //Used to debug select lines
    // choose an input pin on the 74HC4067
    muxValues[i]=analogRead(0);   // read the value on that pin and store in array
    
    if (muxValues[i] > 200 && flag[i] == 0){         //If input is high then flag
      flag[i] = 1;
      mux_id = i;
      saveMux(mux_id, flag[i]);
    }
    else if(muxValues[i] <= 200 && flag[i] == 1){    //Otherwise do nothing
      mux_id = i;
      flag[i] = 0;
      saveMux(mux_id, flag[i]);
    }
    else{
    }
    
    muxValues1[i]=analogRead(1);   // Reads Mux 1 value

    if (muxValues1[i] > 200 && flag1[i] == 0){         //If input is high then flag
      flag1[i] = 1;
      mux_id = i + 16;
      saveMux(mux_id, flag1[i]);
    }
    else if(muxValues1[i] <= 200 && flag1[i] == 1){    //Otherwise do nothing
      mux_id = i + 16;
      flag1[i] = 0;
      saveMux(mux_id, flag1[i]);
    }
    else{
    }
    
    muxValues2[i]=analogRead(2);   // Reads Mux 2 value

    if (muxValues2[i] > 200 && flag2[i] == 0){         //If input is high then flag
      flag2[i] = 1;
      mux_id = i +32;
      saveMux(mux_id, flag2[i]);
    }
    else if(muxValues2[i] <= 200 && flag2[i] == 1){    //Otherwise do nothing
      mux_id = i +32;
      flag2[i] = 0;
      saveMux(mux_id, flag2[i]);
    }
    else{
    }
    
    muxValues3[i]=analogRead(3);   // Reads Mux 3 value

    if (muxValues3[i] > 200 && flag3[i] == 0){         //If input is high then flag
      flag3[i] = 1;
      mux_id = i + 48;
      saveMux(mux_id, flag3[i]);
    }
    else if(muxValues3[i] <= 200 && flag3[i] == 1){    //Otherwise do nothing
      mux_id = i + 48;
      flag3[i] = 0;
      saveMux(mux_id, flag3[i]);
    }
    else{
    }
    
    muxValues4[i]=analogRead(4);   // Reads Mux 4 value

    if (muxValues4[i] > 200 && flag4[i] == 0){         //If input is high then flag
      flag4[i] = 1;
      mux_id = i+ 64;
      saveMux(mux_id, flag4[i]);
    }
    else if(muxValues4[i] <= 200 && flag4[i] == 1){    //Otherwise do nothing
      mux_id = i + 64;
      flag4[i] = 0;
      saveMux(mux_id, flag4[i]);
    }
    else{
    }
    
    muxValues5[i]=analogRead(5);   // Reads Mux 5 value

    if (muxValues5[i] > 200 && flag5[i] == 0){         //If input is high then flag
      flag5[i] = 1;
      mux_id = i +80;
      saveMux(mux_id, flag5[i]);
    }
    else if(muxValues5[i] <= 200 && flag5[i] == 1){    //Otherwise do nothing
      mux_id = i + 80;
      flag5[i] = 0;
      saveMux(mux_id, flag5[i]);
    }
    else{
    }
    
    //muxValues6[i]=analogRead(6);   // Reads Mux 6 value
  }
  
  //displayData();                  // display captured data for debugging purposes
  delay(200); 
}
