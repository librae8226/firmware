/**
 ******************************************************************************
 * @file    application.cpp
 * @authors  Satish Nair, Zachary Crockett and Mohit Bhoite
 * @version V1.0.0
 * @date    05-November-2013
 * @brief   Tinker application
 ******************************************************************************
  Copyright (c) 2013 Spark Labs, Inc.  All rights reserved.

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this program; if not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "application.h"

/* Definitions ---------------------------------------------------------------*/
SYSTEM_MODE(AUTOMATIC);

enum {
	STATE_BOOTING = 0,
	STATE_LISTENING = 1,
	STATE_BREWING = 2,
};

/* Variables -----------------------------------------------------------------*/
static double temperature = 0.0;
static double alcohol1 = 0.0;
static double alcohol2 = 0.0;
static int state = STATE_BOOTING;
static int pumpsOn[8] = {0, 0, 0, 0, 0, 0, 0, 0};

/* Function prototypes -------------------------------------------------------*/
int tinkerDigitalRead(String pin);
int tinkerDigitalWrite(String command);
int tinkerAnalogRead(String pin);
int tinkerAnalogWrite(String command);
int drinkbotSetPumps(String command);
int drinkbotEcho(String command);

/* This function is called once at start up ----------------------------------*/
void setup()
{
	//Setup the communication bus
	Serial.begin(9600);

	//Setup the Tinker application here
	RGB.brightness(12);

	//Register all the Tinker functions
	Spark.function("digitalread", tinkerDigitalRead);
	Spark.function("digitalwrite", tinkerDigitalWrite);

	Spark.function("analogread", tinkerAnalogRead);
	Spark.function("analogwrite", tinkerAnalogWrite);

	//Register all the Drinkbot functions
	Spark.function("setpumps", drinkbotSetPumps);
	Spark.function("echo", drinkbotEcho);

	//Register all the Drinkbot variables
	Spark.variable("temperature", &temperature, DOUBLE);
	pinMode(A7, INPUT);
	Spark.variable("alcohol1", &alcohol1, DOUBLE);
	pinMode(A3, INPUT);
	Spark.variable("alcohol2", &alcohol2, DOUBLE);
	pinMode(A2, INPUT);

	//Pumps
	pinMode(D0, OUTPUT);
	digitalWrite(D0, LOW);
	pinMode(D1, OUTPUT);
	digitalWrite(D1, LOW);
	pinMode(A0, OUTPUT);
	digitalWrite(A0, LOW);
	pinMode(A1, OUTPUT);
	digitalWrite(A1, LOW);
	pinMode(A4, OUTPUT);
	digitalWrite(A4, LOW);
	pinMode(A5, OUTPUT);
	digitalWrite(A5, LOW);
	pinMode(A6, OUTPUT);
	digitalWrite(A6, LOW);
	pinMode(A7, OUTPUT);
	digitalWrite(A7, LOW);

	//RGB LED
	pinMode(D7, OUTPUT);
}

/* This function loops forever --------------------------------------------*/
void loop()
{
	int reading = 0;

	switch (state) {
	case STATE_BOOTING:
		Serial.println("Spark USB Serial is running...");
		state = STATE_LISTENING;
		break;
	case STATE_LISTENING:
		/* sleep here awaiting for remote api calls */
		reading = analogRead(A3);
		alcohol1 = reading;
		reading = analogRead(A2);
		alcohol2 = reading;
		reading = analogRead(A7);
		temperature = reading;
		break;
	case STATE_BREWING:
		/* do brewing job here */

		state = STATE_LISTENING;
		break;
	default:
		Serial.print("no such state: ");
		Serial.println(state);
		break;
	}
}

/*******************************************************************************
 * Function Name  : tinkerDigitalRead
 * Description    : Reads the digital value of a given pin
 * Input          : Pin
 * Output         : None.
 * Return         : Value of the pin (0 or 1) in INT type
                    Returns a negative number on failure
 *******************************************************************************/
int tinkerDigitalRead(String pin)
{
	//convert ascii to integer
	int pinNumber = pin.charAt(1) - '0';
	//Sanity check to see if the pin numbers are within limits
	if (pinNumber< 0 || pinNumber >7) return -1;

	if(pin.startsWith("D"))
	{
		pinMode(pinNumber, INPUT_PULLDOWN);
		return digitalRead(pinNumber);
	}
	else if (pin.startsWith("A"))
	{
		pinMode(pinNumber+10, INPUT_PULLDOWN);
		return digitalRead(pinNumber+10);
	}
	return -2;
}

