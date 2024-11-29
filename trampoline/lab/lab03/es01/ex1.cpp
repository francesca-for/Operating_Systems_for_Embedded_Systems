#include "math.h"
#include "tpl_os.h"
#include "tpl_com.h"
#include "Arduino.h"

DeclareAlarm(alarmA_1000);
DeclareAlarm(alarmB_1500);
DeclareAlarm(alarmC_2800);

int worstR_C = 0;
int prevWR_C_T = 0;
int WR_C_T = 0;

void setup(){
	Serial.begin(115200);
}

void do_things(int ms) {
    unsigned long mul = ms * 198UL;  // corrected to have expected duration of tasks
    unsigned long i;
    for(i=0; i<mul; i++)
        millis();
}

TASK(TaskA) {
    unsigned long finishTime;
    int periodA = 1024;
    static int newInstA_time = 0;
    int respTime;
    
    Serial.println("A start");

    do_things(200);

    finishTime = millis();

    Serial.println("A end");
    
    // calculate response time
    respTime = finishTime - newInstA_time;
    Serial.print("A resTime: ");
    Serial.println(respTime);

    newInstA_time += periodA;

    TerminateTask();
} 

TASK(TaskB) {
    unsigned long finishTime;
    int periodB = 1536;
    static int newInstB_time = 0;
    int respTime;
    
    Serial.println("B start");

    do_things(700);

    finishTime = millis();

    Serial.println("B end");

    // calculate response time
    respTime = finishTime - newInstB_time;
    Serial.print("B resTime: ");
    Serial.println(respTime);

    newInstB_time += periodB;

    TerminateTask();
}

TASK(TaskC) {
    unsigned long finishTime;
    int periodC = 2867;
    static int newInstC_time = 0;
    int respTime;

    Serial.println("C start");

    do_things(300);
    
    finishTime = millis();

    Serial.println("C end");

    // calculate response time
    respTime = finishTime - newInstC_time;
    Serial.print("C resTime: ");
    Serial.println(respTime);

    newInstC_time += periodC;
    
    if (respTime > worstR_C){
        worstR_C = respTime;
        Serial.print("-> worst R: ");
        Serial.println(worstR_C);
    }
    
    TerminateTask();
}
