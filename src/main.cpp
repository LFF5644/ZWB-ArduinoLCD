#include <Arduino.h>
#include <LiquidCrystal.h>

const int 
	rs=12,	// Digital 12
	en=11,	// Digital 11
	d4=5,	// Digital 05
	d5=4,	// Digital 04
	d6=3,	// Digital 03
	d7=2,	// Digital 02
	PIN_button_backLight=6,	// Digital 06
	PIN_backLight=8,		// Digital 08
	PIN_sensorGas=A0;		// Analog 00

const long
	backLight_turnOffTime=1e5+2e4;	// 120s

bool backLight=true;
long backLight_turnOff=millis()+backLight_turnOffTime;
int sensorGas_value;
int sensorGas_lastValue;
long delay_1s;
long delay_500ms;

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
	sensorGas_value=analogRead(PIN_sensorGas);
	if(sensorGas_value!=sensorGas_lastValue){
		sensorGas_lastValue=sensorGas_value;
		clearLine(0);
		lcd.print("Gas Sensor ");
		lcd.print(sensorGas_value);
	}
}

void setup(){
	pinMode(PIN_backLight,OUTPUT);
	pinMode(PIN_button_backLight,INPUT);
	pinMode(PIN_sensorGas,INPUT);
	digitalWrite(PIN_backLight,HIGH);

	Serial.begin(9600);
	lcd.begin(16,2);
	lcd.print("Startup ...");
}

void loop(){
	const long ms=millis();

	if(ms>delay_1s){
		delay_1s=ms+1e3;
		runEvery_1s(ms);
	}
	if(ms>delay_500ms){
		delay_500ms=ms+5e2;
		runEvery_500ms(ms);
	}

	int buttonPressed=digitalRead(PIN_button_backLight);
	if(buttonPressed){
		Serial.print("Button Pressed ");
		const long startPress=millis();
		while(digitalRead(PIN_button_backLight)){}
		Serial.print(millis()-startPress);
		Serial.print("\n");
		if(backLight){
			Serial.println("Backlight turnt off by button");
			backLight_turnOff=0;
		}
		else if(!backLight){
			Serial.println("Backlight turnt on by button");
			backLight=true;
			backLight_turnOff=ms+backLight_turnOffTime ;
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
