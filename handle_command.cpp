#include "handle_command.hpp"

#include "set_breakpoint.hpp"
#include "remove_breakpoint.hpp"
#include "print_reg.hpp"


void handle_command(pid_t pid, string cmd) {

    if (cmd == "break") {

        string addr_str;

        cout << "Enter address for breakpoint (hex): ";
        cin >> addr_str;
        
        // Convert string "40113e" (base 16) -> integer 4198718
        long addr = strtoul(addr_str.c_str(), NULL, 16);

        // void* is a generic pointer. It means "This points to a location in memory,
        //  but I have no idea what type of data is stored there."
        set_breakpoint(pid, (void*)addr);
    } 

    else if (cmd == "remove") {

        string addr_str;

        cout << "Enter address to remove breakpoint (hex): ";
        cin >> addr_str;

        long addr = strtoul(addr_str.c_str(), NULL, 16);

        remove_breakpoint(pid, (void*)addr);

    }

    else if (cmd == "continue") {
       

        struct user_regs_struct regs;

        ptrace(PTRACE_GETREGS, pid, 0, &regs);

        void* current_ip = (void*)(regs.rip - 1); 

        // If we are sitting on an ACTIVE breakpoint, force a re-trap
        if (active_breakpoints.count(current_ip)) {
             cout << "Cannot continue: Stuck at active breakpoint " << current_ip << endl;
             cout << "You MUST 'remove' this breakpoint before continuing." << endl;
             
             // Rewind RIP so we don't skip garbage code
             regs.rip -= 1;
             ptrace(PTRACE_SETREGS, pid, 0, &regs);
             
             // We don't even PTRACE_CONT here. We just return to prompt.
             // This effectively "Stops" the user.
             return;
        }
      

        cout << "Continuing execution..." << endl;
        ptrace(PTRACE_CONT, pid, 0, 0);
        
        int status;
        waitpid(pid, &status, 0);
        
        if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP) {
            ptrace(PTRACE_GETREGS, pid, 0, &regs);
            void* hit_addr = (void*)(regs.rip - 1);
            cout << "\n>>> HIT BREAKPOINT at " << hit_addr << " <<<\n";
        }
        else if (WIFEXITED(status)) {
            cout << "\n>>> Child process exited normally. <<<\n";
            exit(0);
        }
    }
    else if (cmd == "regs") {

        print_registers(pid);
        
    }

    else {
        cout << "Unknown command. Use: break, remove, continue, quit" << endl;
    }


}
