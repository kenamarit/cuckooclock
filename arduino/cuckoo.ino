#include <Stepper.h>
#include <Bridge.h>
#include <HttpClient.h>
#include <Process.h>

 
const int stepsPerRevolution = 400;  // change this to fit the number of steps per revolution
                                     // for your motor
boolean cuckooStarts = true;
int buttonState = 1;
const int buttonPin = 3;
// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 8,9,10,11); 

Process date;                 // process used to get the date
int hours, minutes, seconds;  // for the results
int lastSecond = -1;          // need an impossible value for comparison

void setup() {
  // set the speed at 60 rpm:
  
  pinMode(13, OUTPUT);
  
  digitalWrite(13, LOW);
  Bridge.begin();
  digitalWrite(13, HIGH);
  myStepper.setSpeed(20);
  // initialize the serial port:
  Serial.begin(9600);
  
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT_PULLUP);
  
  if (!date.running()) {
    date.begin("date");
    date.addParameter("+%T");
    date.run();
  }
}
 
void loop() {
  
  // ALL THIS IS ONLY CHECKED ONCE EVERY HOUR:
    
  if (lastSecond != seconds) { // if a second has passed
    Serial.print( hours );
    Serial.print( ", " );
    Serial.print( minutes );
    Serial.print( ", ");
    Serial.println( seconds );
    
    // restart the date process:
    if (!date.running()) {
      date.begin("date");
      date.addParameter("+%T");
      date.run();
    }
  }
  
   //if there's a result from the date process, parse it:
  while (date.available() > 0) {
    // get the result of the date process (should be hh:mm:ss):
    String timeString = date.readString();

    // find the colons:
    int firstColon = timeString.indexOf(":");
    int secondColon = timeString.lastIndexOf(":");

    // get the substrings for hour, minute second:
    String hourString = timeString.substring(0, firstColon);
    String minString = timeString.substring(firstColon + 1, secondColon);
    String secString = timeString.substring(secondColon + 1);

    // convert to ints,saving the previous second:
    hours = hourString.toInt();
    minutes = minString.toInt();
    lastSecond = seconds;          // save to do a time comparison
    seconds = secString.toInt();
  }
  
  HttpClient godMode;
  godMode.get("http://cuckoo-itp.herokuapp.com/godmode");
  if( godMode.available() ) {
    char g = godMode.peek();
    if( g == 't' ) {
      cuckooStarts = true;
    }
  }

  if( minutes == 0 && seconds == 0 ) {

      // step one revolution  in one direction:

        HttpClient clientHour;
        HttpClient clientDay;
        HttpClient clientTime;
        
        clientHour.get("http://cuckoo-itp.herokuapp.com/hour");
        
        if (clientHour.available()) {
          
          char h = clientHour.peek();
          //Serial.println(h);
          
              if(h == 't'){
                cuckooStarts = true;
              }
              
              else if(h == 'f'){
                
                cuckooStarts = false;
                
                Serial.flush();
                clientDay.get("http://cuckoo-itp.herokuapp.com/day");
                
                char d = clientDay.peek();
                
                  if (d == 't'){
                    Serial.flush();
                   
                    clientTime.get("http://cuckoo-itp.herokuapp.com/time");
                    
                    char t = clientTime.peek();
                    if(t == 't'){
                        cuckooStarts = true;
                    }
                  }
              }   
        }
        Serial.flush();
    }
    
     if( cuckooStarts == true ){
            
             Serial.println("clockwise");
             myStepper.step(stepsPerRevolution);
             delay(100);
             
             buttonState = digitalRead(buttonPin);
              Serial.println(buttonState);
              if( buttonState == 0 ) {
                cuckooStarts = false;
              }
         
           // step one revolution in the other direction:
             Serial.println("counterclockwise");
             myStepper.step(-stepsPerRevolution);
             delay(100);
             
      }
    
    // BUTTON CODE
    // read the state of the pushbutton value:
    
    buttonState = digitalRead(buttonPin);
    Serial.println(buttonState);
    if( buttonState == 0 ) {
      cuckooStarts = false;
    }
   
}


    
   

