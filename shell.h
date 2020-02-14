#pragma once

#define LINE_LEN 80
#define MAX_ARGS 64
#define MAX_ARG_LEN 16
#define MAX_PATHS 64
#define MAX_PATH_LEN 96
#define WHITESPACE " .,\t\n"
#define MAX_COMMAND 300

struct command_t {
  char *name;
  int argc;
  char **argv;

};

// print prompt for terminal
void printPrompt();

// takes input from the terminal
void readCommand(char *&buffer);

// returns array of directories to search
int parsePath(char *dirs[]);

// parses string into command_t
void parseCommand(char *commandLine, command_t &command);
