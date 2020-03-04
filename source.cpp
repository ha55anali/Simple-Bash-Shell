#include "shell.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <iostream>

using namespace std;

int main()
{
	char* commandLine;
	command_t command;

	//get list of directories
	char** dirs = new char*[MAX_PATHS];
	parsePath(dirs);

	while(1)
	{
		printPrompt();

		//get command from terminal
		readCommand(commandLine);
    // executeCommand(commandLine,command,dirs);
    pipedExecute(commandLine,command,dirs);
	}

	delete[] commandLine;
	for(int c = 0; c < MAX_ARGS; ++c)
	{
		if(dirs[c] != NULL)
			delete[] dirs[c];
	}
	delete[] dirs;
}
