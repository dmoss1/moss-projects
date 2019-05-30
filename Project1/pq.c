#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0
#define ARRIVAL 0
#define FINISH 1
#define DISK_ARRIVAL 2
#define DISK_FINISH 3
#define SIMULATION_END 4
#define IDLE 0
#define BUSY 1

//Declare global variables & data structures
int SEED = 0;
int INIT_TIME = 0;
int	FIN_TIME = 0;
int ARRIVE_MIN = 0;
int ARRIVE_MAX = 0;
float QUIT_PROB = .2;
int CPU_MIN = 0;
int CPU_MAX = 0;
int DISK1_MIN = 0;
int DISK1_MAX = 0;
int DISK2_MIN = 0;
int DISK2_MAX = 0;
int cpu_state = IDLE, disk1_state = IDLE, disk2_state = IDLE;

//struct to declare an event
typedef struct{
    int type;
    int jobNum;
    int time;
}event;

//struct for the priority queue data structure
typedef struct{
    event *ev;
    int count;
    int size;
}pq;

//struct for the queue data structure
typedef struct{
    event *ev;
    int count;
    int size;
}q;

//Function prototypes
void print_event(event ev);
void print_pq(pq q);
void print_q(q que);
void destroypq(pq q);
void destroyq(q que);
event create_event(int time, int type, int jobNum);
void q_initialize(q *que);
void q_initialize(q *que);
void q_append(q *que, event ev);
event q_remove(q *que);
void pq_initialize(pq *q);
void pq_append(pq *q, event ev);
event pq_remove(pq *q);
int random_gen(int min, int max);
int prob_select(float QUIT_PROB);
void read_file();
event cur_job;

pq pri_q; q disk1; q disk2; q cpu;
int cpu_util = 0; int disk1_util = 0; int disk2_util = 0;
int cur_time = 0, rand_time = 0, eventid = 1, send = 0; int numberadded = 0;
float cpujobs = 0; float disk1jobs = 0; float disk2jobs = 0; int avgsize = 0;
int diskresponse = 0; int cpuresponse = 0;

