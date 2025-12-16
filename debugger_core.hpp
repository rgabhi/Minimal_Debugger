#ifndef DEBUGGER_CORE_HPP
# define DEBUGGER_CORE_HPP

#include <string>
#include <unordered_map>
#include <sys/types.h> 
#include "breakpoint.hpp"

class Debugger{
    public:
        Debugger(std::string prog_name, pid_t pid);
        void run();
        void handle_cmd(const std::string& line);
        void continue_exec();
        void set_breakpoint_(std::intptr_t addr);
        void step_over_();
        void single_step();
        void remove_breakpoint_(std::intptr_t addr);
        void print_status();
        // Reads 'n' bytes starting from 'addr' and prints a hex dump
        void read_memory(std::intptr_t addr, size_t n_bytes);
    
    private:
        std::string m_prog_name; 
        pid_t m_pid;
        std::unordered_map<std::intptr_t, BreakPoint> m_breakpoints;
        int m_last_wait_status;
        
        void wait_for_sig();

};

#endif