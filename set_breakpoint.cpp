#include "set_breakpoint.hpp"


void set_breakpoint(pid_t pid, void* addr) {

    
    //PTRACE_PEEKTEXT :   It tells the kernel: "Go to the child process (pid), 
    // look at address addr, and copy 8 bytes of data back to me."

    //Null since we just want to read data , when writing we require data to be passed.
    long original_data = ptrace(PTRACE_PEEKTEXT, pid, addr, NULL);

    
    active_breakpoints[addr] = original_data;

    // since ptrace reads 8-Byte of data but We want only first Byte to change to 0xcc .So CPU will see it as a trap.
    //So data does not become corrupted
    long data_with_trap = (original_data & ~0xFF) | 0xCC;


    //PTRACE_POKETEXT writes 8 bytes of data back to it.

    ptrace(PTRACE_POKETEXT, pid, addr, data_with_trap);
    
    printf("Breakpoint set at %p. Replaced byte with 0xCC.\n", addr);
}