int main(){
    read_file();        //call function to initialize values from text file
    srand(SEED);     //time needs to be replaced with 'SEED', seeds for rand()
    cur_time = INIT_TIME;       //initializes current time to initial time
    pq_initialize(&pri_q);      //function call to initialize priority queue
    q_initialize(&disk1);       //function call to initialize queue 'disk1'
    q_initialize(&disk2);       //function call to initialize queue 'disk2'
    q_initialize(&cpu);       //function call to initialize queue 'cpu'
    //printf("%d\n", cur_time);

    pq_append(&pri_q, create_event(INIT_TIME, ARRIVAL, 1));         //add the first event starting at time 0
    pq_append(&pri_q, create_event(FIN_TIME, SIMULATION_END, 0));   //add the last event that will kill the program with 1,000 time
    //print_pq(pri_q);

    //while loop with case statement that pops a job from the priority queue and sends it to it's proper function in the case statement
    do{
        cur_time = cur_job.time;        //update the current time
        cur_job = pq_remove(&pri_q);    //remove prioritized event from priority queue
        //printf("job type = %d\n", cur_job.type);
        if(cur_job.type == FINISH)
            cpuresponse = cur_job.time - cur_time;
        if(cur_job.type == DISK_FINISH)
            diskresponse = cur_job.time - cur_time;
        switch (cur_job.type){
            case ARRIVAL: process_cpu(cur_job);break;
            case FINISH: process_cpu(cur_job);break;
            case DISK_ARRIVAL: process_disk(cur_job);break;
            case DISK_FINISH: process_disk(cur_job);break;
            case SIMULATION_END: break;
            }
    }while(pri_q.count != 0 && cur_time < FIN_TIME);

    //print stats
    printf("\n\n");
    printf("Utilization\nCPU: %d\n  Disk1: %d\n  Disk2: %d\n\n", cpu_util, disk1_util, disk2_util);
    printf("Throughput\nCPU: %f\n Disk1: %f\n Disk2: %f\n\n", cpujobs / FIN_TIME, disk1jobs / FIN_TIME, disk2jobs / FIN_TIME);
    avgsize = (disk1.count + disk2.count + cpu.count) / 3;
    printf("Average Queue size: %d\nMax Size\nCPU: %d\n Disk1: %d\n Disk2: %d\n\n", avgsize, cpu.count, disk1.count, disk2.count);
    printf("Response time\nCPU: %d\n Disk1: %d\n Disk2: %d\n", cpuresponse, diskresponse, diskresponse);

    //function calls to free memory
    destroypq(pri_q);
    destroyq(disk1);
    destroyq(disk2);
    destroyq(cpu);

    /*
    int i;
    for(i = 0; i < 1000; i++){
        printf("%d\n", prob_select(20));
    }

*//*
    pq_initialize(&pri_q);
    q_initialize(&disk1);
    event ev1 = create_event(random_gen(ARRIVE_MIN, ARRIVE_MAX), ARRIVAL, 1);
    event ev2 = create_event(random_gen(DISK1_MIN, DISK1_MAX), FINISH, 2);
    event ev3 = create_event(random_gen(DISK2_MIN, DISK2_MAX),  DISK_FINISH, 3);
    event ev4 = create_event(random_gen(ARRIVE_MIN, ARRIVE_MAX), DISK_ARRIVAL, 4);

    q_append(&disk1, create_event(random_gen(ARRIVE_MIN, ARRIVE_MAX), DISK_ARRIVAL, 4));
    q_append(&disk1, create_event(random_gen(ARRIVE_MIN, ARRIVE_MAX), DISK_ARRIVAL, 4));
    q_append(&disk1, ev3);
    q_append(&disk1, ev2);
    q_append(&disk1, ev2);
    q_append(&disk1, ev1);
    q_append(&disk1, ev1);
    printf("disk1 count = %d\n", disk1.count);
    q_append(&disk1, ev2);
    q_append(&disk1, create_event(random_gen(ARRIVE_MIN, ARRIVE_MAX), DISK_ARRIVAL, 4));
    q_append(&disk1, ev2);
    q_append(&disk1, ev4);
    q_append(&disk1, ev2);
    q_append(&disk1, ev4);
    printf("disk1 count = %d\n", disk1.count);
    print_q(disk1);
    q_remove(&disk1);
    q_remove(&disk1);
    q_remove(&disk1);
    q_remove(&disk1);
    q_remove(&disk1);
    q_remove(&disk1);
    printf("disk1 count = %d\n", disk1.count);
    print_q(disk1);

/*
    pq_append(&pri_q, ev1);
    pq_append(&pri_q, ev2);
    pq_append(&pri_q, ev1);
    pq_append(&pri_q, ev2);
    pq_append(&pri_q, ev3);
    pq_append(&pri_q, ev4);
    print_pq(pri_q);

    event tst;
    tst = pq_remove(&pri_q);
    printf("\n");
    print_event(tst);
    printf("\n");
    print_pq(pri_q);
    printf("\n");
    event tst2;
    tst2 = pq_remove(&pri_q);
    printf("\n");
    print_event(tst2);
    printf("\n");
    print_pq(pri_q);
    event tst3;
    tst3 = pq_remove(&pri_q);
    printf("\n");
    print_event(tst3);
    print_pq(pri_q);
    pq_append(&pri_q, ev4);
    pq_append(&pri_q, ev3);
    printf("\n");
    print_pq(pri_q);

    destroypq(pri_q);
    destroyq(disk1);*/
    return 0;
}

