#include "Arduino.h"
#include "Audio.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Preferences.h>
#include <ESP32Encoder.h> 

//Digital I/O used  //Makerfabs Audio V2.0
#define I2S_DOUT   27  //27 //orange pi 40
#define I2S_BCLK   14  //26 //yellow pi 12
#define I2S_LRC    12  //25 //green  pi 35
#define AMPON      26  //13 //amp on blue pi 22

#define OLED_MOSI  23  //23
#define OLED_CLK   22  //18
#define OLED_DC    33  //16
#define OLED_CS    32  //5
#define OLED_RST   25  //17

#define LED 2
#define PWR_CTRL 17

#define ADC_IN          34//2
#define ADC_ENABLE      15
#define batteryCharge   18
#define batteryStandby  19

#define encDebounce 5
#define btnDebounce 500

#define modeReadWrite false
#define modeRead true

#define BUTTON_PIN_BITMASK 0x2000

// 'startup_logo', 128x64px
const unsigned char startup_logo [] PROGMEM = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xea, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xab, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0x80, 0xc0, 0x00, 0x00, 0x3f, 0x80, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 
	0xc0, 0xc0, 0x00, 0x00, 0x30, 0xc0, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0x01, 
	0x80, 0xc0, 0x00, 0x00, 0x30, 0xc0, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 
	0xc0, 0xc0, 0x79, 0x81, 0xb0, 0xcf, 0x30, 0x33, 0xc7, 0x83, 0x00, 0x00, 0x00, 0x00, 0x03, 0x01, 
	0x80, 0xc0, 0xcd, 0x99, 0xb0, 0xd9, 0xb3, 0x36, 0x67, 0x03, 0x00, 0x00, 0x01, 0xe0, 0x03, 0x03, 
	0xc0, 0xc0, 0xcc, 0xdb, 0x3f, 0x99, 0x9b, 0x67, 0xe6, 0x03, 0x00, 0x00, 0x03, 0xf0, 0x03, 0x01, 
	0x80, 0xc0, 0xcc, 0xdb, 0x30, 0x19, 0x9b, 0x66, 0x06, 0x03, 0x00, 0x00, 0x07, 0x38, 0x03, 0x03, 
	0xc0, 0xc0, 0xcc, 0xff, 0x30, 0x19, 0x9f, 0xe6, 0x06, 0x03, 0x00, 0x00, 0x06, 0x1f, 0xff, 0x01, 
	0x80, 0xc0, 0xcc, 0x66, 0x30, 0x19, 0x8c, 0xc6, 0x66, 0x03, 0xff, 0xfe, 0x06, 0x1f, 0xff, 0x03, 
	0xc0, 0xfe, 0x78, 0x66, 0x30, 0x0f, 0x0c, 0xc3, 0xc6, 0x03, 0xff, 0xfe, 0x07, 0x38, 0x03, 0x01, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x06, 0x03, 0xf0, 0x03, 0x03, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x06, 0x01, 0xe0, 0x03, 0x01, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x06, 0x00, 0x00, 0x00, 0x03, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x06, 0x00, 0x00, 0x00, 0x01, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xc6, 0x01, 0xe0, 0x00, 0x03, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xc6, 0x03, 0xf0, 0x00, 0x01, 
	0x80, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc6, 0x07, 0x38, 0x00, 0x03, 
	0xc0, 0xc0, 0x0c, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0xc7, 0xfe, 0x18, 0x00, 0x01, 
	0x80, 0xc0, 0x0c, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0xc7, 0xfe, 0x18, 0x00, 0x03, 
	0xc0, 0xcf, 0x9e, 0x78, 0xf7, 0xc3, 0xcf, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x07, 0x38, 0x00, 0x01, 
	0x80, 0xcc, 0xcc, 0xcc, 0xe6, 0x66, 0x66, 0x00, 0x00, 0x00, 0x01, 0xe0, 0x03, 0xf0, 0x00, 0x03, 
	0xc0, 0xcc, 0xcc, 0xfc, 0xc6, 0x67, 0xe6, 0x00, 0x00, 0x00, 0x03, 0xf0, 0x01, 0xe0, 0x00, 0x01, 
	0x80, 0xcc, 0xcc, 0xc0, 0xc6, 0x66, 0x06, 0x00, 0x00, 0x00, 0x07, 0x38, 0x00, 0x00, 0x00, 0x03, 
	0xc0, 0xcc, 0xcc, 0xc0, 0xc6, 0x66, 0x06, 0x00, 0x00, 0x03, 0xfe, 0x18, 0x00, 0x07, 0xff, 0x01, 
	0x80, 0xcc, 0xcc, 0xcc, 0xc6, 0x66, 0x66, 0x00, 0x00, 0x03, 0xfe, 0x18, 0x00, 0x07, 0xff, 0x03, 
	0xc0, 0xcc, 0xc6, 0x78, 0xc6, 0x63, 0xc3, 0x00, 0x00, 0x03, 0x07, 0x38, 0x00, 0x00, 0x00, 0x01, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0xf0, 0x01, 0xe0, 0x00, 0x03, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x01, 0xe0, 0x03, 0xf0, 0x00, 0x01, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x07, 0x38, 0x03, 0x03, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x3e, 0x18, 0x03, 0x01, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x3e, 0x18, 0x03, 0x03, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x37, 0x38, 0x03, 0x01, 
	0x80, 0xfe, 0x00, 0x03, 0x30, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x33, 0xf0, 0x03, 0x03, 
	0xc0, 0xc3, 0x00, 0x03, 0x30, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x00, 0x31, 0xe0, 0x07, 0x01, 
	0x80, 0xc3, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xe0, 0x00, 0x30, 0x00, 0x1e, 0x03, 
	0xc0, 0xc3, 0x1e, 0x1f, 0x31, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x30, 0x00, 0x78, 0x01, 
	0x80, 0xc3, 0x33, 0x33, 0x33, 0x30, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x30, 0x03, 0xe0, 0x03, 
	0xc0, 0xfe, 0x0f, 0x33, 0x33, 0x30, 0x00, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x30, 0x0f, 0x00, 0x01, 
	0x80, 0xc3, 0x1b, 0x33, 0x33, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x30, 0x7c, 0x00, 0x03, 
	0xc0, 0xc3, 0x33, 0x33, 0x33, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x31, 0xf0, 0x00, 0x01, 
	0x80, 0xc3, 0x33, 0x33, 0x33, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0x80, 0x00, 0x03, 
	0xc0, 0xc1, 0x9f, 0x1f, 0x31, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xfe, 0x00, 0x00, 0x01, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 
	0xd5, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x57, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

