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

int main(){
    //create msgQueue with ftok
    int qid = msgget(ftok(".",'u'), IPC_EXCL|IPC_CREAT|0600);
    
    // sender child
    pid_t cpid1 = fork();

    if (cpid1 < 0){
        cout << "fork failed" << endl;
    } else if (cpid1 > 0){
        cout << "cpid1: " << cpid1 << endl;
    }else{
        //execute sender
        execlp("./sender", NULL);
        exit(0);
    }

    // receiver child
    pid_t cpid2 = fork();
    if (cpid2 < 0){
        cout << "fork failed" << endl;
    } else if (cpid2 > 0){
        cout << "cpid2: " << cpid2 << endl;
    }else{
        //execute sender
        execlp("./receiver", NULL);
        exit(0);
    }

    printf("Child 1: %d, Child 2: %d, Parent: %d\n", cpid1, cpid2, getpid());
    while(wait(NULL) != -1);

    //removes the msgQueue
    msgctl(qid, IPC_RMID, NULL);
    cout << "parent terminating..." << endl;
    exit(0);
}