/********************************************************************
if current job is an arrival
        create a new arrival event
        add the current job to the cpu queue
            if cpu state is idle
                remove the job from cpu queue for processing
                create an event with type finish and add to priority queue
                set cpu state to busy
else if job is not an arrival (a finish)
        set cpu state to idle
        call prob_select function
            if prob_select is true
                exit the loop
            else if prob_select is not true
                create event as a disk arrival
******************************************************************/
void process_cpu(event job){
    char buffer[40];
    if(job.type == ARRIVAL){
        if(job.time < FIN_TIME){
            sprintf(buffer, "At time %d Job%d arrives at CPU.", job.time, job.jobNum);
            write_file(buffer);
        }
        rand_time = random_gen(ARRIVE_MIN, ARRIVE_MAX) + cur_time;
        //printf("%d", rand_time);
        eventid++;
        pq_append(&pri_q, create_event(rand_time, ARRIVAL, eventid));
        //printf("\n\n");
        //print_pq(pri_q);
        q_append(&cpu, job);
        //print_q(cpu);
        //printf("cpustate = %d, idle = %d, busy = %d\n", cpu_state, IDLE, BUSY);
        if(cpu_state == IDLE){
            job = q_remove(&cpu);
            rand_time = random_gen(CPU_MIN, CPU_MAX) + cur_time;
            //printf("%d\n", rand_time);
            event tmp = create_event(rand_time, FINISH, job.jobNum);
            pq_append(&pri_q, tmp);
            //printf("here\n");
            //print_pq(pri_q);
            if(cur_time < FIN_TIME)
                cpu_util += (tmp.time - cur_time);  // current time = time job starts
            cpu_state = BUSY;   //set cpu state to busy
        }
    }
    else{
        cpujobs++;
        sprintf(buffer, "At time %d Job%d finishes at CPU.", job.time, job.jobNum);
        write_file(buffer);
        cpu_state = IDLE;       //set cpu state to idle bc job type is finish

        if(prob_select(QUIT_PROB) == TRUE){ //decides whether or not a process will terminate or not
            rand_time = random_gen(ARRIVE_MIN, ARRIVE_MAX) + cur_time;  //calculate the time a process terminates
            sprintf(buffer, "At time %d Job%d exits.", cur_job.time, cur_job.jobNum);
            write_file(buffer);
        }
        else{
            rand_time = random_gen(ARRIVE_MIN, ARRIVE_MAX) + cur_time;  //calculate the time
            pq_append(&pri_q, create_event(rand_time, DISK_ARRIVAL, cur_job.jobNum));
            //printf("Job continues.\n");
            }
        }
}
/*****************************************
if job is a disk arrival
    decide which disk to send to based on each disk's size
    if sent to disk 1
        add job to disk1 queue
        if disk1 state is idle
            remove the job from disk1 queue
            create new event with type disk1 finish and add to priority queue
            set disk1 state to busy
    if sent to disk 2
        add job to disk2 queue
        if disk2 state is idle
            remove the job from disk2 queue
            create new event with type disk2 finish and add to priority queue
            set disk2 state to busy
else if job is a disk finish
    create a new event as type arrival and add it to the priority queue
    if job processed through disk1
        set disk1 state to idle
    else if job processed through disk2
        set disk2 state to idle
******************************************/
void process_disk(event cur_job){
    char buffer[40];
    if(cur_job.type == DISK_ARRIVAL){
        if(disk1.count < disk2.count)       //if disk2 has more elements send to disk1
            send = 0;
        else if(disk1.count > disk2.count)      //if disk1 has more elements send to disk2
            send = 1;
        else                                   //if disk1 and disk2 have same amount then pick one at random
            send = random_gen(0,1);
        //printf("select = %d\n", select);
        if(send == 0){
            sprintf(buffer, "At time %d Job%d arrives at disk1.", cur_time, cur_job.jobNum);
            write_file(buffer);
            q_append(&disk1, cur_job); //send cur_job to disk1 queue
            //printf("disk1state = %d\n", disk1_state);
            if(disk1_state == IDLE){ //if disk1 is not being used
                event tmp = q_remove(&disk1); //pop from disk1 queue
                rand_time = random_gen(DISK1_MIN, DISK1_MAX) + cur_time;
                event temp = create_event(rand_time, DISK_FINISH, tmp.jobNum);
                pq_append(&pri_q, temp); //add new job to priority queue with type disk finish
                if(temp.time < FIN_TIME)    //collecting data
                    disk1_util += (temp.time - cur_job.time);
                disk1_state = BUSY; //set disk1 state to busy
            }
        }
        else if(send == 1){
            sprintf(buffer, "At time %d Job%d arrives at disk2.", cur_time, cur_job.jobNum);
            write_file(buffer);
            q_append(&disk2, cur_job);  //add to disk2 queue
            if(disk2_state == IDLE){    //if disk2 is not being used
                event tmp = q_remove(&disk2);   //remove from disk2 queue into temp variable
                rand_time = random_gen(DISK2_MIN, DISK2_MAX) + cur_time;
                event temp = create_event(rand_time, DISK_FINISH, tmp.jobNum);
                pq_append(&pri_q, temp);    //add new event to priority queue with disk finish as type
                if(temp.time < FIN_TIME)    //data collection
                    disk2_util += (temp.time - cur_time);
                disk2_state = BUSY;         //set disk2 state to busy
            }
        }
    }
    else{
        disk1jobs++; disk2jobs++;
        rand_time = random_gen(ARRIVE_MIN, ARRIVE_MAX) + cur_time;
        pq_append(&pri_q, create_event(rand_time, ARRIVAL, cur_job.jobNum)); //create new arrival event

        if(send == 0){
            sprintf(buffer, "At time %d Job%d finished at disk1.", cur_time, cur_job.jobNum);
            write_file(buffer);
            disk1_state = IDLE;     //set disk1 state to idle after process completes
        }
        else if(send == 1){
            sprintf(buffer, "At time %d Job%d finished at disk2.", cur_time, cur_job.jobNum);
            write_file(buffer);
            disk2_state = IDLE;     //set disk2 state to idle after process completes
        }
    }
}

