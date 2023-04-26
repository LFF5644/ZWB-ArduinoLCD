#include <Arduino.h>
#include <LiquidCrystal.h>

const int 
	rs=12,
	en=11,
	d4=5,
	d5=4,
	d6=3,
	d7=2,
	PIN_backLight_button=6,
	PIN_backLight=8,
	PIN_sensor01=A0;

bool backLight=true;
long backLight_turnOff=millis()+1e4;
int sensor01_value;
int sensor01_lastValue;
long delay_1s;
long delay_500ms;
//int delay_5s;

LiquidCrystal lcd(rs,en,d4,d5,d6,d7);

void clearLine(int line){
	lcd.setCursor(0,line);
	lcd.print("                "); // clear all 16 columns
	lcd.setCursor(0,line);
}
void runEvery_500ms(long ms){
	if(
		ms<backLight_turnOff&&
		backLight
	){
		clearLine(1);
		lcd.print("OFF in ");
		lcd.print((backLight_turnOff-ms)/1000);
		lcd.print("s");
	}
}
void runEvery_1s(long ms){
	sensor01_value=analogRead(PIN_sensor01);
	if(sensor01_value!=sensor01_lastValue){
		sensor01_lastValue=sensor01_value;
		clearLine(0);
		lcd.print("Gas Sensor ");
		lcd.print(sensor01_value);
	}
}

void setup() {
	pinMode(PIN_backLight,OUTPUT);
	pinMode(PIN_backLight_button,INPUT);
	pinMode(PIN_sensor01,INPUT);
	digitalWrite(PIN_backLight,HIGH);

	Serial.begin(9600);
	lcd.begin(16,2);
	lcd.print("Startup ...");
}

void loop() {
	const long ms=millis();

	if(ms>delay_1s){
		delay_1s=ms+1e3;
		runEvery_1s(ms);
	}
	if(ms>delay_500ms){
		delay_500ms=ms+5e2;
		runEvery_500ms(ms);
	}

	int buttonPressed=digitalRead(PIN_backLight_button);
	if(buttonPressed){
		Serial.print("Button Pressed ");
		const long startPress=millis();
		while(digitalRead(PIN_backLight_button)){}
		Serial.print(millis()-startPress);
		Serial.print("\n");
		if(backLight){
			Serial.println("Backlight turnt off by button");
			backLight_turnOff=0;
		}
		else if(!backLight){
			Serial.println("Backlight turnt on by button");
			backLight=true;
			backLight_turnOff=ms+1e4;
			digitalWrite(PIN_backLight,HIGH);
		}
	}

	if(
		ms>backLight_turnOff&&
		backLight
	){
		backLight=false;
		digitalWrite(PIN_backLight,LOW);
		clearLine(1);
		lcd.print("press button");
	}
}
