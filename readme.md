## Minimal Debugger
A lightweight, custom-built debugger for Linux written in C++. This project demonstrates the fundamentals of how debuggers work by utilizing the `ptrace` system call to control execution, inspect memory, and manipulate CPU registers.

#### 🚀 Features
- **Process Control:** Launch and trace a child process (`fork`, `exec`, `ptrace`).

- **Execution Flow:**

    - Continue execution (`cont`).

    - Single-step through instructions (`step`).

- **Breakpoints:**

    - Set breakpoints at specific memory addresses (`break`).

    - Remove existing breakpoints (`delete`).

    - Handles `SIGTRAP` and transparently steps over breakpoints using the "read-modify-write" technique (injecting `INT 3 / 0xCC`).

- **State Inspection:**

    - **Registers:** Dump the values of general-purpose CPU registers (RIP, RAX, RBX, etc.).

    - **Memory:** Examine raw memory content at specific addresses in hex format.

    - **Status:** View the current state of the debugged process (Running, Stopped, Exited) and the current instruction pointer.

#### 🛠️ Prerequisites
- **Operating System:** Linux (Required for `sys/ptrace.h`, `sys/wait.h`, `sys/user.h`).

- **Compiler:** `g++` supporting C++17 (e.g., GCC 7+).

- **Build Tool:** `make`.

#### 📦 Building the Project
The project includes a `makefile` to automate compilation. It builds both the debugger and a sample target program.

1. **Clone** the repository (or navigate to the source directory).

2. **Compile:**

    ```
    make
    ```
This generates two executables:

- `debugger`: The main debugger tool.

- `target`: A sample executable (compiled with `-no-pie` to stabilize memory addresses).

3. **Clean:** To remove compiled binaries and object files:
    
    ```
    make clean
    ```

#### 🎮 Usage
You can run the debugger manually or use the helper command provided in the makefile.
**1. Quick Start**<br>
The `make run` command will compile the project, print the memory address of the `main` function in the target program, and launch the debugger automatically.
```
make run
```
*Tip: Copy the hex address displayed (e.g., 0x401126) to set your first breakpoint.*

**2. Manual Execution**<br>
    
    ```
    ./debugger <path_to_executable>
    ```

**3. Debugger Commands**<br>
Once inside the `dbg>`  prompt, you can use the following commands:<br>
| Command | Alias | Arguments | Description |
|---------|-------|-----------|-------------|
| `break` | `b` | `<address>` | Set a breakpoint at a hex address (e.g., `b 0x401136`).|
| `delete` | `d`, `del` | `<address>` | Remove a breakpoint from the specified address. |
| `continue` | `c`, `cont` | - | Resume program execution until the next breakpoint or exit. |
| `step` | `s` | - | Execute a single instruction (single-step). |
| `regs` | - | - | Print the current values of CPU registers (RAX, RIP, flags, etc.). |
| `examine` | `x` | `<addr>` `[bytes]` | Inspect memory. Reads 32 bytes by default if count is omitted. |
| `status` | - | - | Show process PID, current state, and Instruction Pointer (PC).
| `quit` | `q` | - | Kill the child process and exit the debugger. |


**Example**
```[text]
dbg> b 0x401126         <-- Set breakpoint at main
Setting breakpoint at 0x401126
dbg> c                  <-- Continue execution
Hit breakpoint at 0x401126
dbg> regs               <-- Inspect registers
RAX: 0x0000000000000000  RBX: 0x0000000000000000 ...
RIP: 0x0000000000401126 ...
dbg> s                  <-- Step to next instruction
Stepped to 0x401127
dbg> x 0x401126 16      <-- Read 16 bytes of memory at EIP
Reading 16 bytes from 0x401126...
dbg> q                  <-- Quit
Child process killed. Goodbye!
```

**📂 Project Structure**<br>
- `debugger.cpp`: Entry point (`main`). Forks the process: the child becomes the target (`ptrace(PTRACE_TRACEME)`), and the parent runs the `Debugger` class.

- `debugger_core.cpp/hpp`: The main `Debugger` class. Handles the command loop, input parsing, and high-level logic for handling signals and breakpoints.

- `breakpoint.cpp/hpp`: Manages breakpoint logic. Reads the original byte at an address, saves it, replaces it with `0xCC` (INT 3), and restores it when stepping over.

- `registers.cpp/hpp`: Wrappers around `ptrace(PTRACE_GETREGS)` and `ptrace(PTRACE_SETREGS)` to read/write CPU registers.

- `colors.hpp`: Helper macros for colored console output (Green for success, Red for errors, Blue for prompt).

- `makefile`: build script. Sets `-no-pie` for the target to ensure static addresses for easier debugging.