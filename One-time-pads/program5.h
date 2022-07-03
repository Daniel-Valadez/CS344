//Header file
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdbool.h>

//Moved this from keygen.c, found I'll need it for other stuff.
char * gen_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

/*FUNCTION PROTOTYPES*/