// 'sleep_logo', 128x64px
const unsigned char sleep_logo [] PROGMEM = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xea, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xab, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0x01, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x03, 0x01, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x01, 0xe0, 0x03, 0x03, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x03, 0xf0, 0x03, 0x01, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x07, 0x38, 0x03, 0x03, 
	0xc0, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x03, 0xc1, 0x98, 0x03, 0x00, 0x00, 0x06, 0x1f, 0xff, 0x01, 
	0x80, 0xc3, 0x00, 0x00, 0x00, 0x00, 0x06, 0x63, 0x30, 0x03, 0xff, 0xfe, 0x06, 0x1f, 0xff, 0x03, 
	0xc0, 0xc3, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x33, 0x30, 0x03, 0xff, 0xfe, 0x07, 0x38, 0x03, 0x01, 
	0x80, 0xc3, 0x3c, 0xc0, 0xcf, 0x1e, 0x0c, 0x37, 0xf8, 0x03, 0x00, 0x06, 0x03, 0xf0, 0x03, 0x03, 
	0xc0, 0xc3, 0x66, 0xcc, 0xd9, 0x9c, 0x0c, 0x33, 0x30, 0x03, 0x00, 0x06, 0x01, 0xe0, 0x03, 0x01, 
	0x80, 0xfe, 0x66, 0x6d, 0x9f, 0x98, 0x0c, 0x33, 0x30, 0x03, 0x00, 0x06, 0x00, 0x00, 0x00, 0x03, 
	0xc0, 0xc0, 0x66, 0x6d, 0x98, 0x18, 0x0c, 0x33, 0x30, 0x03, 0x00, 0x06, 0x00, 0x00, 0x00, 0x01, 
	0x80, 0xc0, 0x66, 0x7f, 0x98, 0x18, 0x0c, 0x33, 0x30, 0x03, 0xff, 0xc6, 0x01, 0xe0, 0x00, 0x03, 
	0xc0, 0xc0, 0x66, 0x33, 0x19, 0x98, 0x06, 0x63, 0x30, 0x03, 0xff, 0xc6, 0x03, 0xf0, 0x00, 0x01, 
	0x80, 0xc0, 0x3c, 0x33, 0x0f, 0x18, 0x03, 0xc3, 0x30, 0x00, 0x00, 0xc6, 0x07, 0x38, 0x00, 0x03, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc7, 0xfe, 0x18, 0x00, 0x01, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc7, 0xfe, 0x18, 0x00, 0x03, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x07, 0x38, 0x00, 0x01, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xe0, 0x03, 0xf0, 0x00, 0x03, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xf0, 0x01, 0xe0, 0x00, 0x01, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x38, 0x00, 0x00, 0x00, 0x03, 
	0xc0, 0x3e, 0x00, 0x00, 0x03, 0x30, 0x00, 0x00, 0x00, 0x03, 0xfe, 0x18, 0x00, 0x07, 0xff, 0x01, 
	0x80, 0x63, 0x00, 0x00, 0x03, 0x30, 0x00, 0x00, 0x00, 0x03, 0xfe, 0x18, 0x00, 0x07, 0xff, 0x03, 
	0xc0, 0xc1, 0x00, 0x00, 0x03, 0x30, 0x00, 0x00, 0x00, 0x03, 0x07, 0x38, 0x00, 0x00, 0x00, 0x01, 
	0x80, 0xc0, 0x1e, 0x1e, 0x1f, 0x3e, 0x61, 0x9e, 0x00, 0x03, 0x03, 0xf0, 0x01, 0xe0, 0x00, 0x03, 
	0xc0, 0xc0, 0x33, 0x33, 0x33, 0x33, 0x61, 0xb3, 0x00, 0x03, 0x01, 0xe0, 0x03, 0xf0, 0x00, 0x01, 
	0x80, 0xcf, 0x33, 0x33, 0x33, 0x33, 0x33, 0x3f, 0x00, 0x03, 0x00, 0x00, 0x07, 0x38, 0x03, 0x03, 
	0xc0, 0xc3, 0x33, 0x33, 0x33, 0x33, 0x33, 0x30, 0x00, 0x03, 0x00, 0x00, 0x3e, 0x18, 0x03, 0x01, 
	0x80, 0xc3, 0x33, 0x33, 0x33, 0x33, 0x1e, 0x30, 0x00, 0x03, 0x00, 0x00, 0x3e, 0x18, 0x03, 0x03, 
	0xc0, 0x63, 0x33, 0x33, 0x33, 0x33, 0x1e, 0x33, 0x00, 0x03, 0x00, 0x00, 0x37, 0x38, 0x03, 0x01, 
	0x80, 0x3f, 0x1e, 0x1e, 0x1f, 0x3e, 0x0c, 0x1e, 0x00, 0x03, 0x00, 0x00, 0x33, 0xf0, 0x03, 0x03, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x03, 0x80, 0x00, 0x31, 0xe0, 0x07, 0x01, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x01, 0xe0, 0x00, 0x30, 0x00, 0x1e, 0x03, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x78, 0x00, 0x30, 0x00, 0x78, 0x01, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x30, 0x03, 0xe0, 0x03, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x30, 0x0f, 0x00, 0x01, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x30, 0x7c, 0x00, 0x03, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x31, 0xf0, 0x00, 0x01, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0x80, 0x00, 0x03, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xfe, 0x00, 0x00, 0x01, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 
	0xd5, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x57, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

Adafruit_SH1106G display = Adafruit_SH1106G(128, 64, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RST, OLED_CS);
Audio audio;
ESP32Encoder encoder;

//flash variables
Preferences preferences;

String ssid;
String pass;

//OLED animation variables
unsigned int previousMillis, currentMillis;

//encoder variables
volatile unsigned int bounceTime = 0;
bool encChange;
volatile int enc = 0;
volatile int encTemp = 0;
volatile int encBtn = 0;
const int Pin_enc_a       =   39; //35;
const int Pin_enc_b       =   36; //32;
const int Pin_enc_btn     =   13; //34;

//button variables
volatile int btnLeft;
volatile int btnCenter;
volatile int btnRight;
const int Pin_btn_left    =   5;  //15;
const int Pin_btn_center  =   4;  //12;
const int Pin_btn_right   =   16; //4;
unsigned int buttonTime;

//Scroller variables
unsigned int waitScrollTime, scrollTime, stopScrollTime;
unsigned int nextChar;
bool scrollActive = false;

//Info Strings
String title;

//menu variables
int signalLevel = 0;
unsigned int menuX = 0;
unsigned int menuY = 0;
String settingsText[2] = {"Network Settings", "Stations Manager"};
String networkSettingsText[2] = {"Add Network", "Saved Networks"};

