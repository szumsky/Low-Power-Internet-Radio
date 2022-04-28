#include "Arduino.h"
#include "Audio.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

//Digital I/O used  //Makerfabs Audio V2.0
#define I2S_DOUT 27 //orange pi 40
#define I2S_BCLK 26 //yellow pi 12
#define I2S_LRC 25  //green  pi 35
                    //amp on blue pi 22

#define OLED_MOSI     23
#define OLED_CLK      18
#define OLED_DC       16
#define OLED_CS       5
#define OLED_RST      17
//Button
const int Pin_vol_up = 4;
const int Pin_vol_down = 15;
//const int Pin_mute = 32;

//const int Pin_previous = 15;
//const int Pin_pause = 33;
//const int Pin_next = 2;
Adafruit_SH1106G display = Adafruit_SH1106G(128, 64,OLED_MOSI, OLED_CLK, OLED_DC, OLED_RST, OLED_CS);
Audio audio;

const char *ssid = "iPhone (Muody)";
const char *password = "abbaabba";


struct Music_info
{
    String name;
    int length;
    int runtime;
    int volume;
    int status;
    int mute_volume;
} music_info = {"", 0, 0, 0, 0, 0};

int volume = 10;
int mute_volume = 0;

unsigned int button_time = 0;

int runtime = 0;
int length = 0;

String stations[] = {
    "0n-80s.radionetz.de:8000/0n-70s.mp3"
    };
int station_index = 0;
int station_count = sizeof(stations) / sizeof(stations[0]);

void IRAM_ATTR VolUP_ISR() {
    if (millis() - button_time > 100)
    {
      button_time = millis();
      if(volume < 21)
      {
        volume++;
        audio.setVolume(volume);
        Serial.print("volume    ");
        Serial.println(volume);
      }
    }
}

void IRAM_ATTR VolDOWN_ISR() {
      if (millis() - button_time > 100)
    {
      button_time = millis();
      if(volume > 0)
      {
        volume--;
        audio.setVolume(volume);
        Serial.print("volume    ");
        Serial.println(volume);
      }
    }
}

void setup()
{
    //IO mode init
    pinMode(Pin_vol_up, INPUT_PULLUP);
    pinMode(Pin_vol_down, INPUT_PULLUP);
    //pinMode(Pin_mute, INPUT_PULLUP);
    //pinMode(Pin_previous, INPUT_PULLUP);
    //pinMode(Pin_pause, INPUT_PULLUP);
    //pinMode(Pin_next, INPUT_PULLUP);
    pinMode(13, OUTPUT);

    digitalWrite(13, HIGH);

    //ISR
    attachInterrupt(Pin_vol_up, VolUP_ISR, FALLING);
    attachInterrupt(Pin_vol_down, VolDOWN_ISR, FALLING);
    
    //Serial
    Serial.begin(115200);

    display.begin(0, true);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0, 0);

    //connect to WiFi
    display.printf("Connecting to %s ", ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        display.print(".");
        display.display();
    }
    Serial.println(" CONNECTED");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println(" CONNECTED");
    display.display();

    //Audio(I2S)
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(volume); // 0...21

    open_new_radio(stations[station_index]);

    
}

