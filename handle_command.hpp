#ifndef HANDLE_COMMAND_HPP
#define HANDLE_COMMAND_HPP



#include <stdio.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <iostream>
#include <sys/user.h>
#include <map>
#include <iomanip> // For nice hex formatting

#include "set_breakpoint.hpp"
#include "remove_breakpoint.hpp"


void handle_command(pid_t pid, string cmd);


using namespace std;


#endif