//volume variables
int volume = 10;
unsigned int volumeTime, volumeEndTime;
bool volumeChange = false;
int tempVolume = volume;

//scanner variables
unsigned int networksCount = 0;
unsigned int chosenNetwork = 0;
bool scanNetworks;

//password typer variables
String characters = " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
String tempPassword = "                                                            ";
char selected = ' ';
int charPosition = 0;

//connectToNetwork variables
String currentSSID;
bool startConnect = false;
bool connecting = false;
unsigned int connectDot = 0;
unsigned int connectTime;
unsigned int connectDotTime;
char* password;
char* SSID;

//volumeFade variables
unsigned int fadeUpTime;
unsigned int fadeDownTime;
unsigned int waitFadeTime;
bool streamPlaying;
bool fadeUpStart = false;
bool fadeDownStart = false;
bool fadeUpComplete = false;
bool fadeDownComplete = false;

//savedNetworks variables
unsigned int savedNetworksTime;
unsigned int savedNetworksTimeCount = 0;

//longPress variables
int lastState = LOW;  // the previous state from the input pin
int currentState;     // the current reading from the input pin
unsigned long pressedTime  = 0;
bool isPressing = false;
bool isLongDetected = false;
bool ledState = false;

//deep sleep variables
bool sleepStart = false;
int counter = 0;

//station variables
String viewStationName;
String stations[10];
String stationNames[10];
int stationIndex = 0;
int tempStationIndex = 0;
int stationCount = sizeof(stations) / sizeof(stations[0]);
bool streamReady;
bool streamConnected;

//station manager variables
bool stationSelected = false;
const int defaultStationsCount = 31;
unsigned int selectedStations[defaultStationsCount];
unsigned int selectedStationsCount = 0;
unsigned int userSaveTime;
unsigned int userSaveCounter = 0;
bool savePreset = false;

//battery variables
unsigned int batteryLevel = 0;
unsigned int adcResult = 0;
unsigned int checkBatteryTime;
unsigned int dividerTime;
int dividerCount = 0;
int measurement = 0;
bool checkBatteryState = false;

void IRAM_ATTR ENC_ISR() 
{
  // if (millis() - bounceTime > btnDebounce)
  // {
  //   bounceTime = millis();
  //   if(menuX == 0 && menuY == 0)
  //   {
  //     volumeChange = true;
  //     enc = volume;
  //     volumeTime = millis();
  //   }
  // }
}

void IRAM_ATTR ENC_BTN_ISR()
{
  if (millis() - bounceTime > btnDebounce)
  {
    bounceTime = millis();
    encBtn++;

    if(stationSelected)
    {
      if(selectedStations[enc] == 1 && selectedStationsCount <= 10)
        selectedStations[enc] = 0;
      else if(selectedStations[enc] == 0 && selectedStationsCount <= 9)
        selectedStations[enc] = 1;
    }
    stationSelected = false;
    encTemp = enc;
    enc = 0;
  }
}

void IRAM_ATTR BTN_LEFT_ISR() 
{
      if (millis() - bounceTime > btnDebounce)
    {
        bounceTime = millis();
        btnLeft++;
        encTemp = enc;
        enc = 0;
    }
}

void IRAM_ATTR BTN_CENTER_ISR() 
{
      if (millis() - bounceTime > btnDebounce)
    {
        bounceTime = millis();
        btnCenter++;
        encTemp = enc;
        enc = 0;
    }
}

void IRAM_ATTR BTN_RIGHT_ISR() 
{
      if (millis() - bounceTime > btnDebounce)
    {
        bounceTime = millis();
        btnRight++;
        encTemp = enc;
        enc = 0;
        savePreset = true;
    }
}

void setup()
{
    //IO mode init
    pinMode(Pin_enc_a,      INPUT_PULLUP);
    pinMode(Pin_enc_b,      INPUT_PULLUP);
    pinMode(Pin_enc_btn,    INPUT_PULLUP);
    pinMode(Pin_btn_left,   INPUT_PULLUP);
    pinMode(Pin_btn_center, INPUT_PULLUP);
    pinMode(Pin_btn_right,  INPUT_PULLUP);
    pinMode(AMPON, OUTPUT);
    pinMode(PWR_CTRL, OUTPUT);
    pinMode(0, INPUT_PULLUP);
    pinMode(34, INPUT);
    pinMode(15, OUTPUT);
    pinMode(batteryCharge, INPUT_PULLUP);
    pinMode(batteryStandby, INPUT);

    analogSetAttenuation(ADC_11db);

    encoder.attachHalfQuad ( 36, 39 );
    encoder.setCount ( 0 );

    digitalWrite(AMPON, HIGH); //amp on
    digitalWrite(PWR_CTRL, HIGH);
    digitalWrite(15, LOW);

    //ISR
    //attachInterrupt(digitalPinToInterrupt(Pin_enc_a),      ENC_ISR,        FALLING);
    attachInterrupt(digitalPinToInterrupt(Pin_enc_btn),    ENC_BTN_ISR,    FALLING);
    attachInterrupt(digitalPinToInterrupt(Pin_btn_left),   BTN_LEFT_ISR,   FALLING);
    attachInterrupt(digitalPinToInterrupt(Pin_btn_center), BTN_CENTER_ISR, FALLING);
    attachInterrupt(digitalPinToInterrupt(Pin_btn_right),  BTN_RIGHT_ISR,  FALLING);
    
    //Serial
    Serial.begin(115200);

    display.begin(0, true);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0, 0);

    //Audio(I2S)
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(volume); // 0...21 

    //enable wake up sources
    esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK,ESP_EXT1_WAKEUP_ALL_LOW); 

    //flash
    flashRead();
    flashReadUserPreset();

    //initalize variables
    menuX = 0;
    menuY = 0;
    enc = tempVolume;
    encTemp = tempVolume;
    encBtn = 0;
    btnLeft = 0;
    btnCenter = 0;
    btnRight = 0;
    sleepStart = false;
    volume = tempVolume;

    //connect to WiFi
    if (ssid == "" || pass == "")
    {
      Serial.println("last Network empty");
      display.println("last Network empty");
      display.display();
      delay(2000);
    }
    else
    {
      display.drawBitmap(0, 0, startup_logo, 128, 64, 1);
      display.display();
      delay(2000);
      startConnect = true;
      connectToNetwork(ssid, pass, false);
    } 
}

