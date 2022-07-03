//Daniel Valadez, 5-2-22, CS344
//This is the header file for program 3.
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h> //Allows me to use fork()
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h> //Signal handling
#include <stdbool.h>

#define BUFFER 2048 //per assignment requirements
/*GLOBAL VARIABLES*/
int flag = 1;
int error;
char input[BUFFER];
int background[40];
int background_counter;
bool bg_flag = false; //Originally there is no background process. 
pid_t spawn_pid = -2;
int child = -2;
bool SIGTSTP_flag = false;
int counter = 0;

/*FUNUCTION PROTOTYPES*/

//These first set of functions are for parsing the input we recieved.
void parse_input();
void builtIns();
void variable_expansion();
void isRunningProcess();
void removeEndNewLine(char *removeItem);
void toggle_background();
void kill_cmd();

//Once we're done parsing the commands, we end up here.
void forkProcesses();
void runCommands();

//Signal Handlers
void SIGINT_handler(int number);
void SIGTSTP_handler();
