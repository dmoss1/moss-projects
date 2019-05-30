#ifndef HEADER_H_
#define HEADER_H_
int execute(char** args);
char* get_input();
char** parse(char* input);
void batch(char* filename);
int parallel(char** args, int count, int arr[]);
int quit(); int cd(char** args); int clr(); int dir(); int environ(); int echo(char** args); int pause(); int help();
#endif