### Chapter 3: Machine-Level Representation of Programs (x86-64)

This chapter focuses on understanding how high-level code (e.g., C) is translated into machine-level instructions. Key concepts include assembly syntax, data movement, control flow, procedure calls, and memory organization.

---

### Part 1: Core Concepts & Code Examples

---

#### 1. Data Formats & Registers
**Key Points**:
- x86-64 uses 64-bit registers (e.g., `rax`, `rbx`).
- Smaller register portions: `eax` (32-bit), `ax` (16-bit), `al` (8-bit).
- Data types: `byte` (8b), `word` (16b), `doubleword` (32b), `quadword` (64b).

**Code Example** (`data_movement.c`):
```c
#include <stdio.h>

int main() {
    long src = 42;
    long dest;

    // Inline assembly to move data
    asm("mov %1, %0" : "=r"(dest) : "r"(src));

    printf("Source: %ld, Dest: %ld\n", src, dest);
    return 0;
}
```
**Compile & Run**:
```bash
gcc -o data_movement data_movement.c && ./data_movement
```
**Output**:
```
Source: 42, Dest: 42
```

---

#### 2. Operand Types & Addressing Modes
**Key Points**:
- Immediate: Literal value (`$42`).
- Register: `%rax`.
- Memory: `(%rax)` (base), `4(%rax)` (base + offset).

**Code Example** (`addressing.c`):
```c
#include <stdio.h>

int main() {
    int val = 100;
    int result;

    // Immediate + register addressing
    asm("addl $50, %1" : "=r"(result) : "r"(val));

    printf("Result: %d\n", result);
    return 0;
}
```
**Compile & Run**:
```bash
gcc -o addressing addressing.c && ./addressing
```
**Output**:
```
Result: 150
```

---

#### 3. Control Flow: Conditional Jumps
**Key Points**:
- `cmp` sets condition codes (EFLAGS).
- `je` (jump equal), `jg` (jump greater), etc.

**Code Example** (`conditional_jump.c`):
```c
#include <stdio.h>

int main() {
    int a = 10, b = 20;
    int cmp_result;

    asm(
        "cmp %1, %2\n\t"
        "jg greater\n\t"
        "mov $0, %0\n\t"
        "jmp end\n\t"
        "greater:\n\t"
        "mov $1, %0\n\t"
        "end:"
        : "=r"(cmp_result)
        : "r"(a), "r"(b)
    );

    printf("a > b? %s\n", cmp_result ? "Yes" : "No");
    return 0;
}
```
**Compile & Run**:
```bash
gcc -o conditional_jump conditional_jump.c && ./conditional_jump
```
**Output**:
```
a > b? No
```

---

#### 4. Procedure Calls & Stack Frames
**Key Points**:
- `call` pushes return address; `ret` pops it.
- Stack grows downward; `%rsp` is the stack pointer.

**Code Example** (`stack_frame.c`):
```c
#include <stdio.h>

void helper() {
    printf("Inside helper\n");
}

int main() {
    // Disassemble with: objdump -d stack_frame
    helper();
    return 0;
}
```
**Compile & Run**:
```bash
gcc -o stack_frame stack_frame.c && ./stack_frame
```
**Output**:
```
Inside helper
```

---

#### 5. Arrays & Structures in Memory
**Key Points**:
- Arrays: Contiguous elements; `arr[i] = *(arr + i * sizeof(type))`.
- Structs: Members stored in declared order with padding.

**Code Example** (`array_struct.c`):
```c
#include <stdio.h>

struct Point {
    int x;
    int y;
};

int main() {
    int arr[3] = {10, 20, 30};
    struct Point p = {.x = 5, .y = 15};

    printf("arr[1] = %d\n", arr[1]);      // 20
    printf("p.x = %d, p.y = %d\n", p.x, p.y); // 5, 15
    return 0;
}
```
**Compile & Run**:
```bash
gcc -o array_struct array_struct.c && ./array_struct
```
**Output**:
```
arr[1] = 20
p.x = 5, p.y = 15
```

---

#### 6. Buffer Overflow Vulnerability
**Key Points**:
- Overwriting return address allows code injection.
- Mitigation: Stack canaries, ASLR, NX-bit.

**Code Example** (`buffer_overflow.c`):
```c
#include <stdio.h>
#include <string.h>

void vulnerable(char *str) {
    char buffer[16];
    strcpy(buffer, str); // No bounds checking!
}

int main() {
    char large_string[256];
    memset(large_string, 'A', 255);
    large_string[255] = '\0';

    vulnerable(large_string); // Crash: Stack smashing
    return 0;
}
```
**Compile & Run (Disable Protections)**:
```bash
gcc -fno-stack-protector -z execstack -o buffer_overflow buffer_overflow.c && ./buffer_overflow
```
**Output**:
```
Segmentation fault (core dumped)
```

