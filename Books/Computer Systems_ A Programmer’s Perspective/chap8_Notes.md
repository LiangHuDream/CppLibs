### Chapter 8: Exceptional Control Flow - Key Concepts and Code Examples

---

#### 1. Exceptions and Process Control
**Key Concepts**:
- **Exceptions**: Mechanisms that transfer control from a program to the kernel in response to events (e.g., hardware interrupts, system calls, errors).
  - **Categories**: Interrupts (async, e.g., I/O), Traps (syscalls), Faults (recoverable errors), Aborts (fatal errors).
- **Process Control**:
  - **`fork()`**: Creates a child process as a copy of the parent.
  - **`execve()`**: Replaces the current process’s code with a new program.
  - **`waitpid()`**: Waits for a child process to terminate.

**Code Example**:
```c
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();
    if (pid == 0) { // Child process
        printf("Child PID: %d\n", getpid());
        _exit(0); // Terminate child
    } else { // Parent process
        printf("Parent PID: %d\n", getpid());
        waitpid(pid, NULL, 0); // Wait for child
    }
    return 0;
}
```
**Test & Compile**:
```bash
gcc -o process process.c
./process
```
**Output**:
```
Parent PID: 1234
Child PID: 1235
```

---

#### 2. Signals
**Key Concepts**:
- **Signals**: Software interrupts sent to processes (e.g., `SIGINT`, `SIGSEGV`).
  - **Signal Handlers**: Functions registered to handle signals.
  - **Blocking Signals**: Use `sigprocmask` to control signal delivery.
  - **Reentrancy**: Signal handlers must use async-signal-safe functions.

**Code Example**:
```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void handler(int sig) {
    printf("Received SIGINT!\n");
}

int main() {
    signal(SIGINT, handler); // Register handler
    printf("Press Ctrl+C to send SIGINT...\n");
    while (1) {
        sleep(1); // Wait for signal
    }
    return 0;
}
```
**Test & Compile**:
```bash
gcc -o signal signal.c
./signal
# Press Ctrl+C to trigger SIGINT
```
**Output**:
```
Press Ctrl+C to send SIGINT...
^CReceived SIGINT!
```

---

#### 3. Non-Local Jumps
**Key Concepts**:
- **`setjmp/longjmp`**: Jump between functions without normal control flow.
  - **`setjmp()`**: Saves the current execution context.
  - **`longjmp()`**: Restores a saved context, bypassing function returns.

**Code Example**:
```c
#include <stdio.h>
#include <setjmp.h>

jmp_buf env;

void func() {
    printf("Jumping back...\n");
    longjmp(env, 1); // Jump to setjmp point
}

int main() {
    if (setjmp(env) == 0) { // First call returns 0
        printf("Calling func()\n");
        func();
    } else { // After longjmp, returns non-zero
        printf("Returned via longjmp\n");
    }
    return 0;
}
```
**Test & Compile**:
```bash
gcc -o jump jump.c
./jump
```
**Output**:
```
Calling func()
Jumping back...
Returned via longjmp
```

---

#### 4. Process Termination & Zombies
**Key Concepts**:
- **Zombie Processes**: Terminated processes whose exit status has not been reaped.
- **Orphan Processes**: Parent dies before child; child is reparented to `init`.

**Code Example**:
```c
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();
    if (pid == 0) { // Child
        printf("Child PID: %d\n", getpid());
        sleep(2);
        _exit(0);
    } else { // Parent
        sleep(1); // Do not wait, child becomes zombie
        printf("Parent exits\n");
    }
    return 0;
}
```
**Test & Compile**:
```bash
gcc -o zombie zombie.c
./zombie
# Use `ps` to observe zombie process
```
**Output**:
```
Child PID: 1234
Parent exits
# Zombie appears briefly before cleanup
```

---

#### 5. Race Conditions
**Key Concepts**:
- **Race Condition**: Outcome depends on non-deterministic ordering of events.
- **Mitigation**: Use synchronization (e.g., signals, semaphores).

**Code Example**:
```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t flag = 0;

void handler(int sig) {
    flag = 1;
}

int main() {
    signal(SIGUSR1, handler);
    printf("Before signal...\n");
    kill(getpid(), SIGUSR1); // Send signal
    if (flag) { // Potential race
        printf("Flag set\n");
    } else {
        printf("Flag not set\n");
    }
    return 0;
}
```
**Test & Compile**:
```bash
gcc -o race race.c
./race
```
**Output** (Non-deterministic):
```
Before signal...
Flag set
```

