
const int PIN_BUTTON_LEFT = A0;
const int PIN_BUTTON_RIGHT = A5;
const int PIN_LED = LED_BUILTIN;

const float RADIUS = 1.0f;
const float SPEED = 0.8f;

void setup()
{
  pinMode(PIN_BUTTON_LEFT, INPUT);
  pinMode(PIN_BUTTON_RIGHT, INPUT);
  pinMode(PIN_LED, OUTPUT);

  Serial.begin(115200);
}

void loop()
{
 int buttonLeftState = !digitalRead(PIN_BUTTON_LEFT);
 int buttonRightState = !digitalRead(PIN_BUTTON_RIGHT);

 if (buttonLeftState == 1 || buttonRightState == 1) {
   digitalWrite(PIN_LED, HIGH);
 } else {
   digitalWrite(PIN_LED, LOW);
 }

 float distanceLeft = (buttonLeftState == 1) ? SPEED : 0.0f;
 float distanceRight = (buttonRightState == 1) ? SPEED : 0.0f;

 float distance = (distanceLeft + distanceRight) / 2.0f;
 float angle = (distanceLeft - distanceRight) / RADIUS;

 Serial.print(distance);
 Serial.print(",");
 Serial.println(angle);

 delay(10);
}
