#include "debugger_core.hpp"
#include "registers.hpp"
#include <iostream>
#include <sstream>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <iomanip>
#include <cstring> // for strsignal
#include <vector>
#include <cctype> // for std::isprint

Debugger::Debugger(std::string prog_name, pid_t pid)
    : m_prog_name(prog_name), m_pid(pid){}

void Debugger::run(){
    // init wait for the child to stop at exec
    int wait_status;
    waitpid(m_pid, &wait_status, 0);
    
    // prompt loop
    while(true){
        std::cout << "dbg> ";
        std::string input;
        if(!std::getline(std::cin, input)) break;
        if(input.empty()) continue;
        handle_cmd(input);
    }
}

void Debugger::handle_cmd(const std::string& line){
    std::stringstream ss(line);
    std::string cmd;
    ss >> cmd;

    if(cmd == "c" || cmd == "cont" || cmd == "continue"){
        continue_exec();
    }
    else if(cmd == "b" || cmd == "break"){
        std::string addr_str;
        ss >> addr_str;
        // parse hex address
        std::intptr_t addr = std::stol(addr_str, nullptr, 16);
        set_breakpoint_(addr);
    }
    else if (cmd == "d" || cmd == "del" || cmd == "delete") {
        std::string addr_str;
        ss >> addr_str;
        std::intptr_t addr = std::stol(addr_str, nullptr, 16);
        remove_breakpoint_(addr);
    }
    else if (cmd == "s" || cmd == "step") {
        single_step();
    }
    else if(cmd == "regs"){
        // inspect registers 
        // std::cout << "RIP: 0x" << std::hex << get_pc(m_pid) << std::dec << std::endl;
        dump_registers(m_pid); 
    }
    else if (cmd == "status") {
        print_status();
    }
    else if(cmd == "q" || cmd == "quit"){
        std::cout << "exiting..." << std::endl;
        kill(m_pid, SIGKILL);
        exit(0);
    }
    else if (cmd == "x" || cmd == "examine") {
        std::string addr_str;
        int n_bytes = 32; //default 32 bytes if not specified

        ss >> addr_str;
        // check if user provided the number of bytes
        if (ss >> n_bytes) {
            // n_bytes is updated
        }
        
        if (!addr_str.empty()) {
            std::intptr_t addr = std::stol(addr_str, nullptr, 16);
            read_memory(addr, n_bytes);
        } else {
            std::cout << "Usage: x <address> [num_bytes]" << std::endl;
        }
    }
    else{
        std::cerr << "Unknown cmd!" << std::endl;
    }
}

void Debugger::set_breakpoint_(std::intptr_t addr){
    std::cout << "Setting breakpoint at 0x" << std::hex << addr << std::dec << std::endl;
    
    // Create the breakpoint object
    BreakPoint bp(m_pid, addr);
    
    // Write 0xCC to memory
    bp.enable();
    
    // Save it in our map
    m_breakpoints.insert({addr, bp}); 
}

void Debugger::remove_breakpoint_(std::intptr_t addr) {
    if (m_breakpoints.count(addr)) {
        // Always ensure we restore the original instruction before deleting the object
        auto& bp = m_breakpoints.at(addr);
        if (bp.is_enabled()) {
            bp.disable();
        }
        m_breakpoints.erase(addr);
        std::cout << "Breakpoint removed from 0x" << std::hex << addr << std::dec << std::endl;
    } else {
        std::cerr << "No breakpoint found at 0x" << std::hex << addr << std::dec << std::endl;
    }
}

void Debugger::continue_exec(){
    // if stopped at breakpoint, step over first
    step_over_();
    //resume exec
    ptrace(PTRACE_CONT, m_pid, 0, 0);
    wait_for_sig();
}

void Debugger::single_step() {
    std::intptr_t current_pc = get_pc(m_pid);

    if (m_breakpoints.count(current_pc)) {
        step_over_();
        std::cout << "Stepped to 0x" << std::hex << get_pc(m_pid) << std::dec << std::endl;
    } 
    else {
        ptrace(PTRACE_SINGLESTEP, m_pid, 0, 0);
        wait_for_sig(); 
    }
}

