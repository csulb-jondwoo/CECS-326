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
#include <string>
using namespace std;


// declare my message buffer
struct buf {
    long mtype; // required
    char greeting[50]; // mesg content
};

int main(){
    cout << "Sender, PID: "<< getpid() << ", begins execution" << endl;
    //create message buffer
    buf msg;
    int size = sizeof(msg)-sizeof(long);

    //create same unique key
    int qid = msgget(ftok(".",'u'), 0);
    
    string input;
    cout << "message to send: ";
    getline (cin, input);

    //insert user input to msgQueue
    strcpy(msg.greeting, input.c_str());

    cout << "Sender " << getpid() << ": sends greeting" << endl;
    msg.mtype = 113; // set message type mtype = 113

    //use unique key to access msgQueue
    msgsnd(qid, (struct msgbuf *)&msg, size, 0);    

    cout << "Sender " << getpid() << ": now exits" << endl;
};

