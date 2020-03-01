#include <IRremote.h>

int button = 13;
int rback = 10;
int rfwd = 5;
int lfwd = 6;
int lback = 9;

int s5 = 5;
int s1 = 1;
int s2 = 2;
int s3 = 3;
int s4 = 4;
//sensors go from 1 to 5 inclusively right to left (mux 1 leftmost)

int start = -1;
int IRpin = 8;
int s1v,s2v,s3v,s4v,s5v;
bool sb[5],left,center,right,readOk;
IRrecv irrecv(IRpin);
decode_results results;

int maxSpeed = 140;
int sv[5], coef;
float weights[]={maxSpeed*0.6,maxSpeed*0.3,0,maxSpeed*0.3,maxSpeed*0.6};
float outCoef = 0.9;

void setup()
{
  pinMode(button,INPUT_PULLUP);
  //Serial.begin(9600);
  if(digitalRead(button)==LOW)
    delay(100000);
  irrecv.enableIRIn(); // Start the IR receiver
  delay(2000);
}

void loop() 
{
  if (irrecv.decode(&results)) 
  {
    //Serial.println(results.value);
    //3772793023 phone
    //4294967295 remote
    if(results.value == 3772793023 || results.value == 4294967295)
      Stop();
    irrecv.resume();   // Receive the next value IR
  }

  ReadSensors();
  Turn(left,center,right,readOk,false);

  //PrintDir();
}

void Stop()
{
  Turn(false,false,false,false,true);
  delay(60000);
}

void ReadSensors()
{
  sv[0]=analogRead(s1);
  sv[4]=analogRead(s5);
  sv[1]=analogRead(s2);
  sv[3]=analogRead(s4);
  sv[2]=analogRead(s3);
  for(int i=0;i<5;i++)
  {
    if(sv[i]<500)
    //daca e false, e inversat////////////////////////////
      sb[i]=true;
    else
      sb[i]=false;
  }
  //PrintSAnalog();
  if(sb[4] || sb[3])
  {
    coef=0;
    left=true;
    readOk=true;
    right=false;
    center=false;
    if(sb[4])
    {
      coef+=weights[4];
      //Serial.println(coef);
      //Serial.println("Hard left!");
    }
    if(sb[3])
    {
      coef+=weights[3];
      //Serial.println(coef);
      //Serial.println("Left!");
    }
  }
  if(sb[1] || sb[0])
  {
    coef=0;
    left=false;
    right=true;
    readOk=true;
    center=false;
    if(sb[1])
    {
      coef+=weights[1];
      //Serial.println(coef);
      //Serial.println("Right!");
    }
    if(sb[0])
    {
      coef+=weights[0];
      //Serial.println(coef);
      //Serial.println("Hard right!");
    }
  }
  if(coef==0 && sb[2])
  {
    center=true;
    readOk=true;
    left=false;
    right=false;
    //Serial.println("Forward!");
  }
  //nothing detected! !!!DANGER OF STOPPING!!
  if(!sb[0] && !sb[1] && !sb[2] && !sb[3] && !sb[4])
    readOk=false;
}

void Turn(bool left, bool center, bool right, bool ok, bool stp)
{
  if(!stp)
  {
    int ind = maxSpeed-coef;
    if(ind < 0)
      ind=0;
    if(left && !center)
    {
      if(ok)
      {
        //analogWrite(lback,0);
        analogWrite(lfwd,ind);
        analogWrite(rfwd,maxSpeed);
      }
      else
      {
        analogWrite(lfwd,outCoef);
        //analogWrite(lback,maxSpeed/2.5);
        analogWrite(rfwd,maxSpeed);
      }
    }
    if(right && !center)
    {
      if(ok)
      {
        analogWrite(lfwd,maxSpeed);
        //analogWrite(rback,0);
        analogWrite(rfwd,ind);
      }
      else
      {
        analogWrite(lfwd,maxSpeed);
        analogWrite(rfwd,outCoef);
        //analogWrite(rback,maxSpeed/2.5);
      }
    }
    if(center)
    {
      analogWrite(lback,0);
      analogWrite(rback,0);
      analogWrite(lfwd,maxSpeed);
      analogWrite(rfwd,maxSpeed);
    }
  }
  else
  {
    analogWrite(lfwd,0);
    analogWrite(rfwd,0);
    analogWrite(lback,0);
    analogWrite(rback,0);
  }
}

void PrintDir()
{
  Serial.print(left);
  Serial.print(" ");
  Serial.print(center);
  Serial.print(" ");
  Serial.print(right);
  Serial.println();
}

void PrintSDigital()
{
  Serial.print(sb[4]);
  Serial.print(" ");
  Serial.print(sb[3]);
  Serial.print(" ");
  Serial.print(sb[2]);
  Serial.print(" ");
  Serial.print(sb[1]);
  Serial.print(" ");
  Serial.print(sb[0]);
  Serial.println();
}

void PrintSAnalog()
{
  Serial.print(sv[4]);
  Serial.print(" ");
  Serial.print(sv[3]);
  Serial.print(" ");
  Serial.print(sv[2]);
  Serial.print(" ");
  Serial.print(sv[1]);
  Serial.print(" ");
  Serial.print(sv[0]);
  Serial.println();
}
