//Daniel Valadez, CS344, 5-15-22
//This file contains the implementations of functions
//first declared in the header file.


#include "program4.h"


pthread_mutex_t mutex_1 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full_1 = PTHREAD_COND_INITIALIZER;

pthread_mutex_t mutex_2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full_2 = PTHREAD_COND_INITIALIZER;

pthread_mutex_t mutex_3 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full_3 = PTHREAD_COND_INITIALIZER;

//I'm going to need three separate buffers for the four threads
//that the prorgam is going to use.
char buffer_1[BUFFER_SIZE][CHARACTERS];
char buffer_2[BUFFER_SIZE][CHARACTERS];
char buffer_3[BUFFER_SIZE][CHARACTERS];


/*PRODUCER AND CONSUMER INDEX VARIABLES*/
int consumer_idx_1 = 0;
int consumer_idx_2 = 0;
int consumer_idx_3 = 0;

int producer_idx_1 = 0;
int producer_idx_2 = 0;
int producer_idx_3 = 0;

int count_1 = 0;
int count_2 = 0;
int count_3 = 0;

//I'm taking a very simlar appraoch to getting input
//as I did in the 3rd programming assignment.
//The difference is that I have to closely monitor the
//stopping condition for accepting input.

void put_buff_1(char * input)
{
		pthread_mutex_lock(&mutex_1);

		strncpy(buffer_1[producer_idx_1], input, strlen(input) + 1);
    //Test
    //printf("Going into the buffer: %s\n", buffer_1[producer_idx_1]);
    //printf("with length: %d\n", strlen(buffer_1[producer_idx_1]));
    ++producer_idx_1;
    ++count_1;
		//printf("This is the count in put_buff: %d\n", count_1);
		pthread_cond_signal(&full_1);
		pthread_mutex_unlock(&mutex_1);
}


//Function of the frist thread.
void * get_input(void * args)
{

    char * input;
    ssize_t buff_size = 0;

    //Going to keep going til we reach a stopping condition. Then we'll break out of the loop.
		//printf("Give us input: \n");
    while(1)
    {
        getline(&input, &buff_size, stdin);
        //Stopping condition...
        if(strcmp(input, "STOP\n") == 0)
        {
            break;
        }

        //Continue to fill the buffer...
        else
        {
            put_buff_1(input);
        }
    }

    return NULL;
}

char * get_buff_1()
{
	  //Lock the mutex before checking if the buffer has data
		pthread_mutex_lock(&mutex_1);
		while (count_1 == 0)
		{
				//Buffer is empty. Wait for the producer to signal that the buffer has data
				pthread_cond_wait(&full_1, &mutex_1);
		}

		//char * string = (char *)calloc(strlen(buffer_1[consumer_idx_1] + 1, sizeof(char *)));
		char * string = malloc(strlen(buffer_1[consumer_idx_1] + 1) *sizeof(char*));
		//char * string;
		strcpy(string, buffer_1[consumer_idx_1]);
		//printf("This is the copied string: %s\n", string);


		//Increment the index from which the item will be picked up
		++consumer_idx_1;
		count_1--;
		// Unlock the mutex
		pthread_mutex_unlock(&mutex_1);
		return string;
}

//This function will be taking a similar appraoch to variable expansion seen in program 3.
void * separator(void * args)
{
		char * string;

		//printf("Outside the loop wiht count: %d\n", count_1);
		//int number = count_1; //Need to do this because grabbing from the buffer will decrement count.
		int variable= 0;
		while(variable != 1000000)
		{
			++variable;
		}
		for(int i = 0; i < producer_idx_1; ++i) //For right now leave it at 8 and handle race condtion after testing.
		{
				//printf("In the loop...\n");
				string = get_buff_1();
				//Now we have to find newline characters and replace them with spaces.
				for(int j = 0; j < strlen(string); ++j)
				{
						//If there's a newline character
						if(string[j] == '\n')
						{
								//printf("We found a newline character...\n");
								string[j] = ' ';
								//printf("The string with spaces: %s\n", string);
						}
				}

				//Once the work is done, we can put it into the next buffer...
				put_buff_2(string);
		}
		return NULL;
}

