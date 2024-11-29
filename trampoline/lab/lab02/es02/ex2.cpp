#include "math.h"
#include "tpl_os.h"
#include "tpl_com.h"
#include "Arduino.h"

DeclareAlarm(aTaskC_100ms);
DeclareAlarm(aTaskM_500ms);
DeclareAlarm(aTaskV_250ms);
DeclareMessage(from_C);
DeclareMessage(M_receive);
DeclareMessage(from_M);
DeclareMessage(V_receive);
DeclareTask(TaskC);
DeclareTask(TaskM);
DeclareTask(TaskV);

const int switchMask = 0x800;  // 0000 0000  0000 0000  0000 1000  0000 0000
const int analogMask = 0x3FF;  // 0000 0000  0000 0000  0000 0011  1111 1111

void setup(){
	Serial.begin(115200);
	pinMode(13, OUTPUT);  // led
    pinMode(12, INPUT_PULLUP);  // switch
    pinMode(A0, INPUT);  // voltage input
    digitalWrite(13, LOW);
}

TASK(TaskC) {
    static int switchStart = 0;
    int message = analogRead(A0) & analogMask;
    
    if(digitalRead(12) == LOW) {
        if(switchStart - millis() >= 1000)  // if switch has been pressed for at least 1 second
            message = message | switchMask;  // set switch bit = 1
    }
    else switchStart = millis();

    Serial.print("C to M: ");
    Serial.println(message, BIN);

    SendMessage(from_C, &message);

	TerminateTask();
}

TASK(TaskM) {
    int received_msg;
    static int new_message = -1;
    static int prev_received_msg = -1;
    int analog_value;
    static int referenceVal = -1;
    int x;

    if( ReceiveMessage(M_receive, &received_msg) != E_OK )
        TerminateTask();

    if (received_msg != prev_received_msg){  // don't compute again the new message if M has received a message = previous one
        analog_value = received_msg & analogMask; 

        if(received_msg & switchMask) {  // if switch bit is set to 1, update reference value
            referenceVal = analog_value;
            Serial.print("NewR = ");
            Serial.println(analog_value);
        }
        if(referenceVal == -1){
            new_message = 3;  // led ON
            Serial.println("M to V: 3");
        }
        else {
            x = abs(analog_value-referenceVal);
            if(x<100){
                new_message = 0;  // led OFF
                Serial.println("M to V: 0");
            }
            else if(x>=200){
                new_message = 2;  // blink fast (4 Hz)
                Serial.println("M to V: 2");
            }
            else{
                new_message = 1;  // blink slow (1 Hz)
                Serial.println("M to V: 1");
            }
        }
    }
    SendMessage(from_M, &new_message);
    prev_received_msg = received_msg;
	TerminateTask();
}

TASK(TaskV) {
    int state = digitalRead(13);
    int received;
    static int Vcnt = 0;

    if( ReceiveMessage(V_receive, &received) != E_OK )
        TerminateTask();

    if(received == 0){  // led is off
        Vcnt = 0;  // reset value for next time we have to make the led blink slowly
        digitalWrite(13, LOW);
    }
    else if(received == 1){  // blink slow: change led state every 4 executions
        if(Vcnt == 0){
            state = !state;
            digitalWrite(13, state);
        }
        Vcnt++;
        Vcnt = (Vcnt)%4;
    }
    else if(received == 2){  // blink fast: change led state at every execution
        Vcnt = 0;  // reset value for next time we have to make the led blink slowly
        state = !state;
        digitalWrite(13, state);
    }
    else {  // received == 3, led is always on
        digitalWrite(13, HIGH);
    }

	TerminateTask();
}
