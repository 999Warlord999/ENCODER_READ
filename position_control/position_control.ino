int Dir,Signal,encoder_pos,target,PWM;


float kp = 1.35;
float ki = 0.00005;
float kd = 0.01;

float preT = 0;
float preErr = 0;
float errI = 0; 
float currentT = 0;
float deltaT = 0;
float err = 0;
float der = 0;

const int r = 11,l = 10;


void Encoder_read(){
  
  if(digitalRead(3)==1){
    encoder_pos++;
  }else{
    encoder_pos--;
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
  if (PWM > 255) //fabs() = floating point absolute value
  {
    PWM = 255; //capping the PWM signal - 8 bit
  }

  if (PWM < 30 && err != 0)
  {
    PWM = 30;
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
}
void PID(){
  currentT = micros();
  deltaT = (currentT - preT) / 4000000.0;
  preT = currentT;
  err = target - encoder_pos;
  der = (err - preErr) / deltaT;
  errI += (err * deltaT);
  Signal = (kp * err) + (kd * der) + (ki * errI);

  preErr = err;
}

void setup() {
  // put your setup code here, to run once:
digitalWrite(r,OUTPUT);
digitalWrite(l,OUTPUT);
digitalWrite(3,INPUT_PULLUP);
digitalWrite(2,INPUT_PULLUP);


attachInterrupt(0,Encoder_read,RISING);
Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()){
    char c =Serial.read();
    if (c == 'a'){
      target = 0;
    }
    if (c == 'b'){
      target = 100;
    }
    if (c == 'c'){
      target = 200;
    }
  }
  PID();
  Drive();
  Serial.println(encoder_pos);
  
}