//Put newly parsed strings into second buffer.
//This follows essentially the same format as the the first put_buff function.
void put_buff_2(char * string)
{
		pthread_mutex_lock(&mutex_2);
		strncpy(buffer_2[producer_idx_2], string, strlen(string) + 1);
    ++producer_idx_2;
    ++count_2;
		pthread_cond_signal(&full_2);
		pthread_mutex_unlock(&mutex_2);
}

//Pass along the second buffer.
char * get_buff_2()
{
		//Lock the mutex before checking if the buffer has data
		pthread_mutex_lock(&mutex_2);
		while (count_2 == 0)
		{
				//Buffer is empty. Wait for the producer to signal that the buffer has data
				pthread_cond_wait(&full_2, &mutex_2);
		}

		//char * string = (char *)calloc(strlen(buffer_1[consumer_idx_1] + 1, sizeof(char *)));
		char * string = malloc(strlen(buffer_2[consumer_idx_2] + 1) *sizeof(char*));
		//char * string;
		strcpy(string, buffer_2[consumer_idx_2]);
		//printf("This is the copied string: %s\n", string);


		//Increment the index from which the item will be picked up
		++consumer_idx_2;
		count_2--;
		// Unlock the mutex
		pthread_mutex_unlock(&mutex_2);
		return string;
}

void * replace(void * args)
{

		char * search = "++";
		char * replacing_char = "^";
		//char * p = strstr(string, search);
		char * p;
		char * string;
		int flag = 1;
		int variable= 0;
		while(variable != 1000000)
		{
			++variable;
		}
		for(int i = 0; i < producer_idx_1; ++i)
		{
				string = get_buff_2();

				//After getting the string from the buffer. Find the substring "++".
				p = strstr(string, search);

				//If the substring is present.
				if(p)
				{
						//printf("The subtring is present..\n");
						/*Because strings can only get smaller by the parameters of the assignment,
						then we don't have to worry about checking for the strings being replaced comapred
						to the new substring. If we did, then this code is horribly optimized.*/
						while(flag)
						{
								memmove(p + strlen(replacing_char),
								p + strlen(search), strlen(p) - strlen(search) + 1
							);

							//Can't use strcpy because it'll also copy the terminating null--breaking the string.
							memcpy(p, replacing_char, strlen(replacing_char));

								//Now, we'll check for the substring again.
								//Lower the flag if not present to get out of the loop.
								p = strstr(string, search);
								if(p == NULL)
								{
										//printf("New String: %s\n", string);
										flag = 0;
								}
						}
				}
				//printf("String going into the third buffer: %s\n", string);
				put_buff_3(string);
		}
		return NULL;
}

void put_buff_3(char * string)
{
		pthread_mutex_lock(&mutex_3);
		strncpy(buffer_3[producer_idx_3], string, strlen(string) + 1);
		++producer_idx_3;
		++count_3;
		pthread_cond_signal(&full_3);
		pthread_mutex_unlock(&mutex_3);
}

char * get_buff_3()
{
		pthread_mutex_lock(&mutex_3);
		while (count_3 == 0)
		{
				pthread_cond_wait(&full_3, &mutex_3);
		}
		char * string = malloc(strlen(buffer_3[consumer_idx_3] + 1) *sizeof(char*));
		strcpy(string, buffer_3[consumer_idx_3]);
		++consumer_idx_3;
		count_3--;
		pthread_mutex_unlock(&mutex_3);
		return string;
}

//For this function I'm printing the output 80 characters at a time. 
void * writeOutput(void * args)
{

		int last_line = 0;
		char output[81] = {0};
		char * print_buffer = calloc(BUFFER_SIZE*CHARACTERS, sizeof(char));
		char * string;

		int variable= 0;
		while(variable != 1000000)
		{
			++variable;
		}
		for(int i = 0; i < producer_idx_1; ++i)
		{
				string = get_buff_3();
				strcat(print_buffer, string);
		}
		//printf("%s\n", print_buffer);

		while(1)
		{
				int cur_line_count = (strlen(print_buffer))/80;

				for(int line = last_line; line < cur_line_count; ++line)
				{
						memset(output, 0, 81);
						strncpy(output, print_buffer + (line*80), 80);
						printf("%s\n", output);
						fflush(stdout);
				}
				last_line = cur_line_count;
				break;
		}
		return NULL;
}
