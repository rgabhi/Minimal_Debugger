#include <iostream>
#include <unistd.h> // fork, execl
#include <sys/ptrace.h> // p_trace
#include <sys/wait.h> // waitpid
#include <sys/types.h> // pid_t


#include "breakpoint.hpp"
#include "registers.hpp"

using namespace std;



int main(int argc, char *argv[]) {
    if(argc < 2){
        cerr << "Usage: " <<argv[0] << "program hex_address" << endl;
        return 1;
    }
    const char* prog_name = argv[1];


    // pass address to test
    std::intptr_t bp_addr = 0;
    if(argc >= 3){
        bp_addr = std::stol(argv[2], nullptr, 16);
    }
    
    // fork process
    pid_t pid = fork();

    if(pid == 0){
        // child preocess
        //PTRACE_TRACEME allows parent to trace this process.
        // it tells kernel that my paretnt is going to control me.
        ptrace(PTRACE_TRACEME, 0, 0, 0);

        //replace this process image with target program.
        execl(prog_name, prog_name, nullptr);

        // if execl returned means there's an error
        cerr << prog_name <<"failed." << endl;
        return 1;
    }
    else if(pid >= 1){
        // parent process -- Debugger
        cout << "Debugger started...\n Child PID: " << pid << endl;
        
        // when child calls exec(), it triggers a SIGTRAP signal, stopping child.
        // parent must wait for this intial stop..
        int status;
        waitpid(pid, &status, 0);

        // test breakpoint
        if(bp_addr != 0){
            BreakPoint bp(pid, bp_addr);
            bp.enable();

            //continue execution
            std::cout << "Continuing..."<< std::endl;
            ptrace(PTRACE_CONT, pid, 0, 0);

            //wait for the breakpoint to hit
            waitpid(pid, &status, 0);

            if(WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP){
                // where we?
                uint64_t current_ip = get_pc(pid);

                // check if at breakpoint addr + 1
                if(current_ip == bp_addr + 1){
                    std::cout << "hit breakpoint at addr 0x"<<std::hex<<bp_addr<<std::dec<<std::endl;
                    
                    //rewind instruction ptr by 1 byte
                    set_pc(pid, bp_addr);
                    
                    //remove breakpoint to continue
                    bp.disable();

                    // single step
                    //we execute just that one og intruction
                    ptrace(PTRACE_SINGLESTEP, pid, 0, 0);
                    waitpid(pid, &status, 0);

                    // re-enable breakpoint in loop
                    bp.enable();
                }
            }

        }

        // todo - add debugger loop

        // continue child until it finishes..
        ptrace(PTRACE_CONT, pid, 0, 0);

        // wait for child to finish executing
        waitpid(pid, &status, 0);
        cout << "child exited." << endl;

    }
    else{
        cerr << "fork failed!" << endl;
        return 1;
    }

    return 0;

}