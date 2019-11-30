/*
1. begin with "Sender (or Receiver), PID xxxxx, begins execution"
2. retrieves the message from msgQ
3. outputs message on screen
4. Identify as RECEIVER and PID
*/
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
using namespace std;

struct buf {
    long mtype; // required
    char greeting[50]; // mesg content
};

int main(){
    cout << "Receiver, PID: "<< getpid() << ", begins execution" << endl;
    buf msg;
    int size = sizeof(msg)-sizeof(long);

    //create same unique key
    int qid = msgget(ftok(".",'u'), 0);

    //use unique key to access msgQueue
    msgrcv(qid, (struct msgbuf *)&msg, size, 113, 0); 

    cout << "Receiver " << getpid() << ": gets message" << endl;
    cout << "message: " << msg.greeting << endl;
    cout << "Receiver " << getpid() << ": now exits" << endl;
}