//function to print an event
void print_event(event ev){
    printf("Type: %d  Job Num: %d  Job Time: %d\n", ev.type,ev.jobNum,ev.time);
}

//function to print the given priority queue argument
void print_pq(pq q){
    int i = 0;
    for(i = 0; i < q.count; i++){
        print_event(q.ev[i]);
    }
}

//function to print the given queue argument
void print_q(q que){
    int i = 0;
    for(i = 0; i < que.count; i++){
        print_event(que.ev[i]);
    }
}

//free memory and set count and size to 0
void destroypq(pq q){
    free(q.ev);
    q.count = 0;
    q.size = 0;
}

//free memory and set count and size to 0
void destroyq(q que){
    free(que.ev);
    que.count = 0;
    que.size = 0;
}

//return an event after initializing event properties with the arguments
event create_event(int time, int type, int jobNum){
    event ev;
    ev.time = time;
    ev.jobNum = jobNum;
    ev.type = type;
    return ev;
}

void q_initialize(q *que){\
    que->ev = (event*)malloc(sizeof(event)*5);  //allocate enough space for the queue
    if(que->ev == NULL)
        exit(0);
    que->size = 5;  //default size is 5
    que->count = 0; //nothing in queue

}

void q_append(q *que, event ev){
    event tmp;
    //if size is equal to count, there is not enough room to add to queue.
    //reallocation is necessary to ensure there is enough space in the queue
    if(que->size == que->count){
        printf("Reallocating..\n");
        que->ev = (event*)realloc(que->ev, sizeof(event)*que->size*2);
        que->size = que->size * 2;
        if(que->ev == NULL)
            exit(0);
    }
    que->ev[que->count] = ev;   //set free space at the que->count location equal to the event
    que->count = que->count + 1;    //increase count by one
}

event q_remove(q *que){
    //can't remove if queue is empty
    if(que->count == 0){
        printf("Queue is empty\n");
        return;
    }
    int i = 0;
    event tmp;
    tmp = que->ev[0];   //set tmp event to the first value in queue
    que->count = que->count - 1;    //decrease one from count
    for(i = 0; i <= que->count - 1; i++)    //shift each element left one to compensate for removed event
        que->ev[i] = que->ev[i+1];
    return tmp;
}

void pq_initialize(pq *q){
    q->ev = (event*)malloc(sizeof(event)*5);  //allocate space for queue
    if(q->ev == NULL)   //if allocation fails then exit
        exit(0);
    q->size = 5;        //set default size to 5
    q->count = 0;       //count is 0 because there is nothing in queue

}

void pq_append(pq *q, event ev){
    int j = 0, i = 0;
    event tmp;

    //if size is equal to count, there is not enough room to add to queue.
    //reallocation is necessary to ensure there is enough space in the queue
    if(q->size == q->count){
        //printf("Reallocating..\n");
        q->ev = (event*)realloc(q->ev, sizeof(event)*q->size*2);
        q->size = q->size * 2;
        if(q->ev == NULL)      //if q->ev == NULL then there was an error while reallocating
            exit(0);
    }
    q->ev[q->count] = ev;      //add the event to the queue
    q->count = q->count + 1;    //add one to the count

    //using a simple sort, sort the queue based on the given priority
    //which is time from lowest to highest in this case
     for (i = 0; i < q->count; i++){
        for (j = i + 1; j < q->count; j++){
            if (q->ev[i].time > q->ev[j].time){
                tmp =  q->ev[i];
                q->ev[i] = q->ev[j];
                q->ev[j] = tmp;
            }
        }
    }
}

event pq_remove(pq *q){
    //can't remove if queue is empty
    if(q->count == 0){
        printf("Queue is empty");
        return;
    }
    int i = 0;
    event tmp;
    tmp = q->ev[0];  //set tmp equal to the value being removed
    q->count = q->count - 1;        //decrease queue count
    for(i = 0; i <= q->count - 1; i++)      //shift elements to compensate for missing value at location [0]
        q->ev[i] = q->ev[i+1];
    return tmp;
}