void loop()
{
  enc = encoder.getCount() / 2;
  
  checkLongPress();
  audio.loop();
  currentMillis = millis();

  if(sleepStart)
  {
    fadeDownStart = true;
    volumeFadeDown();
    display.clearDisplay();
    display.drawBitmap(0, 0, sleep_logo, 128, 64, 1);
    display.display();

    if(fadeDownComplete && fadeDownStart)
    {
      WiFi.disconnect();
      digitalWrite(AMPON, LOW);
      flashDump();
      delay(2000);
      digitalWrite(PWR_CTRL, LOW);
      esp_deep_sleep_start();
    }
  }
  else
  {
    //batteryLevel
    if(currentMillis - checkBatteryTime > 10000)
    {
      if(checkBatteryState == false)
        checkBatteryState = true;
      checkBatteryTime = millis();
    }
    checkBattery();

    //menu logic
    if(menuX == 0 && menuY == 0) //main //volume
    {
      if(fadeDownStart == false && fadeUpStart == false )
        if(enc > tempVolume || enc < tempVolume)
          volumeChange = true;

      if(fadeDownStart == true || fadeUpStart == true )
        volumeChange = false;

      if(volumeChange)
      {
        if(currentMillis - volumeTime > 2000)
        {
          volumeChange = false;
          volumeTime = millis();
          
        }
        
        drawVolume();
      }
      else
      {
        if(tempStationIndex > stationIndex || tempStationIndex < stationIndex)
        {
          fadeDownStart = true;
          tempStationIndex = stationIndex;
          
        }
        volumeFadeDown();
        if(fadeDownComplete && fadeDownStart)
        {
          fadeDownStart = false;
          streamConnected = audio.connecttohost(stations[stationIndex]);
          viewStationName = stationNames[stationIndex];
          fadeUpStart = true;
        }
        if(streamPlaying && fadeUpStart)
          volumeFadeUp();
        if(fadeUpComplete && fadeUpStart)
          fadeUpStart = false;
        
        drawMainView();
        if(btnCenter > 0)
        {
          menuX = 1;
          btnCenter = 0;
        }
      }
      if(btnLeft > 0)
      {
        stationIndex--;
        if(stationIndex < 0)
          stationIndex = stationCount - 1;
        btnLeft = 0;
      }
      if(btnRight > 0)
      {
        stationIndex++;
        if(stationIndex >= stationCount)
          stationIndex = 0;
        btnRight = 0;
      }
    }
    else if(menuX == 1 && menuY == 0) //settings
    {
      drawSettings();
      if(encTemp == 0 && encBtn > 0)
      {
        menuX = 2;
        encBtn = 0;
      }
      if(encTemp == 1 && encBtn > 0)
      {
        menuX = 3;
        encBtn = 0;
      }
      if(btnLeft > 0)
      {
        menuX = 0;
        btnLeft = 0;
        encoder.setCount ( tempVolume * 2 );
      }   
    }
    else if(menuX == 2 && menuY == 0) //network settings
    {   
      drawNetworkSettings();
      if(encTemp == 0 && encBtn > 0)
      {
        menuY = 1;
        encBtn = 0;
      }
      if(encTemp == 1 && encBtn > 0)
      {
        menuY = 4;
        encBtn = 0;
      }
      if(btnLeft > 0)
      {
        menuX = 1;
        btnLeft = 0;
      }
    }
    else if(menuX == 2 && menuY == 1) //network scanner
    {
      drawNetworkScanner();
      if(encBtn > 0)
      {
        menuY = 2;
        encBtn = 0;
      }   
      if(btnLeft > 0)
      {
        menuY = 0;
        btnLeft = 0;
      }   
      chosenNetwork = encTemp;
    }
    else if(menuX == 2 && menuY == 2) //password typer
    {
      drawPasswordTyper();
      if(menuY == 1)
      {
        btnCenter = 0;
        btnRight = 0;
        tempPassword = "                                                            ";
        charPosition = 0;
      }
      if(encBtn > 0)
      {
        startConnect = true;
        fadeDownStart = true;
        menuY = 3;
        encBtn = 0;
      }
      if(btnLeft > 0)
      {
        menuY = 1;
        btnLeft = 0;
      }
    }
    else if(menuX == 2 && menuY == 3) //wifi connect
    {
      volumeFadeDown();
      if(fadeDownComplete)
      {
        fadeDownStart = false;
        connectToNetwork(WiFi.SSID(chosenNetwork), tempPassword, true);
      }
        
      if(btnLeft > 0)
      {
        menuY = 2;
        btnLeft = 0;
      }
    }
    else if(menuX == 2 && menuY == 4) //saved networks
    {
      drawSavedNetworks();
      if(btnLeft > 0)
      {
        menuY = 0;
        btnLeft = 0;
      }
    }
    else if(menuX == 3 && menuY == 0) //station manager
    {
      drawStationManager();
      if(btnLeft > 0)
      {
        menuX = 1;
        btnLeft = 0;
      }   
    }
  }

  //Serial input logic
  // if (Serial.available())
  // {
  //   String r = Serial.readString();
  //   r.trim();
  //   if (r.length() > 5)
  //   {
  //       audio.stopSong();
  //       open_new_radio(r);
  //   }
  //   else
  //   {
  //       audio.setVolume(r.toInt());
  //   }
  // }

  // //button debug
  // if(currentMillis - buttonTime > 1000)
  // {
  //   //viewButtonState();
  //   //Serial.println(xPortGetCoreID());
  //   buttonTime = millis();
  // }
}

void checkBattery()
{
  if(checkBatteryState)
  {
    if(dividerCount < 10)
    {
      digitalWrite(15, HIGH);
      if(currentMillis - dividerTime > 500)
      {
        dividerCount++;
        dividerTime = millis();
        Serial.println(dividerCount);
      }
    }

    if(dividerCount >= 10)
    {
      adcResult = adcResult + analogRead(34);
      measurement++;
      if(measurement > 63)
      { 
        measurement = 0;
        dividerCount = 0;
        adcResult = adcResult / 64;
        batteryLevel = map(adcResult,0,4095,0,10);
        checkBatteryState = false;
        digitalWrite(15, LOW);
        Serial.println(batteryLevel);
        Serial.println(adcResult);
      }
    }   
  }
}

void open_new_radio(String station)
{
    audio.connecttohost(station);
    Serial.println("**********start a new radio************");
}

