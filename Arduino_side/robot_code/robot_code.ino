// include the librareis
#include <MoveRobot.h>
#include <LM35.h>
#include <HCSR04.h>
#include <Speaker.h>

const int sensor[7] = {A0, A1, A2, A3, A4, 3, 4}; // pin of IR sensors
// definetion led pin
#define led 2
// definetion HC-SR04 pins
#define trigPin 7
#define echo 6

const char n = 20; //nD = 10; // n is length of temperature array
int tmp_avrg[n];   // temperature array to save the temperature values and avrageing 
//int d_avrg[nD] ;
//D_output = 0;

MoveRobot Move;  // Create an object as MoveRobot to control the movement of robot
Speaker speaker; // Create an object as a Speaker for sound effects
SR04 sr04;       // Create an object as SR04 sensor on to sensing distance from obstacles
LM35 lm35 = new LM35(A5); // Create an object as LM35 sensor on A5 pin to sensing temperature

void setup() {
  Serial.begin(9600); 
  Move.init(8, 9, 10, 13, 12, 11);
  lm35.init(A5);
  sr04.init(trigPin, echo);
  speaker.init(5);

  pinMode(led, OUTPUT);
  //Wellcom
  digitalWrite(led, HIGH);
  speaker.play(MOBILE);
  digitalWrite(led, LOW);
  
  for(int i=0; i<7; i++){ 
    pinMode(sensor[i], INPUT); 
  } /// Sensors pin mode ///

  // Initialization temperature array
  for(int i=0; i<n; i++) { tmp_avrg[i] = lm35.getTemperatureI(); }
  //for(int i=0; i<n; i++) { temp += tmp_avrg[i]; }
  //temp /= n;
  //for(int i=0; i<nD; i++){ d_avrg[i] = distance.GetI(); }
  //for(int i=0; i<nD; i++){ D_output+=d_avrg[i]; }
  //D_output /= nD;
}

int speedInput = 0, statusInput = 0, iT = 0; //iD = 0;
char chMode;
bool checkEndMove = false, isConnected = false, checkSMS = false, isMoveStarated = false;

void loop() {
  if(isConnected == false){
    if(Serial.available()>0){
      //isConnected = (Serial.read() == 'c')? true:false;
      char ch = Serial.read();
      if(ch == 'c'){ // 'c' --> connect
        isConnected = true;
        speaker.play(MELODY); // --> show connected
      }
    }
  }
  else {
    if(Serial.available()) { chMode = Serial.read();} // get mode from Android
    
    Serial.println(getAvgTemperature()); // Send temperature to Android
    delay(10);
    Serial.println(sr04.getCmInt());     // Send distance to Android
    delay(20);
    
    // Computing temperature, distance and averaging
    //if(iD >= nD){ iD = 0; }
    //D_output = 0;
    //d_avrg[iD] = distance.GetI();
    //for(int i=0; i<nD; i++){ D_output+=d_avrg[i]; }
    //D_output /= nD;
    //iD++;
    // End averaging

    switch(chMode){
      case 'm': { // 'm' --> move
        if(Serial.available()){
          isMoveStarated = true;
          speedInput = map(Serial.parseInt(), 0, 100, 0, 240); // get speed motors in (0 ~ 100) and turn to (0 ~  255)
          statusInput = Serial.parseInt(); // get status move (0 --> front , 1 --> back , 2 --> left , 3 --> right)
        }
      } break;
      case 'e': { Move.Stop(); checkSMS=false; isMoveStarated = false; } break; // 'e' --> end move
      case 's': { checkSMS=true;               } break; // 's' --> start smart move
      case 'b': { speaker.damage();chMode=='e';} break; // 'b' --> buzer
      case 'o': { digitalWrite(led, HIGH);     } break; // 'o' --> On led
      case 'f': { digitalWrite(led, LOW) ;     } break; // 'f' --> off led
      case 'd': { isConnected = false;         } break; // 'd' --> disconnect
      default: break;
    }

    if(isMoveStarated) { 
      switch(statusInput) { // status move --> 0:move forward | 1:move backward | 2:move left | 3:move right | 4:brake
        case 0: { /*if(check_d){*/Move.forward(speedInput, speedInput); checkEndMove = false;/*}*/ break; }
        case 1: { Move.backward(speedInput, speedInput); checkEndMove = false; break; }
        case 2: { Move.left(speedInput);  checkEndMove = false; break; }
        case 3: { Move.right(speedInput); checkEndMove = false; break; }
        //case 4: { Move.Stop(); checkEndMove = true; break; }
      } 
    }
    
    if (checkSMS){ masiryab_mode(); }
    //masiryabPrintData();
  }
}
//=============================================================================================================================
void masiryab_mode()
{
//  if(distance.getI() < 10){
//    Move.Stop();
//    speak.damage();
//    //delay(200);
//  }
//  else {
    if(digitalRead(sensor[3]) == 0)      { Move.forward (190, 190);      }
    else if(digitalRead(sensor[2]) == 0) { Move.left (255); }
    else if(digitalRead(sensor[4]) == 0) { Move.right(255); }
    else if(digitalRead(sensor[1]) == 0) { Move.left_reverse (230, 230); }
    else if(digitalRead(sensor[5]) == 0) { Move.right_reverse(230, 230); }
    else if(digitalRead(sensor[0]) == 0) { Move.left_reverse (255, 255); }
    else if(digitalRead(sensor[6]) == 0) { Move.right_reverse(255, 255); }
    else { Move.forward (140, 140); } // Continue to find the line at low speed
//  }
} //-----------------------------------------------------------------------------

int getAvgTemperature(){
  if(iT >= n) { iT = 0; } // reset iT to 0 if iT >= n
  tmp_avrg[iT] = lm35.getTemperatureI(); // get new temperature now
  unsigned long int temp = 0; // Define the variable sum of numbers
  for(int i=0; i<n; i++) { temp += tmp_avrg[i]; } // Calculate the sum of numbers
  iT++;
  return temp/n; // Calculate average temperatures and return it
}

void masiryabPrintData()
{
  // print the sensor data in serial monitor
  for(int i=0; i<7; i++){Serial.print(digitalRead(sensor[i])); Serial.print("\t");}
  Serial.println("");
} //------------------------------------------------------------------------------
