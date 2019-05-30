#ifndef proj3_header
#define proj3_header

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <pthread.h>

#define FALSE 0
#define TRUE 1
#define BACKLOG 10
#define DEFAULT_PORT_STR "12345"
#define DEFAULT_DICTIONARY "dictionary.txt"
#define EXIT_GETADDRINFO_ERROR 1
#define EXIT_BIND_FAILURE 2
#define EXIT_LISTEN_FAILURE 3
#define MAX_LINE 128

int read_dict(char *dict);
char *remove_newline(char *line);
int spelled_right(char *line);
char **parse_line(char *line);
char *init(int argc, char *argv[]);
int getlistenfd(char *);
ssize_t readLine(int fd, void *buffer, size_t n);
void *worker_func();
int spell_checker_loop(int fd,char *line);
void server_loop(int argc, char **argv);
void log_data(char *data);

typedef struct node {
    int data;
    struct node *next;
} node;

typedef struct {
    node *head;
    node *tail;
    int len;
} queue;

queue *init_queue();
node *init_node(int *data);
void add_queue(queue *q, int data);
int remove_queue(queue *q);
int is_empty(queue *q);

// globals
char dictionary[100000][30];  //2-D array to hold dictionary words
int dict_count;                  //counts number of words in the dictionary file

pthread_mutex_t q_mutex;    //Mutex for mutual exclusion
pthread_mutex_t log_mutex;
pthread_cond_t fd_avail;    //Condition variable to notify when the new descriptor was added
pthread_cond_t slots_free;  //Notify when slot is full
queue *fd_queue;        //Queue that holds the file descriptors
int MAX_SLOTS;
int NUM_WORKERS;

#endif
