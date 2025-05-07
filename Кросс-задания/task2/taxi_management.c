#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include<errno.h>
#include <sys/time.h>

#define MAX_DRIVERS 10

//Message queue structure
typedef struct {
    long mtype;
    pid_t pid;
    int task_timer;
    char status[20];
}msg_buf;

//Driver structure
typedef struct {
    pid_t pid;
    int task_timer;
    char status[20];
    time_t end_time;
} driver;

driver drivers[MAX_DRIVERS];
int driver_count = 0;
int msgqid;

//Function to find driver index by PID
int find_dirver_index(pid_t pid){
    for (int i = 0; i <driver_count;i++){
        if(drivers[i].pid == pid) return i;
    }
    return -1;
}

//Driver process
void driver_process(){
    msg_buf msg;
    while(1){
       
    }

}

//Main CLI process
int main(){
    char command[100];
    pid_t pid;
    int task_timer;

    //Create message queue
    key_t key = ftok("taxi_management.c", 'a');
    if (key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }
    msgqid = msgget(key, IPC_CREAT | 0666);
    if (msgqid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }
    fd_set read_fds;
    struct timeval tv;

    while(1){
        printf("Enter command: (create_driver, send_task <pid> <task_timer>, get_status <pid>, get_drivers)\n");
        if (fgets(command, sizeof(command), stdin) == NULL) continue;
        command[strcspn(command, "\n")] = 0; 
        
        //Update driver status
        time_t current_time = time(NULL);
        for (int i = 0; i < driver_count;i++){
            
        }
    }

}