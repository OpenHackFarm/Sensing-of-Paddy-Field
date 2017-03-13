#include <SoftwareSerial.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>

#define DELAY 60000

#define ONE_WIRE_BUS_1 2
#define ONE_WIRE_BUS_2 3

#define SERIESRESISTOR 560
#define NUMSAMPLES 15
int samples[NUMSAMPLES];
#define SENSORPIN A7
#define FLAP 1
#define FUDGE 1.3
float lastwaterlevel=0;

OneWire oneWire_1(ONE_WIRE_BUS_1);
OneWire onewire_2(ONE_WIRE_BUS_2);

DallasTemperature temp_sensor_1(&oneWire_1);
DallasTemperature temp_sensor_2(&onewire_2);

float etape_liquid_level = 0;
float temp_1 = 0;
float temp_2 = 0;

SoftwareSerial LoRa(8, 9); // UNO (RX, TX)
char val;

/*
    eTape Liquid Level Sensor Sample Code
    https://archive.is/oPUEl#selection-107.42-107.49
*/
void read_water_level() {
     uint8_t i;
     float average;
     float waterlevel;
    // take N samples in a row, with a slight delay
    for (i=0; i< NUMSAMPLES; i++) {
    samples[i] = analogRead(SENSORPIN);
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
    //Serial.print("Sensor resistance ");
    //Serial.println(average);
    waterlevel = 0;
    waterlevel= -1 * 0.006958 * average + 11.506958+ FUDGE;
    if (lastwaterlevel<(waterlevel-FLAP)||lastwaterlevel>(waterlevel+FLAP)) {
        Serial.print("Water flap: ");
        Serial.print(waterlevel);
        Serial.print(" inches, ");
        Serial.print(waterlevel * 2.54);
        Serial.println("cm");
    }
    else {
        Serial.print("Water level: ");
        Serial.print(waterlevel);
        Serial.print(" inches, ");
        Serial.print(waterlevel * 2.54);
        Serial.println("cm");
    }

    if (waterlevel > 0) {
          lastwaterlevel = waterlevel;
    } else {
          lastwaterlevel = 0;
    }

}

void read_temp_1() {
    temp_sensor_1.requestTemperatures();
    temp_1 = temp_sensor_1.getTempCByIndex(0);

    if(temp_1 == -127) {
      temp_1 = 0;
    }

    Serial.print("Temperature 1: ");
    Serial.print(temp_1);
    Serial.println(" *C ");
}

void read_temp_2() {
    temp_sensor_2.requestTemperatures();
    temp_2 = temp_sensor_2.getTempCByIndex(0);

    if(temp_2 == -127) {
      temp_2 = 0;
    }

    Serial.print("Temperature 2: ");
    Serial.print(temp_2);
    Serial.println(" *C ");
}

void setup() {
    Serial.begin(9600);
    LoRa.begin(9600);
    temp_sensor_1.begin();
    temp_sensor_2.begin();
}

void loop() {
    read_water_level();
    read_temp_1();
    read_temp_2();

    Serial.print(temp_1);
    Serial.print(",");
    Serial.print(temp_2);
    Serial.print(",");
    Serial.print(lastwaterlevel * 2.54);
    Serial.println();

    LoRa.print(temp_1);
    LoRa.print(",");
    LoRa.print(temp_2);
    LoRa.print(",");
    LoRa.print(lastwaterlevel * 2.54);
    LoRa.println();

    delay(DELAY);
}
