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

void pipedExecute(char* commandLine, command_t& command, char** dirs);

void executeCommand(char* commandLine, command_t& command, char** dirs, int* inPipe, int* outPipe);

// print prompt for terminal
void printPrompt();

// takes input from the terminal
void readCommand(char*& buffer);

// returns array of directories to search
int parsePath(char* dirs[]);

// parses string into command_t
void parseCommand(char* commandLine, command_t& command);

void parseIO(char* commandLine, command_t& command);

bool checkAlphaNum(char n);

char* lookupPath(char** argv, char** dir);

bool internalComm(command_t& command, char** dirs);
