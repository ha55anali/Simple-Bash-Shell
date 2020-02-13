#include "shell.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

using namespace std;

void printPrompt() { cout << endl << "myBamsh>>"; }

void readCommand(char *&buffer) {
  buffer = new char[500];

  fgets(buffer, sizeof(buffer), stdin);
}

