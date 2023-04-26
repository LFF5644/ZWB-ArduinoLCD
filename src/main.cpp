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
	PIN_backLight=8;

bool backLight=true;
long backLight_turnOff=millis()+1e4;

LiquidCrystal lcd(rs,en,d4,d5,d6,d7);

void clearLine(int line){
	lcd.setCursor(0,line);
	lcd.print("                ");
	lcd.setCursor(0,line);
}

void setup() {
	pinMode(PIN_backLight,OUTPUT);
	pinMode(PIN_backLight_button,INPUT);
	digitalWrite(PIN_backLight,HIGH);

	Serial.begin(9600);
	lcd.begin(16,2);
	lcd.print("HALLO!");
}

void loop() {
	const long ms=millis();
	clearLine(0);
	lcd.print(ms);
	lcd.print(" ");
	lcd.print(backLight);
	lcd.print(" ");
	lcd.print(ms>backLight_turnOff);

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
			backLight_turnOff=ms+3e3;
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
	else if(
		ms<backLight_turnOff&&
		backLight
	){
		clearLine(1);
		lcd.print("OFF in ");
		lcd.print((backLight_turnOff-ms)/1000);
		lcd.print("s");
	}
}
