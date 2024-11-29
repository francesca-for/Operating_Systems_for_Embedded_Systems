#include "tpl_os.h"
#include "tpl_com.h"
#include "Arduino.h"

DeclareAlarm(alarmA_1000);
DeclareAlarm(alarmB_1500);
DeclareAlarm(alarmC_2800);
DeclareMessage(sent);
DeclareMessage(received);
int worstR_C = 0;

void setup(){
	Serial.begin(115200);
}

void do_things(int ms) {
    unsigned long mul = ms * 199UL;  // corretto in modo da avere esecuzione di task A pari a 200 ms
    unsigned long i;
    for(i=0; i<mul; i++)
        millis();
}

TASK(Init) {
    int x = 1;
    if( SendMessage(sent, &x) != E_OK )
        Serial.println("Error sending message");

    TerminateTask();
}


TASK(TaskA) {
    unsigned long time=millis();
    int msg_R, msg_S=1;

    Serial.print("A start: ");
    Serial.println(time);

    while( ReceiveMessage(received, &msg_R) != E_OK );  // do nothing, locked in busy waiting

    Serial.println("A received message");
        
    Serial.println("A in CS");
    do_things(200);

    Serial.println("A out CS");
    
    time = millis();
    Serial.print("A send message\nA end: ");
    Serial.println(time);
    
    if( SendMessage(sent, &msg_S) != E_OK)
        Serial.println("Error sending message");

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
    int msg_R, msg_S=1;

    Serial.print("C start: ");
    Serial.println(time);
    
    do_things(100);
    
    while( ReceiveMessage(received, &msg_R) != E_OK );  // do nothing, locked in busy waiting

    Serial.println("C received message");

    Serial.println("C in CS");

    do_things(200);
    
    Serial.println("C out CS");

    time = millis();
    Serial.print("C send message\nC end: ");
    Serial.println(time);

    SendMessage(sent, &msg_S);

    TerminateTask();
}