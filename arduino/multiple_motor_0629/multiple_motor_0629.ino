#include <Servo.h>
#include <EEPROM.h>
#include <Easing.h>

#define CALIB_MAX 512//CALIB_MAX 512
#define CALIB_MIN 106//CALIB_MIN 100
#define SAMPLE_DELAY 50 // in ms, 50ms seems good


uint8_t recordButtonPin = 12;
uint8_t playButtonPin = 7;
uint8_t servoPin = 8;
uint8_t feedbackPin = A5;

uint8_t prev_Pos[6], cur_Pos[6];
uint8_t myArray[6];
uint8_t sendServo[6];

uint16_t prev_a = 0; //previous memory
int countLoop = 0;



String p = "";
String inputText = "";
String command = "";
String type = "";
uint16_t movingMode = 0; //easeInOut Num
String statusId = "";
int splitIndex = 0;
int m_speed = 0;

unsigned long start;
int sweepsNotFinished;

Servo myServo[6]; //6 servos

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  //Serial.println("Serial Begin");

  readAnalog(feedbackPin, prev_Pos);

  for (int i = 0; i < 6; i++)
  {
    if (i == 0)
      Serial.println("m");
    myArray[i] = 0;
    cur_Pos[i] = 0;
    sendServo[i] = 0;
    //Serial.print("m_");Serial.print(i);Serial.print("_");;
    Serial.println(prev_Pos[i]);
    //Serial.write(13);

  }
}

void loop() {
  // put your main code here, to run repeatedly:
  //establishContact();

  /*if(Serial.available() < 0)
    Serial.println("disconnected");*/
  if (Serial.available() == 0)
  {
    //Serial.println("0 received");
  }

  if (Serial.available() > 0) { //why this is not always in the loop?


    //Serial.println("Serial Available");
    commandSplit(Serial.readString());
    delay(500);


    if (command.equals("move"))
    {
      //Serial.println("move all");
      move_all(servoPin, feedbackPin, myArray);
      //move_servo(statusId.toInt(), servoPin, feedbackPin, motorAngle);
    }

    if (command.equals("save"))
    {
      //Serial.println("save" + type + statusId);
      save(feedbackPin, statusId.toInt());

    }

    if (command.equals("play"))
    {
      //Serial.println("play" + type + statusId);
      play(servoPin, statusId.toInt());

    }
  }
}

void save(uint8_t analogPin, uint16_t statusId) {

  uint16_t addr = 1 + 6 * (statusId); //unsigned short
  //Serial.println("Saving");


  for (int i = 0 ; i < 6; i++)
  {
    pinMode(analogPin - i, INPUT);
    uint16_t a = analogRead(analogPin + i); //Arduino Uno: 1kb EEPROM storage.
    //Serial.println(a);
    //calibaration
    if (a < CALIB_MIN) a = CALIB_MIN;
    if (a > CALIB_MAX) a = CALIB_MAX;
    a = map(a, CALIB_MIN, CALIB_MAX, 0, 254);
    //value ? (CALIB_MIN = 0 , CALIB_MAX 1024, 0 ~ 180)
    
    String address = String(addr + i);

    if (prev_a != a)
      EEPROM.write(addr + i, a); // when the saved value is different from the prev value, store.

    prev_a = EEPROM.read(addr + i);
  }
  delay(SAMPLE_DELAY);
  delay(250);
}


void play(uint8_t servoPin, uint16_t statusId ) {

  uint16_t addr = 1 + 6 * (statusId); //unsigned short
  start = millis();

  for (int i = 0 ; i < 6; i++)
  {
    myServo[i].attach(servoPin + i); // Attach Servo in 8 pin
  }

  for (int i = 0; i < 6; i++) //have to play in one time
  {
    cur_Pos[i] = EEPROM.read(addr + i); //uint8_t : unsigned char
    cur_Pos[i] = map(cur_Pos[i], 0, 254, 0, 180);
  }
  moveServos(m_speed);
  delay(250);
  delay(SAMPLE_DELAY);
  for (int i = 0 ; i < 6; i++)
  {
    prev_Pos[i] = cur_Pos[i];
    myServo[i].detach(); // Attach Servo in 8 pin
  }
}


void move_all(uint8_t servoPin, uint8_t analogPin, uint8_t array[]) {

  //Serial.println("Move all");
  start = millis();
  countLoop = 0;

  for (int i = 0 ; i < 6; i++)
  {
    myServo[i].attach(servoPin + i); // Attach Servo in 8 pin
  }

  for (int i = 0; i < 6; i++) //have to play in one time
  {

    //Serial.print(" -> "); Serial.println(x);
    cur_Pos[i] = array[i];

  }
  //move servos
  moveServos(m_speed);
  delay(250);
  delay(SAMPLE_DELAY);
  //Serial.println("Write Done");
  for (int i = 0 ; i < 6; i++)
  {
    prev_Pos[i] = cur_Pos[i];
    myServo[i].detach(); // Attach Servo in 8 pin
  }
  //readAnalog(analogPin, myArray);
  delay(SAMPLE_DELAY);
  delay(250);
}

