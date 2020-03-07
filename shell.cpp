#include "shell.h"
#include <queue>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

void pipedExecute(char* commandLine, command_t& command, char** dirs){
  queue<char*> comm;
  char* token=strtok(commandLine,"|");
  while (token != NULL){
    char * temp=new char[strlen(token)+1];
    strcpy(temp,token);
    comm.push(temp);

    token=strtok(NULL,"|");
  }

  if (comm.size() == 1){
    token=comm.front();
    comm.pop();

    executeCommand(token,command,dirs,NULL,NULL);
    return;
  }

  int inPipe[2];
  int outPipe[2];

  //get first command
  token=comm.front();
  comm.pop();

  //execute first command
  pipe(outPipe);
  executeCommand(token,command,dirs,NULL,outPipe);

  recCall(comm,command,dirs,outPipe);

  close(outPipe[1]);
  close(outPipe[0]);
}

void recCall(queue<char*>& q,command_t& command, char** dirs,int* inPipe){
  
  char* comman=q.front();
  q.pop();

  if (q.size()==1){
    int pOut[2];
    pipe(pOut);

    executeCommand(comman,command,dirs,inPipe,pOut);

    recCall(q,command,dirs,pOut);
    close(pOut[0]);
    close(pOut[1]);
  }

  executeCommand(comman,command,dirs,inPipe,NULL);
}

void executeCommand(char* commandLine, command_t& command, char** dirs, int *inPipe, int* outPipe){
  parseCommand(commandLine, command);

  if(internalComm(command, dirs) == 0)
		{

			// check if command exists
			command.name = lookupPath(command.argv, dirs);
			if(command.name == NULL)
        {
          /* Report error */
          cout << "error ";
          return;
        }

			//[> Create child and execute the command <]
			int pid = fork();

			if(pid == 0)
        {
          if (inPipe != NULL){
            close(inPipe[1]);
            dup2(inPipe[0],0);
            close(inPipe[0]);
          }
          if (outPipe !=NULL){
            close(outPipe[0]);
            dup2(outPipe[1],1);
            close(outPipe[1]);
          }

          execv(command.name, command.argv);
        }

			//[> Wait for the child to terminate <]
      if (inPipe != NULL)
        close(inPipe[0]);
      if (outPipe != NULL)
        close(outPipe[1]);
			wait(NULL);
		}
}

void printPrompt()
{
	cout << endl << "myBamsh>>";
}

void readCommand(char*& buffer)
{
	buffer = new char[MAX_COMMAND];

	fgets(buffer, MAX_COMMAND, stdin);

	// remove newline char
	for(int c = 0; c < strlen(buffer); ++c)
	{
		if(buffer[c] == '\n')
			buffer[c] = '\0';
	}
}

int parsePath(char* dirs[])
{
	// get system paths
	char* pathEnvVar;
	char* thePath;
	for(int i = 0; i < MAX_PATHS; i++)
		dirs[i] = NULL;
	pathEnvVar = (char*)getenv("PATH");
	thePath = (char*)malloc(strlen(pathEnvVar) + 1);
	strcpy(thePath, pathEnvVar);

	// store path list to dirs
	int count = 0;
	int dirCount = 0;
	char buff[MAX_PATH_LEN];
	int buffCount = 0;
	while(true)
	{
		// if end of line
		if(thePath[count] == '\0')
			break;
		// delimiter :
		else if(thePath[count] == ':')
		{
			dirs[dirCount] = new char[strlen(buff) + 1];
			strcpy(dirs[dirCount], buff);

			++dirCount;
			buff[0] = '\0';
			buffCount = 0;
			++count;
		}
		else
		{
			buff[buffCount] = thePath[count];
			++buffCount;
			buff[buffCount] = '\0';
			++count;
		}
	}

	// get current directory
	char cwd[MAX_PATH_LEN];
	if(getcwd(cwd, sizeof(cwd)) != NULL)
	{
		dirs[dirCount] = new char[strlen(cwd) + 1];
		strcpy(dirs[dirCount], cwd);
		++dirCount;
	}
	return 1;
}

void parseCommand(char* commandLine, command_t& command)
{
  parseIO(commandLine,command);

	const char s[2] = " ";
	char* token;
	command.argc = 0;
	command.argv = new char*[MAX_ARGS];

	token = strtok(commandLine, s);

	while(token != NULL)
	{
		command.argv[command.argc] = new char[strlen(token) + 1];
		strcpy(command.argv[command.argc], token);
		++command.argc;

		token = strtok(NULL, s);
	}
}

bool checkAlphaNum(char n)
{
  bool t=0;
  if (n>='a' && n<='z')
    t=1;

  if (n>='A' && n<='Z')
    t=1;

  if (n>='0' && n<='9')
    t=1;

  return t;
}

void parseIO(char* commandLine,command_t& command)
{
  command.input=NULL;
  command.output=NULL;

  //get input file
  int iC=0;
  //start of input file
  for (;iC<strlen(commandLine);++iC)
    if (commandLine[iC] == '<')
      break;
  //copy to struct
  if (iC != strlen(commandLine))
    {
      int inLenght=0;
      ++iC;

      //get lenght of filename
      for (;checkAlphaNum(commandLine[iC+inLenght])==1;++inLenght);
      command.input=new char[inLenght+1];
      //copy filename
      for (int c=0;c<inLenght;c++){
        command.input[c]=commandLine[c+iC];
        command.input[c+1]='\0';
      }
      
    }
  //get output file
  int oC=0;
  for (;oC<strlen(commandLine);++oC)
    if (commandLine[oC] == '>')
      break;
  if (oC != strlen(commandLine))
    {
      int oLenght=0;
      ++oC;
      for (;checkAlphaNum(commandLine[oC+oLenght])==1;++oLenght);
      command.output=new char[oLenght+1];
      for (int c=0;c<oLenght;c++){
        command.output[c]=commandLine[c+oC];
        command.output[c+1]='\0';
      }
      
    }

  commandLine[min(oC,iC)-1]='\0';
}

char* lookupPath(char** argv, char** dir)
{
	char* result;

	// Check to see if file name is already an absolute path
	if(*argv[0] == '/')
	{
		result = new char[strlen(argv[0]) + 1];
		strcpy(result, argv[0]);

		return result;
	}

	// Look in PATH directories.
	char* tempPath = new char[MAX_PATH_LEN];

	for(int d = 0; dir[d] != NULL; ++d)
	{
		//create file path
		strcpy(tempPath, dir[d]);
		strcat(tempPath, "/");
		strcat(tempPath, argv[0]);

		if(access(tempPath, F_OK) != -1)
		{
			return tempPath;
		}
	}

	// File name not found in any path variable
	fprintf(stderr, "%s: command not found\n", argv[0]);
	delete[] tempPath;
	return NULL;
}

bool internalComm(command_t& command, char** dirs)
{

	if(strcmp(command.argv[0], "cd") == 0)
	{

		if(chdir(command.argv[1]) == 0)
		{

			char cwd[MAX_PATH_LEN];
			// replace working directory in dirs
			if(getcwd(cwd, sizeof(cwd)) != NULL)
			{
				int c = 0;
				for(; dirs[c] != NULL; ++c)
					;
				--c;
				delete[] dirs[c];
				strcpy(dirs[c], cwd);
			}
		}
		else
			cout << "error, cannot change directory";

		return 1;
	}

	return 0;
}
