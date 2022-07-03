//Daniel Valadez, 5-4-21, CS344
//This is the driver for the small shell assignment.
//Smallsh is a program designed to execute
//the basic functionalities of a shell. Built-in
//commands include exit, status, and exit.

#include "program3.h"

//Originally I was having issues with these global variables.
void main()
{
    //For signal handling...
	struct sigaction SIGINT_action = { 0 };
	struct sigaction SIGTSTP_action = { 0 };

    //Setting up the singal handler for SIGINT
	SIGINT_action.sa_handler = SIGINT_handler;
	sigfillset(&SIGINT_action.sa_mask);
	sigaction(SIGINT, &SIGINT_action, NULL);

    //Setting up the handler to catch SIGTSTP
	SIGTSTP_action.sa_handler = SIGTSTP_handler;
	sigfillset(&SIGTSTP_action.sa_mask);
	sigaction(SIGTSTP, &SIGTSTP_action, NULL);

    //The only way we will exit this loop is through the exit
    //command because of our signal handling.
	while(flag == 1)
	{
        //Check for process and then parse.
		isRunningProcess();
		parse_input(); //Need to remember to ignore comments.
		builtIns();
	}
}

//This function sets up a space in which the user can free type their
//commands. We can handle variable expansion and signal handling here.
void parse_input()
{
	//Determining other attributes
	char isOutput[5] = { 0 };

	//Prompt
	printf(": ");
	fflush(stdout);
	fgets(input, sizeof(input), stdin);
	removeEndNewLine(input);

	strncpy(isOutput, input, 4);

	if(strcmp(isOutput, "echo") != 0)
	{
		if(strchr(input, '&') != NULL)
		{
			toggle_background();
		}
	}

	if(strstr(input, "TSTP") == NULL)
    {
		if(strstr(input, "$$") != NULL)
        {
			variable_expansion();
        }
    }

	if(strstr(input, "TSTP") != NULL)
    {
		SIGTSTP_handler();
    }
}

//Change the newline character...
//I was having trouble getting any of my commands to work. Quickly realized the
//newline character in input was causing the error.
void removeEndNewLine(char *removeItem)
{
	removeItem[strcspn(removeItem, "\n")] = '\0';
}

//A function to switch $$ to the PID.
void variable_expansion()
{
	char expandCommand[BUFFER] = { 0 };

    //Since we are removing two $ signs at a time, we need to
    //reduce the string length by two.
	int size = (strlen(input) - 2);

	strncpy(expandCommand, input, size);
	strcpy(input, expandCommand);

	//If another pair exists...
	if(strstr(input, "$$") != NULL){
		//This will make sure we loop until we have exhausted double signs.
		variable_expansion();
	}

    //Replace the dollar signs with the process ID.
	sprintf(expandCommand, "%d", getppid());
	strcat(input, expandCommand);
}

//Uses the child PID to create a new process by which we can return
//what process just ended.
void kill_cmd()
{
	char kill[BUFFER] = { 0 };
	int size = (strlen(input) - 11);

	strncpy(kill, input, size);
	strcpy(input, kill);
	sprintf(kill, "%d", getpid());
	strcat(input, kill);
}

//There are going to be three built-in commands: exit, cd and status.
//Command_cd will handle displaying the current working directory and changing directories.
void builtIns()
{
	if(strncmp(input, "cd", 2) == 0)
	{
        //This will, when run by me, take me to my own home directory of valadezd.
        //homedir = getenv("HOME");
		char cwd[BUFFER];
		char * newPath;
        //We need to know where we are.
		getcwd(cwd, sizeof(cwd));

		newPath = strstr(input, " ");

		if(newPath)
		{
			++newPath; //We don't want to start with the whitespace.
			strcat(cwd, "/");
			strcat(cwd, newPath);
			chdir(cwd);
		}

		else
		{
			chdir(getenv("HOME"));
		}

		getcwd(cwd, sizeof(cwd));
		printf("%s\n", cwd);
		fflush(stdout);
	}

    //Status command
	else if(strcmp(input, "status") == 0)
	{
		printf("exit value %d\n", error);
		fflush(stdout);
	}

    //Exit command
	else if(strcmp(input, "exit") == 0)
	{
		flag = 0;
	}

    //The pound symbol tells us we have a comment.
	else if(strncmp(input, "#", 1) == 0 || strcmp(input, " ") == 0)
	{
		//Do nothing if comment
	}

    //By reaching here, that means that the user did not use one of the basic
    //built-in commands.
	else
	{
		forkProcesses();
	}

    //By defualt, we are in the foreground.
	bg_flag = false;
}

