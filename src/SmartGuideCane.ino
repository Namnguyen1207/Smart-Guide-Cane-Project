const int trigPins[4] = {2, 4, 8, 6};
const int echoPins[4] = {3, 5, 9, 7};
const int buzzerPin = 10;
const float thresholdDistanceLevel1 = 150;
const float thresholdDistanceLevel2 = 100;
const float fallthresholdDistance = 200;
typedef enum distanceStatus {
  WARNING_LEVEL_1,
  WARNING_LEVEL_2,
  NO_WARNING,
}distanceStatus;
distanceStatus warningSensorLeft, warningSensorFront, warningSensorRight;

float getDistance(int trigPin, int echoPin) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long duration = pulseIn(echoPin, HIGH);
    float distance = (duration * 0.034) / 2;
    return distance;
}

distanceStatus sensorProcess(int trigPin, int echoPin)
{
  distanceStatus warningLevel = NO_WARNING;
  static int count = 0;
  static float prevDistance = 0;
  float distance = getDistance(trigPin, echoPin);
  static float averageSpeed = 0;
  float vehicleSpeed;
  float delDistance;
        Serial.print("Sensor ");
        if(trigPin == 2) {
          Serial.print("LEFT");
        } else if(trigPin == 4) {
          Serial.print("FRONT");
        } else if(trigPin == 8) {
          Serial.print("RIGHT");
        }
        Serial.print(": ");
        Serial.print(distance);
        Serial.println(" cm");
        delDistance = abs(distance - prevDistance);
        if(warningSensorRight != WARNING_LEVEL_1 && warningSensorFront != WARNING_LEVEL_1 && warningSensorLeft != WARNING_LEVEL_1) {
          vehicleSpeed = (delDistance / 0.5) * 0.036;
        } else {
          vehicleSpeed = delDistance * 0.036;
        }
        prevDistance = distance;
        if(count == 5){
          averageSpeed = averageSpeed / 5;
          count = 0;
          // xu ly coi theo van toc
          averageSpeed = 0;
        } else {
          averageSpeed += vehicleSpeed; //  averageSpeed = averageSpeed + vehicleSpeed;
          count++; // count = count + 1;
        }
        Serial.println("Vehicle speed: " + String(vehicleSpeed));
        if (distance < thresholdDistanceLevel2 && distance > 0) {
            warningLevel = WARNING_LEVEL_2;
        } else if (distance < thresholdDistanceLevel1 && distance > 0) {
            warningLevel = WARNING_LEVEL_1; 
        } else {}

   return warningLevel;
}

void setup() {
    Serial.begin(9600);
    for (int i = 0; i < 4; i++) {
        pinMode(trigPins[i], OUTPUT);
        pinMode(echoPins[i], INPUT);
    }
    pinMode(buzzerPin, OUTPUT);
}

void loop() {

    warningSensorLeft = sensorProcess(trigPins[0], echoPins[0]);
    warningSensorFront = sensorProcess(trigPins[1], echoPins[1]);
    warningSensorRight = sensorProcess(trigPins[2], echoPins[2]);

    if((warningSensorRight == WARNING_LEVEL_1 && warningSensorFront != WARNING_LEVEL_2 && warningSensorLeft != WARNING_LEVEL_2)|| 
    (warningSensorFront == WARNING_LEVEL_1 && warningSensorRight != WARNING_LEVEL_2 && warningSensorLeft != WARNING_LEVEL_2) || 
    (warningSensorLeft == WARNING_LEVEL_1 && warningSensorFront != WARNING_LEVEL_2 && warningSensorRight != WARNING_LEVEL_2))
    {
            int count = 0;
      while(count < 5) {
        digitalWrite(buzzerPin, LOW);
        delay(50);
        digitalWrite(buzzerPin, HIGH);
        delay(50);
        count++;
      }
    } else if(warningSensorRight == NO_WARNING && warningSensorFront == NO_WARNING && warningSensorLeft == NO_WARNING) {
      digitalWrite(buzzerPin, HIGH);
    } else {
      // Nothing  
    }

    if(warningSensorRight == WARNING_LEVEL_2 || warningSensorFront == WARNING_LEVEL_2 || warningSensorLeft == WARNING_LEVEL_2)
    {
     digitalWrite(buzzerPin, LOW);
    } else if(warningSensorRight == NO_WARNING && warningSensorFront == NO_WARNING && warningSensorLeft == NO_WARNING) {
      digitalWrite(buzzerPin, HIGH);
    } else {
      // Nothing  
    }
 
    float fallDistance = getDistance(trigPins[3], echoPins[3]);
    Serial.println("FALL distance: " + String(fallDistance)); 

    delay(500);
}
