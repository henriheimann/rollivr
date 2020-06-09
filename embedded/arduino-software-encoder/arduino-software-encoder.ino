
#include <Encoder.h>

Encoder encLeft(6, 7);
Encoder encRight(9, 10);

void setup() {
   Serial.begin(9600);
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
  if((currentMills + 100) <= millis()){
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
    
    Serial.print(distance / 1000.0); // Convert to meters
    Serial.print(",");
    Serial.println(angle);
  }
}

//berechnet aus Impulse strecke und anzahl tiks die aktuell zurückgelegte strecke
float impulseToLength(int i){
  return i*wheelSize/ticks;
}
