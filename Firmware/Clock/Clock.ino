#include "WiFiCredentials.h"
#include "DisplayPinDefs.h"

#include <WiFi.h>
#include "time.h"

struct tm timeinfo;

void DisplayNumber(int numberToDisplay){
  int firstDigit = numberToDisplay % 10;
  int secondDigit = ((numberToDisplay % 100) - firstDigit)/10;
  int thirdDigit = ((numberToDisplay % 1000 - (secondDigit*10)) - firstDigit)/100;
  int fourthDigit = (numberToDisplay - (thirdDigit * 100) - (secondDigit * 10) - firstDigit)/1000;
  //shift data to 595
  shiftOut(SER, SRCLK, LSBFIRST, displayData[firstDigit]);
  shiftOut(SER, SRCLK, LSBFIRST, displayData[secondDigit]);
  shiftOut(SER, SRCLK, LSBFIRST, displayData[thirdDigit]);
  shiftOut(SER, SRCLK, LSBFIRST, displayData[fourthDigit]);
  //push data to storage register
  digitalWrite(RCLK, HIGH);
  delayMicroseconds(100);
  digitalWrite(RCLK, LOW);
}

void initWifi() {
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  // Initialize a time structure and set the timezone, e.g., GMT-8
  configTime(-5 * 3600, 3600, "pool.ntp.org", "time.nist.gov");
  Serial.println("Connected to the WiFi network");
}


void setup() {
  Serial.begin(115200);

  pinMode(DISPLAYVDD, OUTPUT);
  digitalWrite(DISPLAYVDD, HIGH);

  pinMode(nOE, OUTPUT);
  pinMode(nCLR, OUTPUT);
  pinMode(SER, OUTPUT);
  pinMode(RCLK, OUTPUT);
  pinMode(SRCLK, OUTPUT);

  digitalWrite(nCLR, HIGH);
  analogWrite(nOE, 254);

  initWifi();
}

int getTime(){

  int timenow;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return 0;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");  // Print the time in a more readable format
  if(timeinfo.tm_hour > 12){
    timenow = (timeinfo.tm_hour % 12) * 100 + timeinfo.tm_min;
  }
  else{
    timenow = (timeinfo.tm_hour) * 100 + timeinfo.tm_min;
  }
  Serial.println(timenow);
  return timenow;
}

void loop() {

  DisplayNumber(getTime());

  if(timeinfo.tm_hour == 1 && millis() > (1000 * 60 * 60 * 23)){
    ESP.restart();
  }

  if((timeinfo.tm_hour == 7 && timeinfo.tm_min >= 30)){
    analogWrite(nOE, 0);
  }
  else if(timeinfo.tm_hour > 7){
    analogWrite(nOE, 127);
  }
  else{
    analogWrite(nOE, 254);
  }

  delay(10000);  // Update every 10 seconds

}
