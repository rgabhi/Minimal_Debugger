#include "breakpoint.hpp"
#include <sys/ptrace.h>
#include <iostream>


BreakPoint::BreakPoint(pid_t pid, std::intptr_t addr)
    :m_pid(pid), m_addr(addr), m_enabled(false), m_saved_data(0){}

void BreakPoint::enable(){
    // read word(8 bytes) at the address
    //PTRACE_PEEKTEXT reads from the text (code) segment
    long data = ptrace(PTRACE_PEEKTEXT, m_pid, m_addr, nullptr);

    // save og bottom byte
    m_saved_data = static_cast<uint8_t>(data &0xFF);

    // mask out bottom byte and replace it with 0xCC (INT 3)
    uint64_t int3 = 0xCC;
    uint64_t data_with_int3 = ((data & ~0xFF) | int3);

    //write modified word back to memory
    //PTRACE_POKETEXT writes to the text segement
    ptrace(PTRACE_POKETEXT,m_pid, m_addr, data_with_int3);

    m_enabled = true;
    std::cout << "breakpoint set at 0x"<<std::hex<<m_addr<<std::dec<<std::endl;

}

void BreakPoint::disable(){
    // read from curr data at the address (having 0xcc)
    long data = ptrace(PTRACE_PEEKTEXT, m_pid, m_addr, nullptr);
    uint64_t restored_data = ((data & ~0xFF) | m_saved_data);
    //write back
    ptrace(PTRACE_POKETEXT, m_pid, m_addr, restored_data);
    m_enabled=false;
    std::cout<<"breakpoint removed from 0x"<<std::hex<<m_addr<<std::dec<<std::endl;
}

bool BreakPoint::is_enabled()const {return m_enabled;}

std::intptr_t BreakPoint::get_address() const{return m_addr;}

