#include "tpl_os.h"
#include "tpl_com.h"
#include "Arduino.h"

DeclareAlarm(alarmW_100);
DeclareAlarm(alarmV_125);
DeclareResource(GlobVar);

int error = 0;
int alarm = 0;

void setup(){
    Serial.begin(115200);
    pinMode(13, OUTPUT);  // led
    pinMode(A0, INPUT);  // voltage input
    digitalWrite(13, HIGH);
}

TASK(TaskW) {
    static int cnt = 0;  // contatore modulo 5
    static int min = 1023, max = 0;
    int diff;
    int x = analogRead(A0);

    Serial.print("-> ");  
    Serial.println(x);

    if(x > max)
        max = x;
    if(x < min)
        min = x;

    GetResource(GlobVar);
    
    if(x<10 || x>1013)
        error = 1;
    else error = 0;
    Serial.print("err = ");
    Serial.println(error);

    if(cnt == 4){
        diff = max-min;
        Serial.print("diff = ");
        Serial.println(diff);
        if(diff > 500)  // determine value of alarm
            alarm = 1;
        else alarm = 0;
        Serial.print("alarm = ");
        Serial.println(alarm);

        cnt=0;  // reset counter and min/max value
        min = 1023;
        max = 0;
    }
    else cnt++;

    ReleaseResource(GlobVar);
    
    TerminateTask();
}

TASK(TaskV) {
    int state = digitalRead(13);
    static int cnt = 0;

    GetResource(GlobVar);

    if(error == 1) {  // blink fast
        Serial.println("led: fast");
        cnt = 0;  // reset for next time the led has to blink slowly
        state = !state;
        digitalWrite(13, state);
    }
    else{
        if(alarm == 1){  // blink slow, 1 change every 4 executions
            Serial.println("led: slow");
            if(cnt == 0){
                state = !state;
                digitalWrite(13, state);
            }
            cnt++;
            cnt = cnt%4;
        }
        else {  // always off
            Serial.println("led: off");
            cnt = 0;
            digitalWrite(13, LOW);
        }
    }

    ReleaseResource(GlobVar);

    TerminateTask();
}