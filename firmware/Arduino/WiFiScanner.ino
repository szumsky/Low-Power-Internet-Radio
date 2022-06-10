/*
 *  This sketch demonstrates how to scan WiFi networks.
 *  The API is almost the same as with the WiFi Shield library,
 *  the most obvious difference being the different file you need to include:
 */
#include "WiFi.h"
#include "Arduino.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define OLED_MOSI     23
#define OLED_CLK      18
#define OLED_DC       16
#define OLED_CS       5
#define OLED_RST      17

#define enc_debounce 200
#define btn_debounce 200

const int Pin_btn_left    =   15;
const int Pin_btn_centre  =   12;
const int Pin_btn_right   =   4;
const int Pin_enc_a       =   35;
const int Pin_enc_b       =   32;
const int Pin_enc_btn     =   34;

int enc = 0;
int temp_enc = 0;
int enc_btn = 0;
int n = 0;
int btn = 0;

unsigned int bounce_time = 0;

boolean escape = false;
boolean scan = true;
boolean newSSID = true;

String characters = " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
String temp_password = "                                                            ";

char* ssid;
char* password;

char selected = ' ';

Adafruit_SH1106G display = Adafruit_SH1106G(128, 64,OLED_MOSI, OLED_CLK, OLED_DC, OLED_RST, OLED_CS);

void IRAM_ATTR ENC_ISR() 
{
      if (millis() - bounce_time > enc_debounce)
    {
        bounce_time = millis();
        if(digitalRead(Pin_enc_b) == HIGH)
        {
          enc--;
        }
        else
        {
          enc++;
        }
    }
}

void IRAM_ATTR ENC_BTN_ISR()
{
    if (millis() - bounce_time > btn_debounce)
    {
        bounce_time = millis();
        enc_btn++;
        temp_enc = enc;
        enc = 0;
    }
}

void IRAM_ATTR BTN_LEFT_ISR() {
      if (millis() - bounce_time > btn_debounce)
    {
        bounce_time = millis();
        btn--;
        enc = 0;
        if(btn < 0)
          btn = 0;
    }
}

void IRAM_ATTR BTN_RIGHT_ISR() {
      if (millis() - bounce_time > btn_debounce)
    {
        bounce_time = millis();
        btn++;
        enc = 0;
    }
}

void setup()
{
    pinMode(Pin_enc_a,      INPUT_PULLUP);
    pinMode(Pin_enc_b,      INPUT_PULLUP);
    pinMode(Pin_enc_btn,    INPUT_PULLUP);
    pinMode(Pin_btn_left,   INPUT_PULLUP);
    pinMode(Pin_btn_centre, INPUT_PULLUP);
    pinMode(Pin_btn_right,  INPUT_PULLUP);

    attachInterrupt(Pin_enc_a,      ENC_ISR,        FALLING);
    attachInterrupt(Pin_enc_btn,    ENC_BTN_ISR,    FALLING);
    attachInterrupt(Pin_btn_left,   BTN_LEFT_ISR,   FALLING);
    //attachInterrupt(Pin_btn_centre, BTN_CENTRE_ISR, FALLING);
    attachInterrupt(Pin_btn_right,  BTN_RIGHT_ISR,  FALLING);
  
    display.begin(0, true);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0, 0);
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    Serial.begin(115200);
}

void scanWIFI(void)
{
  if(scan)
    { 
              
      WiFi.disconnect();
      delay(100);
      display.clearDisplay();
      display.setCursor(0, 0);
      display.setTextColor(SH110X_WHITE);
      display.println("scan start");
      display.display();
      n = WiFi.scanNetworks();
      temp_enc = 0;
      enc = 0;
      scan = false;
      newSSID = true;
      temp_password = "                                                            ";
    }
    display.clearDisplay();
    display.setCursor(0, 0);
    if (n == 0) 
    {
      display.println("no networks found");
    } 
    else 
    {
      display.print(n);
      display.println(" networks found");
      for (int i = 0 + enc; i < n + enc; ++i) 
      {
          if(enc < 0)
            enc = n;
          if(enc > n)
            enc = 0;
          if(i == enc)
          {
            display.setTextColor(SH110X_BLACK, SH110X_WHITE);
          }
          else
          {
            display.setTextColor(SH110X_WHITE);
          }
          if(i < n)
          {
            display.print(i + 1);
            display.print(". ");
            display.println(WiFi.SSID(i));
          }
          else if(i == n)
          {
            display.println("Exit");
          }
      }
    }
    display.display();
}

void loginWIFI(void)
{
  if(newSSID)
  {
    btn = 0;
    String temp_ssid = WiFi.SSID(temp_enc);
    ssid = new char[temp_ssid.length()];
    for(int i = 0; i < (temp_ssid.length()); i++)
    {
      ssid[i] = temp_ssid[i];
    }
    ssid[temp_ssid.length()] = '\0';
    newSSID = false;
  }
  
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextColor(SH110X_WHITE);
  display.println("SSID: ");
  display.println(ssid);
  display.println("Password: ");

  for(int i = 0; i < (btn + 1); i++)
  {
    selected = (char)characters[enc];
    temp_password[btn] = selected;
    if(btn == i)
      display.setTextColor(SH110X_BLACK, SH110X_WHITE);
    else
      display.setTextColor(SH110X_WHITE);
    temp_password[btn + 1] = '\0';
    display.print(temp_password[i]);
  }
  display.display();
}

void connectWIFI(void)
{
  if(WiFi.status() != WL_CONNECTED)
  {
    password = new char[temp_password.length()];
    for(int i = 0; i < temp_password.length(); i++)
    {
      password[i] = temp_password[i];
    }
    password[temp_password.length()] = '\0';
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextColor(SH110X_WHITE);
    display.println("Connecting to: ");
    display.println(ssid);
    WiFi.begin(ssid, password);
    display.setTextColor(SH110X_BLACK, SH110X_WHITE);
    display.println("Exit");
    display.setTextColor(SH110X_WHITE);
    while (WiFi.status() != WL_CONNECTED) 
    {
        if(enc_btn > 2)
        {
          escape = true;
          break;
        }
        delay(500);
        display.print(".");
        display.display();
        
    }
    if(escape == false)
    {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextColor(SH110X_WHITE);
    display.println("WiFi connected");
    display.println(ssid);
    display.println("IP address: ");
    display.println(WiFi.localIP());
    display.display();
    }
    escape = false;
  }
}

void loop()
{
    switch(enc_btn)
    {
      case 0:
      {
        scanWIFI();
        break;
      }
      case 1:
      {
        if(temp_enc == n)
        {
          scan = true;
          enc_btn = enc_btn - 1;
          break;
        }
        loginWIFI();
        break;
      }
      case 2:
      {
        connectWIFI();
        break;
      }
      case 3:
      {
        enc_btn = 0;
        scan = true; 
        break;
      }
    }
}
