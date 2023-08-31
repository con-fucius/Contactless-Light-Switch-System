
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

// Checks for new messages every 1 second.
int botRequestDelay = 120000;
unsigned long lastTimeBotRan;

//initialise pins
 const int greenled = 4;          
 const int SensorPin = 16;    
 const int ldrpin = A0;     
 bool ledState = LOW;
 
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
     welcome += "/led_on to turn GPIO ON \n";
      welcome += "/led_off to turn GPIO OFF \n";
      welcome += "/state to request current GPIO state \n";
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
  
    if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 8);//give updates after 8 sec 

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 8);//update every 8 sec
    }
    lastTimeBotRan = millis();
  }
}