//OLED Draw routines
void drawMainView()
{
  display.clearDisplay();

  //antenna
  display.drawFastVLine(0,0,2, 1);
  display.drawPixel(1,2, 1);
  display.drawFastVLine(2,0,7, 1);
  display.drawPixel(3,2, 1);
  display.drawFastVLine(4,0,2, 1);

  //wifi signal
  if(WiFi.status() == WL_CONNECTED)
  {
    signalLevel = WiFi.RSSI();
    if(signalLevel >= -90)
      display.drawFastVLine(5,4,3, 1);
    if(signalLevel >= -80)
      display.drawFastVLine(7,3,4, 1);
    if(signalLevel >= -75)
      display.drawFastVLine(9,2,5, 1);
    if(signalLevel >= -70)
      display.drawFastVLine(11,1,6, 1);
    if(signalLevel >= -65)
      display.drawFastVLine(13,0,7, 1);
    display.setCursor(15, 0);
    stringShorter(currentSSID, 16);
  }
  else
  {
    display.setCursor(6, 0);
    display.println("Disconnected");
    display.setCursor(0, 10);
    display.println("Connect to WiFi");
  }
  
  //battery
  display.drawRect(114, 0, 14, 7, 1);
  display.drawFastVLine(113, 1, 5, 1);
  display.fillRect(126 - batteryLevel, 2, batteryLevel, 3, 1);

  if(digitalRead(batteryCharge) == LOW)
  { 
    //chargingAnimation
    if (currentMillis - previousMillis >= 100) {
      previousMillis = currentMillis;
      batteryLevel++;
      if(batteryLevel > 10)
        batteryLevel = 0;
    }
  }

  //context buttons
  display.setCursor(10, 57);
  display.println("PREV");

  display.setCursor(53, 57);
  display.println("MENU");

  display.setCursor(95, 57);
  display.println("NEXT");

  //brackets
  display.drawFastHLine(0,8,128,SH110X_WHITE);
  display.drawFastHLine(0,55,128,SH110X_WHITE);
  display.drawFastVLine(43,55,9,SH110X_WHITE);
  display.drawFastVLine(85,55,9,SH110X_WHITE);

  //audio info
  if(streamPlaying && (fadeDownStart == false) && (WiFi.status() == WL_CONNECTED) && streamConnected == true)
  {
    display.setCursor(0, 10);
    display.println(viewStationName);
    display.setCursor(0, 38);
    display.println("Now playing:");
    display.setCursor(0, 46);
    stringScroller(title, 21);
  }
  else if(WiFi.status() == WL_CONNECTED && streamConnected == true)
  {
    display.setCursor(0, 10);
    display.println("Connecting to stream:");
    display.println(viewStationName);
  }
  else if(WiFi.status() == WL_CONNECTED && streamConnected == false)
  {
    display.setCursor(0, 10);
    display.println(viewStationName);
    display.println("Request Failed.");
  }
  display.display();
}

void drawVolume()
{
    display.clearDisplay();

    if(enc > 21)
      encoder.setCount (21 * 2);
      //enc = 21;
    if(enc < 0)
      encoder.setCount ( 0 );
      //enc = 0;

    volume = enc;
    tempVolume = volume;
    audio.setVolume(volume);

    display.setCursor(44, 30);
    display.println("VOLUME");

    unsigned int val = map(volume, 0, 21, 0, 100);

    display.drawRoundRect(14, 38, 100, 8, 4, 1);
    if(volume > 0)
      display.fillRoundRect(14, 38, val, 8, 4, 1);
    display.display();
}

void drawSettings()
{
  display.clearDisplay();

  //Menu title
  display.setCursor(0, 0);
  display.setTextColor(SH110X_WHITE);
  display.println("Settings");

  display.setCursor(0, 10);

  if(enc < 0)
    encoder.setCount (1 * 2);
    //enc = 1;
  if(enc > 1)
    encoder.setCount ( 0 );
    //enc = 0;

  for(int i = 0; i < 2; i++)
  {
    if(enc == i)
    {
      display.setTextColor(SH110X_BLACK, SH110X_WHITE);
    }
    else
    {
      display.setTextColor(SH110X_WHITE);
    }
    display.println(settingsText[i]);
  }

  //brackets
  display.drawFastHLine(0,8,128,SH110X_WHITE);
  display.drawFastHLine(0,55,128,SH110X_WHITE);
  display.drawFastVLine(43,55,9,SH110X_WHITE);
  display.drawFastVLine(85,55,9,SH110X_WHITE);

  //context buttons
  display.setCursor(10, 57);
  display.setTextColor(SH110X_WHITE);
  display.println("EXIT");

  display.display();
}

void drawNetworkSettings()
{
  display.clearDisplay();

  //Menu title
  display.setCursor(0, 0);
  display.setTextColor(SH110X_WHITE);
  display.println("Network Settings");

  display.setCursor(0, 10);

  if(enc < 0)
    encoder.setCount (1);
    //enc = 1;
  if(enc > 1)
    encoder.setCount ( 0 );
    //enc = 0;

  for(int i = 0; i < 2; i++)
  {
    if(enc == i)
    {
      display.setTextColor(SH110X_BLACK, SH110X_WHITE);
    }
    else
    {
      display.setTextColor(SH110X_WHITE);
    }
    display.println(networkSettingsText[i]);
  }

  //brackets
  display.drawFastHLine(0,8,128,SH110X_WHITE);
  display.drawFastHLine(0,55,128,SH110X_WHITE);
  display.drawFastVLine(43,55,9,SH110X_WHITE);
  display.drawFastVLine(85,55,9,SH110X_WHITE);

  //context buttons
  display.setCursor(10, 57);
  display.setTextColor(SH110X_WHITE);
  display.println("EXIT");

  display.display();
}

void drawNetworkScanner()
{
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextColor(SH110X_WHITE);
  
  if(WiFi.scanComplete())
  {
    display.printf("%i networks found", networksCount);
  }

  //brackets
  display.drawFastHLine(0,8,128,SH110X_WHITE);
  display.drawFastHLine(0,55,128,SH110X_WHITE);
  display.drawFastVLine(43,55,9,SH110X_WHITE);
  display.drawFastVLine(85,55,9,SH110X_WHITE);

  //context buttons
  display.setCursor(10, 57);
  display.setTextColor(SH110X_WHITE);
  display.println("EXIT");
  display.setCursor(95, 57);
  display.println("SCAN");

  if(fadeDownComplete && fadeDownStart == false)
  {
    volumeFadeUp();
    scanNetworks = false;
    if(fadeUpComplete)
    {
      fadeUpStart = false;
      if(WiFi.scanComplete())
      {
        display.setCursor(0, 10);
        for (int j = 0 + enc; j < 5 + enc; ++j)
        {   
          unsigned int width = 0;   
          if(j > 9)
            width = 17;
          else
            width = 18;  
          
          bool active = false;        
          if(enc < 0)
            encoder.setCount ( networksCount * 2);
            //enc = networksCount;
          if(enc >= networksCount)
            encoder.setCount ( 0 );
            //enc = 0;
          if(j == enc)
          {
            display.setTextColor(SH110X_BLACK, SH110X_WHITE);
            active = true;          
          }
          else
          {
            display.setTextColor(SH110X_WHITE);
            active = false;
          }
          if(j < networksCount)
          {
            display.print(j + 1);
            display.print(". ");
            if(active)
              stringScroller(WiFi.SSID(j), width);
            else
              for (int k = 0; k < width; k++ ) 
              {
                display.print(WiFi.SSID(j)[k]);
              }     
          }
        }
      }
    } 
  }

  if(btnRight > 0)
  {
    fadeDownStart = true;
    scanNetworks = true;
    btnRight = 0;
  }

  display.display();

  if(scanNetworks)
  {
    display.clearDisplay();
    //screen title
    display.setCursor(0, 0);
    display.println("Scanning...");
    //brackets
    display.drawFastHLine(0,8,128,SH110X_WHITE);
    display.drawFastHLine(0,55,128,SH110X_WHITE);
    display.drawFastVLine(43,55,9,SH110X_WHITE);
    display.drawFastVLine(85,55,9,SH110X_WHITE);

    display.display();

    volumeFadeDown();
    if(fadeDownComplete)
    {
      fadeDownStart = false;
      fadeUpStart = true;
      networksCount = WiFi.scanNetworks();
    }
  }
    
}

