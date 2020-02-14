#include "shell.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

using namespace std;

void printPrompt() { cout << endl << "myBamsh>>"; }

void readCommand(char *&buffer) {
  buffer = new char[MAX_COMMAND];

  fgets(buffer, MAX_COMMAND, stdin);
}

int parsePath(char *dirs[]) {
  // get system paths
  char *pathEnvVar;
  char *thePath;
  for (int i = 0; i < MAX_PATHS; i++)
	dirs[i] = NULL;
  pathEnvVar = (char *)getenv("PATH");
  thePath = (char *)malloc(strlen(pathEnvVar) + 1);
  strcpy(thePath, pathEnvVar);

  // store path list to dirs
  int count = 0;
  int dirCount = 0;
  char buff[MAX_PATH_LEN];
  int buffCount = 0;
  while (true) {
	// if end of line
	if (thePath[count] == '\0')
	  break;
	// delimiter :
	else if (thePath[count] == ':') {
	  dirs[dirCount] = new char[strlen(buff) + 1];
	  strcpy(dirs[dirCount], buff);

	  ++dirCount;
	  buff[0] = '\0';
	  buffCount = 0;
	  ++count;
	} else {
	  buff[buffCount] = thePath[count];
	  ++buffCount;
	  buff[buffCount] = '\0';
	  ++count;
	}
  }

  // get current directory
  char cwd[MAX_PATH_LEN];
  if (getcwd(cwd, sizeof(cwd)) != NULL) {
	dirs[dirCount] = new char[strlen(cwd) + 1];
	strcpy(dirs[dirCount], cwd);
	++dirCount;
  }
  return 1;
}

void parseCommand(char *commandLine, command_t &command) {
  const char s[2] = " ";
  char *token;
  command.argc = 0;
  command.argv=new char*[MAX_ARGS];

  token = strtok(commandLine, s);

  while (token != NULL) {
	command.argv[command.argc] = new char[strlen(token) + 1];
	strcpy(command.argv[command.argc], token);
	++command.argc;
	
	token = strtok(NULL, s);
  }
}

char *lookupPath(char **argv, char **dir) {
  char *result;

  // Check to see if file name is already an absolute path
  if (*argv[0] == '/') {
	result =new char[strlen(argv[0]) + 1];
	strcpy(result, argv[0]);

	return result;
  }

  // Look in PATH directories.
  char* tempPath=new char[MAX_PATH_LEN];

  for (int d = 0; dir[d] != NULL; ++d) {
	//create file path
	strcpy(tempPath, dir[d]);
	strcat(tempPath,"/");
	strcat(tempPath, argv[0]);

	// santize input
	result = new char[strlen(tempPath) + 1];
	for (int tempCount = 0, resCount = 0; tempCount < strlen(tempPath);
		 ++tempCount) {
	  if (tempPath[tempCount] != '\n') {
		result[resCount] = tempPath[tempCount];
		++resCount;
	  }
	}

	if (access(result, F_OK) != -1) {
       delete[] tempPath;
	  return result;
	}
  }

  // File name not found in any path variable
  fprintf(stderr, "%s: command not found\n", argv[0]);
  delete[] tempPath;
  return NULL;
}
