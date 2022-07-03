//Daniel Valadez, CS344, 5-15-22
//This is the header file for the 4th programming assignment.

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include <pthread.h> //Or compile "-pthread"

#define BUFFER_SIZE 50
#define CHARACTERS 1000

/*FUNCTION PROTOTYPES*/
void put_buff_1(char * input);
void * get_input(void * args); //Will use either user input or file.
void * separator(void * args); //Second thread function.
void * replace(void * args); //Third thread function.
void * writeOutput(void * args); //Final thread function.
char * get_buff_1();
void put_buff_2();
char * get_buff_2();
void put_buff_3(char * string);
char * get_buff_3();