void loop()
{
    audio.loop();
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("VOLUME");
    display.println(volume);
    display.display();
    //print_song_time();

//    //Display logic
//    if (millis() - run_time > 1000)
//    {
//        run_time = millis();
//        display_music();
//    }
//
//    //Button logic
//    if (millis() - button_time > 300)
//    {
//        if (digitalRead(Pin_next) == 0)
//        {
//            Serial.println("Pin_next");
//            if (station_index < station_count - 1)
//            {
//                station_index++;
//            }
//            else
//            {
//                station_index = 0;
//            }
//            button_time = millis();
//            open_new_radio(stations[station_index]);
//        }
//        if (digitalRead(Pin_previous) == 0)
//        {
//            Serial.println("Pin_previous");
//            if (station_index > 0)
//            {
//                station_index--;
//            }
//            else
//            {
//                station_index = station_count - 1;
//            }
//            button_time = millis();
//            open_new_radio(stations[station_index]);
//        }
//        if (digitalRead(Pin_vol_up) == 0)
//        {
//            Serial.println("Pin_vol_up");
//            if (volume < 21)
//                volume++;
//            audio.setVolume(volume);
//            button_time = millis();
//        }
//        if (digitalRead(Pin_vol_down) == 0)
//        {
//            Serial.println("Pin_vol_down");
//            if (volume > 0)
//                volume--;
//            audio.setVolume(volume);
//            button_time = millis();
//        }
//        if (digitalRead(Pin_mute) == 0)
//        {
//            Serial.println("Pin_mute");
//            if (volume != 0)
//            {
//                mute_volume = volume;
//                volume = 0;
//            }
//            else
//            {
//                volume = mute_volume;
//            }
//            audio.setVolume(volume);
//            button_time = millis();
//        }
//        if (digitalRead(Pin_pause) == 0)
//        {
//            Serial.println("Pin_pause");
//            audio.pauseResume();
//            button_time = millis();
//        }
//    }

    //Serial logic
    if (Serial.available())
    {
        String r = Serial.readString();
        r.trim();
        if (r.length() > 5)
        {
            audio.stopSong();
            open_new_radio(r);
        }
        else
        {
            audio.setVolume(r.toInt());
        }
    }
}

void print_song_time()
{
    runtime = audio.getAudioCurrentTime();
    length = audio.getAudioFileDuration();
}

void open_new_radio(String station)
{
    audio.connecttohost(station);
    runtime = audio.getAudioCurrentTime();
    length = audio.getAudioFileDuration();
    Serial.println("**********start a new radio************");
}

//void display_music()
//{
//    int line_step = 24;
//    int line = 0;
//    char buff[20];
//    ;
//    sprintf(buff, "RunningTime:%d",runtime);
//
//    display.clearDisplay();
//
//    display.setTextSize(1);              // Normal 1:1 pixel scale
//    display.setTextColor(SSD1306_WHITE); // Draw white text
//
//    display.setCursor(0, line); // Start at top-left corner
//    display.println(stations[station_index]);
//    line += line_step * 2;
//
//    display.setCursor(0, line);
//    display.println(buff);
//    line += line_step;
//
//    display.display();
//}

//void logoshow(void)
//{
//    display.clearDisplay();
//
//    display.setTextSize(2);              // Normal 1:1 pixel scale
//    display.setTextColor(SSD1306_WHITE); // Draw white text
//    display.setCursor(0, 0);             // Start at top-left corner
//    display.println(F("MakePython"));
//    display.setCursor(0, 20); // Start at top-left corner
//    display.println(F("WEB RADIO"));
//    display.setCursor(0, 40); // Start at top-left corner
//    display.println(F(""));
//    display.display();
//    delay(2000);
//}

//void lcd_text(String text)
//{
//    display.clearDisplay();
//
//    display.setTextSize(2);              // Normal 1:1 pixel scale
//    display.setTextColor(SSD1306_WHITE); // Draw white text
//    display.setCursor(0, 0);             // Start at top-left corner
//    display.println(text);
//    display.display();
//    delay(500);
//}

//**********************************************
// optional
void audio_info(const char *info)
{
    Serial.print("info        ");
    Serial.println(info);
}
void audio_id3data(const char *info)
{ //id3 metadata
    Serial.print("id3data     ");
    Serial.println(info);
}

void audio_eof_mp3(const char *info)
{ //end of file
    Serial.print("eof_mp3     ");
    Serial.println(info);
}
void audio_showstation(const char *info)
{
    Serial.print("station     ");
    Serial.println(info);
}
void audio_showstreaminfo(const char *info)
{
    Serial.print("streaminfo  ");
    Serial.println(info);
}
void audio_showstreamtitle(const char *info)
{
    Serial.print("streamtitle ");
    Serial.println(info);
}
void audio_bitrate(const char *info)
{
    Serial.print("bitrate     ");
    Serial.println(info);
}
void audio_commercial(const char *info)
{ //duration in sec
    Serial.print("commercial  ");
    Serial.println(info);
}
void audio_icyurl(const char *info)
{ //homepage
    Serial.print("icyurl      ");
    Serial.println(info);
}
void audio_lasthost(const char *info)
{ //stream URL played
    Serial.print("lasthost    ");
    Serial.println(info);
}
void audio_eof_speech(const char *info)
{
    Serial.print("eof_speech  ");
    Serial.println(info);
}
