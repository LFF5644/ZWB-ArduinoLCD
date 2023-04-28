#include <Arduino.h>
#include <LiquidCrystal.h>
#include <IRremote.hpp>
#include <Adafruit_BMP280.h>

#define BMP280_I2C_ADDRESS  0x76

Adafruit_BMP280 bmp; // use I2C interface
Adafruit_Sensor *bmp_temp = bmp.getTemperatureSensor();
Adafruit_Sensor *bmp_pressure = bmp.getPressureSensor();

const unsigned char
	rs = 12,				  // Digital 12
	en = 11,				  // Digital 11
	d4 = 5,					  // Digital 05
	d5 = 4,					  // Digital 04
	d6 = 3,					  // Digital 03
	d7 = 2,					  // Digital 02
	PIN_button_backLight = 6, // Digital 06
	PIN_backLight = 8,		  // Digital 08
	PIN_irRemote = 7,		  // Digital 07
	PIN_sensorGas = A0;		  // Analog  00

const unsigned long
	backLight_turnOffTime = 1e5 + 2e4; // 120s

bool backLight = true;
unsigned long backLight_turnOff = millis() + backLight_turnOffTime;
int sensorGas_value;
int sensorGas_lastValue;
unsigned long delay_1s;
unsigned long delay_500ms;
long BUTTON_OFF = -1169817856;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void clearLine(int line)
{
	lcd.setCursor(0, line);
	lcd.print("                "); // clear all 16 columns
	lcd.setCursor(0, line);
}
void changeBackLightBool(unsigned char newMode = 2)
{
	if (newMode == 2)
	{
		if (backLight)
		{
			newMode = 0;
		}
		else if (!backLight)
		{
			newMode = 1;
		}
	}

	if (newMode == 1)
	{
		backLight = true;
		backLight_turnOff = millis() + backLight_turnOffTime;
		digitalWrite(PIN_backLight, HIGH);
	}
	else if (newMode == 0)
	{
		backLight_turnOff = 0;
	}
}
void runEvery_500ms(unsigned long ms)
{
	if (
		ms < backLight_turnOff &&
		backLight)
	{
		clearLine(1);
		lcd.print("OFF in ");
		lcd.print((backLight_turnOff - ms) / 1000);
		lcd.print("s");
	}
}
void runEvery_1s(unsigned long ms)
{
	sensorGas_value = analogRead(PIN_sensorGas);
	if (sensorGas_value != sensorGas_lastValue)
	{
		sensorGas_lastValue = sensorGas_value;
		clearLine(0);
		lcd.print("Gas Sensor: ");
		lcd.print(sensorGas_value);
	}
	Serial.println(bmp.readTemperature());
}

void setup()
{
	Serial.begin(9600);

	// status = bmp.begin(BMP280_ADDRESS_ALT, BMP280_CHIPID);
	bool status = bmp.begin(BMP280_I2C_ADDRESS);
	if (!status)
	{
		Serial.println("SENSOR ERROR");
		Serial.print("SensorID was: 0x");
		Serial.println(bmp.sensorID(), 16);
		delay(10000);
	}

	bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,	  /* Operating Mode. */
					Adafruit_BMP280::SAMPLING_X2,	  /* Temp. oversampling */
					Adafruit_BMP280::SAMPLING_X16,	  /* Pressure oversampling */
					Adafruit_BMP280::FILTER_X16,	  /* Filtering. */
					Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

	pinMode(PIN_backLight, OUTPUT);

	pinMode(PIN_button_backLight, INPUT);
	digitalWrite(PIN_button_backLight, HIGH);

	pinMode(PIN_sensorGas, INPUT);
	digitalWrite(PIN_backLight, HIGH);

	IrReceiver.begin(PIN_irRemote, DISABLE_LED_FEEDBACK);

	lcd.begin(16, 2);
	lcd.print("Startup ...");
}
bool lastButtonState = false;
unsigned long startPress;
void loop()
{
	const unsigned long ms = millis();

	if (ms > delay_1s)
	{
		delay_1s = ms + 1e3;
		runEvery_1s(ms);
	}
	if (ms > delay_500ms)
	{
		delay_500ms = ms + 5e2;
		runEvery_500ms(ms);
	}

	bool buttonPressed = digitalRead(PIN_button_backLight) == LOW;
	if (buttonPressed != lastButtonState)
	{
		if (buttonPressed)
		{
			Serial.print("Button Pressed ");
			startPress = millis();
			changeBackLightBool();
		}
		else
		{
			Serial.print(millis() - startPress);
			Serial.print("\n");
		}
		lastButtonState = buttonPressed;
		delay(1);
	}

	if (
		ms > backLight_turnOff &&
		backLight)
	{
		backLight = false;
		digitalWrite(PIN_backLight, LOW);
		clearLine(1);
		lcd.print("press button");
	}
	if (IrReceiver.decode())
	{
		long irReceived = IrReceiver.decodedIRData.decodedRawData;
		if (irReceived != 0)
		{
			Serial.println(irReceived);
			if (irReceived == BUTTON_OFF)
			{ // if power off btn pressed
				Serial.println("OFF BTN PRESSED!");
				changeBackLightBool();
			}
		}
		IrReceiver.resume();
	}
}
