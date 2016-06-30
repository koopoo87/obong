#include <Servo.h>
#include <EEPROM.h>
#include <Easing.h>

#define CALIB_MAX 512//CALIB_MAX 512
#define CALIB_MIN 106//CALIB_MIN 100
#define SAMPLE_DELAY 50 // in ms, 50ms seems good

//pin allocation
uint8_t servoPin = 8;
uint8_t feedbackPin = A5;

uint8_t prevPos = 0;
uint8_t curPos = 0;
uint8_t receivedAngle = 0;
uint8_t sendServo = 0;
uint8_t prev_a = 0; // previous memory
int Loop = 0;

unsigned long start;
int sweepsNotFinished;

Servo myServo;

String command = "";
int m_Speed = 0;
int easingMode = 0;
int splitIndex = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("m");
  prevPos = readAnalog(feedbackPin, prevPos);
  Serial.println(prevPos);
}

void loop() {
  // put your main code here, to run repeatedly:

  if(Serial.available() > 0)
  {
    //command = Serial.readString();
    commandDecoder(Serial.readString());
    delay(500);

    if(command.equals("m"))//move
    {
        moveServo(servoPin, feedbackPin, receivedAngle);
    }
  }

}

uint8_t readAnalog(uint8_t analogPin, uint8_t savePos)
{
    pinMode(analogPin, INPUT);
    uint16_t a = analogRead(analogPin);
    //calibration
    if(a < CALIB_MIN) a = CALIB_MIN;
    if(a > CALIB_MAX) a = CALIB_MAX;
    a = map(a, CALIB_MIN, CALIB_MAX, 0, 180);
    savePos = a;
    return savePos;
}

void moveServo(uint8_t servoPin, uint8_t analogPin, uint8_t arrayPos)
{
  start = millis();
  myServo.attach(servoPin); //Attach Servo in 8 pin
  curPos = arrayPos;
  
  do
  {
    unsigned long now = millis() - start;
    sweepsNotFinished = 0;
    sweepsNotFinished += sweep(now, 0, 400 * (11 - m_Speed), prevPos, curPos);

    if ( now % 100 == 0)
    {
      Serial.println("m");
      Serial.println(sendServo);
     }


  }while(sweepsNotFinished>0);

  delay(250);
  prevPos = curPos;
  myServo.detach();
}

int sweep(unsigned long curTime, unsigned long startTime, unsigned long endTime, int startPos, int endPos)
{
   int angle = 0;
   if(curTime < startTime )
      return 1;
   if(curTime > endTime)
      return 0;


   
   if(easingMode == 0)
   {
      angle = Easing::linearTween(curTime, startPos, (endPos- startPos), (endTime - startTime));
   }

   else if(easingMode == 1)
   {
      angle = Easing::easeInOutCubic(curTime, startPos, (endPos- startPos), (endTime - startTime));
   }
   
   else if(easingMode == 2)
   {
      angle = Easing::easeOutBounce(curTime, startPos, (endPos- startPos), (endTime - startTime));
   }

   sendServo = angle;
   myServo.write(angle);
   
   return 1;
}

void commandDecoder(String inputText)
{
   splitIndex = inputText.indexOf(':');
   int count = 0;

   while(splitIndex != -1)
   {

      if(count == 0)
      {
          command = inputText.substring(0, splitIndex);
      }
      if(count == 1)
      {
          receivedAngle = inputText.substring(0, splitIndex).toInt();
      }
      if(count == 2)
      {
          easingMode = inputText.substring(0, splitIndex).toInt();
      }
      if(count == 3)
      {
          m_Speed = inputText.substring(0, splitIndex).toInt();
      }
      inputText = inputText.substring(splitIndex+1);
      splitIndex = inputText.indexOf('_');
      count ++;
   }
      //Serial.println("Command:" + command+", Angle: "+ receivedAngle +", easingMode: "+ easingMode +", m_Speed: "+ m_Speed);
}
