
#include "remove_breakpoint.hpp"


void remove_breakpoint(pid_t pid, void* addr){


    if (active_breakpoints.find(addr) == active_breakpoints.end()) {
        printf("Error: No breakpoint found at %p to remove.\n", addr);
        return;
    }

   
    ptrace(PTRACE_POKETEXT, pid, addr, active_breakpoints[addr]);

    
    active_breakpoints.erase(addr);

    printf("Breakpoint removed at %p. Original data restored.\n", addr);

    // Rewind the Instruction Pointer (RIP)
    // The CPU executed 0xCC and moved RIP forward by 1. We must pull it back.

    struct user_regs_struct regs;
    ptrace(PTRACE_GETREGS, pid, 0, &regs);
    
    // Only rewind if the CPU is actually AT the instruction after the breakpoint
   
    if (regs.rip == (unsigned long)addr + 1) {
        regs.rip -= 1; 
        ptrace(PTRACE_SETREGS, pid, 0, &regs);
        printf("RIP rewound to %p\n", (void*)regs.rip);
    }
}