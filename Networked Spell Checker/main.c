#include "proj3_header.h"

int main(int argc, char **argv)  {

    //Initialize queues
    fd_queue = init_queue();

    //Set the buffer size from user input
    printf("Enter the size of the buffer: ");
    scanf("%d", &MAX_SLOTS);
    if ((MAX_SLOTS > 1000) || (MAX_SLOTS < 0)){
        printf("Out of range, set to default");
        MAX_SLOTS = 5;
    }

    //Set the thread count from user input
    printf("Enter number of worker threads:");
    scanf("%d", &NUM_WORKERS);
    if ((NUM_WORKERS > 1000) || (NUM_WORKERS < 0)) {
        printf("Out of range, set to default");
        NUM_WORKERS = 5;
    }

    //Initialize mutex and condition variables
    pthread_mutex_init(&q_mutex, NULL);
    pthread_mutex_init(&log_mutex,NULL);
    pthread_cond_init(&fd_avail, NULL);
    pthread_cond_init(&slots_free, NULL);

    //Array of worker threads
    pthread_t worker_threads[NUM_WORKERS];

    int count = 1;
    //Loop to create worker threads
    for (int i = 0; i < NUM_WORKERS; i++) {
        //printf("%d thread created\n", count);
        //count++;
        pthread_create(&(worker_threads[i]), NULL, worker_func, NULL);
    }

    //Server call
    server_loop(argc, argv);

    return 0;
}
