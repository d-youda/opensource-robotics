/******************************************************************************************/
//        MOTOR RIGHT     ==>  MOTOR DRIVER A
//        MOTOR LEFT      ==>  MOTOR DRIVER B
//        IR SENSOR RIGHT ==>  A0 (SENSOR0)
//        IR SENSOR LEFT  ==>  A1 (SENSOR1)
//        ECHO            ==>  8
//        TRIG            ==>  9
//        TXD             ==>  4
//        RXD             ==>  5
//
//     Appinventor file : Robot_Controller_II_robotics_2020.aia
/******************************************************************************************/
#include <SoftwareSerial.h>
#include <Ultrasonic.h>


//Ultrasonic sensor
char trig = 9;
char echo = 8;
int distance;

Ultrasonic ultrasonic(trig,echo);//ultrasonic sensor object
//bluetooth communication

int blueRx = 4;
int blueTx = 5;
unsigned char commandReadyFlag = 0;
unsigned char command, power;
SoftwareSerial mySerial(blueRx,blueTx);//소프트웨어 시리얼통신 포트 생성



#define BLACK 0
#define WHITE 1
//회전방향
#define CW 1
#define CCW 0

//바퀴
#define MOTOR_LEFT 0
#define MOTOR_RIGHT 1

//moter settings
//아두이노 핀 설정

const byte PWMA = 3;
const byte PWMB = 11;
const byte DIRA = 12;
const byte DIRB = 13;

int sensor0, sensor1;// sensor reading0-1023
int  sensorLeft, sensorRight; //BLACK(0), WHITE(1)

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  mySerial.begin(9600);
   setupArdumoto();

}

void loop() {
  
  
  
 
  ultra();//초음파센서
  int cutDistance = 10;//장애물과의 거리
  //Blutooth communicate
  communication();

  if(command == 1)//어플 작동
  {
    if(distance > cutDistance)//장애물 없을 떄
    {
      //read Sensor
      readSensors();
  
      //color Finder
      sensorRight = colorFinder(sensor0);
      sensorLeft = colorFinder(sensor1);

       delay(1000);

      robotControl(sensorLeft,sensorRight);
    }
    /*
    else if(distance<cutDistance)
    {
      robotStop();
      delay(1000);

      //장애물 피하기
      robotBackward(95,95);//일단 안전거리 유지 위한 후진
      delay(50);
      
      int rnd = random(0,1);//오른쪽 왼쪽 랜덤으로 고르기
      if(rnd == 0)
      {
        robotRight(95,95);
        delay(50);

        //검은 선 찾기
        //read Sensor
        readSensors();
  
        //color Finder
        sensorRight = colorFinder(sensor0);
        sensorLeft = colorFinder(sensor1);
      }
      else
      {
        robotLeft(95,95);
        delay(50);

        //검은 선 찾기
        //read Sensor
        readSensors();
  
        //color Finder
        sensorRight = colorFinder(sensor0);
        sensorLeft = colorFinder(sensor1); 
      }

    */
     
      
    }
    
    
  
  }
  else
  robotStop();
  
  

  
}
//ULTRASONIC

void ultra()
{
  //Ultrasonic sensor
  distance = ultrasonic.read();
  
  Serial.print("distance(CM):");
  Serial.println(distance); // distance in cm
  delay(500);
  
  
}
//Read Ssensor measurements
void readSensors()
{
  sensor0 = analogRead(A0); //sensorRight
  sensor1 = analogRead(A1); //sensorLeft
  
  sensorLeft = sensor0;
  sensorRight = sensor1;
  
  Serial.print("sensor R:");
  Serial.println(sensorRight);
  Serial.print("sensorL:");
  Serial.println(sensorLeft);
  delay(500);
}

//color finder
char colorFinder(int sensorValue)
{
  char color;
  const int THRESHOLD = 650;
  if(sensorValue >THRESHOLD)
  color = WHITE;
  else
  color = BLACK;

  return color;
}

void robotControl(int sensorLeft, int sensorRight)
{
  if(sensorLeft == BLACK && sensorRight == BLACK )
  robotFoward(100,100);
  else if(sensorLeft == BLACK && sensorRight == WHITE)
  {
    robotLeft(100,100);
    delay(30);
  }
  else if(sensorLeft == WHITE && sensorRight == BLACK)
  {
    robotRight(100,100);
    delay(30);
  }
  else
  robotStop();
  
}
 // setupArdumoto initialize all pins
void setupArdumoto()
{
  // All pins should be setup as outputs:
  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(DIRA, OUTPUT);
  pinMode(DIRB, OUTPUT);
  
  // Initialize all pins as low:
  digitalWrite(PWMA, LOW);
  digitalWrite(PWMB, LOW);
  digitalWrite(DIRA, LOW);
  digitalWrite(DIRB, LOW);
}

void driveArdumoto(byte motor, byte dir, byte spd)
{
  if(motor == MOTOR_RIGHT)
  {
    digitalWrite(DIRA, dir);
    analogWrite(PWMA, spd);
  }
  else if(motor == MOTOR_LEFT)
  {
    digitalWrite(DIRB, dir);
    analogWrite(PWMB, spd);
  }  
}

void robotFoward(int powerLeft, int powerRight)
{
  driveArdumoto(MOTOR_RIGHT,CW,powerRight);
  driveArdumoto(MOTOR_LEFT,CCW,powerLeft);
}

void robotBackward(int powerLeft, int powerRight)
{
  driveArdumoto(MOTOR_RIGHT,CCW,powerRight);
  driveArdumoto(MOTOR_LEFT,CW,powerLeft);
}

void robotRight(int powerLeft, int powerRight)
{
  driveArdumoto(MOTOR_RIGHT,CCW,powerRight);
  driveArdumoto(MOTOR_LEFT,CCW,powerLeft);
}

void robotLeft(int powerLeft, int powerRight)
{
  driveArdumoto(MOTOR_RIGHT,CW,powerRight);
  driveArdumoto(MOTOR_LEFT,CW,powerLeft);
} 

void robotStop()
{
  stopArdumoto(MOTOR_LEFT);
  stopArdumoto(MOTOR_RIGHT);
}

 // stopArdumoto makes a motor stop
void stopArdumoto(byte motor)
{
  driveArdumoto(motor, 0, 0);
}

//Bluetooth communication

void communication()
{
  if(mySerial.available() >= 4)
  {
   // Serial.println("data arrived:");
    unsigned char buffer[4];

    //read data from buffer and save to array
    for(char i=0; i<4; i++)
    {
      buffer[i] = mySerial.read();
    }

    //verify data

    if(buffer[0]== 255 && buffer[3]== 100)
    {
      command = buffer[1];
      power = buffer[2];
      /*
       Serial.print("command:");
       Serial.println(command);
       Serial.print("power:");
       Serial.println(power);
       */
    }
  }
}
