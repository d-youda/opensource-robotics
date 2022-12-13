from PyMata.pymata import PyMata
import time

# Pin
rightIrSensorPin = 0
leftIrSensorPin =1
PWMA = 3      # PWM control (speed) for motor A - RIGHT WHEEL
DIRA = 12     # Direction control for motor A
PWMB = 11     # PWM control (speed) for motor B - LEFT WHEEL
DIRB = 13     # Direction control for motor B

# connection port
PORT = '/dev/ttyACM1'

# Create PyMata instance
board = PyMata(PORT, verbose=True)
time.sleep(2)

# Motor settings
CW = 1   # clockwise
CCW = 0  # counter-clockwise
MOTOR_LEFT = 0 # left motor
MOTOR_RIGHT = 1 # right motor
colorRightSensor = 0
colorLeftSensor = 0
BLACK = 0
WHITE = 1

# Set analog pins 0 and 1 to be analog input pins
board.set_pin_mode(rightIrSensorPin,board.INPUT,board.ANALOG)
board.set_pin_mode(leftIrSensorPin,board.INPUT,board.ANALOG)

def readIrSensor(pin):
    value = 1023 - board.analog_read(pin)
    return value

def colorFinder(sensorValue):
    THRESHOLD_IR = 650
    color = WHITE if sensorValue > THRESHOLD_IR else BLACK
    return color

# Read IR sensor values
#  rightIrSensor = readIrSensor(rightIrSensorPin)
#  leftIrSensor = readIrSen#sor(leftIrSensorPin)

# Set motor pwm & directions
def setMotor(PWM, DIR):
    board.set_pin_mode(PWM,board.PWM,board.DIGITAL)
    board.set_pin_mode(DIR,board.OUTPUT,board.DIGITAL)
    #time.sleep(0.5)

def setRightAndLeftMotors():
    setMotor(PWMA,DIRA)
    setMotor(PWMB,DIRB)

# Drive Motor
def driveMotor(motor, direction, speed):
    if (motor == MOTOR_RIGHT):
        board.analog_write(PWMA,speed)
        board.digital_write(DIRA,direction)
    elif (motor == MOTOR_LEFT):
        board.analog_write(PWMB,speed)
        board.digital_write(DIRB,direction)

# Robot Motion Control
def robotForward(power):
    driveMotor(MOTOR_RIGHT,CW,power)
    driveMotor(MOTOR_LEFT,CCW,power)

def robotBackward(power):
    driveMotor(MOTOR_RIGHT,CCW,power)
    driveMotor(MOTOR_LEFT,CW,power)

def robotRight(power):
    driveMotor(MOTOR_RIGHT,CCW,power)
    driveMotor(MOTOR_LEFT,CCW,power)

def robotLeft(power):
    driveMotor(MOTOR_RIGHT,CW,power)
    driveMotor(MOTOR_LEFT,CW,power)

def robotStop():
    power = 0
    driveMotor(MOTOR_RIGHT,CW,power)
    driveMotor(MOTOR_LEFT,CW,power)

##########robot Control Function################
def robotControl(colorLeftSensor,colorRightSensor):
 if(colorLeftSensor == BLACK and colorRightSensor==BLACK):
    robotForward(100)

 elif(colorLeftSensor == WHITE and colorRightSensor==BLACK):
    robotLeft(100)
    time.sleep(0.1)

 elif(colorLeftSensor == BLACK and colorRightSensor==WHITE):
    robotRight(100)
    time.sleep(0.1)

 else:
    robotStop()

def follow_line():
#sensor value read
 rightIrSensor = readIrSensor(rightIrSensorPin)
 leftIrSensor = readIrSensor(leftIrSensorPin)

#color finder
 colorRightSensor = colorFinder(rightIrSensor)
 colorLeftSensor = colorFinder(leftIrSensor)

#robot control
 robotControl(colorLeftSensor , colorRightSensor)

############### Main ############################
# Set Right and Left Motors
setRightAndLeftMotors()

while True:
 follow_line()
