#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <stdio.h>
#include <vector>
#include <sstream>
#include "secure.h"
#include "driver/uart.h"

WiFiMulti wifimulti;
using namespace std;
double setPoint, input, output;
volatile double conKp = 3, conKi = 1, conKd = 15;
volatile int DELAY = 200;
void connectWiFi()
{
  wifimulti.addAP(ssid, password);
  WiFi.mode(WIFI_STA);
  printf("Connecting to :%s\n", ssid);
  while ((wifimulti.run() != WL_CONNECTED))
  {
    Serial.print(".");
  }
  printf("[WiFi]:%s connected\n ", ssid);
  printf("[WiFi] ip is : %s\n", WiFi.localIP().toString().c_str());
}
void sendDiscord(String content)
{
  WiFiClientSecure *client = new WiFiClientSecure;
  if (client)
  {
    client->setCACert(discordCertificate);
    {
      HTTPClient https;
      printf("[HTTP] Connecting to Discord...\n");
      printf("[HTTP] Message: %s \n ", content.c_str());
      printf("[HTTP] TTS: %s \n", TTS);
      if (https.begin(*client, discordWebhook))
      { // HTTPS
        // start connection and send HTTP header
        https.addHeader("Content-Type", "application/json");
        String jcontent = "{\"content\":\"" + content + "\",\"tts\":" + TTS + "}";
        printf("Posting content = %s\n", jcontent.c_str());
        int httpCode = https.POST(jcontent);
        printf("http Code =%i\n", httpCode);
        printf("[HTTP] Response > %s\n", https.getString().c_str());
        // httpCode will be negative on error
        if (httpCode > 0)
        {
          // HTTP header has been send and Server response header has been handled
          printf("[HTTP] Status code:%i\n", httpCode);
          // file found at server
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
          {
            String payload = https.getString();
            Serial.print("[HTTP] Response: ");
            Serial.println(payload);
          }
        }
        else
        {
          Serial.print("[HTTP] Post... failed, error: ");
          Serial.println(https.errorToString(httpCode).c_str());
        }

        https.end();
      }
      else
      {
        Serial.printf("[HTTP] Unable to connect\n");
      }

      // End extra scoping block
    }

    delete client;
  }
  else
  {
    Serial.println("[HTTP] Unable to create client");
  }
}
void setup()
{
  // put your setup code here, to run once:
  connectWiFi();
  sendDiscord("උඹ සහ මම ");
  sendDiscord("Docker Run this and also this after that do some more texting text that is so long like this but that is not enough when quick brown fox jumps over the lazy dogs tail");
}
vector<string> split(string stringToBeSplitted, string delimeter)
{
  vector<string> splittedString;
  int startIndex = 0;
  int endIndex = 0;
  while ((endIndex = stringToBeSplitted.find(delimeter, startIndex)) < stringToBeSplitted.size())
  {

    string val = stringToBeSplitted.substr(startIndex, endIndex - startIndex);
    splittedString.push_back(val);
    startIndex = endIndex + delimeter.size();
  }
  if (startIndex < stringToBeSplitted.size())
  {
    std::string val = stringToBeSplitted.substr(startIndex);
    splittedString.push_back(val);
  }
  return splittedString;
}
void processInput()
{
  if (Serial.available())
  {

    String text = Serial.readString();

    if (text != NULL)
      if (Serial.availableForWrite())
      {
        std::vector<char> vx(text.begin(), text.end());
        char *ca = &vx[0];
        vector<string> x = split(ca, " ");

        for (auto i : x)
        {
          if (x[0] == "@ECHO")
          {
            if (i != "@ECHO")
              if (Serial.availableForWrite())
                printf("%s\n", i.c_str());
            printf("that was echoed \n");
          }
          if (x[0] == "@GET_DELAY")
            if (Serial.availableForWrite())
              printf("DELAY = %i\r\n", DELAY);
          if (x[0] == "@GET_PID")
            if (Serial.availableForWrite())
            {
              printf("@PID=(Kp=%.2lf)(Ki=%.2lf)(Kd=%.2lf)(SetPoint=%.2lf)\r\n", conKp, conKi, conKd, setPoint);
            }
          if (x[0] == "@SET_DELAY")
          {
            vector<string> y = split(ca, "=");
            DELAY = std::atoi(y[1].c_str());
          }
          if (x[0] == "@SET_POINT")
          {
            vector<string> y = split(ca, "=");
            setPoint = std::atoi(y[1].c_str());
          }
          if (x[0] == "@SET_KP")
          {
            printf("kp\n");
            printf("set kp = %.2lf\n", conKp);

            vector<string> y = split(ca, "=");
            conKp = atof(y[1].c_str());
            printf("@SET_kP =  %.2lf\n", conKp);
          }
          if (x[0] == "@SET_KI")
          {
            vector<string> y = split(ca, "=");
            conKi = atof(y[1].c_str());
            printf("@SET_kI =  %.2lf\n", conKi);
          }
          if (x[0] == "@SET_KD")
          {
            vector<string> y = split(ca, "=");
            conKd = atof(y[1].c_str());
            printf("@SET_kD =  %.2lf\n", conKd);
          }
          // if (x[0] == "@SET_WINDOW")
          // {
          // 	vector<string> y = split(ca, "=");
          // 	int n = atoi(y[1].c_str());
          // //	myPID.SetOutputLimits(0, n);
          // }
        }
      }
  }
}
String x = "";
void loop()
{

  processInput();
 
  delay(DELAY);
}