#include "proj3_header.h"

//Function to read dictionary.txt file
int read_dict(char *dict) {
    int file_size = 0;
    dict_count = 0;
    FILE *fd;
    if ((fd = fopen(dict, "r")) == NULL) {
        perror("error opening file");
        return FALSE;
    }

    /*while (fgetc(fd) != EOF) {
        file_size += 1;
    }
    printf("%d\n", file_size);
    rewind(fd);

   if (dictionary = malloc(sizeof(char *) * file_size)) == 0){
       printf("malloc error");
    }*/

    //If there is any new line characters following string, replace with null terminating characters
    while (fgets(dictionary[dict_count], 25, fd)) {
        if (dictionary[dict_count][strlen(dictionary[dict_count]) - 1] == '\n') {
            dictionary[dict_count][strlen(dictionary[dict_count]) - 2] = '\0';
        } else {
            dictionary[dict_count][strlen(dictionary[dict_count])] = '\0';
        }
        //printf("|%s|\n",dictionary[dict_count]);
        dict_count++;
    }

    return TRUE;
}

//Function to remove newline
char *remove_newline(char *line) {
    //For loop to determine if any of the characters are newline
    for (int i = 0; i < strlen(line); i++) {
        //If any of the characters are newline, replace them with a null terminating character
        if ((line[i] == '\n') || (line[i] == ' ')) {
            line[i] = '\0';
            break;
        }
    }
    return line;
}

//Function to determine whether or not word is in dictionary array
int spelled_right(char *line) {
    //If it is in dictionary array, return TRUE
    for (int i = 0; i <= dict_count; i++) {
        if (strcmp(line, dictionary[i]) == 0) {
            return TRUE;
        }
    }
    //If not, return FALSE
    return FALSE;
}

char **parse_line(char *line) {
    char **tokens;
    char *token;
    char *temp;

    int i = 0;
    temp = strdup(line);

    //Allocate an array of string tokens
    if ((tokens = malloc(sizeof(char *) * 1000)) == NULL)
        exit(EXIT_FAILURE);

    token = strtok(temp, " ");
    while (token != NULL) {
        //For the final token we need to remove the newline for processing
        remove_newline(token);
        tokens[i++] = strdup(token);
        token = strtok(NULL, " ");
    }
    tokens[i] = NULL;
    free(temp);
    free(token);
    return tokens;
}

char *init(int argc, char *argv[]) {
    if (argc > 3) {
        printf("Too many arguments");
        exit(0);
    }
    if (argc > 1) {
        if (read_dict(argv[1]) == FALSE) {
            printf("error opening:%s", argv[1]);
            printf("setting to DEFAULT_DICTIONARY\n");
        }

    } else if (read_dict(DEFAULT_DICTIONARY) == FALSE) {
        perror("Could not read file DEFAULT_DICTIONARY");
        exit(1);
    }
    if (argc < 3) {
        return DEFAULT_PORT_STR;
    } else {
        return argv[2];
    }
}

void *worker_func() {
    char line[MAX_LINE];
    char *p = line;
    char *log_message = malloc(sizeof(char)*200);
    int err_val;

    while (1) {
        int err_val = 0;
        //Lock mutex
        pthread_mutex_lock(&q_mutex);

        //If queue is empty then wait
        while (is_empty(fd_queue) == TRUE) {
            //printf("Queue is empty\n");
            pthread_cond_wait(&fd_avail, &q_mutex);
        }

        //Remove the file descriptor from queue
        int fd = remove_queue(fd_queue);
        printf("Worker thread says 'file descriptor is %d'", fd);
        //printf("value removed %d : new val %d\n",fd,fd_queue->len);

        //Unlock mutex
        pthread_mutex_unlock(&q_mutex);

        //Spell check line loop
        while ((readLine(fd, p, MAX_LINE - 1) > 0)) {
            printf("User entered: %s", line);
            //Call spell checker loop function
            err_val = spell_checker_loop(fd, p);
            //Replace newline with null terminating character
            if(line[strlen(line)-1]=='\n'){
                line[(strlen(line)-1)]='\0';
            }

            //Send log to string called log_msg
            sprintf(log_message,"Thread id: %ld, Read: %s, # of words misspelled: %d\n",            //log event
                    pthread_self(),line,err_val);
            //printf("%s\n",log_message);
            pthread_mutex_lock(&log_mutex);
            log_data(log_message);
            pthread_mutex_unlock(&log_mutex);

        }
        pthread_cond_signal(&slots_free);
        printf("Connection closed.\n");
        close(fd);
    }
}

int spell_checker_loop(int fd, char *line) {
    //char line[MAX_LINE];
    char **temp = parse_line(line);
    int tst = 0;
    //if (line[strlen(line) - 1] == '\n')
    //line[strlen(line) - 1 ] = '\0';

    for (int i = 0; temp[i] != NULL; i++) {
        //If statement to determinee if there is a misspelled word or not
        if (spelled_right(temp[i]) == FALSE) {
            write(fd, "MISSPELLED:", strlen("MISSPELLED:"));
            write(fd, temp[i], strlen(temp[i]));
            write(fd, "\n", strlen("\n"));
            tst++;
        }
    }

    //If val = 0, everything matches up!
    if (tst == 0) {
        write(fd, "OK!\n", strlen("OK!\n"));
    }
    return val;

}