void moveServos(int m_speed) {

  do {

    unsigned long now = millis() - start;
    sweepsNotFinished = 0;
    sweepsNotFinished += sweep(now, 0, 400 * (11 - m_speed), prev_Pos[0], cur_Pos[0], 0);
    sweepsNotFinished += sweep(now, 0, 400 * (11 - m_speed), prev_Pos[1], cur_Pos[1], 1);
    sweepsNotFinished += sweep(now, 0, 400 * (11 - m_speed), prev_Pos[2], cur_Pos[2], 2);
    sweepsNotFinished += sweep(now, 0, 400 * (11 - m_speed), prev_Pos[3], cur_Pos[3], 3);
    sweepsNotFinished += sweep(now, 0, 400 * (11 - m_speed), prev_Pos[4], cur_Pos[4], 4);
    sweepsNotFinished += sweep(now, 0, 400 * (11 - m_speed), prev_Pos[5], cur_Pos[5], 5);
    //Serial.println(countLoop);
    countLoop ++;


    if ( now % 100 == 0)
    {
      //readAnalog(feedbackPin, sendServo);
      Serial.println("m");
      Serial.println(sendServo[0]);
      Serial.println(sendServo[1]);
      Serial.println(sendServo[2]);
      Serial.println(sendServo[3]);
      Serial.println(sendServo[4]);
      Serial.println(sendServo[5]);
    }

  } while (sweepsNotFinished > 0);

}

int sweep(unsigned long curTime, unsigned long startTime, unsigned long endTime, int startPos, int endPos, int servoNum)
{

  int angle = 0;
  if (curTime < startTime)
    return 1;
  if (curTime > endTime)
    return 0;


  if (movingMode == 0)
    angle = Easing::linearTween(curTime, startPos, (endPos- startPos), (endTime - startTime));
  if (movingMode == 1)
    angle = Easing::easeInOutCubic(curTime, startPos, (endPos- startPos), (endTime - startTime));
  if (movingMode == 2)
    angle = Easing::easeOutBounce(curTime, startPos, (endPos- startPos), (endTime - startTime));


  //angle = SimpleTweening(currentTime, startTime, endTime, startPosition, endPosition);
  sendServo[servoNum] = angle;
  myServo[servoNum].write(angle);

  return 1;
}


void readAnalog(uint8_t analogPin, uint8_t arr[]) {
  for (int i = 0 ; i < 6; i++)
  {
    pinMode(analogPin + i, INPUT);
    uint16_t a = analogRead(analogPin + i);
    //calibaration
    if (a < CALIB_MIN) a = CALIB_MIN;
    if (a > CALIB_MAX) a = CALIB_MAX;
    a = map(a, CALIB_MIN, CALIB_MAX, 0, 180);
    arr[i] = a;
    //Serial.println(a);
  }
}


void commandSplit(String inputText)
{

  splitIndex = inputText.indexOf(':'); //save:s_1, save:s_2
  if (splitIndex != -1)
    command_main(inputText);

  if (inputText.indexOf('(') != -1)
    command_debug(inputText);


}

void command_main(String inputText) {

  int count = 0;

  while (splitIndex != -1 )
  {
    // split Index found

    if (count == 0)
    {
      command = inputText.substring(0, splitIndex);
    }
    if (count == 1)
    {
      type = inputText.substring(0, splitIndex);
    }
    if (count == 2)
    {
      statusId = inputText.substring(0, splitIndex);
    }
    inputText =  inputText.substring(splitIndex + 1);
    splitIndex = inputText.indexOf('_');

    //these data should be sent only once !
    count++;
  }
  m_speed = inputText.substring(splitIndex + 1).toInt();
  //Serial.println("Command:" + command+", type: "+ type +", statusId: "+ statusId + ", motor speed : "+ m_speed);
}



void command_debug(String inputText) {

  command = "move";
  inputText.remove(0, 1);
  splitIndex = inputText.indexOf(',');
  //inputText =  inputText.substring(splitIndex+1);
  int count = 0;

  while (splitIndex != -1 )
  {

    //p = inputText.substring(0, splitIndex);
    myArray[count] = inputText.substring(0, splitIndex).toInt();
    //Serial.print(count);Serial.println(","+p);
    inputText =  inputText.substring(splitIndex + 1);
    splitIndex = inputText.indexOf(',');
    count++;
  }
  //p = inputText.substring(splitIndex+1,inputText.indexOf(')'));
  myArray[count] =  inputText.substring(splitIndex + 1, inputText.indexOf(')')).toInt();

  splitIndex = inputText.indexOf(')');
  inputText =  inputText.substring(splitIndex + 1);
  movingMode = inputText.substring(0, inputText.indexOf('_')).toInt();
  m_speed = inputText.substring(inputText.indexOf('_')+1).toInt();

  Serial.println("Command:" + command+", type: "+ type +", moving Mode: "+ movingMode + ", motor speed : "+ m_speed );
}