int random_gen(int min, int max){
    //return random value between a certain range
    return (rand() % (max + 1 - min)) + min;
}

int prob_select(float QUIT_PROB){
    int tmp = 0;
    //if under 0 or over 100 out of range
    if(QUIT_PROB < 0 || QUIT_PROB > 100){
        perror("Out of range.");
    }
    //receive random value
    tmp = random_gen(0, 100);

    //return whether to exit or not
    if(tmp <= QUIT_PROB)
        return TRUE;
    else
        return FALSE;
}

void read_file(){
    //declare tmp varables and file pointer
    char buffer[50];
    char temp[50];
    int tmp = 0;
    FILE *fp;

    //open file for reading, if file error then exit program
    fp = fopen("text.txt", "r");
    if(!fp){
        perror("File opening error.\n");
        exit(0);
    }

    /*
        each block of code follows the same premise here.
        1) get line and store it in buffer value
        2) remove new line character
        3) parse the text and value from the buffer string into temporary variables
        4) store tmp integer in respective location
    */

    fgets(buffer, 1000, fp);
    strtok(buffer, "\n");
    sscanf(buffer, "%s %d", temp, &tmp);
    SEED = tmp;
    //printf("SEED = %d\n", SEED);

    fgets(buffer, 1000, fp);
    strtok(buffer, "\n");
    sscanf(buffer, "%s %d", temp, &tmp);
    INIT_TIME = tmp;
    //printf("INIT_TIME = %d\n", INIT_TIME);

    fgets(buffer, 1000, fp);
    strtok(buffer, "\n");
    sscanf(buffer, "%s %d", temp, &tmp);
    FIN_TIME = tmp;
    //printf("FIN_TIME = %d\n", FIN_TIME);

    fgets(buffer, 1000, fp);
    strtok(buffer, "\n");
    sscanf(buffer, "%s %d", temp, &tmp);
    ARRIVE_MIN = tmp;
    //printf("ARRIVE_MIN = %d\n", ARRIVE_MIN);

    fgets(buffer, 1000, fp);
    strtok(buffer, "\n");
    sscanf(buffer, "%s %d", temp, &tmp);
    ARRIVE_MAX = tmp;
    //printf("ARRIVE_MAX = %d\n", ARRIVE_MAX);

    fgets(buffer, 1000, fp);
    strtok(buffer, "\n");
    sscanf(buffer, "%s %d", temp, &tmp);
    QUIT_PROB = tmp;
    //printf("QUIT_PROB = %f\n", QUIT_PROB);

    fgets(buffer, 1000, fp);
    strtok(buffer, "\n");
    sscanf(buffer, "%s %d", temp, &tmp);
    CPU_MIN = tmp;
    //printf("CPU_MIN = %d\n", CPU_MIN);

    fgets(buffer, 1000, fp);
    strtok(buffer, "\n");
    sscanf(buffer, "%s %d", temp, &tmp);
    CPU_MAX = tmp;
    //printf("CPU_MAX = %d\n", CPU_MAX);

    fgets(buffer, 1000, fp);
    strtok(buffer, "\n");
    sscanf(buffer, "%s %d", temp, &tmp);
    DISK1_MIN = tmp;
    //printf("DISK1_MIN = %d\n", DISK1_MIN);

    fgets(buffer, 1000, fp);
    strtok(buffer, "\n");
    sscanf(buffer, "%s %d", temp, &tmp);
    DISK1_MAX = tmp;
    //printf("DISK1_MAX = %d\n", DISK1_MAX);

    fgets(buffer, 1000, fp);
    strtok(buffer, "\n");
    sscanf(buffer, "%s %d", temp, &tmp);
    DISK2_MIN = tmp;
    //printf("DISK2_MIN = %d\n", DISK2_MIN);

    fgets(buffer, 1000, fp);
    strtok(buffer, "\n");
    sscanf(buffer, "%s %d", temp, &tmp);
    DISK2_MAX = tmp;
    //printf("DISK2_MAX = %d\n", DISK2_MAX);

    fclose(fp);
}

//write file function to write text argument to log.txt
void write_file(char *text){
    FILE *fp = fopen("log.txt", "a"); //open file log.txt for writing

    //if file fails to open exit
    if(!fp)
        perror("File open error.\n");

    //print text argument to log.txt
    fprintf(fp, "%s\n", text);

    //close the file
    fclose(fp);
}