void Debugger::wait_for_sig(){
    int wait_status;
    waitpid(m_pid, &wait_status, 0);
    m_last_wait_status = wait_status;

    if(WIFEXITED(wait_status)){
        std::cout << "Child process exited with code: " << WEXITSTATUS(wait_status) << std::endl; 
        exit(0);
    }
    else if(WIFSTOPPED(wait_status)){
        int sig = WSTOPSIG(wait_status);
        
        // only print if it's NOT a SIGTRAP (step) to keep output clean, 
        std::cout << "Process stopped. Signal: " << strsignal(sig) << " (" << sig << ")" << std::endl;

        if(sig == SIGTRAP){
            // check if hit a breakpoint
            // rip at address  + 1
            std::intptr_t current_pc = get_pc(m_pid);
            std::intptr_t bp_addr = current_pc - 1;
            
            if(m_breakpoints.count(bp_addr)){
                std::cout << "Hit breakpoint at 0x" << std::hex << bp_addr << std::dec << std::endl;
                // rewind instruction ptr
                set_pc(m_pid, bp_addr); // Rewind
            } 
        }
    }
}

void Debugger::step_over_(){
    // check if curr pc is a breakpoint
    std::intptr_t curr_pc = get_pc(m_pid);
    if(m_breakpoints.count(curr_pc)){
        auto& bp = m_breakpoints.at(curr_pc);
        if(bp.is_enabled()){
            // disable: restore og instruction
            bp.disable();
            // single step
            ptrace(PTRACE_SINGLESTEP, m_pid, 0, 0);
            int wait_status;
            waitpid(m_pid, &wait_status, 0);
            bp.enable();
        }
    }
}

void Debugger::print_status() {
    std::cout << "--- Process Status ---" << std::endl;
    std::cout << "PID: " << m_pid << std::endl;
    std::cout << "Target: " << m_prog_name << std::endl;
    
    if (WIFSTOPPED(m_last_wait_status)) {
        int sig = WSTOPSIG(m_last_wait_status);
        std::cout << "State: STOPPED" << std::endl;
        std::cout << "Last Signal: " << strsignal(sig) << " (" << sig << ")" << std::endl;
        std::cout << "Current PC: 0x" << std::hex << get_pc(m_pid) << std::dec << std::endl;
    } 
    else if (WIFEXITED(m_last_wait_status)) {
        std::cout << "State: EXITED" << std::endl;
        std::cout << "Exit Code: " << WEXITSTATUS(m_last_wait_status) << std::endl;
    }
    std::cout << "----------------------" << std::endl;
}

void Debugger::read_memory(std::intptr_t addr, size_t n_bytes) {
    std::cout << "Reading " << std::dec << n_bytes << " bytes from 0x" << std::hex << addr << "\n";
    for (size_t i = 0; i < n_bytes; i += 16) { 
        std::cout << "0x" << std::hex << std::setfill('0') << std::setw(16) << (addr + i) << ":  ";
        std::vector<uint8_t> line_bytes;
        for (size_t j = 0; j < 16; ++j) {
            if (i + j < n_bytes) {
                std::intptr_t current_addr = addr + i + j;
                std::intptr_t aligned_addr = current_addr & ~0x7; 
                errno = 0;
                long data = ptrace(PTRACE_PEEKDATA, m_pid, aligned_addr, nullptr);
                if (errno != 0) {
                    std::cout << "?? ";
                    line_bytes.push_back(0); 
                    continue;
                }
                int byte_offset = current_addr - aligned_addr;
                uint8_t byte = (data >> (8 * byte_offset)) & 0xFF;
                std::cout << std::hex << std::setw(2) << (int)byte << " ";
                line_bytes.push_back(byte);
            } else {
                std::cout << "   "; 
            }
        }
        std::cout << " |";
        for (auto b : line_bytes) {
            if (std::isprint(b)) std::cout << (char)b;
            else std::cout << ".";
        }
        std::cout << "|" << std::endl;
    }
}