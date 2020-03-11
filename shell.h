#pragma once
#include <queue>

using namespace std;

#define LINE_LEN 80
#define MAX_ARGS 64
#define MAX_ARG_LEN 16
#define MAX_PATHS 64
#define MAX_PATH_LEN 96
#define WHITESPACE " .,\t\n"
#define MAX_COMMAND 300

struct command_t
{
	char* name;
	int argc;
	char** argv;

	char* input;
	char* output;
};

//recursive execution of commands in queue
void recCall(queue<char*>& q, command_t& command, char** dirs, int* inPipe);

//Execute commandLine with pipes
void pipedExecute(char* commandLine, command_t& command, char** dirs);

//execute single nonpiped command
void executeCommand(char* commandLine, command_t& command, char** dirs, int* inPipe, int* outPipe);

// print prompt for terminal
void printPrompt();

// takes input from the terminal
void readCommand(char*& buffer);

// returns array of directories to search
int parsePath(char* dirs[]);

// parses arguments and io redirection into command_t
void parseCommand(char* commandLine, command_t& command);

// parses IO redirection to command_t
void parseIO(char* commandLine, command_t& command);

bool checkAlphaNum(char n);

//returns absolute path of a command in **dirs
char* lookupPath(char** argv, char** dir);

//checks if command is internal
bool internalComm(command_t& command, char** dirs);
