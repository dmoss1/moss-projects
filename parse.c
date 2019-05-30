#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
    //declare variables, malloc space for args_arr
    int i = 0;
    char** args_arr = malloc(1024);
    char input[] = "This is the parse function";

    //set arg = to the result of strtok on input using " "
    char* arg = strtok(input, " ");

    //while loop to fill args_arr[i] with with each arg being the return value of strtok null and " "
    while(arg != NULL){
        args_arr[i] = arg;
        arg = strtok(NULL, " ");
        printf("|%s|\n", args_arr[i]);
        i++;
    }
    args_arr[i] = NULL;
}

