#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include "header.h"

#define clear() printf("\033[H\033[J")
#define MAX 1024

int stdinDup, stdoutDup, infile, outfile;
int isRedirectIn = 0;
int isRedirectOut = 0; built_in = 0, background = 0;

char* builtins[] = {"quit", "cd", "clr", "dir", "environ", "echo", "pause", "help"};
int (*builtinFN[])(char **) = {&quit, &cd, &clr, &dir, &environ, &echo, &pause, &help};

void para(char** args, int index);

int main(int argc, char **argv){
    //declare variables
    int flag = 1, i = 0;
    char* input;
    char** input_arr;
    char cwd[1024];

    if(argv[1] != NULL){
        batch(argv[1]);
        exit(0);
    }

    //loops until flag is set to 1, which means users calls quit() function.
    while(flag != 0){
        //get the current working directory
        getcwd(cwd, sizeof(cwd));
        //print the cwd and shell name
        printf("%s\nmyshell> ",cwd);
        //call function to get input and store in input variable
        input = get_input();
        //pass input to be parsed using spaces, put into **array holding each word user input
        input_arr = parse(input);
        //pass input_arr into executed function, and return 1/0 to set flag
        flag = execute(input_arr);
    }
    //clear the screen
    clear();
    return 0;
}

//function to get the user input, return it in a char* variable
char* get_input(){
    //malloc space for input to be stored
    char* input = malloc(MAX);
    int flag = 0, len = 0;
    size_t length = 0;

    //set return flag to see if getline() executed correctly
    flag = getline(&input, &length, stdin);
    //if getline() returns -1 exit the function
    if(flag == -1){
        perror("getline error:");
        exit(0);
    }

    //determine length of input usng strlen
    len = strlen(input);
    //getline automatically adds newline character to end of input, we need to get rid of it using this if statement and '\0'
    if(len > 0 && input[len-1] == '\n')
        input[--len] = '\0';

    return input;
}

//function to parse the input using " " and strtok, returns char** argument
char** parse(char* input){
    //declare variables, malloc space for args_arr
    int i = 0;
    char** args_arr = malloc(MAX);

    //set arg = to the result of strtok on input using " "
    char* arg = strtok(input, " ");

    //while loop to fill args_arr[i] with with each arg being the return value of strtok null and " "
    while(arg != NULL){
        args_arr[i] = arg;
        arg = strtok(NULL, " ");
        i++;
    }
    //set index i equal to null
    args_arr[i] = NULL;
    return args_arr;
}

int execute(char** args){
    //declare variables -- most will be used as flags
    int i = 0, j = 0, k = 0, ret = 1, count = 0, doit = 0;
    char* batchtxt = malloc(MAX);
    char** bg = malloc(MAX);

    //if statement saying user entered nothing
    if(args[0] == NULL) {
        return ret;
    }
    i = 0;
    //while loop determining if  any special characters (&,<,>,>>,|) were entered, if not it will continue
    while(args[i] != NULL) {
        //comparing each argument to character '&' and execute code if it is detected
        if(strcmp(args[i], "&") == 0){
                /*
            count = i;
            if(args[i+1] != NULL){
                while(args[i+1] != NULL){
                    bg[k++] = args[i++];
                    i = count;
                    doit = 1;
                }
            }*/
            //else
            if(args[i+1] != NULL)
                para(args, i+1);
            background = 1;
        }
        //comparing each argument to character '<' and execute code if it is detected
        else if(strcmp(args[i], "<") == 0){
            infile = open(args[i+1], O_RDONLY);
            args[i] = NULL;
            isRedirectIn = 1;
        }
        //comparing each argument to character '>' and execute code if it is detected
        else if(strcmp(args[i], ">") == 0){
            outfile = open(args[i+1], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR | O_CLOEXEC);
            args[i] = NULL;
            isRedirectOut = 1;
        }
        //comparing each argument to character '>>' and execute code if it is detected
        else if(strcmp(args[i], ">>") == 0){
            outfile = open(args[i+1], O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR | O_CLOEXEC);
            args[i] = NULL;
            isRedirectOut = 1;
        }
        //comparing each argument to character '|' and execute code if it is detected
        else if(strcmp(args[i], "|") == 0){
            args[i] = NULL;
            ret = pipefn(&args[i-1], &args[i+1]);
            return ret;
        }
        i++;
    }

    //for loop to determine if any of the user commands are built-ins
    for(j = 0; j < 8; j++) {
        //if statement to check if there is a builtin
        if(strcmp(args[0], builtins[j]) == 0){
            //save current i/o values
            stdinDup = dup(STDIN_FILENO);
            stdoutDup = dup(STDOUT_FILENO);
            //determine if redirection is needed
            if(isRedirectIn == 1) {
                dup2(infile, STDIN_FILENO);
            }
            if(isRedirectOut == 1) {
                dup2(outfile, STDOUT_FILENO);
            }
            //execute the matching built in and set the return flag
            ret = (builtinFN[j])(args);
            //reset i/o defaults
            dup2(stdoutDup, STDOUT_FILENO);
            dup2(stdinDup, STDIN_FILENO);
            //set built-in flag to one showing a built-in was executed
            built_in = 1;
        }
    }

    //if there was no built-in call fork() and execute a file
    if(built_in == 0) {
        //declare process id
        pid_t pid;
        //call fork() and the return value is the pid
        pid = fork();
        //if the pid is less than zero there was an error
        if(pid < 0) {
            printf("Error forking\n");
        } //if pid is equal to 0, returned to the newly created process
        else if(pid == 0) {
            //attempt redirection if necessary
            if(isRedirectIn == 1) {
                dup2(infile, STDIN_FILENO);
            }
            if(isRedirectOut == 1) {
                dup2(outfile, STDOUT_FILENO);
            }
            //use execvp to overlay the child process with the new arg
            execvp(args[0], args);
            exit(0);
        }
        else{
            //if not running in background, wait for child process to terminate
            if(background == 0)
                waitpid(pid, NULL, 0);
        }
    }
    //reset built in flag
    built_in = 0;
    return ret;
}

