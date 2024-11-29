#include "tpl_os.h"
#include "stdio.h"
#include "stdlib.h"
#include "Arduino.h"

void setup()
{
	Serial.begin(115200);
	// initialize digital pin 13 as an output.
	pinMode(13, OUTPUT);
	StartOS(OSDEFAULTAPPMODE);
}

DeclareAlarm(a500msec);
DeclareAlarm(a750msec);

int deltaA = 500;
int deltaB = 750;

TASK(TaskA)
{
	static unsigned int counterA = 0;
	digitalWrite(13, HIGH);
	Serial.print("TaskA ");
	Serial.println(counterA);
	counterA+=deltaA;
	TerminateTask();
}

TASK(TaskB)
{
	static unsigned int counterB = 1500;
	digitalWrite(13, LOW);
	Serial.print("TaskB ");
	Serial.println(counterB);
	counterB+=deltaB;
	TerminateTask();
}

TASK(stop){
    CancelAlarm(a500msec);
    CancelAlarm(a750msec);
	digitalWrite(13, LOW);
    ShutdownOS(E_OK);
    TerminateTask();
}