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
int RECV_PIN = 12; 
int SEND_PIN = 05; 
int IR_CNT=5;
int IR_DELAY=2;
/* pin that is attached to interrupt */
byte interruptPin = 13;

/* Interrupt counter pin */
int interruptCounter=0;


decode_results results;

//const uint16_t kNecTick = 550;
//const uint16_t kNecHdrMarkTicks = 8;

//818100FF (32 bits) POWER BUTTON
uint16_t Signal_PWR[] = {4462, 4486528, 1704528, 584530, 582534, 580532, 582534, 582532, 582532, 1702528, 1700530, 582534, 580532, 582530, 584532, 584530, 586530, 1700530, 584534, 578532, 582532, 584530, 582534, 582530, 582532, 584530, 1702528, 1724506, 1698534, 1696530, 1698532, 1722506, 1698532, 1700528};
//8181807F (32 bits) BLUE HOME BUTTON
uint16_t Signal_HOME[] = {4464, 4482534, 1694532, 582534, 582532, 582532, 582532, 582534, 582532, 1722508, 1696534, 580532, 582530, 586528, 584530, 584530, 582532, 1702528, 1700528, 582532, 584532, 582532, 582530, 584532, 584532, 580532, 582536, 1718506, 1700530, 1700528, 1706526, 1700528, 1702528, 1702530};
//818140BF (32 bits) AUTO BTN
uint16_t Signal_AUTO[] = {4464, 4484532, 1698528, 586530, 584530, 582532, 586530, 584530, 584532, 1720508, 1700530, 582532, 582532, 584530, 584530, 580534, 582532, 1698530, 584532, 1702528, 580534, 582530, 584532, 582532, 582544, 572530, 1702528, 582532, 1702530, 1720508, 1700530, 1724506, 1698534, 1702526};
//8181C03F (32 bits) MID BTN
uint16_t Signal_MID_[] = {4464, 4486530, 1700528, 582532, 582534, 582532, 584530, 582532, 582536, 1696530, 1702528, 582532, 582532, 584530, 582534, 580534, 582534, 1722506, 1698532, 1696532, 582532, 584534, 580532, 582532, 584532, 582554, 562534, 582528, 1702528, 1698532, 1698534, 1694532, 1702530, 1698532};
//818120DF (32 bits) SPOT BTN
uint16_t Signal_SPOT[] = {4466, 4484528, 1702530, 580534, 582530, 582532, 584534, 580532, 582532, 1700528, 1726508, 580532, 582534, 580532, 584530, 584530, 582532, 1700528, 584530, 584532, 1698534, 580530, 584534, 578532, 584532, 584530, 1700528, 1700530, 582532, 1700528, 1702526, 1722506, 1700532, 1722508};
//8181A05F (32 bits) GO AHEAD
uint16_t Signal_AHEAD[] = {4464, 4484530, 1726504, 582532, 584530, 582536, 580532, 580532, 582532, 1702528, 1702526, 584532, 582530, 584532, 582534, 580532, 584530, 1698532, 1698554, 560530, 1702530, 580532, 582532, 584532, 584532, 580534, 582530, 1700530, 584530, 1702528, 1698532, 1700528, 1700530, 1702530};
//8181609F (32 bits) LEFT BTN
uint16_t Signal_LEFT[] = {4466, 4484530, 1702530, 582530, 584532, 582532, 582532, 584526, 586534, 1696530, 1700530, 582530, 584532, 582532, 584528, 584534, 582532, 1698534, 580532, 1722508, 1700528, 582528, 586532, 582536, 578532, 582532, 1700528, 584530, 586530, 1698530, 1702530, 1698536, 1692532, 1700532};
//8181E01F (32 bits) RIGHT BTN
uint16_t Signal_RIGT[] = {4462, 4480536, 1722508, 582532, 582532, 584532, 582530, 584534, 580532, 1722508, 1698534, 582534, 582532, 584530, 584532, 582532, 582534, 1698530, 1728504, 1698532, 1698530, 582532, 582534, 582530, 582532, 584530, 582532, 584530, 584530, 1706526, 1722508, 1702530, 1722508, 1698532};
//818110EF (32 bits) PLAY STOP
uint16_t Signal_PLST[] = {4464, 4480532, 1702530, 582530, 584532, 584528, 586530, 586534, 576532, 1698530, 1726508, 580532, 584532, 580532, 584532, 580534, 580532, 1700528, 584530, 582532, 582534, 1700528, 584532, 584530, 584532, 580532, 1700530, 1698530, 1702530, 580532, 1700530, 1724506, 1722508, 1700528};
//8181906F (32 bits) MODE1
uint16_t Signal_MOD1[] = {4462, 4482532, 1700528, 586532, 584530, 586528, 584534, 580532, 582536, 1698530, 1698530, 584530, 582532, 584532, 582530, 582532, 582532, 1700530, 1700530, 584530, 582530, 1698532, 582532, 586528, 580534, 584530, 582534, 1700528, 1698532, 582534, 1700528, 1700532, 1722506, 1700532};
//8181906F (32 bits) MODE2
uint16_t Signal_MOD2[] = {4462, 4486530, 1702528, 582534, 580532, 584536, 580534, 580532, 582536, 1696530, 1700530, 582536, 580532, 582530, 582534, 580532, 580560, 1672532, 580530, 1698532, 584532, 1698532, 582532, 582536, 578536, 580530, 1700530, 584532, 1698532, 582532, 1698530, 1700530, 1698532, 1698530};
//8181D02F (32 bits) MUTE
uint16_t Signal_SOUN[] = {4464, 4482534, 1698530, 584532, 586528, 584532, 584530, 582532, 582534, 1702528, 1696532, 582534, 580534, 580534, 580532, 582532, 584530, 1702528, 1700532, 1700528, 584532, 1702526, 582534, 580532, 582534, 582532, 582532, 584528, 1700530, 582532, 1700530, 1700528, 1702528, 1700532};







