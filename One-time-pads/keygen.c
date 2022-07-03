//Daniel Valadez, 5-30-22, CS344
//This is the keygen file for the one-time pads assignment.
#include "program5.h"

int main(int argc, char * argv[])
{
	//Seed for the randomness. SHOULD ONLY BE USED ONCE.
	srand(time(0));
	int counter = 0; //Incremented later in the loop.
	int number = 0; //Random number
	//char * gen_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ "; //Alphabet and space.
	//We'll first need to check the arguments passed into main.
	//printf("Number of arguments: %d\n", argc); //argc will at least always be 1.
	if(argc < 2)
	{
		//printf("There are no arguments supplied to the keygen program.\n");

		//prints to stderr...
		fprintf(stderr, "There are no arguments supplied to the keygen program.\n");
		exit(0);
	}
	//Else continue...
	else
	{
		//int key_length = strlen(argv[1]); //Not necessarily giving me the correct answer in the context of the problem.
		int key_length = atoi(argv[1]);
		char key[key_length + 1];

		//printf("Outside the loop with key_length = %d\n", key_length);
		while(counter < key_length)
		{
			number = rand() % 27; //Our range is 0-26. A-Z including space.
			//Testing randomness
			//printf("Random number is: %d\n", number);

			//So, we'll see which character we pull from the
			//random number generated at the top of the loop.
			key[counter] = gen_chars[number];
			++counter;
		}
		key[key_length] = '\0';

		//As per the specifications, should also print a new line.
		printf("%s\n", key);
	}
	return 0;
}
