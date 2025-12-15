#include <stdio.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <iostream>
#include <sys/user.h>
#include <map>
#include <iomanip> // For nice hex formatting

#include "handle_command.hpp"

using namespace std;


// Map to store: Address -> Original Instruction (8 bytes)
map<void*, long> active_breakpoints;



int main(int argc, char *argv[]) {
  
    if(argc < 2){
        cout << "Usage: ./debugger <program>" << endl;
        exit(0);
    }

    pid_t p1 = fork();

    if(p1 == 0){
        // Child

        //Child tells kernel: "My parent will debug me."

        ptrace(PTRACE_TRACEME, 0, NULL, NULL);

        //execl() replaces the current running process 
        //image with the executable file you give it (like ./target).

        execl(argv[1], argv[1], NULL);

    } else {

        // Parent
        int status;

        // Wait for the child to stop at the start 
        waitpid(p1, &status, 0);
        cout << "Debugger started. Child paused at entry." << endl;

        string command;
        while(true) {
            cout << "(my_debugger) > ";
            cin >> command;

            if (command == "quit") {
                cout << "Terminating debugger and child process..." << endl;
            
            // Kill the child hard
            kill(p1, SIGKILL);
            
            // Wait for it to die (clean up zombie)
            int status;
            waitpid(p1, &status, 0);
            
            cout << "Child process killed. Goodbye!" << endl;
            break; 
            } 
            else {
                handle_command(p1, command);
            }
        }
    }
    return 0;
}