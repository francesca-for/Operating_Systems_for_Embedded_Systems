#include <stdio.h>
#include "tpl_os.h"

int main(void){
    StartOS(OSDEFAULTAPPMODE);
    return 0;
}

DeclareAlarm(a500msec);
DeclareAlarm(a750msec);

int deltaA = 500;
int deltaB = 750;

TASK(TaskA){
    static unsigned int counterA = 0;
    printf("TaskA: %d\n\r", counterA);
    counterA+=deltaA;
    TerminateTask();
}

TASK(TaskB){
    static unsigned int counterB = 1500;
    printf("TaskB: %d\n\r", counterB);
    counterB+=deltaB;
    TerminateTask();
}

TASK(stop){
    CancelAlarm(a500msec);
    CancelAlarm(a750msec);
    printf("Shutdown\n\r");
    ShutdownOS(E_OK);
    TerminateTask();
}