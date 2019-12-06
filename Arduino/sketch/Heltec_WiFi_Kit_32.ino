#include "WiFi.h"
#include <U8x8lib.h>


/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include <Arduino.h>
#include <IRutils.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <TimeLib.h>
#include <WidgetRTC.h>
#include <IRrecv.h>
#include <IRsend.h> //   https://github.com/crankyoldgit/IRremoteESP8266
//#include "SlaveSPI.h" //https://github.com/iPAS/esp32-slave-spi
//#include <SPI.h>


// the OLED used
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "LqpN-AtehvHl4MaeU6WPZEOrJhzE7Mcg";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "nonamegsm.biz";
char pass[] = "0663376682";

// IRDA Sygnals
//int RECV_PIN = 12; 
int SEND_PIN = 05; 
//int SEND_PIN = 12; 

int IR_CNT=50;
int IR_DELAY=1;
/* pin that is attached to interrupt */
byte interruptPin = 13;
/* Interrupt counter pin */
int interruptCounter=0;
/* Run timer */
int runTimer=0;


//decode_results results;

uint16_t kMyNecTick = 555;


const uint16_t kMyNecHdrMarkTicks = 8;
const uint16_t kMyNecHdrMark = kMyNecHdrMarkTicks * kMyNecTick;
const uint16_t kMyNecHdrSpaceTicks = 8;
const uint16_t kMyNecHdrSpace = kMyNecHdrSpaceTicks * kMyNecTick;
const uint16_t kMyNecBitMarkTicks = 1;
const uint16_t kMyNecBitMark = kMyNecBitMarkTicks * kMyNecTick;
const uint16_t kMyNecOneSpaceTicks = 3;
const uint16_t kMyNecOneSpace = kMyNecOneSpaceTicks * kMyNecTick;
const uint16_t kMyNecZeroSpaceTicks = 1;
const uint16_t kMyNecZeroSpace = kMyNecZeroSpaceTicks * kMyNecTick;
const uint16_t kMyNecRptSpaceTicks = 4;
const uint16_t kMyNecRptSpace = kMyNecRptSpaceTicks * kMyNecTick;
const uint16_t kMyNecRptLength = 4;
const uint16_t kMyNecMinCommandLengthTicks = 193;
const uint32_t kMyNecMinCommandLength = kMyNecMinCommandLengthTicks * kMyNecTick;
const uint32_t kMyNecMinGap =
    kMyNecMinCommandLength -
    (kMyNecHdrMark + kMyNecHdrSpace + kNECBits * (kMyNecBitMark + kMyNecOneSpace) +
     kMyNecBitMark);
const uint16_t kMyNecMinGapTicks =
    kMyNecMinCommandLengthTicks -
    (kMyNecHdrMarkTicks + kMyNecHdrSpaceTicks +
     kNECBits * (kMyNecBitMarkTicks + kMyNecOneSpaceTicks) + kMyNecBitMarkTicks);


uint32_t cmdPWR=0x818100FFL; //(32 bits) POWER BUTTON
uint32_t cmdHOME=0x8181807FL; //(32 bits) BLUE HOME BUTTON
uint32_t cmdAUTO=0x818140BFL; //(32 bits) AUTO BTN
uint32_t cmdMID=0x8181C03FL; //(32 bits) MID BTN
uint32_t cmdSPOT=0x818120DFL; //(32 bits) SPOT BTN
uint32_t cmdAHEAD=0x8181A05FL; //(32 bits) GO AHEAD
uint32_t cmdLEFT=0x8181609FL; //(32 bits) LEFT BTN
uint32_t cmdRIGHT=0x8181E01FL; //(32 bits) RIGHT BTN
uint32_t cmdPLAYSTOP=0x818110EFL; //(32 bits) PLAY STOP
uint32_t cmdMODE1=0x8181906FL; //(32 bits) MODE1
uint32_t cmdMODE2=0x8181906FL; //(32 bits) MODE2
uint32_t cmdSOUND=0x8181D02FL; //(32 bits) MUTE


// Attach virtual serial terminal to Virtual Pin V1
WidgetTerminal terminal(V1);
BlynkTimer timer;
WidgetRTC rtc;
IRsend irsend(SEND_PIN,true,false);  // direct wiring
//IRsend irsend(SEND_PIN,true,true); // LED experiment
//IRrecv irrecv(RECV_PIN);


void setup()
{
  // Set WiFi to station mode and disconnect from an AP if it was previously connected

  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.drawString(0, 0,"Blynk v" BLYNK_VERSION "");
  u8x8.drawString(0, 1,"-------------");
  u8x8.drawString(0, 2,"Device started");
  u8x8.drawString(0, 3,"Connect to cloud");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  rtc.begin();
  timer.setInterval(1000L, doSomeWork);


  terminal.println(F("Blynk v" BLYNK_VERSION ": Device started"));
  terminal.println(F("-------------"));
  terminal.println(F("Type 'Marco' and get a reply, or type"));
  terminal.println(F("anything else and get it printed back."));
  terminal.flush();
  u8x8.clear();
  u8x8.drawString(0, 0, "Running Blynk.");

//  irrecv.enableIRIn();  // Start the receiver


  pinMode(SEND_PIN, INPUT);

  
  pinMode(interruptPin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(interruptPin), blink, FALLING);

  
}

void blink() 
{
  
  interruptCounter++;
}


void DoSend(uint64_t data)
{
      pinMode(SEND_PIN, OUTPUT);
      digitalWrite(SEND_PIN, HIGH);  

      noInterrupts();
      
      irsend.sendGeneric(kMyNecHdrMark, kMyNecHdrSpace, kMyNecBitMark, kMyNecOneSpace, kMyNecBitMark,
                  kMyNecZeroSpace, kMyNecBitMark, kMyNecTick*16, kMyNecTick*145,               
                  data, 32, 38, true, 3,  // Repeats are handled later.
                  33);    
      interrupts();
      
      pinMode(SEND_PIN, INPUT);  

}


