#include <Servo.h> 
#include <Metro.h>
Metro measureDistance = Metro(50);
Metro sweepServo = Metro(20);

unsigned long actualDistance = 0;

Servo myservo;  // create servo object to control a servo 
int pos = 60; 
int sweepFlag = 1;


int URPWM = 3; // PWM Output 0－25000US，Every 50US represent 1cm
int URTRIG= 10; // PWM trigger pin
uint8_t EnPwmCmd[4]={0x44,0x02,0xbb,0x01};    // distance measure command

int m_NumObstacles = 0;
int m_LastDistanceAverage = 0;

// motor control 
//Standard PWM DC control
int E1 = 5;     //M1 Speed Control
int E2 = 6;     //M2 Speed Control
int M1 = 4;    //M1 Direction Control
int M2 = 7;    //M1 Direction Control
 
///For previous Romeo, please use these pins.
//int E1 = 6;     //M1 Speed Control
//int E2 = 9;     //M2 Speed Control
//int M1 = 7;    //M1 Direction Control
//int M2 = 8;    //M1 Direction Control


void setup(){                                 // Serial initialization
  myservo.attach(9); 
  Serial.begin(115200);                         // Sets the baud rate to 9600
  SensorSetup();
  MotorSetup();
}
 
void loop(){
  if (m_NumObstacles == 0)
  {      
    advance (100,100);   //move forward in max speed
  }
  
  if(measureDistance.check() == 1)
  {
    actualDistance = MeasureDistance();
    m_LastDistanceAverage = (m_LastDistanceAverage * 4 + actualDistance) / 5;
    if (m_LastDistanceAverage < 50)
    {
      back_off (255,255);   //move back in max speed
      delay(2000);
      turn_R (100,100);
      delay(2000);
      advance (100,100);   //move forward in max speed
      
      if (m_NumObstacles++ > 2)
        stop();
    }
    
    Serial.println(actualDistance);
    delay(100);
  }

 /*
  if(sweepServo.check() == 1){
    servoSweep();
  }
 */
}

void SensorSetup(){ 
  pinMode(URTRIG,OUTPUT);                     // A low pull on pin COMP/TRIG
  digitalWrite(URTRIG,HIGH);                  // Set to HIGH 
  pinMode(URPWM, INPUT);                      // Sending Enable PWM mode command
  /*
  for(int i=0;i<4;i++){
      Serial.write(EnPwmCmd[i]);
   }
   */ 
}

int MeasureDistance(){        // a low pull on pin COMP/TRIG  triggering a sensor reading
    digitalWrite(URTRIG, LOW);
    digitalWrite(URTRIG, HIGH);               // reading Pin PWM will output pulses    
    unsigned long distance=pulseIn(URPWM,LOW);    
//    if(distance==50000){              // the reading is invalid.
//      Serial.print("Invalid");    
//    }else{
      distance=distance/50;           // every 50us low level stands for 1cm
//    }
    return distance;
}

void servoSweep(){
  if(sweepFlag ){  
     if(pos>=60 && pos<=120){                   
        pos=pos+1;                                  // in steps of 1 degree 
        myservo.write(pos);                         // tell servo to go to position in variable 'pos' 
    }
      if(pos>119)  sweepFlag = false;                       // assign the variable again
  }else {                                       
      if(pos>=60 && pos<=120){    
        pos=pos-1;
        myservo.write(pos);
      }
      if(pos<61)  sweepFlag = true;
   }
}

/////////////////////////////////////////////////////////////////////////////////////
// Motor control
///////////////////////////////////////////////////////////////////////////////////// 
 
void stop(void)                    //Stop
{
  Serial.println("stop");
  digitalWrite(E1,LOW);   
  digitalWrite(E2,LOW);      
}   
void advance(char a,char b)          //Move forward
{
  Serial.println("FWD");
  analogWrite (E1,a);      //PWM Speed Control
  digitalWrite(M1,HIGH);    
  analogWrite (E2,b);    
  digitalWrite(M2,HIGH);
}  
void back_off (char a,char b)          //Move backward
{
  Serial.println("Back");
  analogWrite (E1,a);
  digitalWrite(M1,LOW);   
  analogWrite (E2,b);    
  digitalWrite(M2,LOW);
}
void turn_L (char a,char b)             //Turn Left
{
  Serial.println("Turn left");
  analogWrite (E1,a);
  digitalWrite(M1,LOW);    
  analogWrite (E2,b);    
  digitalWrite(M2,HIGH);
}
void turn_R (char a,char b)             //Turn Right
{
  Serial.println("Turn right");
  analogWrite (E1,a);
  digitalWrite(M1,HIGH);    
  analogWrite (E2,b);    
  digitalWrite(M2,LOW);
}
void MotorSetup(void) 
{ 
  int i;
  for(i=4;i<=7;i++)
    pinMode(i, OUTPUT);  
  //Serial.begin(115200);      //Set Baud Rate
  //Serial.println("Run keyboard control");
} 

