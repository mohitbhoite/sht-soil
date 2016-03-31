/**
 * ReadSHT1xValues
 *
 * Read temperature and humidity values from an SHT1x-series (SHT10,
 * SHT11, SHT15) sensor.
 *
 * Copyright 2009 Jonathan Oxer <jon@oxer.com.au>
 * www.practicalarduino.com
 */


#include "application.h"
#include "SHT1x.h"

//SYSTEM_MODE(MANUAL);

//Connections:
// SHT_RED      ELECTRON_3V3
// SHT_BLACK    ELECTRON_GND
// SHT_YELLOW   ELECTRON_D1   CLOCK
// SHT_GREEN    ELECTRON_D0   DATA

// Specify data and clock connections and instantiate SHT1x object
#define dataPin  D0
#define clockPin D1
SHT1x sht1x(dataPin, clockPin);

#define MAX_TEMP        100.0
#define MIN_TEMP        40.0  
#define MIN_HUMIDITY    30.0

uint32_t previousMillis = 0;
const uint32_t interval = 1000; //milliseconds
bool state = 0;

int soilPublish(String command);

void setup()
{
  Particle.function("soil", soilPublish);
  Serial.begin(9600); // Open serial connection to report values to host
  Serial.println("Starting up");
  pinMode(D7,OUTPUT);
}

void loop()
{
  float temp_c;
  float temp_f;
  float humidity;


  uint32_t currentMillis = millis();

  // read sensor at set intervals
  if (currentMillis - previousMillis >= interval) 
  {
    previousMillis = currentMillis;

    state =! state;
    digitalWrite(D7,state);

    // Read values from the sensor
    temp_c = sht1x.readTemperatureC();
    temp_f = sht1x.readTemperatureF();
    humidity = sht1x.readHumidity();

    //check to see if the measurements are out of bound
    if(((temp_f > MAX_TEMP) || (temp_f < MIN_TEMP)) ||
      ((humidity < MIN_HUMIDITY)))
    {
      String soil_reading = "{\"temp\":" + String(temp_f,2) + ",\"humidity\":" + String(humidity,2) + "}";
      //publish an alert event along with the readings
      Particle.publish("alert", soil_reading, 60, PRIVATE);
    }

    // Print the values to the serial port
    // good for debugging
    Serial.print("Temperature: ");
    Serial.print(temp_c, DEC);
    Serial.print("C / ");
    Serial.print(temp_f, DEC);
    Serial.print("F. Humidity: ");
    Serial.print(humidity);
    Serial.println("%");
  }  

}


int soilPublish(String command)
{
  String soil_reading = "{\"temp\":" + String(sht1x.readTemperatureF(),2) + ",\"humidity\":" + String(sht1x.readHumidity(),2) + "}";
  Particle.publish("soil", soil_reading, 60, PRIVATE);
  return 1;
}