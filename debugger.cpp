#include <iostream>
#include <unistd.h> // fork, execl
#include <sys/ptrace.h> // p_trace
// #include <sys/wait.h> // waitpid
// #include <sys/types.h> // pid_t

#include "debugger_core.hpp"
#include "colors.hpp"


using namespace std;


int main(int argc, char* argv[]) {
    if(argc < 2){
        std::cerr << "Usage: " <<argv[0] << "[program]" << endl;
        return 1;
    }
    std::string prog_name = argv[1];

    // fork process
    pid_t pid = fork();
    
    if(pid == 0){
        // draw_ui();
        // child preocess
        //PTRACE_TRACEME allows parent to trace this process.
        // it tells kernel that my paretnt is going to control me.
        ptrace(PTRACE_TRACEME, 0, 0, 0);

         //execl() replaces the current running process 
        //image with the executable file you give it (like ./target).
        execl(prog_name.c_str(), prog_name.c_str(), nullptr);

        // if execl returned means there's an error
        std::cerr <<RED<<"Error: "<< prog_name <<" failed." << endl;
        return 1;
    }
    else if(pid >= 1){
        // draw_ui();
        // parent process -- Debugger
        std::cout << GREEN<<"Debugger started...\nChild PID: " << pid << std::endl;
        Debugger  dbg(prog_name, pid);
        dbg.run();
    }    
    return 0;
}

