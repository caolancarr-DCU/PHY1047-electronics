/* This code works with MAX30102 + 128x32 OLED i2c + Buzzer and Arduino  UNO
 * It's displays the Average BPM on the screen, with an animation and a buzzer  sound
 * everytime a heart pulse is detected
 * It's a modified version of  the HeartRate library example
 * Refer to www.surtrtech.com for more details  or SurtrTech YouTube channel
 */


#include <Wire.h>
#include <DFRobot_RGBLCD1602.h>
#include "MAX30105.h"           //MAX3010x library
#include "heartRate.h"          //Heart rate  calculating algorithm

MAX30105 particleSensor;


const byte RATE_SIZE  = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array  of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last  beat occurred
float beatsPerMinute;
int beatAvg;
float spo2;
int spo2Avg;

// LCD configuration
DFRobot_RGBLCD1602 lcd(/*RGBAddr*/0x6B ,/*lcdCols*/16,/*lcdRows*/2);  //16 characters and 2 lines of show

byte heart[] = {
  B00000,
  B00000,
  B00000,
  B01010,
  B01110,
  B00100,
  B00000,
  B00000
};

byte heartBeat[] = {
  B00000,
  B00000,
  B01010,
  B11111,
  B11110,
  B01110,
  B00100,
  B00000
};


void setup() {  
  Serial.begin(115200);
  Serial.println("Initialising...");

  // Initialize LCD
  lcd.init();               // Initialize the LCD
  lcd.setPWM(WHITE, 255);         // Turn on the backlight

  #if 1
    lcd.customSymbol(0, heart);
    lcd.customSymbol(1, heartBeat);
  #endif
  
  // Display "Monaghan Best County" on two lines of the LCD
  lcd.clear();              // Clear any previous content
  
  lcd.setCursor(0, 0);
  lcd.print("Monaghan");
  lcd.setCursor(0,1);
  lcd.print("Best County ");
  lcd.write(1);

  tone(3,256);                                        //And  tone the buzzer for a 100ms you can reduce it it will be better
  delay(200);
  tone(3,323);                                        //And  tone the buzzer for a 100ms you can reduce it it will be better
  delay(200);
  tone(3,386);                                        //And  tone the buzzer for a 100ms you can reduce it it will be better
  delay(200); 
  tone(3,512);                                        //And  tone the buzzer for a 100ms you can reduce it it will be better
  delay(400);
  noTone(3);

  //  Initialize sensor
  particleSensor.begin(Wire, I2C_SPEED_FAST); //Use default  I2C port, 400kHz speed
  particleSensor.setup(); //Configure sensor with default  settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to  indicate sensor is running
}

void loop() {
 long irValue = particleSensor.getIR();    //Reading the IR value it will permit us to know if there's a finger on the  sensor or not
                                           //Also detecting a heartbeat
if(irValue  > 7000){                                           //If a finger is detected
    lcd.clear();                                              //Clears the LCD
    lcd.setCursor(0,0);
    lcd.write((unsigned char)0);
    lcd.print(" Heart rate:");
    lcd.setCursor(0,1);
    lcd.print(beatAvg);                                   //Prints the average beats per minute
    lcd.print(" BPM");
    
  if (checkForBeat(irValue) == true)                        //If  a heart beat is detected
  {
    lcd.clear();                                              //Clears the LCD
    lcd.setCursor(0,0);
    lcd.write((unsigned char)1);
    lcd.print(" Heart rate:");
    lcd.setCursor(0,1);
    lcd.print(beatAvg);                                     //Prints the average beats per minute
    lcd.print(" BPM");
    Serial.println(beatAvg);                             //Extremely quick and simple code to verify sensor is working
    tone(3,1000);                                        //And  tone the buzzer for a 100ms you can reduce it it will be better
    delay(100);
    noTone(3);                                          //Deactivate the buzzer  to have the effect of a "bip"
    //We sensed a beat!
    long delta = millis()  - lastBeat;                   //Measure duration between two beats
    lastBeat  = millis();

    beatsPerMinute = 60 / (delta / 1000.0);           //Calculating  the BPM

    if (beatsPerMinute < 255 && beatsPerMinute > 20)               //To  calculate the average we strore some values (4) then do some math to calculate the  average
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this  reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take  average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE  ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

}

  if (irValue < 7000){       //If no finger is detected it inform  the user and put the average BPM to 0 or it will be stored for the next measure
    beatAvg=0;
    lcd.clear();              // Clear any previous content 
    lcd.setCursor(0, 0);      // Set the cursor to the first column of the first row
    lcd.print("Please place");      // Print the first part of the text on the first row
    lcd.setCursor(0, 1);      // Set the cursor to the first column othe sef cond row
    lcd.print("your finger"); // Print the second part of the text on the second row
    noTone(3);
    }

}
