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

//초음파 센서
char trig = 9;
char echo = 8;
int distance;
Ultrasonic ultrasonic(trig,echo);//초음파센서 생성자 호출

// bluetooth communication
int blueRx = 4;
int blueTx = 5;
unsigned char commandReadyFlag = 0;
unsigned char command, power;
SoftwareSerial mySerial(blueRx,blueTx); // 소프트웨어 시리얼통신 포트 생성

// sensor
#define BLACK 0
#define WHITE 1
// 회전 방향  
#define CW  1   // clockwise
#define CCW 0   // counter-clockwise
// 왼쪽, 오른쪽 바퀴  
#define MOTOR_LEFT 0
#define MOTOR_RIGHT 1
// motor settings
// 아두이노 PIN 설정
const byte PWMA = 3;      // PWM control (speed) for motor A
const byte PWMB = 11;     // PWM control (speed) for motor B
const byte DIRA = 12;     // Direction control for motor A
const byte DIRB = 13;     // Direction control for motor B
int sensor0, sensor1; // sensor reading 0 ~ 1023
int sensorLeft, sensorRight;  // BLACK(0), WHITE(1)

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  mySerial.begin(9600);
  setupArdumoto();      // Set all pins as outputs
}

void loop() {
  readSensors();
  ultra();
  int cutDistance = 10; //장애물 보고 멈출 위치 지정
  // Bluetooth 연결
  communication();

  if (command == 1){
    follow_line();

    //장애물 있을 때 멈춤
    if (distance<cutDistance)
      obstacle_avoid(cutDistance);
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
void robotForward(int powerLeft, int powerRight)
{
  driveArdumoto(MOTOR_RIGHT,CW,powerRight);
  driveArdumoto(MOTOR_LEFT,CCW,powerRight);
  
}
void robotBackward(int powerLeft, int powerRight)
{
  driveArdumoto(MOTOR_RIGHT,CCW,powerRight);
  driveArdumoto(MOTOR_LEFT,CW,powerRight);
  
}
void robotRight(int powerLeft, int powerRight)
{
  driveArdumoto(MOTOR_RIGHT,CCW,powerRight);
  driveArdumoto(MOTOR_LEFT,CCW,powerRight);
}
void robotLeft(int powerLeft, int powerRight)
{
  driveArdumoto(MOTOR_RIGHT,CW,powerRight);
  driveArdumoto(MOTOR_LEFT,CW,powerRight);
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
// Read sensor measurements
void readSensors(){
  sensor0 = 1023 - analogRead(A0);  // sensor Right
  sensor1 = 1023 - analogRead(A1);  // sensor Left
  sensorRight = sensor0;
  sensorLeft = sensor1;
  Serial.print("sensor R:");
  Serial.println(sensorRight);
  Serial.print("sensor L:");
  Serial.println(sensorLeft); 
  //delay(300);
}
// color finder
char colorFinder(int sensorValue) {
   char color;
   const int THRESHOLD = 650;
  if(sensorValue > THRESHOLD)
    color = WHITE;
   else
     color = BLACK;
   return color;
}
////////////////////////////////////////////////////////////////
// 로봇제어 함수
/////////////////////////////////////////////////////////////////
void robotControl(int sensorLeft, int sensorRight){
  if (sensorLeft == BLACK && sensorRight == BLACK){
      robotForward(100,100);
   
  }
            
   else if (sensorLeft == BLACK && sensorRight == WHITE) {
              robotLeft(100,100);
              delay(30);
    }
   else if (sensorLeft == WHITE && sensorRight == BLACK) {
              robotRight(100,100);
              delay(30);
    }
   else
              robotStop();
 }
// Bluetooth communication
void communication(){
  if (mySerial.available() >= 4){
    Serial.println("data arrived ");
    unsigned char buffer[4];
    // read data from buffer and save to array
    for (char i=0; i<4; i++){
      buffer[i] = mySerial.read();
    }
    // verify data
    if ( buffer[0]== 255 && buffer[3] == 100) {
         command = buffer[1];
         power = buffer[2];   
         Serial.print("command:");
         Serial.println(command);
          Serial.print("power:");
         Serial.println(power);
    }    
  }
}
void follow_line(){
  // 센서값 읽기
  readSensors();

  // color finder
  sensorRight = colorFinder(sensor1);
  sensorLeft  = colorFinder(sensor0);

  // 로봇제어
  robotControl(sensorLeft,sensorRight); 
}

 void ultra(){
   distance = ultrasonic.read();
  // read sensor
  readSensors();
//  Serial.print("distance:");
//  Serial.println(distance); // cm단위 측정
//  delay(500);
 }

void obstacle_avoid(int cutDistance){

   //학교 트랙에 맞게 값 조정하기
      robotRight(90,90);
      delay(400);
      robotForward(100,80);
      delay(1400);
        
      robotLeft(90,90);
      delay(500);
      robotForward(100,80);
      delay(1400);
      
//      //로봇이 라인 찾을 때까지 직진
//      while(true){
//        if(sensorLeft == WHITE && sensorRight == WHITE){
//            robotForward(100,80);
//            delay(10);
//      }
//      //라인 찾으면 lineFollowing으로 복귀함
//      else
//        break;
//      }
    }
