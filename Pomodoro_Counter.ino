//PROJECT: POMODORO COUNTER;
//AUTHOR : DOREEN CHEMWENO;
//CHANNEL: TECHDICTED - https://www.youtube.com/channel/UCCKPQdhTAAf1WR0rftGgWPQ

/*
 * THE 2-DIGIT 7-SEGMENT DISPLAY
                           1               1*
                       2       3      2*        3*
                           4               4*
                       5       6      5*        6*
                           7               7*

* Led position in matrix - {i,j} 
1={2,3}  ,2={2,0}  ,3={2,2}  ,4={2,1}  ,5={1,2}  ,6={1,3}  ,7={1,0}  
1*={3,3}  ,2*={3,0}  ,3*={3,2}  ,4*={3,1}  ,5*={0,2}  ,6*={0,3}  ,7*={0,0}  
S={0,1}

available={1,1}
*/

#define latchPin A1
#define clockPin A0
#define dataPin A2

#define buttonPin 10
#define settingStatus 11
#define runningStatus 9

int buttonPushCounter = 0;
int buttonState = 0;
int lastButtonState = 0;

//Time
unsigned long interval=1000; //time to wait
unsigned long previousMillis=0; //millis() returns an unsigned long


//looping variables
byte i;
byte j;
//storage variable
byte dataToSend;

int count=9;


void numberMatrix(int n,int pos){
  //Each segment in an array
  int seg0[][2]={{2,3},{2,0},{2,2},{2,1},{1,2},{1,3},{1,0}}; 
  int seg1[][2]={{3,3},{3,0},{3,2},{3,1},{0,2},{0,3},{0,0}};
  
  //Array of numbers - segments needed to draw each number;
  int numbers[][7]={
    {1,2,3,5,6,7},
    {3,6},
    {1,3,4,5,7},
    {1,3,4,6,7},
    {2,4,3,6},
    {1,2,4,6,7},
    {1,2,5,7,6,4},
    {1,3,6},
    {1,2,3,4,5,6,7},
    {1,2,3,4,6,7}
    };
  
  /* Example
  int one[4][4]={
  {0,0,0,0},
  {0,0,0,1},
  {0,0,1,0},
  {0,0,0,0}
  };
  */
  
  int nMatrix[4][4]={0};

  for(int i=0; i<7; i++){
    if(numbers[n][i]!=0){
      if(pos==0){
        nMatrix[seg0[numbers[n][i]-1][0]][seg0[numbers[n][i]-1][1]]=1;
      }else if(pos==1){
        nMatrix[seg1[numbers[n][i]-1][0]][seg1[numbers[n][i]-1][1]]=1;
        }
    }else{
      break;
    }
  }

//
//  for(int i=0; i<4; i++){
//    for(int j=0; j<4; j++){
//      Serial.print(nMatrix[i][j]);
//      Serial.print(", ");
//  }
//  Serial.println(" ");
//  }

  drawNumber(nMatrix);
}

void drawNumber(int arr[4][4]){
  for (i=0;i<4;i++){
    for (j=0;j<4;j++){
     //To learn about bit manipulation, check this out:- https://playground.arduino.cc/Code/BitMath/
     if(arr[i][j]==1){
       dataToSend = (1 << (i+4)) | (15 &~(1 << j));
       //setlatch pin low so the LEDs don't change while sending in bits
       digitalWrite(latchPin, LOW);
       // shift out the bits of dataToSend to the 74HC595
       shiftOut(dataPin, clockPin, LSBFIRST, dataToSend);
       //set latch pin high- this sends data to outputs so the LEDs will light up
       digitalWrite(latchPin, HIGH);
     }
    }
  }
}


void ledTest(){ //test all LEDs to ensure they work properly
  for (i=0;i<4;i++){
    for (j=0;j<4;j++){
     //To learn about bit manipulation, check this out:- https://playground.arduino.cc/Code/BitMath/
     dataToSend = (1 << (i+4)) | (15 &~(1 << j));
     //setlatch pin low so the LEDs don't change while sending in bits
     digitalWrite(latchPin, LOW);
     // shift out the bits of dataToSend to the 74HC595
     shiftOut(dataPin, clockPin, LSBFIRST, dataToSend);
     //set latch pin high- this sends data to outputs so the LEDs will light up
     digitalWrite(latchPin, HIGH);
    }
  }
}

void displayNumber(int number){
  if(number<10){
    numberMatrix(0,0);
    numberMatrix(number,1);
  }else{
     numberMatrix(number/10,0);
     numberMatrix(number%10,1); 
  }
}

void setup() {
 Serial.begin(9600);
 //set pins as output
 pinMode(latchPin, OUTPUT);
 pinMode(clockPin, OUTPUT);
 pinMode(dataPin, OUTPUT);

 pinMode(buttonPin, INPUT);
 pinMode(settingStatus, OUTPUT);
 pinMode(runningStatus, OUTPUT);
}
void loop() {
unsigned long currentMillis=millis();

displayNumber(0);

buttonState = digitalRead(buttonPin);
// compare the buttonState to its previous state
  if (buttonState != lastButtonState) {
    // if the state has changed, increment the counter
    if (buttonState == HIGH) {
      // if the current state is HIGH then the button went from off to on:
      buttonPushCounter+=15;
      if(buttonPushCounter>60){
        buttonPushCounter=0;
      }
      while((unsigned long)(currentMillis - previousMillis) < 2000){
        currentMillis=millis();
        displayNumber(buttonPushCounter);
        digitalWrite(settingStatus,HIGH);
      }
      digitalWrite(settingStatus,LOW);
      
//      if ((unsigned long)(currentMillis - previousMillis) >= 500) {
//        
//        previousMillis = millis();
//      }
      
      Serial.print("SELECTION: ");
      Serial.println(buttonPushCounter);
      previousMillis = millis();
    } else {
      // if the current state is LOW then the button went from on to off:
      Serial.println("off");
    }
    // Delay a little bit to avoid bouncing
    delay(50);
    
  } else if(buttonState == lastButtonState && buttonPushCounter!=0 && (unsigned long)(currentMillis - previousMillis) >= 8000){
      
      digitalWrite(runningStatus,HIGH);
      displayNumber(buttonPushCounter);
      while(buttonPushCounter>0){
        currentMillis=millis();
        displayNumber(buttonPushCounter);
        if ((unsigned long)(currentMillis - previousMillis) >= interval) {
          buttonPushCounter--;
          previousMillis = millis();
        }
      }
      digitalWrite(runningStatus,LOW);
  }

  // save the current state as the last state, for next time through the loop
  lastButtonState = buttonState;
  
}
