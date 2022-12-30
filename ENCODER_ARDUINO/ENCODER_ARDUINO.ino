long temp, counter = 0; //This variable will increase or decrease depending on the rotation of encoder
int Dir,Signal,encoder_pos,target,PWM;

uint8_t r = 10;
uint8_t l = 11;
float preT = 0;
float preErr = 0;
float errI = 0; 
float currentT = 0;
float deltaT = 0;
float err = 0;
float der = 0;
 

float kp = 2;
float ki = 0.00005;
float kd = 0.01;

void setup() {
  Serial.begin (9600);
 // internal pullup input pin 2 
  
  pinMode(3, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);// internalเป็น pullup input pin 3
//Setting up interrupt
  pinMode(r, OUTPUT);
  pinMode(l, OUTPUT);
  //A rising pulse from encodenren activated ai0(). AttachInterrupt 0 is DigitalPin nr 2 on moust Arduino.
  attachInterrupt(0, ai0, RISING);
 
   
  //B rising pulse from encodenren activated ai1(). AttachInterrupt 1 is DigitalPin nr 3 on moust Arduino.

  }

  void PID(){
  currentT = micros();
  deltaT = (currentT - preT) / 1000000.0;
  preT = currentT;
  err = target - counter;
  der = (err - preErr) / deltaT;
  errI += (err * deltaT);
  Signal = (kp * err) + (kd * der) + (ki * errI);
  preErr = err;
}
   
  void loop() {
  // Send the value of counter
 if (Serial.available()>0){
  char c = Serial.read();
  if (c == 'a')
  {
    target= 1000;
    }
   if (c == 'b')
  {
    target= 2000 ;
    }
  if (c == 'c')
  {
    target= 0 ;
    }
   if (c == 'd')
  {
    target= 100 ;
    }
  if (c == 'e')
  {
    target= -1000 ;
    }
     
  }
  PID();
  Drive();
  Serial.println(counter);
  }
   
  void ai0() {
  // ai0 is activated if DigitalPin nr 2 is going from LOW to HIGH
  // Check pin 3 to determine the direction
  if(digitalRead(3)==LOW) {
  counter++;
  }else{
  counter--;
  }
  }

  void ai1() {
  // ai0 is activated if DigitalPin nr 2 is going from LOW to HIGH
  // Check pin 3 to determine the direction
  if(digitalRead(2)==LOW) {
  counter--;
  }else{
  counter++;
  }
  
  
  }

  void Drive(){
   if (Signal < 0) //negative value: CCW
  {
    Dir = -1;
  }
  else if (Signal > 0) //positive: CW
  {
    Dir = 1;
  }
  else //0: STOP - this might be a bad practice when you overshoot the setpoint
  {
    Dir = 0;
  }

  PWM = (int)fabs(Signal); //PWM values cannot be negative and have to be integers
  if (PWM > 200) //fabs() = floating point absolute value
  {
    PWM = 200; //capping the PWM signal - 8 bit
  }

  if (PWM < 5 && err != 0)
  {
    PWM = 5;
  }
  if (Dir == -1) //-1 == CCW
  {
    analogWrite(r, 0);
    analogWrite(l, PWM);
  }
  else if (Dir == 1) // == 1, CW
  {
    analogWrite(r, PWM);
    analogWrite(l, 0);
  }
  else // == 0, stop/break
  {
    analogWrite(r, 0);
    analogWrite(l, 0);
    PWM = 0;
    //In this block we also shut down the motor and set the PWM to zero
  }
  //Serial.println(PWM);
}
   
