#include "limits.h"
#include "tpl_os.h"
#include "tpl_com.h"
#include "Arduino.h"

DeclareAlarm(alarmS_100);
DeclareAlarm(alarmB_500);
DeclareAlarm(alarmV_125);
DeclareResource(Res);

const int K = 5;
int queue[K];
int nQ = 0;
int error = 0;
int alarm = 0;

void setup(){ 
    Serial.begin(115200);
    pinMode(13, OUTPUT);  // led
    pinMode(A0, INPUT);  // voltage input
    digitalWrite(13, HIGH);
}

TASK(TaskS) {
    int x = analogRead(A0);
    
    Serial.print("-> ");  
    Serial.println(x);

    GetResource(Res);

    if(nQ >= K)
        Serial.println("Queue is full");
    else
        queue[nQ++] = x;

    if(x<10 || x>1013)
        error = 1;
    else error = 0;
    Serial.print("S: err=");
    Serial.println(error);

    ReleaseResource(Res);

    TerminateTask();
}

TASK(TaskB) {
    int min = INT_MAX, max = 0;

    GetResource(Res);

    for(int i=0; i<nQ; i++){  // find min and max
        if(queue[i]>max)
            max = queue[i];
        if(queue[i]<min)
            min = queue[i];
    }
    
    int diff= max-min;  
    Serial.print("B: diff=");
    Serial.println(diff);

    if(max-min > 500)  // determine value of alarm
        alarm = 1;
    else alarm = 0;

    Serial.print("B: alarm=");
    Serial.println(alarm);

    nQ = 0;  // reset of the queue
    ReleaseResource(Res);

    TerminateTask();
}

TASK(TaskV) {
    int state = digitalRead(13);
    static int cnt = 0;

    GetResource(Res);

    if(error == 1) {  // blink fast
        Serial.println("V: fast");
        cnt = 0;  // reset for next time the led has to blink slowly
        state = !state;
        digitalWrite(13, state);
    }
    else{
        if(alarm == 1){  // blink slow, 1 change every 4 executions
            Serial.println("V: slow");
            if(cnt == 0){
                state = !state;
                digitalWrite(13, state);
            }
            cnt++;
            cnt = cnt%4;
        }
        else {  // always off
            Serial.println("V: off");
            cnt = 0;
            digitalWrite(13, LOW);
        }
    }

    ReleaseResource(Res);

    TerminateTask();
}


// TASK(TaskS) {
//     // DEBUG
//     int period = 100;
//     static int act = 0;
//     int x = millis();
//     Serial.print("Sact ");
//     Serial.println(act);
//     Serial.print("Sstart ");
//     Serial.println(x);
//     act+=period;
//     // END DEBUG 
//     TerminateTask();
// }
// TASK(TaskB) {
//     // DEBUG
//     int period = 500;
//     static int act = 0;
//     int x = millis();
//     Serial.print("Bact ");
//     Serial.println(act);
//     Serial.print("Bstart ");
//     Serial.println(x);
//     act+=period;
//     // END DEBUG
//     TerminateTask();
// }
// TASK(TaskV) {
//     // DEBUG
//     // int period = 125;
//     // static int act = 0;
//     // int x = millis();
//     // Serial.print("Vact ");
//     // Serial.println(act);
//     // Serial.print("Vstart ");
//     // Serial.println(x);
//     // act+=period;
//     // END DEBUG
//     TerminateTask();
// }