---

### Summary
- **Exceptions** enable kernel/process interactions (e.g., syscalls, errors).
- **Process Control** requires understanding `fork`, `execve`, and `waitpid`.
- **Signals** demand careful handling to avoid reentrancy bugs.
- **Non-Local Jumps** bypass normal control flow but can complicate state management.
- **Race Conditions** are subtle bugs requiring synchronization.

---

### Chapter 8: Exceptional Control Flow - 15 Hard Multiple-Choice Questions

---

**Question 1: Signal Handling Safety**
Which of the following functions are **async-signal-safe** and safe to call from a signal handler? (Choose two)  
A. `printf`  
B. `exit`  
C. `sigprocmask`  
D. `malloc`  
E. `write`

---

**Question 2: Process Termination**
When a parent process terminates before its child processes, which of the following are true? (Choose two)  
A. The child processes become orphaned and are adopted by `init` (PID 1).  
B. The child processes immediately terminate.  
C. The child processes remain in a zombie state until reaped by the kernel.  
D. The parent process cannot be terminated until all children exit.  
E. The child processes inherit the parent's process group ID.

---

**Question 3: Signal Blocking**
Which mechanisms can be used to **block signals** during critical sections of code? (Choose two)  
A. Using `signal` to ignore the signal  
B. Masking signals with `sigprocmask`  
C. Using `sigaction` with the `SA_RESTART` flag  
D. Installing a signal handler that does nothing  
E. Using `sigemptyset` and `sigaddset` to modify the signal mask

---

**Question 4: Nonlocal Jumps**
Which statements about `setjmp` and `longjmp` are true? (Choose two)  
A. They can be used to implement exception handling in C.  
B. `longjmp` restores the signal mask saved by `setjmp`.  
C. Using `longjmp` to jump into a function that has already returned causes undefined behavior.  
D. They preserve the values of register variables.  
E. `setjmp` returns 0 when called directly and non-zero when returning via `longjmp`.

---

**Question 5: Fork Behavior**
After a successful `fork()`, which properties are shared between the parent and child processes? (Choose two)  
A. File descriptor table  
B. Virtual memory address space  
C. Signal handlers  
D. Process ID (PID)  
E. Pending signals

---

**Question 6: Context Switching**
During a context switch between two processes, which of the following are saved/restored by the kernel? (Choose two)  
A. Program counter (PC)  
B. Register values  
C. Page table base register (CR3 on x86-64)  
D. Open file descriptors  
E. Heap memory contents

---

**Question 7: Signal Delivery**
Which statements about signal delivery are true? (Choose two)  
A. A signal is delivered to a process as soon as it is sent.  
B. A pending signal is delivered when the process transitions from kernel to user mode.  
C. Signals of the same type are queued in a FIFO order.  
D. A blocked signal remains pending until unblocked.  
E. The `SIGKILL` signal can be caught by a custom signal handler.

---

**Question 8: Reentrancy**
Which of the following are **non-reentrant** and unsafe to use in signal handlers? (Choose two)  
A. `strtok`  
B. `read`  
C. `free`  
D. `memcpy`  
E. `exit`

---

**Question 9: Zombie Processes**
How can a zombie process be eliminated? (Choose two)  
A. Sending `SIGKILL` to the zombie process  
B. The parent process calls `waitpid` to reap the child.  
C. Rebooting the system  
D. The parent process installs a `SIGCHLD` handler with `SA_NOCLDWAIT`.  
E. Using `execve` to replace the parent process image.

---

**Question 10: Race Conditions**
Which techniques prevent race conditions in signal handling? (Choose two)  
A. Blocking signals before modifying global data and unblocking afterward.  
B. Using `volatile` qualifiers for global variables accessed in handlers.  
C. Using `sigaction` with the `SA_SIGINFO` flag.  
D. Making all signal handlers idempotent.  
E. Using atomic operations to update shared variables.

---

**Question 11: Process States**
Which transitions between process states are possible? (Choose two)  
A. Running → Zombie  
B. Blocked → Running  
C. Zombie → Terminated  
D. Running → Blocked  
E. Terminated → Running

---

**Question 12: Exec Functions**
Which statements about `execve` are true? (Choose two)  
A. `execve` preserves the process’s open file descriptors unless explicitly closed.  
B. `execve` returns to the caller on success.  
C. The new program inherits the parent’s signal handlers.  
D. `execve` replaces the current process’s text, data, heap, and stack segments.  
E. Environment variables are passed via the `argv` array.

