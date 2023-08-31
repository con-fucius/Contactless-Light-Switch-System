
//kariithi anne
//node automatic lighting system
#include <UniversalTelegramBot.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

 const char* ssid= "NEVEREST";   // REPLACE SSID WITH YOUR WIFI SSID  
 const char* password = "nikoworks"; // REPLACE Password YOUR WIFI PASSWORD, IF ANY  
 // Initialize Telegram BOT
  #define BOTtoken "5270990053:AAEww_Nc1X9MfU26PmnPCNJ2N_U_MzugrEo" // REPLACE Token WITH YOUR TELEGRAM BOT TOKEN  
// Also note that you need to click "start" on a bot before it can
// message you
 #define CHAT_ID  "1125047785"

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Checks for new messages every 2 minutes.
int botRequestDelay = 120000;
unsigned long lastTimeBotRan;

//initialize pins
const int greenled = 4;     
const int sensorpin = 5     
const int trigpin = 13; //trigger pin connection to pin 13 in the case of a ultrasonic sensor
const int echopin = 12; //echo pin connection to pin 12
const int relaypin = 16; //connection of the relay pin   
const int ldrpin = A0;     
bool ledState = LOW;
const int maximum_distance=3;//set the threshold distance of the ultrasonic sensor to turn led on or off

//variables
long duration;
int distance;

 
// Handle what happens when you receive new messages
void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
// Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);

String from_name = bot.messages[i].from_name;//message the bot
//commands
if (text == "/start") {
      String welcome = "Welcome, " + from_name + ".\n";
     welcome += "Use the following commands to control your outputs.\n\n";
     welcome += "/led_on to turn ligh on \n";
      welcome += "/led_off to turn light off \n";
      welcome += "/state to request current state of the light \n";
      bot.sendMessage(chat_id, welcome, "");
    }

    if (text == "/led_on") {
      bot.sendMessage(chat_id, "LED state set to ON", "");
      ledState = HIGH;
      digitalWrite(greenled, ledState);
    }
    
    if (text == "/led_off") {
      bot.sendMessage(chat_id, "LED state set to OFF", "");
      ledState = LOW;
      digitalWrite(greenled, ledState);
    }
    
    if (text == "/state") {
      if (digitalRead(greenled)){
        bot.sendMessage(chat_id, "LED is ON", "");
      }
      else{
        bot.sendMessage(chat_id, "LED is OFF", "");
      }
    }
  }
}
 void setup() {  
  // initialize the Serial  
  Serial.begin(115200);

  #ifdef ESP8266
  client.setInsecure();
  #endif
  //set the pin connected to the LED to act as output pin  
            pinMode(greenled, OUTPUT);  
            digitalWrite(greenled, LOW); 
            pinMode(ldrpin,INPUT);//set pin to input mode
            pinMode(SensorPin, INPUT);
            digitalWrite(SensorPin,LOW);
            digitalWrite(trigpin,LOW);
            delayMicroseconds(2); //generate 2 microseconds pulse to trigpin
            digitalWrite(trigpin,HIGH);
            delayMicroseconds(10); 
            
  //init the wifi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.println("Connecting to WiFi..");
  }
}
void loop() {
  int SensorValue = digitalRead(SensorPin);
  Serial.print("SensorPin Value: ");
  Serial.println(SensorValue);
  delay(1000);
  
  int ldrstatus = analogRead(ldrpin);
  Serial.print("Status:");//print value to serial monitor
  Serial.println(ldrstatus);
  delay(1000);
  
 if (millis() > lastTimeBotRan + botRequestDelay){
  if((SensorValue==LOW)||(ldrstatus<500)){ // LOW MEANS Object Detected
         Serial.println("Turn ON the lights");
         digitalWrite(greenled, HIGH);
         bot.sendMessage(CHAT_ID,"Hand is closer or the room is dim!!", "");
          }
              else
              {
                 Serial.println("Turn OFF the lights");
                 digitalWrite(greenled, LOW); 
                 bot.sendMessage(CHAT_ID,"Hand is far or the room is bright!!", "");
              }
//  generate message for the sender
  
   // if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);//give updates after 2 minutes

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);//update every 2 minutes
    }
    lastTimeBotRan = millis();
  }
}

void ultrasonic()
{
digitalWrite(trigpin, HIGH);
delayMicroseconds(1000);  
digitalWrite(trigpin, LOW);
duration = pulseIn(echopin,HIGH);
distance = (0.034*duration)/2;
Serial.println("The distance is: cm"); 
nodemcu.println(distance); // send to nodemcu module

//measure duration of pulse from ECHO pin
  duration = pulseIn(echopin,HIGH);
  //calculatin the distance in cm
  //distance=speed*duration per cycle
  //speed=340m/s=0.034cm/ms
  distance = (0.017 *duration);//per cycle
  int ldrstatus = analogRead(ldrpin);//read status of the ldr
  
if (millis() > lastTimeBotRan + botRequestDelay){
if(ldrstatus<150)
      {
       digitalWrite(relaypin1,LOW); 
       digitalWrite(ledpin,HIGH);
       Serial.print("TURN LED ON:");//print value to serial monitor
       Serial.println(ldrstatus);
       Serial.print("Thedistance is:");//print value to serial monitor
       Serial.println(distance);
       bot.sendMessage(CHAT_ID,"Hand is closer or the room is dim!!", "");
       delay(2000);
      }
  
       else if((ldrstatus<150) || (distance<maximum_distance))
        {
           digitalWrite(relaypin1,LOW);   
           digitalWrite(ledpin,HIGH);
           Serial.print("TURN LED ON:");//print value to serial monitor
           Serial.println(ldrstatus);
           Serial.print("Thedistance is:");//print value to serial monitor
           Serial.println(distance);
         delay(3000);
        }
     
          else {
            digitalWrite(relaypin1,HIGH); 
            digitalWrite(ledpin,LOW);
            Serial.print("TURN LED OFF:");
            Serial.println(ldrstatus);
            Serial.print("Thedistance is:");//print value to serial monitor
            Serial.println(distance);
            bot.sendMessage(CHAT_ID,"Hand is far or the room is bright!!", "");
        }    
        delay(500);

    //  generate message for the sender
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);//give updates after 2 minutes

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);//update every 2 minutes
    }
    lastTimeBotRan = millis();
  }
}
