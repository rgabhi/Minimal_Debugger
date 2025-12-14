#ifndef BREAKPOINT_HPP
#define BREAKPOINT_HPP

#include <sys/types.h> // pid_t
#include <cstdint> // std::intptr_t, uint8_t

class BreakPoint {
    public:
        BreakPoint(pid_t pid, std::intptr_t addr);

        void enable();
        void disable();

        bool is_enabled() const;
        std::intptr_t get_address() const;
    private:
        pid_t m_pid;
        std::intptr_t m_addr;
        bool m_enabled;
        uint8_t m_saved_data; // to store og byte

};
#endif