// You can send commands from Terminal to your hardware. Just use
// the same Virtual Pin as your Terminal Widget
BLYNK_WRITE(V0)
{


int virtualPin0 = param.asInt(); // assigning incoming value from pin V0 to a variable
if (virtualPin0 == 1)
{
terminal.println("Button 0 Pressed") ;
u8x8.drawString(0, 2, "Timer start");
DoSend(cmdPLAYSTOP);
delay(1000);
}
else
{
terminal.println("Button 0 Pressed") ;
u8x8.drawString(0, 2, "Timer stop");
DoSend(cmdHOME);
delay(1000);
}



  


}

// You can send commands from Terminal to your hardware. Just use
// the same Virtual Pin as your Terminal Widget
BLYNK_WRITE(V2)
{
terminal.println("Button 1 Pressed") ;
u8x8.drawString(0, 2, "Starting...");
DoSend(cmdPLAYSTOP);

}


// You can send commands from Terminal to your hardware. Just use
// the same Virtual Pin as your Terminal Widget
BLYNK_WRITE(V3)
{
terminal.println("Button 2 Pressed") ;
u8x8.drawString(0, 2, "Going HOME");
DoSend(cmdHOME);
      
}

// You can send commands from Terminal to your hardware. Just use
// the same Virtual Pin as your Terminal Widget
BLYNK_WRITE(V4)
{
terminal.println("Button 3 Pressed") ;
u8x8.drawString(0, 2, "Power OFF...");
DoSend(cmdPWR);
}


BLYNK_WRITE(V5)
{
terminal.println("Button 4 Pressed") ;
u8x8.drawString(0, 2, "Auto Mode");
DoSend(cmdAUTO);
}

BLYNK_WRITE(V6)
{
terminal.println("Button 5 Pressed") ;
u8x8.drawString(0, 2, "Spot Mode");
DoSend(cmdSPOT);
}

BLYNK_WRITE(V7)
{
terminal.println("Button 6 Pressed") ;
u8x8.drawString(0, 2, "Go Ahead");
DoSend(cmdAHEAD);
}



BLYNK_WRITE(V8)
{
terminal.println("Button 7 Pressed") ;
u8x8.drawString(0, 2, "Left");
DoSend(cmdLEFT);
}


BLYNK_WRITE(V9)
{
terminal.println("Button 8 Pressed") ;
u8x8.drawString(0, 2, "Right");
DoSend(cmdRIGHT);
}



BLYNK_WRITE(V10)
{
terminal.println("Button 9 Pressed") ;
u8x8.drawString(0, 2, "cmdMODE1");
DoSend(cmdMODE1);
}

BLYNK_WRITE(V11)
{
terminal.println("Button 10 Pressed") ;
u8x8.drawString(0, 2, "cmdMODE2");
DoSend(cmdMODE2);
}


BLYNK_WRITE(V12)
{
terminal.println("Button 11 Pressed") ;
u8x8.drawString(0, 2, "cmdSOUND");
DoSend(cmdSOUND);
}







// You can send commands from Terminal to your hardware. Just use
// the same Virtual Pin as your Terminal Widget
BLYNK_WRITE(V1)
{

  // if you type "Marco" into Terminal Widget - it will respond: "Polo:"
  if (String("Marco") == param.asStr()) {
    terminal.println("You said: 'Marco'") ;
    terminal.println("I said: 'Polo'") ;
  } else 
  // if you type "pullup" into Terminal Widget - it will respond: "Polo:"
  if (String("pullup") == param.asStr()) {
    terminal.println("You said: 'pullup'") ;
    pinMode(SEND_PIN, OUTPUT);
    digitalWrite(SEND_PIN, HIGH);  
  } else   
  // if you type "pulldown" into Terminal Widget - it will respond: "Polo:"
  if (String("pulldown") == param.asStr()) {
    terminal.println("You said: 'pulldown'") ;
    pinMode(SEND_PIN, OUTPUT);
    digitalWrite(SEND_PIN, LOW);  
  } else     
  // if you type "pulldown" into Terminal Widget - it will respond: "Polo:"
  if (String("disable") == param.asStr()) {
    terminal.println("You said: 'disable'") ;
    pinMode(SEND_PIN, INPUT);
  } else       
  
  
  {

    // Send it back
    terminal.print("You said:");
    u8x8.drawString(0, 1, param.asStr());
    terminal.write(param.getBuffer(), param.getLength());
    terminal.println();
    DoSend(cmdSOUND);
  }

  // Ensure everything is sent
  terminal.flush();
  
}


void doDECODEIRWork(decode_results *results) 
{

}


void doSomeIRWork()
{
//  if (irrecv.decode(&results)) {
//    doDECODEIRWork(&results);
//    Serial.println("");
//    irrecv.resume();  // Receive the next value
//  }
}


void doSomeWork()
{
  u8x8.drawString(0, 1, "                ");
  u8x8.drawString(0, 2, "                ");  
  
  String currentTime = "Time :"+String(hour()) + ":" + minute() + ":" + second() + "  ";
  String currentCounter = "SPI :" + String(interruptCounter);
  String runCounter = "Run :" + String(runTimer)+" sec.";  
  u8x8.drawString(0, 3, currentTime.c_str());
  u8x8.drawString(0, 4, currentCounter.c_str());
  u8x8.drawString(0, 5, runCounter.c_str());
  
  runTimer++;


  
}


void loop()
{
  //doSomeIRWork();
  Blynk.run();
  timer.run();
}