#define BLYNK_PRINT Serial    
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h> 

WidgetLED PUMP(V0); 

#define SOIL_MOIST_1_PIN A0 // pin A0 with A0
#define PUMP_PIN 13   // PUMP RELAY
#define DRY_SOIL      68
#define WET_SOIL      90
#define TIME_PUMP_ON  15
#define READ_SOIL_HUM_TM  10L 
#define SEND_UP_DATA_TM   10L 
#define AUTO_CTRL_TM      60L 

char auth[] = "token blynk";
char ssid[] = "wifi name";
char pass[] = "wifi pass";

 int soilMoist = 0;
 int timePumpOn = 1; 
 boolean pumpStatus = 0;
 long sampleTimingSeconds = 20; 
 long startTiming = 0;
 long elapsedTime = 0;
 SimpleTimer timer;

void setup() {
  pinMode(PUMP_PIN, OUTPUT);
  aplyCmd();
  Serial.begin(115200);
  Blynk.begin( auth, ssid , pass );
  PUMP.off();
  startTimers();
}
void loop() {
  timer.run(); 
  Blynk.run();
}
BLYNK_WRITE(3) 
{
  int i = param.asInt();
  if (i == 1)
  {
    pumpStatus = !pumpStatus;
    aplyCmd();
  }
}
void getSoilMoist(void)
{
  int i = 0;
  soilMoist = 0;
  for (i = 0; i < 10; i++)  //
  {
    soilMoist += analogRead(SOIL_MOIST_1_PIN); 
    delay(20);   
  }
  soilMoist = soilMoist / (i);
  soilMoist = map(soilMoist, 1023, 0, 0, 100); 

}

void aplyCmd()
{
  if (pumpStatus == 1)
  {
    Blynk.notify("pump ON");
    digitalWrite(PUMP_PIN, LOW);
    PUMP.on();
  }

  else {
    digitalWrite(PUMP_PIN, HIGH);
    PUMP.off();
  }
}
void autoControlPlantation(void)
{
  if (soilMoist < DRY_SOIL)
  {
    turnPumpOn();
  }
}
void turnPumpOn()
{
  pumpStatus = 1;
  aplyCmd();
  delay (TIME_PUMP_ON * 1000);
  pumpStatus = 0;
  aplyCmd();
}
void startTimers(void)
{
  timer.setInterval(READ_SOIL_HUM_TM * 1000, getSoilMoist);
  timer.setInterval(SEND_UP_DATA_TM * 1000, sendUptime);
  timer.setInterval(AUTO_CTRL_TM * 1000, autoControlPlantation);
}
void sendUptime()
{
  
  Blynk.virtualWrite(V1, soilMoist);
}
