#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define FALSE 0
#define TRUE 1

char dictionary[100000][30];
int dict_count;

int main(){
    char *input;
    read_dictionary();

    while(1){
        printf("Enter word to be spell checked.\n");
        scanf("%s", &input);
        if(is_correct(&input) == TRUE){
            printf("Spelled right\n");
        }
        else{
            printf("Spelled wrong\n");
        }
        if(strcmp(&input, "quit") == 0){
            break;
        }
    }

}

int read_dictionary() {
    int file_size = 0;
    dict_count = 0;
    FILE *fd;
    if ((fd = fopen("dictionary.txt", "r")) == NULL) {
        perror("error opening file");
        return FALSE;
    }

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

//Function to determine whether or not word is in dictionary array
int is_correct(char *line) {
    //If it is in dictionary array, return TRUE
    for (int i = 0; i <= dict_count; i++) {
        if (strcmp(line, dictionary[i]) == 0) {
            return TRUE;
        }
    }
    //If not, return FALSE
    return FALSE;
}
