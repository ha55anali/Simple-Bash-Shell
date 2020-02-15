#include "shell.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>


#include <iostream>

using namespace std;

int main() {
  char *commandLine;
  command_t command;

  char **dirs = new char *[MAX_PATHS];
  parsePath(dirs);

  while (1) {
	printPrompt();

	readCommand(commandLine);
	parseCommand(commandLine, command);

	command.name = lookupPath(command.argv, dirs);
	if (command.name == NULL) {
	  /* Report error */
	  cout<<"error ";
	  continue;
	}

	//[> Create child and execute the command <]
	int pid=fork();

	if (pid==0){
		execv(command.name,command.argv);
	}
		//[> Wait for the child to terminate <]
	wait(NULL);
  }
  return 1;
}