void drawPasswordTyper()
{
  display.clearDisplay();

  //Menu title
  display.setCursor(0, 0);
  display.setTextColor(SH110X_WHITE);
  display.println("Enter password");
  display.setCursor(0, 10);
  display.println("SSID:");
  stringScroller(WiFi.SSID(chosenNetwork), 21);
  display.println("Password:");

  for(int i = 0; i < (charPosition + 1); i++)
  {
    if(charPosition < btnRight)
    {
      charPosition++;
      btnCenter = btnRight;
    }
    if(charPosition < btnCenter)
    {
      charPosition--;
      btnCenter--;
      btnCenter--;
      btnRight--;
      if(btnCenter < 0)
        btnCenter = 0;
      if(btnRight < 0)
        btnRight = 0;
    }
    if(charPosition < 0)
      charPosition = 0;
    selected = (char)characters[enc];
    tempPassword[charPosition] = selected;
    if(charPosition == i)
      display.setTextColor(SH110X_BLACK, SH110X_WHITE);
    else
      display.setTextColor(SH110X_WHITE);
    tempPassword[charPosition + 2] = '\0';
    display.print(tempPassword[i]);
  }

  if(enc < 0)
    encoder.setCount(characters.length() * 2);
  if(enc > characters.length())
    encoder.setCount ( 0 );
  //brackets
  display.drawFastHLine(0,8,128,SH110X_WHITE);
  display.drawFastHLine(0,55,128,SH110X_WHITE);
  display.drawFastVLine(43,55,9,SH110X_WHITE);
  display.drawFastVLine(85,55,9,SH110X_WHITE);

  display.setTextColor(SH110X_WHITE);

  display.setCursor(10, 57);
  display.println("EXIT");

  display.setCursor(65, 57);
  display.println("<-");

  display.setCursor(105, 57);
  display.println(">");

  display.display();
}

void drawSavedNetworks()
{
  display.clearDisplay();

  //Menu title
  display.setCursor(0, 0);
  display.setTextColor(SH110X_WHITE);
  display.println("Saved Networks");

  //brackets
  display.drawFastHLine(0,8,128,SH110X_WHITE);
  display.drawFastHLine(0,55,128,SH110X_WHITE);
  display.drawFastVLine(42,55,9,SH110X_WHITE);
  display.drawFastVLine(88,55,9,SH110X_WHITE);

  //context buttons
  display.setTextColor(SH110X_WHITE);
  display.setCursor(10, 57);
  display.println("EXIT");
  display.setCursor(91, 57);
  display.println("DELETE");

  display.setCursor(0, 10);
  for(int i = 0 + enc; i < 5 + enc; i++) //list Saved networks
  {
    unsigned int width = 0;   
    if(i > 9)
      width = 17;
    else
      width = 18;
    
    if(enc < 0)
      encoder.setCount ( 10 * 2 );
      //enc = 10;
    if(enc >= 10)
      encoder.setCount ( 0 );
      //enc = 0;
    
    if(enc == i)
      display.setTextColor(SH110X_BLACK, SH110X_WHITE);
    else
      display.setTextColor(SH110X_WHITE);

    if(i < 10)
    {
      preferences.begin("savedNetworks", modeRead);
      display.print(i + 1);
      display.print(". ");
      String tempValue;
      char buffer[13];
      sprintf(buffer, "networkName%i", i);
      const char* tempName = buffer;
      tempValue = preferences.getString(tempName, "");
      stringScroller(tempValue, width);
      preferences.end();
    }
  }

  if(encBtn > 0) //connect
  {
    char buffer1[13];
    char buffer2[13];
    sprintf(buffer1, "networkName%i", encTemp);
    sprintf(buffer2, "networkPass%i", encTemp);
    const char* tempString1 = buffer1;
    const char* tempString2 = buffer2;
    preferences.begin("savedNetworks", modeRead);
    String Name = preferences.getString(tempString1, "");
    String Pass = preferences.getString(tempString2, "");
    preferences.end();
    if(Name == "" || Pass == "")
    {
      if(currentMillis - savedNetworksTime > 100)
      {
        savedNetworksTimeCount++;
        savedNetworksTime = millis();
      }
      if(savedNetworksTimeCount < 21)
      {
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("Connection failed.");
        display.println("Empty Network.");
      }
      else
      {
        savedNetworksTimeCount = 0;
        encBtn = 0;
      }
    }
    else
    {
      fadeDownStart = true;
      volumeFadeDown();
      if(fadeDownComplete)
      {
        fadeDownStart = false;
        startConnect = true;
        connectToNetwork(Name, Pass, false);
        encBtn = 0;
      }
    }
  }

  if(btnRight > 0) //delete
  {
      preferences.begin("savedNetworks", modeReadWrite);
      char buffer1[13];
      char buffer2[13];
      sprintf(buffer1, "networkName%i", encTemp);
      sprintf(buffer2, "networkPass%i", encTemp);
      const char* tempName1 = buffer1;
      const char* tempName2 = buffer2;
      preferences.putString(tempName1, "");
      preferences.putString(tempName2, "");
      preferences.end();
      enc = encTemp;
      btnRight = 0;
  }
  display.display();
}

