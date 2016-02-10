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

//-------------------PINS TO CHANGE----------------------
#define STBY_GEAR  1 //standby

//Motor A
#define PWMA_GEAR  1 //Speed control 
#define AIN1_GEAR  1 //Direction
#define AIN2_GEAR  1 //Direction

//Motor B
#define PWMB_GEAR  1 //Speed control
#define BIN1_GEAR  1 //Direction
#define BIN2_GEAR  1 //Direction
//-------------------------------------------------------

void motorDrive    (int select, int speed, int orientation);
void motorStop     (void);
void motorMaxSpeed (int orientation);

void setup() {
  pinMode(STBY_GEAR, OUTPUT);  // Must be added in setup

  pinMode(PWMA_GEAR, OUTPUT);
  pinMode(AIN1_GEAR, OUTPUT);
  pinMode(AIN2_GEAR, OUTPUT);

  pinMode(PWMB_GEAR, OUTPUT);
  pinMode(BIN1_GEAR, OUTPUT);
  pinMode(BIN2_GEAR, OUTPUT);
}

void loop(){
  
}

void motorDrive(int select, int speed, int orientation){
/* Run specific motor at speed and orientation
 * select      : 0 for both, 1 for A and 2 for B
 * speed       : 0 is off, and 255 is full speed
 * orientation : 0 clockwise, 1 counter-clockwise
 */
  
  digitalWrite(STBY_GEAR, HIGH); // Ensure not to be on standby

  boolean direc1 = LOW;
  boolean direc2 = HIGH;

  if(orientation == CCW){
    direc1 = HIGH;
    direc2 = LOW;
  }
  
  if(select == BOTH){
    digitalWrite(AIN1_GEAR, direc1);
    digitalWrite(AIN2_GEAR, direc2);
    digitalWrite(BIN1_GEAR, direc2);  // Assuming both motors are
    digitalWrite(BIN2_GEAR, direc1);  // in opposition on robot
    digitalWrite(PWMA_GEAR, speed);
    digitalWrite(PWMB_GEAR, speed);
  }
  else if(select == MOTOR_A){
    digitalWrite(AIN1_GEAR, direc1);
    digitalWrite(AIN2_GEAR, direc2);
    digitalWrite(PWMA_GEAR, speed);
  }
  else{
    digitalWrite(BIN1_GEAR, direc1);
    digitalWrite(BIN2_GEAR, direc2);
    digitalWrite(PWMB_GEAR, speed);
  }
}

void motorStop(void){
/* Stops all motor activity and sets them
 * on standby
 */
 
  digitalWrite(STBY_GEAR, LOW);
  digitalWrite(AIN1_GEAR, LOW);
  digitalWrite(AIN2_GEAR, LOW);
  digitalWrite(BIN1_GEAR, LOW);
  digitalWrite(BIN2_GEAR, LOW);
  digitalWrite(PWMA_GEAR, STOP);
  digitalWrite(PWMB_GEAR, STOP);
}

void motorMaxSpeed(int orientation){
/* Sets both motors at maximum speed
 * orientation : 0 forward, 1 backward
 */

 digitalWrite(STBY_GEAR, HIGH); // Ensure not to be on standby

  boolean direc1 = LOW;
  boolean direc2 = HIGH;

  if(orientation == BACKWARD){
    direc1 = HIGH;
    direc2 = LOW;
  }

  digitalWrite(AIN1_GEAR, direc1);
  digitalWrite(AIN2_GEAR, direc2);
  digitalWrite(BIN1_GEAR, direc2);
  digitalWrite(BIN2_GEAR, direc1);
  digitalWrite(PWMA_GEAR, MAX_SPEED);    // Max speed is 255
  digitalWrite(PWMB_GEAR, MAX_SPEED);
}