/*******************************************************************************
 * Function Name  : tinkerDigitalWrite
 * Description    : Sets the specified pin HIGH or LOW
 * Input          : Pin and value
 * Output         : None.
 * Return         : 1 on success and a negative number on failure
 *******************************************************************************/
int tinkerDigitalWrite(String command)
{
	bool value = 0;
	//convert ascii to integer
	int pinNumber = command.charAt(1) - '0';
	//Sanity check to see if the pin numbers are within limits
	if (pinNumber< 0 || pinNumber >7) return -1;

	if(command.substring(3,7) == "HIGH") value = 1;
	else if(command.substring(3,6) == "LOW") value = 0;
	else return -2;

	if(command.startsWith("D"))
	{
		pinMode(pinNumber, OUTPUT);
		digitalWrite(pinNumber, value);
		return 1;
	}
	else if(command.startsWith("A"))
	{
		pinMode(pinNumber+10, OUTPUT);
		digitalWrite(pinNumber+10, value);
		return 1;
	}
	else return -3;
}

/*******************************************************************************
 * Function Name  : tinkerAnalogRead
 * Description    : Reads the analog value of a pin
 * Input          : Pin
 * Output         : None.
 * Return         : Returns the analog value in INT type (0 to 4095)
                    Returns a negative number on failure
 *******************************************************************************/
int tinkerAnalogRead(String pin)
{
	//convert ascii to integer
	int pinNumber = pin.charAt(1) - '0';
	//Sanity check to see if the pin numbers are within limits
	if (pinNumber< 0 || pinNumber >7) return -1;

	if(pin.startsWith("D"))
	{
		pinMode(pinNumber, INPUT);
		return analogRead(pinNumber);
	}
	else if (pin.startsWith("A"))
	{
		pinMode(pinNumber+10, INPUT);
		return analogRead(pinNumber+10);
	}
	return -2;
}

/*******************************************************************************
 * Function Name  : tinkerAnalogWrite
 * Description    : Writes an analog value (PWM) to the specified pin
 * Input          : Pin and Value (0 to 255)
 * Output         : None.
 * Return         : 1 on success and a negative number on failure
 *******************************************************************************/
int tinkerAnalogWrite(String command)
{
	//convert ascii to integer
	int pinNumber = command.charAt(1) - '0';
	//Sanity check to see if the pin numbers are within limits
	if (pinNumber< 0 || pinNumber >7) return -1;

	String value = command.substring(3);

	if(command.startsWith("D"))
	{
		pinMode(pinNumber, OUTPUT);
		analogWrite(pinNumber, value.toInt());
		return 1;
	}
	else if(command.startsWith("A"))
	{
		pinMode(pinNumber+10, OUTPUT);
		analogWrite(pinNumber+10, value.toInt());
		return 1;
	}
	else return -2;
}

/*******************************************************************************
 * Function Name  : drinkbotSetPumps
 * Description    : Setup and start the pumping jobs,
 *                  e.g. P0:xx;P1:xx;P2:xx;P3:xx;P4:xx;P5:xx;P6:xx;P7:xx
 *                  where P0 ~ P7 are the 8 pumps and xx is the cl value.
 * Input          : pumps id and corresponding cl value string
 * Output         : None.
 * Return         : 0 for success, others for error numbers
 *******************************************************************************/
int drinkbotSetPumps(String command)
{
	int i;

	Serial.print(__func__);
	Serial.print(", args: ");
	Serial.println();

	/* parse command */
	Serial.print("pumpsOn: ");
	for (i = 0; i < 8; i++) {
		pumpsOn[i] = command.substring(6*i+3, 6*i+5).toInt();
		Serial.print(pumpsOn[i]);
		Serial.print(' ');
	}
	Serial.println("");

	/* excute pumping jobs */
	if (command == "on" || command == "ON")
	{
		pinMode(D7, OUTPUT);
		digitalWrite(D7, HIGH);
		return 1;
	}
	else
	{
		pinMode(D7, OUTPUT);
		digitalWrite(D7, LOW);
		return 0;
	}
}

/*******************************************************************************
 * Function Name  : drinkbotEcho
 * Description    : Echo what it receives
 * Input          : Random things
 * Output         : None
 * Return         : The character ascii code it received
 *******************************************************************************/
int drinkbotEcho(String command)
{
	Serial.print("input: ");
	Serial.println(command);
	return command.charAt(0);
}
