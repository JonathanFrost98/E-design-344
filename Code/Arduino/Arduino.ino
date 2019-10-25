// Serial communication setup
constexpr long serial_baud_rate = 19200;
constexpr auto serial_config = SERIAL_8E1;

// Leonardo board information
constexpr int digital_pins[] = {9, 10, 11};
constexpr int analogue_pins[] = {A0, A1, A2};

const String STUDENT_NUMBER = "21100977";

unsigned long time1 = millis(); // LED receive indicator timer
unsigned long time2 = millis(); // Debug mode timer
unsigned long time3 = millis(); // pulstrain timer
unsigned long time4 = millis(); // reset timer
unsigned long time5 = micros();
unsigned long MillisecondsUpdtime = 0; // Uptime Counter

const byte numChars = 3; // two read bytes + end-of-line
char receivedChars[numChars]; // an array to store the received data

boolean newData = false;
boolean DebugMode = false;

String BinaryString = "";
String Outputstring = "";
float Aread = 0.0;
boolean yeet = true;


void setup() {
 Serial.begin(19200,serial_config);
 pinMode(digital_pins[0], OUTPUT);
 pinMode(digital_pins[1], INPUT);
 pinMode(digital_pins[2], INPUT);
}

void loop() 
{
 ReceiveData();
 TransmitData();
 DebugCheck();
 
 
 // LED receive notifier timeout
 if(millis()-time1 > 200) {
    digitalWrite(LED_BUILTIN, LOW);  
    time1 = millis(); }

 if(millis()-time4 > 500){
  digitalWrite(digital_pins[2], LOW);
  time4 = millis();
 }
 if(micros()-time5 > 100){
  if(yeet == true){
    digitalWrite(digital_pins[0], HIGH);
    yeet = false;
  } else {
    digitalWrite(digital_pins[0], LOW);
    yeet = true;
  }
  time5 = micros();
 }
//analogWrite(9,128);
 

}

void ReceiveData() {
  
 char Read_end = '\n';
 static byte rdx = 0;
 char ReadCharacter;
 

 while (Serial.available() > 0 && newData == false) {
  
  ReadCharacter = Serial.read();
 
  digitalWrite(LED_BUILTIN, HIGH); 
 
  if (ReadCharacter != Read_end) {
    receivedChars[rdx] = ReadCharacter;
    rdx++;
    if (rdx >= numChars) {
      rdx = numChars - 1;
    }
  }
  else {
    receivedChars[rdx] = '\0'; // terminate the string
    rdx = 0;
    newData = true;
    }
  }
}

void TransmitData(){
  if (newData == true){
    if (receivedChars[0] == '0' && DebugMode == false){
        Serial.println(receivedChars[0]);
        Serial.println(STUDENT_NUMBER);
    }
    if (receivedChars[0] == '1' && DebugMode == false){
     Serial.println(receivedChars[0]);
     Serial.println(receivedChars[1]);
     double foo=0;
		 //Here you can apply calibration as necessary. 
		 switch (receivedChars[1]) {
			 case '0' : 
			  Aread = analogRead(analogue_pins[0]);
        foo = (Aread/1023)*5;
			 break; // voltage transducer
       case '1' :
        Aread = analogRead(analogue_pins[1]);
        foo = (double)(Aread/1023)*5 + 0.02;
       break; // phase transducer
			 case '2' : 
			  Aread = analogRead(analogue_pins[2]);
        foo = (double)(Aread/1023)*5 + 0.03;
			 break; // current transducer
     }
  	Serial.println(foo);
    }
    if (receivedChars[0] == '2' && DebugMode == false){
      Serial.println(receivedChars[0]);
      Serial.println(receivedChars[1]);
		switch (receivedChars[1]) {
			case '0' : Serial.println(digitalRead(digital_pins[0])); break;
      case '1' : Serial.println(digitalRead(digital_pins[1])); break;
			case '2' : Serial.println(digitalRead(digital_pins[2])); break;
    }
  }
 	if (receivedChars[0] == 'x' || receivedChars[0] == 'X'){
         if (receivedChars[1] == '0'){
            DebugMode = false;
         }
         else if (receivedChars[1] == '1'){
            DebugMode = true;
         }
    }
       if (receivedChars[0] == 'R' ){
          time4 = millis();
          digitalWrite(digital_pins[2], HIGH);
       }

    

   if ((receivedChars[0] == 'U') && DebugMode == false) // Return uptime if '?' is received
     {
       MillisecondsUpdtime=millis(); 
       uptime(); 
     }    
 }
  newData = false;
}

void DebugCheck(){
  if (DebugMode == true){
          String DebugOutput = "";
          double Aread0 = analogRead(analogue_pins[0]);
          
          double Aread1 = ((analogRead(analogue_pins[1])/1023)*5 + 0.02);
          
          double Aread2 = (analogRead(analogue_pins[2])*3.3)/4;
          
          DebugOutput = STUDENT_NUMBER + ',' + "Voltage:" + (((Aread0/1023)*5 + 0.4)*7.1) + "V," + "Phase:" + Aread1 + "deg.," + "Current:" + Aread2 + 
          "mA," + ',' + "Reset:" + ReturnDigitalRead(digitalRead(digital_pins[2]));
          if(millis()-time2 > 2000) { // LED receive notifier timeout 
            Serial.println(DebugOutput);
            time2 = millis(); }
       }
}

String ReturnDigitalRead(int Input){
  if (Input == 0){
    return "LOW";
  }
  else {
    return "HIGH";
  }
}



void uptime()
{
 long days=0;
 long hours=0;
 long mins=0;
 long secs=0;
 secs = MillisecondsUpdtime/1000; //convect milliseconds to seconds
 mins=secs/60; //convert seconds to minutes
 hours=mins/60; //convert minutes to hours
 days=hours/24; //convert hours to days
 secs=secs-(mins*60); //subtract the coverted seconds to minutes in order to display 59 secs max
 mins=mins-(hours*60); //subtract the coverted minutes to hours in order to display 59 minutes max
 hours=hours-(days*24); //subtract the coverted hours to days in order to display 23 hours max
 //Display results
 Serial.print("Uptime : ");
 Serial.print(hours);
 Serial.print(":");
 Serial.print(mins);
 Serial.print(":");
 Serial.println(secs);
}
 