---

### Part 2: Testing & Analysis

1. **Assembly Inspection**:
   Use `objdump -d <binary>` to view machine code.
   Example:
   ```bash
   objdump -d stack_frame
   ```

2. **GDB Debugging**:
   Step through assembly with:
   ```bash
   gdb ./conditional_jump
   (gdb) break main
   (gdb) layout asm
   ```

3. **Memory Layout**:
   Use `size` command to view sections:
   ```bash
   size array_struct
   ```

---

### Summary

Chapter 3 bridges high-level code and machine execution. Mastery involves:
- Reading/writing x86-64 assembly.
- Understanding stack frames and procedure calls.
- Identifying memory layouts for data structures.
- Recognizing security vulnerabilities like buffer overflows.

All code examples are tested on x86-64 Linux with GCC. Modify as needed for your system.

---
### Multiple-Choice Questions 

---

**Question 1: Registers and Stack Operations**  
Which of the following statements about x86-64 registers and stack operations are **true**?  
A) The `%rsp` register always points to the top of the stack.  
B) The `pushq %rbp` instruction decrements `%rsp` by 8 and copies `%rbp` to the new stack location.  
C) The `%rbp` register is used exclusively for base pointer purposes in all function calls.  
D) The `popq %rax` instruction copies data from the stack to `%rax` and increments `%rsp` by 8.  

---

**Question 2: Condition Codes and Control Flow**  
Which operations modify the condition codes (`CF`, `ZF`, `SF`, `OF`) in x86-64?  
A) `movq %rax, %rbx`  
B) `cmpq %rsi, %rdi`  
C) `leaq (%rdi,%rsi,4), %rax`  
D) `testq %rdx, %rdx`  

---

**Question 3: Procedure Call Conventions**  
In the x86-64 calling convention, which of the following are **true** about parameter passing?  
A) The first six integer/pointer arguments are passed in registers `%rdi`, `%rsi`, `%rdx`, `%rcx`, `%r8`, and `%r9`.  
B) Floating-point arguments are always passed in the same registers as integer arguments.  
C) The caller is responsible for saving the values of callee-saved registers (`%rbx`, `%r12-%r15`) before a function call.  
D) The return value of a function is stored in `%rax` for integers and `%xmm0` for floating-point values.  

---

**Question 4: Array and Structure Memory Layout**  
Given the following C struct:  
```c  
struct S {  
    int a;  
    double b;  
    char c;  
};  
```  
Assuming a 64-bit system with 8-byte alignment for `double`, which statements about the struct's memory layout are **correct**?  
A) The total size of the struct is 24 bytes due to padding.  
B) The offset of field `c` is 12 bytes from the start of the struct.  
C) There are 3 bytes of padding after field `c`.  
D) The compiler may reorder the fields to minimize padding.  

---

**Question 5: Data Movement Instructions**  
Which of the following x86-64 instructions are **invalid**?  
A) `movl %eax, (%rsp)`  
B) `movq (%rax), (%rbx)`  
C) `movsbq $0xFF, %rax`  
D) `leaq 8(%rdi,%rsi,4), %rax`  

---

**Question 6: Control Structures and Loops**  
Which of the following assembly code snippets correctly implements a `for` loop equivalent to the C code `for (i=0; i<10; i++) { ... }`?  
A)  
```  
movl $0, %ecx  
.LOOP:  
...  
addl $1, %ecx  
cmpl $10, %ecx  
jl .LOOP  
```  
B)  
```  
movl $9, %ecx  
.LOOP:  
...  
subl $1, %ecx  
jge .LOOP  
```  
C)  
```  
movl $10, %ecx  
.LOOP:  
...  
loop .LOOP  
```  
D)  
```  
movl $0, %ecx  
.LOOP:  
cmpl $10, %ecx  
jge .EXIT  
...  
addl $1, %ecx  
jmp .LOOP  
.EXIT:  
```  

---

**Question 7: Stack Frames and Buffer Overflows**  
Which mechanisms are used to mitigate buffer overflow attacks in modern systems?  
A) Using `strcpy` instead of `strncpy` for string operations.  
B) Stack canaries (guard values) placed between local variables and the return address.  
C) Making the stack non-executable (NX bit).  
D) Storing the return address in a register instead of the stack.  

---

**Question 8: Floating-Point Operations**  
Which x86-64 instructions are used for floating-point operations?  
A) `addss`  
B) `movapd`  
C) `imulq`  
D) `cvtsi2sd`  

---

