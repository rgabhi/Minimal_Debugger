#include "registers.hpp"
#include <sys/ptrace.h>
#include <sys/user.h>
#include <iostream>
#include <iomanip>
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


void dump_registers(pid_t pid) {
   
    struct user_regs_struct regs;
    ptrace(PTRACE_GETREGS, pid, nullptr, &regs);

    std::cout << std::hex << std::setfill('0');
    std::cout << "RAX: 0x" << std::setw(16) << regs.rax << "  ";
    std::cout << "RBX: 0x" << std::setw(16) << regs.rbx << "  ";
    std::cout << "RCX: 0x" << std::setw(16) << regs.rcx << "  ";
    std::cout << "RDX: 0x" << std::setw(16) << regs.rdx << "\n";
    
    std::cout << "RSI: 0x" << std::setw(16) << regs.rsi << "  ";
    std::cout << "RDI: 0x" << std::setw(16) << regs.rdi << "  ";
    std::cout << "RBP: 0x" << std::setw(16) << regs.rbp << "  ";
    std::cout << "RSP: 0x" << std::setw(16) << regs.rsp << "\n";
    
    std::cout << "RIP: 0x" << std::setw(16) << regs.rip << "  ";
    std::cout << "RFLAGS: 0x" << std::setw(16) << regs.eflags << "\n";
    std::cout << std::dec; // Reset to decimal
}

