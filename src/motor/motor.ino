/* motor.ino : high level functions optimised to use 
 * two motors on an arduino board (pins must be defined
 * before usage)
 * 
 * 02.2016
 */

#define FORWARD     0
#define BACKWARD    1
#define BOTH        0
#define MOTOR_A     1
#define MOTOR_B     2
#define STOP        0
#define MAX_SPEED   255
#define CW          0
#define CCW         1

//-------------------TO BE MODIFIED----------------------
int STBY = 1; //standby

//Motor A
int PWMA = 1; //Speed control 
int AIN1 = 1; //Direction
int AIN2 = 1; //Direction

//Motor B
int PWMB = 1; //Speed control
int BIN1 = 1; //Direction
int BIN2 = 1; //Direction
//-------------------------------------------------------

void motorDrive    (int select, int speed, int orientation);
void motorStop     (void);
void motorMaxSpeed (int orientation);

void setup() {
  pinMode(STBY, OUTPUT);  // Must be added in setup

  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);

  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
}

void loop(){
  
}

void motorDrive(int select, int speed, int orientation){
/* Run specific motor at speed and orientation
 * select      : 0 for both, 1 for A and 2 for B
 * speed       : 0 is off, and 255 is full speed
 * orientation : 0 clockwise, 1 counter-clockwise
 */
  
  digitalWrite(STBY, HIGH); // Ensure not to be on standby

  boolean direc1 = LOW;
  boolean direc2 = HIGH;

  if(orientation == CCW){
    direc1 = HIGH;
    direc2 = LOW;
  }
  
  if(select == BOTH){
    digitalWrite(AIN1, direc1);
    digitalWrite(AIN2, direc2);
    digitalWrite(BIN1, direc2);  // Assuming both motors are
    digitalWrite(BIN2, direc1);  // in opposition on robot
    digitalWrite(PWMA, speed);
    digitalWrite(PWMB, speed);
  }
  else if(select == MOTOR_A){
    digitalWrite(AIN1, direc1);
    digitalWrite(AIN2, direc2);
    digitalWrite(PWMA, speed);
  }
  else{
    digitalWrite(BIN1, direc1);
    digitalWrite(BIN2, direc2);
    digitalWrite(PWMB, speed);
  }
}

void motorStop(void){
/* Stops all motor activity and sets them
 * on standby
 */
 
  digitalWrite(STBY, LOW);
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
  digitalWrite(PWMA, STOP);
  digitalWrite(PWMB, STOP);
}

void motorMaxSpeed(int orientation){
/* Sets both motors at maximum speed
 * orientation : 0 forward, 1 backward
 */

 digitalWrite(STBY, HIGH); // Ensure not to be on standby

  boolean direc1 = LOW;
  boolean direc2 = HIGH;

  if(orientation == BACKWARD){
    direc1 = HIGH;
    direc2 = LOW;
  }

  digitalWrite(AIN1, direc1);
  digitalWrite(AIN2, direc2);
  digitalWrite(BIN1, direc2);
  digitalWrite(BIN2, direc1);
  digitalWrite(PWMA, MAX_SPEED);    // Max speed is 255
  digitalWrite(PWMB, MAX_SPEED);
}

