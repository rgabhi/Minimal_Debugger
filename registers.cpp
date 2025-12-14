#include "registers.hpp"
#include <sys/ptrace.h>
#include <sys/user.h>
#include <iostream>
#include <algorithm>

// map our request to user_req_struct
// on x86_64, instruction ptr is rip

uint64_t get_register_val(pid_t pid, int reg_offset){
    struct user_regs_struct regs;
    ptrace(PTRACE_GETREGS, pid, nullptr, &regs);
    return 0;
}

uint64_t get_pc(pid_t pid){
    struct user_regs_struct regs;
    ptrace(PTRACE_GETREGS, pid, nullptr, &regs);
    return regs.rip;
}

void set_pc(pid_t pid, uint64_t pc){
    struct user_regs_struct regs;
    //read curr reg
    ptrace(PTRACE_GETREGS, pid, nullptr, &regs);
    // update instruction ptr
    regs.rip = pc;
    //write back the modified reg
    ptrace(PTRACE_SETREGS, pid, nullptr, &regs);

}