---

**Question 13: Signal Priorities**
Which signals have the **highest priority** for delivery? (Choose two)  
A. `SIGINT`  
B. `SIGSEGV`  
C. `SIGTERM`  
D. `SIGKILL`  
E. `SIGCHLD`

---

**Question 14: Deadlock Conditions**
Which combinations of actions by parent and child processes could lead to a deadlock? (Choose two)  
A. Parent blocks on `waitpid`, child blocks on I/O.  
B. Parent and child both call `fork` in a loop.  
C. Parent and child both block on reading from the same pipe with no writer.  
D. Parent sends `SIGSTOP` to child, child sends `SIGCONT` to parent.  
E. Parent and child both acquire a mutex in reverse order.

---

**Question 15: Signal Handlers**
Which are valid restrictions on **signal handler functions**? (Choose two)  
A. They cannot call non-reentrant functions.  
B. They must execute in user mode.  
C. They cannot modify global variables.  
D. They must terminate with a call to `exit`.  
E. They cannot be interrupted by another signal.

---

### Answers & Explanations

---

**Question 1**  
**Correct Answers**: B, E  
- `exit` (B) terminates the process and is async-signal-safe.  
- `write` (E) is a low-level I/O function and safe.  
- `printf` (A) and `malloc` (D) are **not** async-signal-safe.  
- `sigprocmask` (C) is unsafe because it modifies the signal mask asynchronously.

---

**Question 2**  
**Correct Answers**: A, E  
- Orphaned processes are adopted by `init` (A).  
- Children inherit the parent’s process group ID (E).  
- Zombie processes (C) occur when children exit before parents.  
- Parents can terminate before children (B, D are false).

---

**Question 3**  
**Correct Answers**: B, E  
- `sigprocmask` (B) masks signals.  
- `sigemptyset`/`sigaddset` (E) modify the signal mask used with `sigprocmask`.  
- `SA_RESTART` (C) affects system call restarting, not blocking.  

---

**Question 4**  
**Correct Answers**: A, C  
- `setjmp`/`longjmp` can implement exceptions (A).  
- Jumping into a returned function causes UB (C).  
- `longjmp` does **not** restore the signal mask unless `sigsetjmp` is used (B false).  

---

**Question 5**  
**Correct Answers**: A, E  
- File descriptors (A) and pending signals (E) are inherited.  
- Virtual memory (B) is copied, not shared.  
- PID (D) differs between parent and child.  

---

**Question 6**  
**Correct Answers**: A, B  
- PC (A) and registers (B) are saved/restored.  
- CR3 (C) changes with address space. File descriptors (D) and heap (E) are process-specific.  

---

**Question 7**  
**Correct Answers**: B, D  
- Signals are delivered on transition to user mode (B).  
- Blocked signals stay pending (D).  
- Signals are **not** queued (C false).  

---

**Question 8**  
**Correct Answers**: A, C  
- `strtok` (A) uses static buffers; `free` (C) is non-reentrant if interrupted mid-operation.  
- `read` (B), `memcpy` (D), and `exit` (E) are safe.  

---

**Question 9**  
**Correct Answers**: B, D  
- `waitpid` reaps zombies (B).  
- `SA_NOCLDWAIT` (D) prevents zombies.  
- `SIGKILL` (A) has no effect on zombies.  

---

**Question 10**  
**Correct Answers**: A, E  
- Blocking signals (A) and atomic operations (E) prevent races.  
- `volatile` (B) ensures visibility but not atomicity.  

---

**Question 11**  
**Correct Answers**: D, A  
- Running → Blocked (D) occurs during I/O waits.  
- Running → Zombie (A) when child exits.  
- Zombie → Terminated (C) only after reaping.  

---

**Question 12**  
**Correct Answers**: A, D  
- `execve` preserves open files (A) and replaces memory segments (D).  
- It does **not** return on success (B false).  

---

**Question 13**  
**Correct Answers**: B, D  
- `SIGSEGV` (B) and `SIGKILL` (D) are non-maskable and high-priority.  

---

**Question 14**  
**Correct Answers**: C, E  
- Both reading from an empty pipe (C) and circular mutex acquisition (E) cause deadlocks.  

---

**Question 15**  
**Correct Answers**: A, B  
- Handlers cannot use non-reentrant functions (A).  
- They execute in user mode (B).  
- They **can** modify globals (C false) but must do so cautiously.

---
**After you are done, it is highly recommended to do the exercises from the original book**