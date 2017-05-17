#include <Adafruit_SSD1351.h>

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
#define SETTEMP 20                    // Temperature in *C of the hotbed
#define HYSTERESIS 3                  // Hysteresis


int samples[NUMSAMPLES];
int tempvalue = 0;
/***************************SETUP***********************************************/ 
void setup(void) {
  Serial.begin(9600);
  analogReference(EXTERNAL);
}
/***************************LOOP************************************************/ 
void loop(void) {
  measureTemp();
  delay(1000);
  
  if (tempvalue < -30) {
    digitalWrite(HOTBED, LOW);
    digitalWrite(LED, LOW);
    Serial.print("Sensor fehlt!");
  }
  
  else if(tempvalue > (SETTEMP + HYSTERESIS)) {
    digitalWrite(HOTBED, LOW);
    digitalWrite(LED, LOW);
    Serial.print("NICE ES IST WARM!");
  }
  
  else if(tempvalue < (SETTEMP - HYSTERESIS)) {
    digitalWrite(HOTBED, HIGH);
    digitalWrite(LED, HIGH);
    Serial.print("FUCK ES IST KALT!");
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