void drawStationManager()
{
  display.clearDisplay();

  selectedStationsCount = 0;
  for(int i = 0; i < 31; i++)
  {
    if(selectedStations[i] > 0)
      selectedStationsCount++;
  }

  display.setCursor(0, 0);
  display.printf("%i Stations Selected", selectedStationsCount);

  //brackets
  display.drawFastHLine(0,8,128,SH110X_WHITE);
  display.drawFastHLine(0,55,128,SH110X_WHITE);
  display.drawFastVLine(41,55,9,SH110X_WHITE);
  display.drawFastVLine(87,55,9,SH110X_WHITE);

  //context buttons
  display.setCursor(10, 57);
  display.println("EXIT");

  display.setCursor(44, 57);
  display.println("CONNECT");

  display.setCursor(97, 57);
  display.println("SAVE");

  for(int i = 0 + enc; i < 5 + enc; i++)
  {
    if(enc > defaultStationsCount - 1)
      encoder.setCount ( 0 );
      //enc = 0;
    if(enc < 0)
      encoder.setCount ( (defaultStationsCount - 1) * 2 );
      //enc = defaultStationsCount - 1;
    
    bool active;
    if(enc == i)
    {
      active = true;
      display.setTextColor(SH110X_BLACK, SH110X_WHITE);
    }
    else
    {
      active = false;
      display.setTextColor(SH110X_WHITE);
    }

    if(i < defaultStationsCount)
    {
      if(selectedStations[i] == 1)
        display.fillCircle(3,  13 + 9 * (i - enc), 3,  SH110X_WHITE);
      else
        display.drawCircle(3,  13 + 9 * (i - enc), 3,  SH110X_WHITE);
      display.setCursor(9, 10 + 9 * (i - enc));

      preferences.begin("defaultStations", modeRead);
      char buffer[14];
      String tempValue;
      sprintf(buffer, "stationName%i", i);
      const char* tempName = buffer;
      tempValue = preferences.getString(tempName, "");
      preferences.end();      

      if(active)
        stringScroller(tempValue, 19);
      else
        stringShorter(tempValue, 19);
    }
  }

  if(encBtn > 0)
  {
    stationSelected = true;
    enc = encTemp;
    encBtn = 0;
  }

  if(btnCenter > 0)
  {
    enc = encTemp;
    preferences.begin("defaultStations", modeRead);
    char buffer[14];
    String tempValue;
    sprintf(buffer, "stationAddr%i", enc);
    const char* tempName = buffer;
    tempValue = preferences.getString(tempName, "");
    audio.connecttohost(tempValue);
    sprintf(buffer, "stationName%i", enc);
    tempName = buffer;
    viewStationName = preferences.getString(tempName, "");
    preferences.end(); 
    btnCenter = 0;
  }

  if(btnRight > 0)
  {
    if(savePreset)
    {
      saveUserStations();
      savePreset = false;
    }
    if(currentMillis - userSaveTime > 100)
    {
      userSaveTime = millis();
      userSaveCounter++;
    }
    
    if(userSaveCounter < 20)
    {
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("User Preset Saved.");
    }
    else
    {
      userSaveCounter = 0;
      menuX = 0;
      menuY = 0;
      btnRight = 0;
      encoder.setCount ( tempVolume * 2 );
    }
  }  

  display.display();

}

void saveUserStations()
{  
  int userCounter = 0;
  for(int i = 0; i < defaultStationsCount; i++)
  {
    if(selectedStations[i] == 1)
    {
      preferences.begin("userStations", modeReadWrite);
      char buffer[14];
      sprintf(buffer, "userStation%i", userCounter);
      const char* tempName = buffer;
      preferences.putInt(tempName, i);
      preferences.end();
      userCounter++;
    }
  }
  flashReadUserPreset();
}

//Utility Functions
void connectToNetwork(String name, String key, bool newNetwork)
{
  if(startConnect)
  {
    WiFi.disconnect();
    startConnect = false;
    WiFi.begin(name.c_str(), key.c_str());
    connectTime = millis();
  }

  while(WiFi.status() != WL_CONNECTED) 
  {
    currentMillis = millis();

    display.clearDisplay();

    //Menu title
    display.setCursor(0, 0);
    display.setTextColor(SH110X_WHITE);
    display.println("Connecting");


    if(connectDot >= 1)
      display.drawPixel(60,5, 1);
    if(connectDot >= 2) 
      display.drawPixel(62,5, 1);
    if(connectDot >= 3)
      display.drawPixel(64,5, 1);

    display.setCursor(0, 10);
    display.println("SSID:");
    stringShorter(name, 21);
    if(newNetwork)
    {
      display.println("Password:");
      stringShorter(key, 21);
    }
    //display.println(WiFi.status());
    //brackets
    display.drawFastHLine(0,8,128,SH110X_WHITE);
    display.drawFastHLine(0,55,128,SH110X_WHITE);
    display.drawFastVLine(43,55,9,SH110X_WHITE);
    display.drawFastVLine(85,55,9,SH110X_WHITE);

    //context buttons
    display.setCursor(10, 57);
    display.setTextColor(SH110X_WHITE);
    display.println("EXIT");

    display.display();
    
    if(currentMillis - connectDotTime > 250)
    {
      connectDot++;
      if(connectDot > 3)
        connectDot = 0;
      connectDotTime = millis();
    }

    if(currentMillis - connectTime > 5000)
    {
      connectTime = millis();
      break;
    }

    if(btnLeft > 0)
    {
      menuY = 0;
      btnLeft = 0;
      break;
    }
    
  }

  while(WiFi.status() == WL_CONNECT_FAILED)
  {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextColor(SH110X_WHITE);
    display.println("Connection Failed.");
    display.display();
    delay(2000);
    menuX = 0;
    menuY = 0;
    btnCenter = 0;
    btnRight = 0;
    encoder.setCount ( tempVolume * 2 );
    break;
  }

  while(WiFi.status() == WL_NO_SSID_AVAIL)
  {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextColor(SH110X_WHITE);
    display.println("Network not available.");
    display.display();
    delay(2000);
    menuX = 0;
    menuY = 0;
    btnCenter = 0;
    btnRight = 0;
    encoder.setCount ( tempVolume * 2 );
    break;
  }

  while(WiFi.status() == WL_CONNECTED) 
  {
    currentMillis = millis();
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextColor(SH110X_WHITE);
    display.println("WiFi connected to:");
    stringShorter(name, 21);
    currentSSID = name;
    preferences.begin("lastCredentials", modeReadWrite);
    preferences.putString("networkName", name);
    preferences.putString("networkPass", key);
    preferences.end();
    if(newNetwork)
    {
      int emptyBucket = findEmptyBucket();
      if(emptyBucket == -1)
      {
        display.println("Save Failed!");
        display.println("Network Memory Full!");
      }
      else
      {
        preferences.begin("savedNetworks", modeReadWrite);
        char buffer1[13];
        char buffer2[13];
        sprintf(buffer1, "networkName%i", emptyBucket);
        const char* tempName = buffer1;
        sprintf(buffer2, "networkPass%i", emptyBucket);
        const char* tempPass = buffer2;
        preferences.putString(tempName, name);
        preferences.putString(tempPass, key);
        preferences.end();
      }
    }
    display.display();
    delay(2000);
    fadeDownStart = true;
    menuX = 0;
    menuY = 0;
    btnCenter = 0;
    btnRight = 0;
    encoder.setCount ( tempVolume * 2 );
    break; 
  }
}  

