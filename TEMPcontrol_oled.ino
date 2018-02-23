/*
Wiring:
(3,3V) ---- (10k-Resistor) -------|------- (Thermistor) ---- (GND)
                                  |          NTC 3950
                              Analog Pin 0
Connect AREF to 3,3V

Output PIN 8 to MOSFET- Driver
onboard LED as indicator
*/

#define THERMISTORPIN A0              // which analog pin to connect
#define HOTBED 8                      // which digital pin controls the MOSFET of the hotbed
#define LED 13                        // indicator LED
#define THERMISTORNOMINAL 100000      // resistance at 25 degrees C   
#define TEMPERATURENOMINAL 25         // temp. for nominal resistance (almost always 25 C)
#define NUMSAMPLES 10                 // how many samples to take and average, more takes longer
#define BCOEFFICIENT 3950             // The beta coefficient of the thermistor (usually 3000-4000)
#define SERIESRESISTOR 10000          // the value of the 'other' resistor
#define SETTEMP 70                    // Temperature in *C of the hotbed
#define HYSTERESIS 3                  // Hysteresis

/*include Libraries and stuff for LCD*******************************************/
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
#define CHARWIDTH           5
#define CHARHEIGHT          8

/*******************************************************************************/
int samples[NUMSAMPLES];
int tempvalue = 0;

/***************************SETUP***********************************************/ 
void setup(void) {
  Serial.begin(9600);
  initDisplay();
  analogReference(EXTERNAL);
}
/***************************LOOP************************************************/ 
void loop(void) {
  measureTemp();
//  delay(1000);
  
  if (tempvalue < 5) {
    digitalWrite(HOTBED, LOW);
    digitalWrite(LED, LOW);
    Serial.print("Sensor fehlt!");
    sensorFrame();
  }
  
  else if(tempvalue > (SETTEMP + HYSTERESIS)) {
    digitalWrite(HOTBED, LOW);
    digitalWrite(LED, LOW);
    Serial.print("Heizung aus!!!!!!!!!!!");
    mainFrame();
  }
  
  else if(tempvalue < (SETTEMP - HYSTERESIS)) {
    digitalWrite(HOTBED, HIGH);
    digitalWrite(LED, HIGH);
    Serial.print("Heizung an!!!!!!!!!!!!");
    mainFrame();
  }
  
}

/***************************Functions*******************************************/

void measureTemp(void) {
    uint8_t i;
  float average;
 
  // take N samples in a row, with a slight delay
  for (i=0; i< NUMSAMPLES; i++) {
   samples[i] = analogRead(THERMISTORPIN);
   delay(10);
  }
 
  // average all the samples out
  average = 0;
  for (i=0; i< NUMSAMPLES; i++) {
     average += samples[i];
  }
  average /= NUMSAMPLES;
 
  //Serial.print("Average analog reading "); 
  //Serial.println(average);
 
  // convert the value to resistance
  average = 1023 / average - 1;
  average = SERIESRESISTOR / average;
  //Serial.print("Thermistor resistance "); 
  //Serial.println(average);
 
  float tempInC;
  tempInC = average / THERMISTORNOMINAL;     // (R/Ro)
  tempInC = log(tempInC);                  // ln(R/Ro)
  tempInC /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  tempInC += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  tempInC = 1.0 / tempInC;                 // Invert
  tempInC -= 273.15;                         // convert to C
  
  tempvalue = tempInC;                      //save tempValue in int
 
  Serial.print("Temperatur "); 
  Serial.print(tempInC);
  Serial.println(" *C");
}

void dispSet(int X, int Y, int color, int Texsize) {
  display.setTextSize(Texsize);
  display.setTextColor(color);
  display.setCursor(X, Y);
}

void initDisplay(void) {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  dispSet(0, 0, WHITE, 2);
  display.println("TEMP");
  dispSet(30, 23, WHITE, 2);
  display.println("control");
  dispSet(0, 56, WHITE, 1);
  display.println("V1.2 - universal PID");
  display.display();
  delay(1500);
  display.clearDisplay();
}

void mainFrame(void) {
  dispSet(0, 0, WHITE, 2);
  display.println("random    Speed");
  dispSet(0, 35, WHITE, 1);
  display.println("Temperatur:");
  dispSet(36, 56, WHITE, 1);
  display.println("*C");
  dispSet(64, 35, WHITE, 1);
  display.println("for:");
  dispSet(110, 56, WHITE, 1);
  display.println("sec");
    dispSet(0, 50, WHITE, 2);
  display.println(tempvalue);
  display.display();  
}
/*
void sensorFrame(void) {
  display.clearDisplay();
  dispSet(50, 5, WHITE, 3);
  display.println("no");
  dispSet(0, 25, WHITE, 3);
  display.println("Sensor!");
  dispSet(0, 55, WHITE, 1);
  display.println("Connect a Sensor!");
  display.display();
  delay(500);
  display.clearDisplay();
  dispSet(50, 5, WHITE, 3);
  display.println("no");
  dispSet(0, 25, WHITE, 3);
  display.println("Sensor!");
  dispSet(0, 55, WHITE, 1);
  display.println("Connect a Sensor!");
  display.display();
  delay(500);
}*/

void sensorFrame(void) {
  display.clearDisplay();
  dispSet(45, 5, WHITE, 3);
  display.println("no");
  dispSet(0, 25, WHITE, 3);
  display.println("Sensor!");
  dispSet(15, 55, WHITE, 1);
  display.println("Connect a Sensor!");
  display.display();
  delay(500);
  dispSet(45, 5, WHITE, 3);
  display.println("no");
  dispSet(0, 25, WHITE, 3);
  display.println("Sensor!");
  display.clearDisplay();
  dispSet(15, 55, WHITE, 1);
  display.println("Connect a Sensor!");
  display.display();
  delay(500);

}
