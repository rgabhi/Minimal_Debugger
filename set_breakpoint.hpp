#ifndef SET_BREAKPOINT_HPP
#define SET_BREAKPOINT_HPP

#include<stdio.h>
#include <sys/ptrace.h> // p_trace
#include <sys/wait.h> // waitpid
#include <sys/types.h> // pid_t


using namespace std;

void set_breakpoint(pid_t pid, void* addr);

// Map to store: Address -> Original Instruction (8 bytes)
extern map<void*, long> active_breakpoints;


#endif

