/*
Written February 2023, Winnie Szeto

For 2.007's gameboard.

Inputs: two single-channel encoders 
Output: 4-digit display on a 74HC595 shift register,
        2 Servos to control ball release
###################################################################
Initialize all variables associated with counting the encoder ticks
###################################################################
 */

#include <Servo.h>

Servo servoInner;
Servo servoOuter;

#define servoInnerPin 5
#define servoOuterPin 4

#define servoInnerEngagedPos 90
#define servoInnerDisengagedPos 130
#define servoInnerDelayms 400

#define servoOuterEngagedPos 110
#define servoOuterDisengagedPos 0
#define servoOuterDelayms 550


#define DEBUG_MODE 1

#define PI 3.14159

#define encoderOuter 2
#define encoderInner 3
//Encoders are attached to the nano's INT pins

#define EncoderTicks 20
//ticks are defined as the number of pulses/revolution

#define AveragingSamples 3

unsigned long previousEncoderTimeOuter = 0;
unsigned long previousEncoderTimeInner = 0;
//time references to calculate speed

//time references to calculate if it's been 5 seconds in a bucket
unsigned long newBucketTimeOuter = 0;
unsigned long newBucketTimeInner = 0;

int OuterAverageVelocity;
int InnerAverageVelocity;

int InnerVelocities[AveragingSamples]; 
int OuterVelocities[AveragingSamples];

int SpeedBins[] = {0,25,51,101,151,201,251};
int pointBinsInner[] = {0,5,10,20,40,47,99};
int pointBinsOuter[] = {0,3,7,15,21,26,99};

uint8_t currentBucketOuter = 0;
uint8_t currentBucketInner = 0;

uint8_t previousBucketOuter = 0;
uint8_t previousBucketInner = 0;

uint8_t maxBucketOuter = 0;
uint8_t maxBucketInner = 0;
  
/*
###################################################################
Initialize all variables associated with the shift register display
###################################################################
 */


#define isOnLeftSide 1 //whether the display is on the left or right half of the gameboard

#define MAX_BRIGHTNESS 25 //out of 255

#define SER 12
#define nOE 11
#define RCLK 10
#define SRCLK 9
#define nCLR 8


//displayData[number] will return the data to display the number
uint8_t displayData[] ={ 
0b10111011,
0b00000011,
0b00111010,
0b00011110,
0b01000110,
0b01011100,
0b00101110,
0b00110010,
0b01111110,
0b01100110
};


#define buttonUpper A1
#define buttonLower A5

/*
#######################################################
Function declaration
########################################################
 */
//Function will update Outer/Inner angular velocity if accel<maxaccel
void CalculateEncoderTimeOuter(){
    unsigned long elapsedTime = micros() - previousEncoderTimeOuter;
    previousEncoderTimeOuter = micros();
    //calculates elapsed time, resets previous time reference to new time

    //does averaging maths
    unsigned long sum = 0;
    for(int k = AveragingSamples-1; k > 0; k+= -1){
      OuterVelocities[k] = OuterVelocities[k-1]; 
      sum += OuterVelocities[k];
    }

    //add new sample to velocities
    OuterVelocities[0] = (float)1000000* 60 / EncoderTicks * 1/elapsedTime;
    sum += OuterVelocities[0];
    OuterAverageVelocity = sum / AveragingSamples;

    //convert velocity into point bucket
    previousBucketOuter = currentBucketOuter;
    for(int i = 0;i<7;i++){
      if(OuterAverageVelocity > SpeedBins[i]){
        currentBucketOuter = i;
      }
    }
    if(currentBucketOuter != previousBucketOuter){
      newBucketTimeOuter = millis();  
    }

}


void CalculateEncoderTimeInner(){
    unsigned long elapsedTime = micros() - previousEncoderTimeInner;
    previousEncoderTimeInner = micros();
    //calculates elapsed time, resets previous time reference to new time

    //does averaging maths
    unsigned long sum = 0;
    for(int k = AveragingSamples-1; k > 0; k+= -1){
      InnerVelocities[k] = InnerVelocities[k-1]; 
      sum += InnerVelocities[k];
    }

    //add new sample to velocities
    InnerVelocities[0] = (float)1000000* 60 / EncoderTicks * 1/elapsedTime;
    sum += InnerVelocities[0];
    InnerAverageVelocity = sum / AveragingSamples;

    //convert velocity into point bucket
    previousBucketInner = currentBucketInner;
    for(int i = 0;i<7;i++){
      if(InnerAverageVelocity > SpeedBins[i]){
        currentBucketInner = i;
      }
    }
    if(currentBucketInner != previousBucketInner){
      newBucketTimeInner = millis();  
    }

}