//String Scroller
void stringScroller(String word, int width)
{
  if(word.length() > width)
  {
    if(currentMillis - waitScrollTime > 2000) 
    {
      scrollActive = true;
      stopScrollTime = millis();
    }
    if(scrollActive) 
    {
      if(currentMillis - scrollTime > 100) 
      {
        nextChar++;
        scrollTime = millis();
      }
    }
    else 
    {
      if(currentMillis - stopScrollTime > 2000) 
      {
        nextChar = 0;
      }
    }
    for (int i = 0 + nextChar; i < width + nextChar; i++) 
    {
      if (i == word.length()) 
      {
          i = 0;
          scrollActive = false;
          waitScrollTime = millis();
          break;
      }
      display.print(word[i]);
    }
    display.println();
  }
  else
  {
    display.println(word);
  }
}

//String Shorter
void stringShorter(String word, int width)
{
  for(int i = 0; i < width; i++)
      display.print(word[i]);
}

//find empty network name
int findEmptyBucket()
{
  int i;
  preferences.begin("savedNetworks", modeRead);
  for(i = 0; i < 10; i++)
  { 
    char buffer[13];
    String tempValue;
    sprintf(buffer, "networkName%i", i);
    const char* tempName = buffer;
    tempValue = preferences.getString(tempName, "");
    if(tempValue == "")
    {
      preferences.end();
      return i;
    }
  }
  if(i > 9)
  {
    preferences.end();
    return -1;
  }
}

//count saved networks
int countSavedNetworks()
{
  int counter = 10;
  preferences.begin("savedNetworks", modeRead);
  for(int i = 0; i < 10; i++)
  { 
    char buffer[13];
    String tempValue;
    sprintf(buffer, "networkName%i", i);
    const char* tempName = buffer;
    tempValue = preferences.getString(tempName, "");
    if(tempValue == "")
      counter--;
  }
  preferences.end();
  return counter;
}

//volume Fades
void volumeFadeDown()
{
  if(volume > 0 && fadeDownStart)
  {
    if(currentMillis - fadeDownTime > 100)
    {
      volume--;
      audio.setVolume(volume);
      fadeDownTime = millis();
    }
    fadeDownComplete = false;
  }
  else if(fadeDownStart)
  {
    volume = 0;
    audio.setVolume(volume);
    fadeDownComplete = true;
  } 
}

void volumeFadeUp()
{
  if((volume < tempVolume) && fadeUpStart)
  {
    if(currentMillis - fadeUpTime > 100)
    {
      volume++;
      audio.setVolume(volume);
      fadeUpTime = millis();
    }
    fadeUpComplete = false;
  }
  else if(fadeUpStart)
  {
    fadeUpComplete = true;
  } 
}

void checkLongPress() 
{
  // read the state of the switch/button:
  currentState = digitalRead(Pin_enc_btn);

  if(lastState == HIGH && currentState == LOW) {        // button is pressed
    pressedTime = millis();
    isPressing = true;
    isLongDetected = false;
  } else if(lastState == LOW && currentState == HIGH) { // button is released
    isPressing = false;
  }

  if(isPressing == true && isLongDetected == false) {
    long pressDuration = millis() - pressedTime;

    if( pressDuration > 1000 ) {
      sleepStart = true;
      isLongDetected = true;
    }
  }

  // save the the last state
  lastState = currentState;
}

void flashDump()
{
  preferences.begin("lastValues", modeReadWrite);
  preferences.putInt("tempVolumeValue", tempVolume);
  preferences.putInt("stationIndex", tempStationIndex);
  preferences.end();
}

void flashRead()
{
  preferences.begin("lastValues", modeRead);
  tempVolume = preferences.getInt("tempVolumeValue", 0);
  stationIndex = preferences.getInt("stationIndex", 0);
  encoder.setCount ( tempVolume * 2 );
  preferences.end();
  preferences.begin("lastCredentials", modeRead);
  ssid = preferences.getString("networkName", ""); 
  pass = preferences.getString("networkPass", "");
  preferences.end();
}

void flashReadUserPreset()
{
  int index;
  for(int i = 0; i < 10; i++)
  {
    preferences.begin("userStations", modeRead);
    char buffer[14];
    sprintf(buffer, "userStation%i", i);
    const char* tempName = buffer;
    index = preferences.getInt(tempName, 0);
    preferences.end();
    preferences.begin("defaultStations", modeRead);
    char buffer1[14];
    sprintf(buffer1, "stationName%i", index);
    const char* tempName1 = buffer1;
    stationNames[i] = preferences.getString(tempName1, "");
    char buffer2[14];
    sprintf(buffer2, "stationAddr%i", index);
    const char* tempName2 = buffer2;
    stations[i] = preferences.getString(tempName2, "");
    preferences.end();
    selectedStations[index] = 1;
  } 
}

//debug
void viewButtonState()
{
  int boolState;
  Serial.println("buttons left center right enc encBtn menuX menuY volume tempVolume upStart downStart volChange encoder");
  Serial.print("buttons ");
  Serial.print(btnLeft);
  Serial.print("    ");
  Serial.print(btnCenter);
  Serial.print("      ");
  Serial.print(btnRight);
  Serial.print("     ");
  Serial.print(enc);
  Serial.print("   ");
  Serial.print(encBtn);
  Serial.print("      ");
  Serial.print(menuX);
  Serial.print("     ");
  Serial.print(menuY);
  Serial.print("     ");
  Serial.print(volume);
  Serial.print("      ");
  Serial.print(tempVolume);
  Serial.print("          ");
  if(fadeUpStart)
    boolState = 1;
  else
    boolState = 0;
  Serial.print(boolState);
  Serial.print("        ");
  if(fadeDownStart)
    boolState = 1;
  else
    boolState = 0;
  Serial.print(boolState);
  Serial.print("        ");
  if(volumeChange)
    boolState = 1;
  else
    boolState = 0;
  Serial.print(boolState);
  Serial.print("        ");
  Serial.print(encoder.getCount());
  Serial.println();
  

}

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
    // Serial.print("streamtitle ");
    // Serial.println(info);
    title = info;
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