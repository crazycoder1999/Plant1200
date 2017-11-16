
#include <SigFox.h>
#include <ArduinoLowPower.h>
int LEDPIN = 6;
bool up = false;
int moisture_sensor1 = A0;
int moisture_sensor2 = A1;
int moisture_sensor3 = A2;
int moisture_output1,moisture_output2, moisture_output3;

bool iAmDebug = false; //USE THIS TO ENABLE DEBUG MODE!!
volatile int alarm_source = 0;
void setup() {
  if(iAmDebug)
    Serial.begin(9600);
  else
    LowPower.attachInterruptWakeup(RTC_ALARM_WAKEUP, alarmEvent0, CHANGE); 
  pinMode(LEDPIN,OUTPUT);

  if (!SigFox.begin()) {
    if(iAmDebug)
      Serial.println("Shield error or not present!");
    return;
  }
  
  /*
//USE THIS COMMENTED CODE IF YOU NEED TO REGISTER THE BOARD WITH SIGFOX BACKEND!
String version = SigFox.SigVersion();
  String ID = SigFox.ID();
  String PAC = SigFox.PAC();
  // Display module informations
  Serial.println("MKRFox1200 Sigfox first configuration");
  Serial.println("SigFox FW version " + version);
  Serial.println("ID  = " + ID);
  Serial.println("PAC = " + PAC);**/

//  Serial.println(SigFox.internalTemperature());
  SigFox.end();
 
}
void alarmEvent0() {
  alarm_source = 0;
}

void loop() {
  //reading from the 3 connected moisture_sensors
  delay(3000);
  moisture_output1 = analogRead(moisture_sensor1);
  moisture_output1 = map(moisture_output1,1000,0,0,100);
  moisture_output2 = analogRead(moisture_sensor2);
  moisture_output2 = map(moisture_output2,1000,0,0,100);
  moisture_output3 = analogRead(moisture_sensor3);
  moisture_output3 = map(moisture_output3,1000,0,0,100);
  if( moisture_output1 < 0 )
    moisture_output1 = 0;
  if( moisture_output2 < 0 )
    moisture_output2 = 0;
  if( moisture_output3 < 0 )
    moisture_output3 = 0;
  delay(3000);
  if(iAmDebug) {    
    Serial.println("RAW");
    Serial.println(moisture_output1);
    Serial.println(moisture_output2);
    Serial.println(moisture_output3);
    delay(5000);
  }

  //preparing the message to send to backend o serial (debug)
  String message = "";
  message = message + moisture_output1 + ";" + moisture_output2 + ";" + moisture_output3 + ";";
  message.trim();
  
  if(iAmDebug) {    
    if(up)
      digitalWrite(LEDPIN, LOW);
    else
      digitalWrite(LEDPIN, LOW);
    up = !up;
    Serial.println(message);
  } else {
    digitalWrite(LEDPIN, HIGH);
    sendMsgAndGetResponse(message);
    digitalWrite(LEDPIN, LOW);
    //I put the board in sleep mode for 20 minutes
    LowPower.sleep( 20*60*1000 );
  }
}
//code for sending message(str) to sigfox network
void sendMsgAndGetResponse(String str) {
  // Start the module
  SigFox.begin();
  // Wait at least 30mS after first configuration (100mS before)
  delay(100);
  // Clears all pending interrupts
  SigFox.status();
  delay(1);

  SigFox.beginPacket();
  SigFox.print(str);
  int ret = SigFox.endPacket(true);  // send buffer to SIGFOX network and wait for a response
  if(iAmDebug) {
    Serial.println(ret);
  }
  SigFox.end();
}
