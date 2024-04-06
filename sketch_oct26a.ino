
#include <Adafruit_SSD1306.h>     // Library for controlling SSD1306-based OLED displays.
#include <ESP8266WiFi.h>          //Library for connecting the ESP8266 to a Wi-Fi network.  
#include "ThingSpeak.h"           //Library for interfacing with ThingSpeak

 
char ssid[] = "Abinaya kiruthiga";
char pass[] = "poil09871";
unsigned long lastTime = 0;
unsigned long timerDelay = 5000;
 

int emptyTankDistance = 12;
int fullTankDistance = 2;
int triggerPer = 80;
int nf;                         //Flag to indicate whether the motor is ON (1) or OFF (0)

 
#define TRIG 12                 //D6    //Pins for the ultrasonic sensor.
#define ECHO 13                 //D7    
#define Relay 00            //D3    //Pin connected to the relay module.
#define BUZZER_PIN 14           //D5    //Pin connected to the buzzer.
 

//Configuring the OLED display with a resolution of 128x32 pixels
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


WiFiClient client;


//ThingSpeak Configuration
unsigned long myChannelNumber = 2341329;              // Channel number shown in private window tab
const char * myWriteAPIKey = "HDCY0FIT60WASOVZ";
 
float duration;               //Time taken for the ultrasonic pulse to return.
float distance;
int waterLevelPer;
bool toggleRelay = false ;    //Flag to track the state of the relay (ON/OFF)
bool modeFlag = true;
String currMode;
 


//Displays the water level percentage, distance, and current mode on the OLED display.
void displayData() {
  display.clearDisplay();
  display.setTextSize(3);
  display.setCursor(30, 0);
  display.println(waterLevelPer);
  Serial.println("\n ");
  Serial.println(distance);
  Serial.println("cm");
  Serial.print("\n ");
  Serial.print(waterLevelPer);
  Serial.print("%");
  display.setTextSize(1);
  display.print(distance);
  display.setTextSize(1);
  display.setCursor(20, 25);
  display.print(currMode);
  display.setCursor(95, 25);
  if(nf==1)
  {
    display.print("ON");
  }
  else{
    display.print("OFF");
  }
  display.display();
}
 

//Measures the distance using an ultrasonic sensor, calculates water level percentage, and controls the motor and buzzer accordingly
void measureDistance() {

 if ((millis() - lastTime) > timerDelay) {
    // Connect or reconnect to WiFi
    if (WiFi.status() != WL_CONNECTED) {
      Serial.print("Attempting to connect");
      while (WiFi.status() != WL_CONNECTED) {
        WiFi.begin(ssid, pass);
        delay(1000);
      }
      Serial.println("\nConnected.");
    }

  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(20);
  digitalWrite(TRIG, LOW);
  duration = pulseIn(ECHO, HIGH);
  
  distance= duration * 0.0344 / 2;              //0.0344: Conversion factor representing the speed of sound in air 
  if (distance > (fullTankDistance ) && distance < emptyTankDistance) {
    
    if(distance>=emptyTankDistance)
    {
      waterLevelPer=0;
  
    }
    else
    {
    waterLevelPer=((emptyTankDistance - distance) *100/ (emptyTankDistance - fullTankDistance)) ;
    }

     Serial.print("Reached");

    if (waterLevelPer > triggerPer) {
      
      nf=1;
      if (modeFlag) {
        if (!toggleRelay) {
          tone(BUZZER_PIN, 1000);               // 1000 Hz frequency
          delay(500);                           // Change the delay as needed
          Serial.println(" On Motor ");
          // keep the motor ON by keeping  LOW
          digitalWrite(Relay, LOW);
          toggleRelay = true;
          
        }
      }
    }

    

    else if (waterLevelPer < triggerPer) {
      
      
      nf=0;
      if (modeFlag) {
        if (toggleRelay) {
            Serial.print(" Motor Off ");
          // keep the motor off by keeping  HIGH
          digitalWrite(Relay, HIGH);
          noTone(BUZZER_PIN);                   // Turn off the buzzer
          delay(1000);                          // Change the delay as needed
          toggleRelay = false;
       
        }
      }
    }
    else
    {
      // keep the motor off by keeping  HIGH
      digitalWrite(Relay, HIGH);
      nf=0;
      noTone(BUZZER_PIN);                       // Turn off the buzzer
      delay(1000);                              // Change the delay as needed
    }

  }


  displayData();
  
   int x = ThingSpeak.writeField(myChannelNumber, 1, waterLevelPer, myWriteAPIKey);   //Sends the water level percentage to a ThingSpeak channel.
  delay(1000);
  if (x == 200) {
      Serial.println("Channel update successful.");
    } else {
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
    lastTime = millis();
  }

}
void setup() {
  Serial.begin(115200);
  pinMode(ECHO, INPUT);
  pinMode(TRIG, OUTPUT);
  pinMode(Relay, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client); 
 
  digitalWrite(Relay, HIGH);

 
 
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.clearDisplay();
 
  WiFi.begin(ssid, pass);

  delay(1000);
 

 
}
 
void loop() {
  
measureDistance();
}
 

