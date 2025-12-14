#include <iostream>
#include <unistd.h> // fork, execl
#include <sys/ptrace.h> // p_trace
#include <sys/wait.h> // waitpid
#include <sys/types.h> // pid_t


#include "breakpoint.hpp"

using namespace std;



int main(int argc, char *argv[]) {
    if(argc < 2){
        cerr << "Usage: " <<argv[0] << "program" << endl;
        return 1;
    }
    const char* prog_name = argv[1];
    
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

        if(WIFSTOPPED(status)){
            cout << "child started and stopped at entry point." << endl;
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