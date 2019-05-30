	There are two different types of data structures in this program. One priority queue and three FIFO Queues.
In both of these types structures there are three variables declared as int size, int count, and event *ev. The 
size variable is the max size before the queue has to realloc, the count is the number of elements in the queue,
and event is a typedef struct included in each of the data structures. Using an array, when size is equal to data 
the queue will reallocate so there will be enough room in the queue for more elements. For the priority queue, 
there is a queue algorithm  that when a element is added using the pq_append() function, it sorts the 
queue by time making it a priority queue. When an element is removed from either queue, each element is 
shifted to the left one to compensate for the now missing element. Saying this, there is also a typedef struct 
called event that includes three variables as well. The event struct describes each job that will be processed. 
Each job has three parts, the job number, the job time, and the job type.
There are five different types of events
	1) Arrival
	2) Finish
	3) Disk Arrival
	4) Disk Finish
	5) Simulation End

	The program starts with by seeding the rand() function and initializing variables/structs.
It then adds the two initial events, start and end. Proceeding to the while loop, it will run until the pq is empty
or until the current time is more than the fin time. Using a switch statement, we send a job to a certain function
that coincides with it's job type. Each time the while loop is executed, an event is popped off of the pq.
Included are a couple of functions used for data collection.

CPU PROCESS PSEUDOCODE:

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

DISK PROCESS PSEUDOCODE:

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

All function prototypes:
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
	int prob_select(int QUIT_PROB);
	void read_file();

Testing of the queue/priority queue:

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

    printf("\n\n");
    q_append(&disk1, ev1);
    q_append(&disk1, ev2);
    q_append(&disk1, ev3);
    q_append(&disk1, ev4);
    q_append(&disk1, ev1);
    q_append(&disk1, ev1);
    q_append(&disk1, ev2);
    q_append(&disk1, ev3);
    q_append(&disk1, ev4);
    q_append(&disk1, ev1);
    print_q(disk1);
    printf("\n\n\n");
    q_remove(&disk1);
    q_remove(&disk1);
    q_remove(&disk1);
    print_q(disk1);

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
    destroyq(disk1);

Testing prob_select function:

	int i;
	for(i = 0; i < 1000; i++){
        printf("%d\n", prob_select(20));
    	}

int prob_select(QUIT_PROB){
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

Statistical result is printed to screen.