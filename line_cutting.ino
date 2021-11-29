#include <SoftwareSerial.h>





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

  setupArdumoto();

  mySerial.begin(9600);



}



void loop() {

// readSensors();

  //Blutooth communicate

  communication();



  if(command == 1)

  {

  //read Sensor

  readSensors();

  

  //color Finder

  sensorRight = colorFinder(sensor0);

  sensorLeft = colorFinder(sensor1);





  delay(1000);


robotControl_Black(sensorLeft,sensorRight);
robotControl_White(sensorLeft,sensorRight);
robotControl_Black(sensorLeft,sensorRight);
robotControl_White(sensorLeft,sensorRight);
robotControl_Black(sensorLeft,sensorRight);
  

  }

  else

  robotStop();



 

}



//Read Ssensor measurements

void readSensors()

{
  double avg;

  sensor0 = 1023 - analogRead(A0); //sensorRight

  sensor1 = 1023 - analogRead(1); //sensorLeft

  

  sensorLeft = sensor0;

  sensorRight = sensor1;
 /*

  avg = (sensor0+sensor1)/2;

  Serial.print("sensor R:");

  Serial.print(sensorRight);

  Serial.print(" sensorL:");

  Serial.println(sensorLeft);

  Serial.print(" average:");
  Serial.println(avg);
  
  delay(500);
  */
 
}



//color finder

char colorFinder(int sensorValue)

{

  char color;

  const int THRESHOLD = 610;//내일 센서값에 맞게 조정하기.

  if(sensorValue >THRESHOLD)

  color = WHITE;

  else

  color = BLACK;



  return color;

}



void robotControl_Black(int sensorLeft, int sensorRight)

{
  
  if(sensorLeft == WHITE && sensorRight == WHITE )

  robotFoward(100,100);

  else if(sensorLeft == WHITE && sensorRight == BLACK)

  {

    robotLeft(100,100);

    delay(30);

  }

  else if(sensorLeft == BLACK && sensorRight == WHITE)

  {

    robotRight(100,100);

    delay(30);

  }

  else

  robotStop();

  

}

void robotControl_White(int sensorLeft, int sensorRight)

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

//cut-line robot Control

void robotControl_cutline(int sensorLeft, int sensorRight)
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

    Serial.println("data arrived:");

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





       Serial.print("command:");

       Serial.println(command);

       Serial.print("power:");

       Serial.println(power);

    }

  }

}