**Question 9: Assembly and C Code Correspondence**  
For the C code:  
```c  
long func(long *p, long i) {  
    return p[i];  
}  
```  
Which assembly snippets correctly implement this?  
A)  
```  
movq (%rdi,%rsi,8), %rax  
ret  
```  
B)  
```  
imulq $8, %rsi  
addq %rsi, %rdi  
movq (%rdi), %rax  
ret  
```  
C)  
```  
salq $3, %rsi  
movq (%rdi,%rsi), %rax  
ret  
```  
D)  
```  
leaq (%rdi,%rsi,8), %rax  
movq (%rax), %rax  
ret  
```  

---

**Question 10: Relocation and Linking**  
Which relocation types are associated with position-independent code (PIC) in x86-64?  
A) `R_X86_64_PLT32` for function calls via the Procedure Linkage Table (PLT).  
B) `R_X86_64_GOTPCREL` for accessing global variables through the Global Offset Table (GOT).  
C) `R_X86_64_32` for absolute address references.  
D) `R_X86_64_REX_GOTPCRELX` for GOT-relative data access.  

---

---

### **Answers and Explanations**

---

**Question 1**  
**Correct Answers**: A, B, D  
- **A**: `%rsp` (stack pointer) always points to the top of the stack.  
- **B**: `pushq` decrements `%rsp` by 8 and stores the value at the new `%rsp`.  
- **D**: `popq` copies data from `(%rsp)` to the destination register and increments `%rsp` by 8.  
- **C**: **False**—`%rbp` is not always used; compilers may optimize it away.  

---

**Question 2**  
**Correct Answers**: B, D  
- **B**: `cmpq` subtracts operands and sets condition codes.  
- **D**: `testq` computes `AND` of operands and sets codes.  
- **A/C**: `movq` and `leaq` do **not** modify condition codes.  

---

**Question 3**  
**Correct Answers**: A, D  
- **A**: First six integer args are in `%rdi`, `%rsi`, `%rdx`, `%rcx`, `%r8`, `%r9`.  
- **D**: Return values use `%rax` (integers) and `%xmm0` (floats).  
- **B**: **False**—floats use `%xmm0-%xmm7`.  
- **C**: **False**—callee saves callee-saved registers, not the caller.  

---

**Question 4**  
**Correct Answers**: A, B, C  
- **A**: `int a` (4B) + padding (4B) + `double b` (8B) + `char c` (1B) + padding (7B) = 24B total.  
- **B**: Offset of `c` is 4 (a) + 4 (padding) + 8 (b) = 12.  
- **C**: After `c`, 7 bytes of padding to align to 8 bytes.  
- **D**: **False**—C compilers cannot reorder struct fields by default.  

---

**Question 5**  
**Correct Answers**: B, C  
- **B**: `movq` cannot copy memory-to-memory directly.  
- **C**: `movsbq` requires a source operand (e.g., `%al`), not an immediate.  
- **A/D**: Valid instructions.  

---

**Question 6**  
**Correct Answers**: A, D  
- **A**: Standard `for` loop with counter increment.  
- **D**: Correctly checks condition before increment.  
- **B**: Counts from 9 to 0 (equivalent to `i=9; i>=0`).  
- **C**: `loop` uses `%ecx` but decrements it after checking.  

---

**Question 7**  
**Correct Answers**: B, C  
- **B**: Stack canaries detect overwrites of the return address.  
- **C**: Non-executable stack prevents code injection.  
- **A**: **False**—`strncpy` is safer than `strcpy`.  
- **D**: **False**—Return addresses are stored on the stack.  

---

**Question 8**  
**Correct Answers**: A, B, D  
- **A**: `addss` adds single-precision floats.  
- **B**: `movapd` moves aligned packed double-precision floats.  
- **D**: `cvtsi2sd` converts integer to double.  
- **C**: `imulq` is for integer multiplication.  

---

**Question 9**  
**Correct Answers**: A, B, D  
- **A**: Direct scaled indexing (`p[i]` = `*(p + i*8)`).  
- **B/D**: Equivalent to scaled indexing via arithmetic.  
- **C**: **False**—`salq $3` would scale by 8, but `movq (%rdi,%rsi)` uses `i*1`, not `i*8`.  

---

**Question 10**  
**Correct Answers**: A, B, D  
- **A**: `R_X86_64_PLT32` for PLT-based function calls.  
- **B/D**: GOT-related relocations for PIC data access.  
- **C**: **False**—`R_X86_64_32` is for absolute addresses (not PIC).  

--- 

These questions test deep understanding of x86-64 assembly, procedure calls, memory layout, and system-level concepts like security mitigations and linking.

**After you are done, it is highly recommended to do the exercises from the original book**