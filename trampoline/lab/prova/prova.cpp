#include "tpl_os.h"
#include "tpl_com.h"
#include "Arduino.h"

DeclareAlarm(alarmZ_25);

void setup(){
    Serial.begin(115200);
    pinMode(13, OUTPUT);  // led
    pinMode(A0, INPUT);  // voltage input
    digitalWrite(13, HIGH);
}

TASK(TaskZ) {
    static int error = 0;
    static int alarm = 0;
    static int cntSample = 0;  // contatore mod 4
    static int cntCalc = 0;  // contatore mod 20
    static int cntLed = 0;  // contatore modulo 5
    static int cntBlink = 0;  // per far lampeggiare il led lentamente
    static int min = 1023, max = 0;
    int x;
    int diff;
    int state;

    if(cntSample==0) {
        x = analogRead(A0);

        Serial.print("-> ");  
        Serial.println(x);

        if(x<10 || x>1013)
            error = 1;
        else 
            error = 0;
        Serial.print("err = ");
        Serial.println(error);

        if(x > max)
            max = x;
        if(x < min)
            min = x;
    }

    if(cntCalc == 19){  // diff only if we sampled 5 values -> 20 executions of W
        diff = max-min;
        Serial.print("diff = ");
        Serial.println(diff);
        if(diff > 500)  // determine value of alarm
            alarm = 1;
        else alarm = 0;
        Serial.print("alarm = ");
        Serial.println(alarm);

        cntCalc=0;  // reset counter and min/max value
        min = 1023;
        max = 0;
    }

    if(cntLed == 0) {  // task executes every 25 ms, led changes every 5 executions
        state = digitalRead(13);
        if(error == 1) {  // blink fast
            Serial.println("led fast");
            state = !state;
            digitalWrite(13, state);
            cntBlink = 0;
        } else{
            if(alarm == 1){  // blink slow, 1 change every 4 executions
                Serial.println("led slow");
                if(cntBlink == 0){
                    state = !state;
                    digitalWrite(13, state);
                }
                cntBlink++;
                cntBlink = cntBlink%4;
            }
            else {  // always off
                Serial.println("led off");
                cntBlink = 0;
                digitalWrite(13, LOW);
            }
        }
    }

    cntSample++;
    cntSample = cntSample%4;
    cntCalc++;
    cntLed++;
    cntLed = cntLed%5;

    TerminateTask();
}