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
#include <time.h>
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

// Signal handler for cleanup
void cleanup(int sig) {
    msgctl(msgqid, IPC_RMID, NULL);
    for (int i = 0; i < driver_count; i++) {
        kill(drivers[i].pid, SIGTERM);
    }
    exit(0);
}
//Function to find driver index by PID
int find_driver_index(pid_t pid){
    for (int i = 0; i <driver_count;i++){
        if(drivers[i].pid == pid) return i;
    }
    return -1;
}

//Driver process
void driver_process(){
    msg_buf msg;
    while(1){
        if (msgrcv(msgqid, &msg, sizeof(msg) - sizeof(long), getpid(), 0) == -1) {
            perror("msgrcv");
            exit(1);
        }

        if (strcmp(msg.status, "get_status") == 0) {
            msg.mtype = 1;
            msg.pid = getpid();
            strcpy(msg.status, drivers[find_driver_index(getpid())].status);
            msg.task_timer = drivers[find_driver_index(getpid())].task_timer;
            if (msgsnd(msgqid, &msg, sizeof(msg) - sizeof(long), 0) == -1) {
                perror("msgsnd");
                exit(1);
            }
        } else if (strcmp(msg.status, "send_task") == 0) {
            int index = find_driver_index(getpid());
            if (strcmp(drivers[index].status, "Busy") == 0) {
                msg.mtype = 1;
                msg.pid = getpid();
                strcpy(msg.status, "Busy");
                msg.task_timer = drivers[index].task_timer;
                if (msgsnd(msgqid, &msg, sizeof(msg) - sizeof(long), 0) == -1) {
                    perror("msgsnd");
                    exit(1);
                }
            } else {
                strcpy(drivers[index].status, "Busy");
                drivers[index].task_timer = msg.task_timer;
                drivers[index].end_time = time(NULL) + msg.task_timer;
                
                msg.mtype = 1;
                msg.pid = getpid();
                strcpy(msg.status, "Task assigned");
                if (msgsnd(msgqid, &msg, sizeof(msg) - sizeof(long), 0) == -1) {
                    perror("msgsnd");
                    exit(1);
                }
            }
        }
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
    signal(SIGINT, cleanup);
    while(1){
        printf("Enter command: (create_driver, send_task <pid> <task_timer>, get_status <pid>, get_drivers)\n");
        if (fgets(command, sizeof(command), stdin) == NULL) continue;
        command[strcspn(command, "\n")] = 0; 
        
        //Update driver status
        time_t current_time = time(NULL);
        for (int i = 0; i < driver_count;i++){
            if(strcmp(drivers[i].status, "Busy") == 0 && current_time > drivers[i].end_time){
                strcpy(drivers[i].status, "Available");
                drivers[i].task_timer = 0;
            }
        }

        //Parse command
        if (strncmp(command, "create_driver", 13) == 0){
        
            if (driver_count >= MAX_DRIVERS)
            {
                printf("Max drivers limit reached.\n");
                continue;
            }

            pid = fork();
            if (pid == 0){
                driver_process();
            } else if(pid > 0){
                drivers[driver_count].pid = pid;
                strcpy(drivers[driver_count].status, "Available");
                drivers[driver_count].task_timer = 0;
                driver_count++;
                printf("Driver created with PID: %d\n", pid);
            } else {
                perror("fork");
            }

            
        }
        else if (strncmp(command, "send_task", 9) == 0){
            if (sscanf(command, "send_task %d %d", &pid, &task_timer) != 2){
                printf("Usage: send_task <pid> <task_timer>\n");
                continue;
            }   
            int index = find_driver_index(pid);
            if (index == -1){
                printf("Driver with PID %d not found.\n", pid);
                continue;
            }

            msg_buf msg;
            msg.mtype = pid;
            msg.pid = pid;
            msg.task_timer = task_timer;
            strcpy(msg.status, "send_task");
            if (msgsnd(msgqid, &msg, sizeof(msg) - sizeof(long), 0) == -1){
                perror("msgsnd");
                continue;
            }

            //wait for response 
            FD_ZERO(&read_fds);
            FD_SET(0, &read_fds);
            tv.tv_sec = 1; //wait for 1 second
            tv.tv_usec = 0;

            if(select(1, &read_fds, NULL,NULL,&tv)>0){
                 msg_buf response;
                if (msgrcv(msgqid, &response, sizeof(response) - sizeof(long), 1, 0) != -1){
                    if (strcmp(response.status,"Busy") == 0){
                        printf("Driver %d is Busy for %d seconds\n", response.pid, response.task_timer);
                    } else{
                        printf("Task sent to driver with PID %d.\n", pid);
                    }
                   
                }
            }


        }
        else if (strncmp(command, "get_status", 10) == 0){
            if (sscanf(command, "get_status %d", &pid) != 1){
                printf("Usage: get_status <pid>\n");
                continue;
            }
            int index = find_driver_index(pid);
            if (index == -1){
                printf("Driver with PID %d not found.\n", pid);
                continue;
            }
            msg_buf msg;
            msg.mtype = pid;
            msg.pid = pid;
            strcpy(msg.status, "get_status");   
            if (msgsnd(msgqid, &msg, sizeof(msg) - sizeof(long), 0) == -1){
                perror("msgsnd");
                continue;
            }

            //wait for response
            FD_ZERO(&read_fds);
            FD_SET(0, &read_fds);
            tv.tv_sec = 1; 
            tv.tv_usec = 0;
            if(select(1, &read_fds, NULL,NULL,&tv)>0){
                msg_buf response;
                if (msgrcv(msgqid, &response, sizeof(response) - sizeof(long), 1, 0) != -1) {
                    if (strcmp(response.status, "Busy") == 0) {
                        printf("Driver %d is Busy for %d seconds\n", response.pid, response.task_timer);
                    } else {
                        printf("Driver %d is Available\n", response.pid);
                    }
                }
            }
        }
        else if (strcmp(command, "get_drivers") == 0) {
            if (driver_count == 0) {
                printf("No drivers\n");
                continue;
            }

            for (int i = 0; i < driver_count; i++) {
                if (strcmp(drivers[i].status, "Busy") == 0) {
                    printf("Driver %d: Busy (%d seconds)\n", drivers[i].pid, drivers[i].task_timer);
                } else {
                    printf("Driver %d: Available\n", drivers[i].pid);
                }
            }
        }
        else {
            printf("Unknown command: %s\n", command);
        }

    }
    
    return 0;
}