//Now we'll handle other commands using functions from the exec family.
void forkProcesses()
{
	spawn_pid = fork();

    //Is there an error?
	if(spawn_pid < 0)
	{
		printf("Error Forking\n");
		fflush(stdout); //Clear the buffer...
		exit(1);
	}

	else if(spawn_pid == 0)
	{
		if(counter > 0)
		{
			if(strstr(input, "kill") != NULL)
			{
				kill_cmd();
			}

		}

        //If no signal handling...
		runCommands();
	}

	else
	{
        //If we have a background process...
		if(bg_flag == true)
		{
			background[background_counter] = spawn_pid;
			++background_counter;

            //The parent will not wait for the child.
			waitpid(spawn_pid, &child, WNOHANG);
			bg_flag = false;

			printf("background pid is %d\n", spawn_pid);
			fflush(stdout); //Clear the buffer...
		}

		else
		{
			waitpid(spawn_pid, &child, 0);

            //If there was an error with ending the child process.
			if(WIFEXITED(child))
            {
                /*Sometimes this makes a zombie process but I don't think it should*/
				error = WEXITSTATUS(child);
            }
		}
	}
}

//Going to be tokenizing the inputs.
void runCommands()
{
	char * commandArgv[512];
	int count = 0;
	int redirect = 0;
	int fileDesc;
	int i = 0;
	int std = 2;

	//Tokenizing arguments makes them easier to work with.
	commandArgv[0] = strtok(input, " ");

	while(commandArgv[count] != NULL)
	{
		++count;
		commandArgv[count] = strtok(NULL, " ");
	}

	while(count != 0)
	{
		if(strcmp(commandArgv[i], "<") == 0)
		{
			fileDesc = open(commandArgv[i + 1], O_RDONLY, 0);

            //Exception handling...
			if(fileDesc < 0)
			{
				printf("cannot open %s for input\n", commandArgv[i + 1]);
				fflush(stdout);
				exit(1);
			}

            /*STDIN = 0 STDOUT = 1*/
			else
			{
				std = 0;
				redirect = 1;
			}
		}
		else if(strcmp(commandArgv[i], ">") == 0)
		{
			fileDesc = open(commandArgv[i + 1], O_CREAT | O_WRONLY, 0755);
			std = 1;
			redirect = 1;
		}

		if(redirect == 1)
		{
			dup2(fileDesc, std);
			commandArgv[i] = 0;
			execvp(commandArgv[0], commandArgv);
			close(fileDesc);
		}

		--count;
		++i;
		redirect = 0;
		std = -2;
	}

    //If we do not have a redirect...
	if(error=execvp(commandArgv[0], commandArgv) != 0);
	{
		printf("%s: no such file or directory\n", input);
		exit(error);
	}
}

//This will allow us to check on children...
void isRunningProcess()
{
	int i;

	for(i = 0; i < background_counter; ++i)
	{
		if(waitpid(background[i], &child, WNOHANG) > 0)
		{
			if(WIFSIGNALED(child))
			{
				printf("background pid terminated is %d\n", background[i]);
				printf("terminated by signal %d\n", WTERMSIG(child));
			}
			if(WIFEXITED(child))
			{
				printf("exit value %d\n", WEXITSTATUS(child));
			}
		}
	}
}

//Toggles foreground and background...
void toggle_background()
{
	char backgroundCommand[BUFFER] = { 0 };
	int size = (strlen(input) - 2);

    //Run the program in the background.
	if(SIGTSTP_flag == false)
    {
		bg_flag = true;
    }

	strncpy(backgroundCommand, input, size);
	strcpy(input, backgroundCommand);
}

/*This last bit is for the signal handling. We need to
handle sigint and sigtstp.*/
void SIGINT_handler(int number)
{
	printf("terminated by signal %d\n", number);
	fflush(stdout); //Clear the buffer
}

void SIGTSTP_handler()
{
	if(SIGTSTP_flag == false)
	{
		counter = 0;
		bg_flag = false;
		SIGTSTP_flag = true;
		printf("Entering foreground-only mode (& is now ignored)\n");
		fflush(stdout);

		++counter;
	}

    //Enabling the background mode.
	else
	{
		SIGTSTP_flag = false;

		printf("Exiting foreground-only mode\n");
		fflush(stdout);

		++counter;
	}
}

//Signal handler for both SIGINT and SIGTSTP
/*void handler(int number)
{
    //If we have a SIGINT...
    if(number == 2)
    {
        //SIGINT CODE
    }

    //Else we have SIGTSTP
    else
    {

    }
}*/