void para(char** args, int index){
    int i;
    //if statement to check if there is a builtin
    for(i = 0; i < 8; i++) {
    if(strcmp(args[index], builtins[i]) == 0){
        //execute the matching built in and set the return flag
        (builtinFN[i])(args);
        //set built-in flag to one showing a built-in was executed
        built_in = 1;
        }
    }
    //if there was no built-in call fork() and execute a file
    if(built_in == 0) {
        //declare process id
        pid_t pid;
        //call fork() and the return value is the pid
        pid = fork();
        //if the pid is less than zero there was an error
        if(pid < 0) {
            printf("Error forking\n");
        } //if pid is equal to 0, returned to the newly created process
        else if(pid == 0) {
            //use execvp to overlay the child process with the new arg
            execvp(args[index], args);
            exit(0);
        }
        else{
            waitpid(pid, NULL, 0);
        }
    }
}

void batch(char* filename){
    //declare variables
    char buffer[500];
    char** input_arr;
    int size;
    //open file 'filename' for reading
    FILE *fp = fopen(filename, "r");

    //if fp is equal to null, error opening file and quit the program
    if(fp == NULL) {
      perror("Error in opening file");
      exit(0);
    }

    // reads text until newline
    while(fgets(buffer, 254, fp)){
        //get rid of trailing newline from fgets statement
        buffer[strcspn(buffer, "\r\n")] = 0;
        //pass the line from file into the parse and input it into a double pointer char
        input_arr = parse(buffer);
        //pass input_arr into execute function to execute the command
        execute(input_arr);
    }
    //close the file
    fclose(fp);
    return 1;
}

int pipefn(char** arg1, char** arg2){
    pid_t pid1, pid2;
    int fd[2];

    //call pipe function to create pipe
    pipe(fd);

    //fork to execute first program and return pid1
    pid1 = fork();
    //if pid1 is less than 0 there is an error
    if(pid1 < 0) {
        printf("Error forking.\n");
    }
    else if(pid1 == 0) {
        //send stdout to the end of the pipe
        dup2(fd[1], STDOUT_FILENO);
        //close read the end of pipe
        close(fd[0]);
        execute(arg1);
        exit(0);
    }
    else{
        //return to parent process
        //fork to execute the second program
        pid2 = fork();
        if(pid2 < 0) {
            printf("Error forking.\n");
        }
        else if(pid2 == 0) {
            //send stdin to read end of pipe
            dup2(fd[0], STDIN_FILENO);
            //close write end of pipe
            close(fd[1]);
            execute(arg2);
            exit(0);
        }
        else {
            //in parent process and wait for child processes to finish
            //close the pipe
            close(fd[0]);
            close(fd[1]);
            //wait for children to finish
            waitpid(pid1, NULL, 0);
            waitpid(pid2, NULL, 0);
        }
    }
    return 1;
}

//returns 0 which will exit the while loop in the main function
int quit(){
    printf("Goodbye.\n");
    return 0;
}

//use chdir to change the directory, if sub 1 is equal to 0 there is no directory found
int cd(char** args){
    if(chdir(args[1]) != 0) {
        printf("No directory found.\n");
    }
    return 1;
}

//call the clear() define wiping the screen
int clr(){
    clear();
    return 1;
}

//display current files in a directory
int dir(){
    //declare DIR pointer
    DIR *dp;
    //declare dirent pointer
    struct dirent *ep;
    //open the directory
    dp = opendir("./");

    if (dp != NULL){
        while (ep = readdir(dp))
            puts (ep->d_name);
        (void) closedir (dp);
    }
    else
        perror ("Couldn't open the directory");

    return 1;
}

int environ(){
    //declare variables and array with the type of environment variables
    int numVars = 5;
    char* vars[] = {"USER", "HOME", "PATH", "SHELL", "PWD"};
    int i;
    //for loop to get each environment status for respective variables
    for(i = 0; i < numVars; i++) {
        const char* v = getenv(vars[i]);
        //if there is a valid environment variable, print to screen
        if(v != NULL) {
            printf("%s=%s\n", vars[i], v);
        }
        //if not, display error message
        else {
            printf("Error: Could not find environment variable %s.\n", vars[i]);
        }
    }
    return 1;
}

int echo(char** args){
    int i = 1;
    //while loop to print args entered after echo command
    while(args[i] != NULL){
        printf("%s ", args[i]);
        i++;
    }
    printf("\n");
    return 1;
}

int pause(){
    //while loop waiting for user to press 'enter'
    while(getchar() != '/n')
    return 1;
}

int help(){
    FILE *fptr;
    char c;

    // Open file
    fptr = fopen("readme.txt", "r");
    if (fptr == NULL) {
        printf("Cannot open file \n");
        exit(0);
    }

    // Read contents from file
    printf("\n\n\n");
    c = fgetc(fptr);
    //display contents of the file to output screen
    while (c != EOF)
    {
        printf ("%c", c);
        c = fgetc(fptr);
    }

    printf("\n");
    fclose(fptr);
    return 1;
}
