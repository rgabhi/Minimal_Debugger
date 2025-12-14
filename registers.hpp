#ifndef REGISTERS_HPP
#define REGISTERS_HPP

#include <sys/types.h>
#include <cstdint>

// read register val (lookupp for rip)
uint64_t get_register_val(pid_t pid, int reg_offset);

// write val to a register
void set_register_val(pid_t pid, int reg_offset, uint64_t val);

// helper to get instruction ptr - rip
uint64_t get_pc(pid_t pid);

// set instruction ptr
void set_pc(pid_t pid, uint64_t pc);

#endif


