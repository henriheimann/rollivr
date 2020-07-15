
#include <Encoder.h>

#define ROLLING_AVERAGE_SAMPLES 5
float rollingAverageDistance[ROLLING_AVERAGE_SAMPLES];
float rollingAverageAngle[ROLLING_AVERAGE_SAMPLES];

Encoder encLeft(6, 7);
Encoder encRight(9, 10);

void setup() {
   Serial.begin(115200);

   for (int i = 0; i < ROLLING_AVERAGE_SAMPLES; i++) {
    rollingAverageDistance[i] = rollingAverageAngle[i] = 0;
   }
}

long oldLeft  = 0;
long oldRight  = 0;

long currentMills = 0;

//strecke
float distanceLeft = 0;
float distanceRight = 0;

//impulse
int impulseLeft = 0;
int impulseRight = 0; 

float wheelSize = 254.0;//Radumfang in mm
int ticks = 128; //anzahl Tiks pro Umdrehung
float radius = 285.0; //in mm abstand von mitte bis rad da wurde die mitte der rollen gewählt

void loop() {

  long newLeft = encLeft.read();
    long newRight = encRight.read();
  
 // Serial.println(oldLeft);
  if((currentMills + 10) <= millis()){
    currentMills = millis();
    
    //berechnunge linkes Rad
    impulseLeft = oldLeft - newLeft ;
    oldLeft = newLeft;
    distanceLeft = impulseToLength(impulseLeft);
  
    //berechnunge rechtes Rad
    impulseRight = oldRight - newRight;
    oldRight = newRight;
    distanceRight = impulseToLength(impulseRight);

    float distance = (distanceLeft + distanceRight)/2;
    float angle = (distanceLeft - distanceRight)/ radius;

    angle = angle * 57.3;

    for (int i = 0; i < ROLLING_AVERAGE_SAMPLES - 1; i++) {
      rollingAverageDistance[i + 1] = rollingAverageDistance[i];
      rollingAverageAngle[i + 1] = rollingAverageAngle[i];
    }

    rollingAverageDistance[0] = distance / 10.0;
    rollingAverageAngle[0] = angle;

    float averageDistance = 0;
    float averageAngle = 0;
    for (int i = 0; i < ROLLING_AVERAGE_SAMPLES; i++) {
      averageDistance += rollingAverageDistance[i];
      averageAngle += rollingAverageAngle[i];
    }

    averageDistance /= ROLLING_AVERAGE_SAMPLES;
    averageAngle /= ROLLING_AVERAGE_SAMPLES;
    
    
    Serial.print(averageDistance, 5);
    Serial.print(",");
    Serial.println(averageAngle, 3);
  }
}

//berechnet aus Impulse strecke und anzahl tiks die aktuell zurückgelegte strecke
float impulseToLength(int i){
  return i*wheelSize/ticks;
}