//function inputs a 4 digit number and pushes it to the display panel
void DisplayNumber(int numberToDisplay){
  int firstDigit = numberToDisplay % 10;
  int secondDigit = ((numberToDisplay % 100) - firstDigit)/10;
  int thirdDigit = ((numberToDisplay % 1000 - (secondDigit*10)) - firstDigit)/100;
  int fourthDigit = (numberToDisplay - (thirdDigit * 100) - (secondDigit * 10) - firstDigit)/1000;
  //shift data to 595
  shiftOut(SER, SRCLK, LSBFIRST, displayData[firstDigit]);
  shiftOut(SER, SRCLK, LSBFIRST, displayData[secondDigit]);
  shiftOut(SER, SRCLK, LSBFIRST, displayData[thirdDigit]);
  shiftOut(SER, SRCLK, LSBFIRST, displayData[fourthDigit]);
  //push data to storage register
  digitalWrite(RCLK, HIGH);
  delayMicroseconds(100);
  digitalWrite(RCLK, LOW);
}

void setup() {
  pinMode(buttonLower, INPUT_PULLUP);
  pinMode(buttonUpper, INPUT_PULLUP);
  
  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH);
  pinMode(encoderOuter, INPUT_PULLUP);
  pinMode(encoderInner, INPUT_PULLUP);
  previousEncoderTimeOuter = millis();
  previousEncoderTimeInner = millis();
  attachInterrupt(digitalPinToInterrupt(encoderOuter), CalculateEncoderTimeOuter, RISING);
  attachInterrupt(digitalPinToInterrupt(encoderInner), CalculateEncoderTimeInner, RISING);

  pinMode(nOE, OUTPUT);
  pinMode(nCLR, OUTPUT);
  pinMode(SER, OUTPUT);
  pinMode(RCLK, OUTPUT);
  pinMode(SRCLK, OUTPUT);
  
  digitalWrite(nCLR, HIGH);
  analogWrite(nOE, 255-MAX_BRIGHTNESS);


  servoInner.attach(servoInnerPin);
  servoOuter.attach(servoOuterPin);

  servoOuter.write(servoOuterEngagedPos);
  servoInner.write(servoInnerEngagedPos);
  if(DEBUG_MODE){
    Serial.begin(115200);
  }

}

void loop() {

  //sets velocity to 0 if no encoder ticks were detected in the past second
  if(micros() - previousEncoderTimeOuter > 1000000){
    currentBucketOuter = 0;
    if(OuterAverageVelocity > 1000){
    OuterAverageVelocity = 0;
    } 
    if(OuterAverageVelocity > 0){
    OuterAverageVelocity = OuterAverageVelocity -1;
    }  
    for(int k = 0; k < AveragingSamples; k++){
      OuterVelocities[k] = 0;
    }
  } 
  
  if(micros() - previousEncoderTimeInner > 1000000){
    currentBucketInner = 0;
    if(InnerAverageVelocity > 1000){
      InnerAverageVelocity = 0;
    }
    if(InnerAverageVelocity > 0){
      InnerAverageVelocity = InnerAverageVelocity -1;
    }
    for(int k = 0; k < AveragingSamples; k++){
      InnerVelocities[k] = 0;
    }  
  } 

  if(!digitalRead(buttonLower) || ((millis() - newBucketTimeOuter > 5000) && currentBucketOuter > 0)){ //if it's been at a bucket for 5s and the bucket is nonzero
    newBucketTimeOuter = millis();//reset 5s time reference
    if(maxBucketOuter < currentBucketOuter){ //update the max bucket if needed
      maxBucketOuter = currentBucketOuter;
    }
    
    servoOuter.write(servoOuterDisengagedPos);
    delay(servoOuterDelayms);
    servoOuter.write(servoOuterEngagedPos);   
  }

  if(!digitalRead(buttonUpper) || ((millis() - newBucketTimeInner > 5000) && currentBucketInner > 0)){ //if it's been at a bucket for 5s and the bucket is nonzero
    newBucketTimeInner = millis();//reset 5s time reference
    if(maxBucketInner < currentBucketInner){ //update the max bucket if needed
      maxBucketInner = currentBucketInner;
    }
    
    servoInner.write(servoInnerDisengagedPos);
    delay(servoInnerDelayms);
    servoInner.write(servoInnerEngagedPos);    
  }
  
  if(DEBUG_MODE){
    Serial.print("Current_bucket:");
    Serial.print(currentBucketOuter);
    Serial.print(",TimAtCurrentOuterBucket(s):");
    Serial.print((millis() - newBucketTimeOuter)/1000);
    Serial.print(",OuterAverageVelocity:");
    Serial.print(OuterAverageVelocity);
    Serial.print(",OuterPointsDisplayed:");
    Serial.print(pointBinsOuter[maxBucketOuter]);
    Serial.print(",InnerAverageVelocity:");
    Serial.print(InnerAverageVelocity);
    Serial.print(",InnerPointsDisplayed:");
    Serial.println(pointBinsInner[maxBucketInner]);
  }


  if(isOnLeftSide){
    DisplayNumber(pointBinsOuter[maxBucketOuter] * 100 + pointBinsInner[maxBucketInner]);
  }
  else{
    DisplayNumber(pointBinsInner[maxBucketInner] * 100 + pointBinsOuter[maxBucketOuter]);
  }
  
  analogWrite(nOE, 253 - (OuterAverageVelocity + InnerAverageVelocity));
  
}