//Given a port number or service as string, returns a descriptor that we can pass to accept()
int getlistenfd(char *port) {
    int listenfd, status;
    struct addrinfo hints, *res, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM; /* TCP */
    hints.ai_family = AF_INET;       /* IPv4 */

    if ((status = getaddrinfo(NULL, port, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo error %s\n", gai_strerror(status));
        exit(EXIT_GETADDRINFO_ERROR);
    }

    // try to bind to the first available address/port in the list. if we fail, try the next one.
    for (p = res; p != NULL; p = p->ai_next) {
        if ((listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) {
            continue;
        }

        if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0) {
            break;
        }
    }
    freeaddrinfo(res);
    if (p == NULL) {
        exit(EXIT_BIND_FAILURE);
    }

    if (listen(listenfd, BACKLOG) < 0) {
        close(listenfd);
        exit(EXIT_LISTEN_FAILURE);
    }
    return listenfd;
}

/* Read characters from 'fd' until a newline is encountered. If a newline
   character is not encountered in the first (n - 1) bytes, then the excess
   characters are discarded. The returned string placed in 'buf' is
   null-terminated and includes the newline character if it was read in the
   first (n - 1) bytes. The function return value is the number of bytes
   placed in buffer (which includes the newline character if encountered,
   but excludes the terminating null byte). */
ssize_t readLine(int fd, void *buffer, size_t n) {
    ssize_t numRead;                    /* # of bytes fetched by last read() */
    size_t totRead;                     /* Total bytes read so far */
    char *buf;
    char ch;

    if (n <= 0 || buffer == NULL) {
        errno = EINVAL;
        return -1;
    }

    buf = buffer;                       /* No pointer arithmetic on "void *" */
    totRead = 0;

    for (;;) {
        numRead = read(fd, &ch, 1);
        if (numRead == -1) {
            if (errno == EINTR)         /* Interrupted --> restart read() */
                continue;
            else
                return -1;              /* Some other error */
        }
        else if (numRead == 0) {      /* EOF */
            if (totRead == 0)           /* No bytes read; return 0 */
                return 0;
            else                        /* Some bytes read; add '\0' */
                break;
        }
        else {                        /* 'numRead' must be 1 if we get here */
            if (totRead < n - 1) {      /* Discard > (n - 1) bytes */
                totRead++;
                *buf++ = ch;
            }
            if (ch == '\n')
                break;
        }
    }

    *buf = '\0';
    return totRead;
}

void server_loop(int argc, char **argv) {
    //Listen socket descriptor
    int listenfd;
    //Connected socket descriptor
    int connectedfd;
    struct sockaddr_storage client_addr;
    socklen_t client_addr_size;
    char client_name[MAX_LINE];
    char client_port[MAX_LINE];
    char *port = init(argc, argv);

    //Setting listen descriptor
    listenfd = getlistenfd(port);

    for (;;) {

        client_addr_size = sizeof(client_addr);
        if ((connectedfd = accept(listenfd, (struct sockaddr *) &client_addr, &client_addr_size)) == -1) {
            fprintf(stderr, "Accept error\n");
            continue;
        }
        if (getnameinfo((struct sockaddr *) &client_addr, client_addr_size,
                        client_name, MAX_LINE, client_port, MAX_LINE, 0) != 0) {
            fprintf(stderr, "Error getting name information about client\n");
        } else {
            printf("accepted connection from %s:%s\n", client_name, client_port);

            pthread_mutex_lock(&q_mutex);

            while (fd_queue->len == MAX_SLOTS) {
                printf("ERROR: Queue Full.\n");
                pthread_cond_wait(&slots_free, &q_mutex);
            }

            add_queue(fd_queue, connectedfd);
            printf("Value added %d : new val %d\n", connectedfd, fd_queue->len);
            pthread_cond_signal(&fd_avail);
            pthread_mutex_unlock(&q_mutex);

        }
    }
}

void log_data(char *data) {
    //Opens log.txt file and prints the data
    FILE *fp = fopen("log.txt","a");
    fprintf(fp,"%s",data);
    fclose(fp);

}

//Functions with basic queue actions
queue *init_queue() {
    queue *temp = (queue *) malloc(sizeof(queue));
    temp->head = NULL;
    temp->tail = NULL;
    temp->len = 0;
    return temp;
}

node *init_node(int *data) {
    node *temp = (node *) malloc(sizeof(node));
    temp->next = NULL;
    temp->data = *data;
    return temp;
}

void add_queue(queue *queue, int data) {
    node *temp = init_node(&data);
    if (is_empty(queue)) {
        queue->head = temp;
        queue->tail = temp;
    } else {
        queue->tail->next = temp;
        queue->tail = temp;
    }
    queue->len += 1;
}

int remove_queue(queue *queue) {
    if (is_empty(queue)) {
        return -1;
    }
    node *temp = queue->head;
    queue->head = temp->next;
    int ret = temp->data;
    free(temp);
    queue->len -= 1;
    if (is_empty(queue)) {
        queue->head = NULL;
        queue->tail = NULL;
    }
    return ret;
}

int is_empty(queue *q) {
    if (q->len == 0) {
        return 1;
    } else
        return 0;
}






