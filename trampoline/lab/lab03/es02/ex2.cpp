#include "tpl_os.h"
#include "tpl_com.h"
#include "Arduino.h"

DeclareAlarm(alarmA_1000);
DeclareAlarm(alarmB_1500);
DeclareAlarm(alarmC_2800);
DeclareResource(Sem);

void setup(){
	Serial.begin(115200);
    Serial.println("Output: start/finish time of tasks and CS, response time of every task");
}

void do_things(int ms) {
    unsigned long mul = ms * 198UL;  // corrected to have expected duration of tasks
    unsigned long i;
    for(i=0; i<mul; i++)
        millis();
}

TASK(TaskA) {
    unsigned long time=millis();
    int deltaT;
    int periodA = 1024;
    static int newInstA_time = 0;
    
    Serial.print("A start: ");
    Serial.println(time);
    
    if((deltaT = time-newInstA_time) > 1){  // > 0 but i wanted to have some margin
        Serial.print("-> Priority inversion: A waited ");
        Serial.println(deltaT);
    }

    Serial.println("A get res");
    GetResource(Sem);

    Serial.println("A in CS");
    do_things(200);

    newInstA_time += periodA;

    Serial.println("A out CS");
    
    time = millis();
    Serial.print("A release res\nA end: ");
    Serial.println(time);
    
    ReleaseResource(Sem);

    TerminateTask();
}

TASK(TaskB) {
    unsigned long time = millis();

    Serial.print("B start: ");
    Serial.println(time);
    
    do_things(700);

    time = millis();
    Serial.print("B end: ");
    Serial.println(time);

    TerminateTask();
}

TASK(TaskC) {
    unsigned long time = millis();

    Serial.print("C start: ");
    Serial.println(time);
    
    do_things(120);
    
    time = millis();
    Serial.print("C get res: ");
    Serial.println(time);

    GetResource(Sem);

    Serial.println("C in CS");

    do_things(200);
    
    Serial.println("C out CS");

    time = millis();
    Serial.print("C release res\nC end: ");
    Serial.println(time);

    ReleaseResource(Sem);

    TerminateTask();
}