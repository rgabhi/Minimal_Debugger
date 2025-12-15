#ifndef REMOVE_BREAKPOINT_HPP
#define REMOVE_BREAKPOINT_HPP

#include<stdio.h>
#include <sys/ptrace.h> // p_trace
#include <sys/wait.h> // waitpid
#include <sys/types.h> // pid_t
#include <map>
#include <sys/reg.h>


using namespace std;

void remove_breakpoint(pid_t pid, void* addr);

// Map to store: Address -> Original Instruction (8 bytes)
extern map<void*, long> active_breakpoints;


#endif