// Attach virtual serial terminal to Virtual Pin V1
WidgetTerminal terminal(V1);
BlynkTimer timer;
WidgetRTC rtc;
IRsend irsend(SEND_PIN,true,false); 
IRrecv irrecv(RECV_PIN);


//void serialPrintUint64(uint64_t input, uint8_t base) 
//{
//  Serial.print(uint64ToString(input, base));
//}

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

  irrecv.enableIRIn();  // Start the receiver

  
  pinMode(interruptPin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(interruptPin), blink, FALLING);

  
}

void blink() 
{
  
  interruptCounter++;
}


// You can send commands from Terminal to your hardware. Just use
// the same Virtual Pin as your Terminal Widget
BLYNK_WRITE(V2)
{
terminal.println("Button 1 Pressed") ;
u8x8.drawString(0, 2, "Starting work");

      pinMode(SEND_PIN, OUTPUT);
      digitalWrite(SEND_PIN, HIGH);  

      for (int i = 0; i < IR_CNT; i++) 
      {
      irsend.sendNEC(0X818110EF);
      }      
      delay(IR_DELAY);
      
      pinMode(SEND_PIN, INPUT);
}


// You can send commands from Terminal to your hardware. Just use
// the same Virtual Pin as your Terminal Widget
BLYNK_WRITE(V3)
{
terminal.println("Button 2 Pressed") ;
u8x8.drawString(0, 2, "GOING HOME");



      pinMode(SEND_PIN, OUTPUT);
      digitalWrite(SEND_PIN, HIGH);  


      for (int i = 0; i < IR_CNT; i++) 
      {
      irsend.sendNEC(0X8181807F);
      }      

      delay(IR_DELAY);
      pinMode(SEND_PIN, INPUT);

      
}


// You can send commands from Terminal to your hardware. Just use
// the same Virtual Pin as your Terminal Widget
BLYNK_WRITE(V1)
{

  // if you type "Marco" into Terminal Widget - it will respond: "Polo:"
  if (String("Marco") == param.asStr()) {
    terminal.println("You said: 'Marco'") ;
    terminal.println("I said: 'Polo'") ;
  } else {

    // Send it back
    terminal.print("You said:");
    u8x8.drawString(0, 1, param.asStr());
    terminal.write(param.getBuffer(), param.getLength());
    terminal.println();
  }

  // Ensure everything is sent
  terminal.flush();
  
}


void doDECODEIRWork(decode_results *results) 
{

}


void doSomeIRWork()
{
  if (irrecv.decode(&results)) {
    doDECODEIRWork(&results);
    Serial.println("");
    irrecv.resume();  // Receive the next value
  }
}


void doSomeWork()
{
  String currentTime = String(hour()) + ":" + minute() + ":" + second() + " - " + interruptCounter;
  u8x8.drawString(0, 3, "                ");
  u8x8.drawString(0, 3, currentTime.c_str());
  u8x8.drawString(0, 1, "                ");
  u8x8.drawString(0, 2, "                ");
}


void loop()
{
  //doSomeIRWork();
  Blynk.run();
  timer.run();
}
