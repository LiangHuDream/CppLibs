### Chapter 7: Linking - Key Concepts & Code Walkthrough

---

#### 1. Static Linking Process
**Key Concepts**:
- Combines multiple relocatable object files into a single executable
- Two main tasks: **Symbol Resolution** (linking references to definitions) and **Relocation** (adjusting code/data addresses)

**Code Example**:  
File `main.c`:
```c
#include <stdio.h>

extern int global_var;  // External symbol declaration
extern void foo();      // External function declaration

int main() {
    printf("global_var = %d\n", global_var);
    foo();
    return 0;
}
```

File `module.c`:
```c
int global_var = 42;  // Symbol definition

void foo() {
    printf("Hello from foo!\n");
}
```

**Compilation**:
```bash
gcc -c main.c -o main.o
gcc -c module.c -o module.o
gcc main.o module.o -o final_executable
```

**Output**:
```
global_var = 42
Hello from foo!
```

---

#### 2. Symbol Resolution & Strong/Weak Symbols
**Key Concepts**:
- **Strong Symbols**: Functions/initialized global variables
- **Weak Symbols**: Uninitialized global variables
- Linker chooses strong symbols over weak ones

**Conflict Example** (Error Case):  
File `a.c`:
```c
int x = 10;  // Strong symbol
```

File `b.c`:
```c
int x = 20;  // Another strong symbol -> Linker Error
```

**Correct Version**:  
File `a.c`:
```c
int x = 10;  // Strong symbol
```

File `b.c`:
```c
extern int x;  // Proper declaration
void print_x() {
    printf("%d\n", x);
}
```

---

#### 3. Static Libraries (.a files)
**Key Concepts**:
- Archive of relocatable object files
- Resolved at link time

**Implementation**:  
1. Create library:  
File `mylib.c`:
```c
int add(int a, int b) { return a + b; }
int mult(int a, int b) { return a * b; }
```

File `main.c`:
```c
#include <stdio.h>

int add(int, int);  // Declaration
int mult(int, int);

int main() {
    printf("3+4=%d\n", add(3,4));
    printf("3*4=%d\n", mult(3,4));
    return 0;
}
```

**Commands**:
```bash
gcc -c mylib.c -o mylib.o
ar rcs libmylib.a mylib.o
gcc main.c -L. -lmylib -o static_demo
```

**Output**:
```
3+4=7
3*4=12
```

---

#### 4. Dynamic Linking with Shared Libraries (.so)
**Key Concepts**:
- Loaded at runtime
- Position-Independent Code (PIC) required
- Smaller executable size, memory sharing

**Implementation**:  
File `shlib.c`:
```c
#include <stdio.h>

void shared_func() {
    printf("From shared library!\n");
}
```

**Commands**:
```bash
gcc -fPIC -c shlib.c -o shlib.o
gcc -shared shlib.o -o libshlib.so
export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH  # For runtime
```

File `main.c`:
```c
int main() {
    extern void shared_func();
    shared_func();
    return 0;
}
```

**Compile & Run**:
```bash
gcc main.c -L. -lshlib -o dynamic_demo
./dynamic_demo
```

**Output**:
```
From shared library!
```

---

#### 5. Position-Independent Code (PIC)
**Key Concepts**:
- Uses GOT (Global Offset Table) for external references
- Enables shared library code to load at any address

**Inspection Command**:
```bash
objdump -d -j .plt libshlib.so  # View PLT entries
```

---

#### 6. Library Interposition
**Three Methods**:
1. **Compile-Time**: Use macros
2. **Link-Time**: Use `--wrap` flag
3. **Runtime**: `LD_PRELOAD`

**Runtime Interposition Example**:  
File `mymalloc.c`:
```c
#include <dlfcn.h>
#include <stdio.h>

void *malloc(size_t size) {
    static void *(*real_malloc)(size_t) = NULL;
    if (!real_malloc)
        real_malloc = dlsym(RTLD_NEXT, "malloc");
    
    void *p = real_malloc(size);
    printf("Allocated %zu bytes at %p\n", size, p);
    return p;
}
```

**Compile & Test**:
```bash
gcc -shared -fPIC -ldl mymalloc.c -o mymalloc.so
LD_PRELOAD=./mymalloc.so ./any_program_using_malloc
```

---

#### Testing & Verification
**Useful Tools**:
```bash
readelf -s main.o        # View symbol table
nm --dynamic executable  # Show dynamic symbols
ldd executable           # List shared library dependencies
objdump -d main.o        # Disassemble object file
```

---

### Summary Table
| Concept               | Key Mechanism                     | Error Example                    |
|-----------------------|------------------------------------|-----------------------------------|
| Symbol Resolution     | Strong vs. weak symbols           | Duplicate strong symbols         |
| Static Libraries      | `.a` archive format               | Missing library during linking    |
| Shared Libraries       | PIC + GOT/PLT                     | Missing `.so` at runtime          |
| Relocation            | `.rel.text` & `.rel.data` sections| Incorrect address adjustments     |
| Interposition         | LD_PRELOAD/wrap symbol            | Function signature mismatch      |

All code examples are compilable using standard GCC on Linux. For interposition examples, ensure `-ldl` is used when necessary.
---

### Multiple-Choice Questions 

---

**Question 1: Symbol Resolution and Strong/Weak Symbols**
**Which statements about symbol resolution in static linking are true?**  
A) A strong symbol (e.g., a defined function) overrides a weak symbol (e.g., an uninitialized global variable).  
B) Multiple weak symbols with the same name will cause a linker error.  
C) If two strong symbols have the same name but different types, the linker will arbitrarily choose one.  
D) Weak symbols are typically resolved using the largest definition available.  

---

**Question 2: Relocation Entries**
**Which tasks are performed during the relocation phase of linking?**  
A) Merging sections from multiple input object files.  
B) Resolving absolute addresses for global symbols.  
C) Calculating offsets for PC-relative addressing in machine code.  
D) Generating the Global Offset Table (GOT) for dynamic linking.  

---

**Question 3: Static vs. Dynamic Libraries**
**Which statements about static and dynamic libraries are correct?**  
A) Static libraries (*.a*) are linked into the executable at compile time.  
B) Dynamic libraries (*.so*) reduce memory usage when multiple processes use the same library.  
C) Static libraries allow for smaller executable file sizes compared to dynamic libraries.  
D) Dynamic libraries require the entire library to be loaded into memory at runtime.  

---

**Question 4: Address Space Allocation**
**Which linker strategies are used to assign addresses to code and data sections?**  
A) Placing all text sections from input files into a single contiguous text segment.  
B) Randomizing segment addresses to prevent security exploits (ASLR).  
C) Aligning sections to memory page boundaries for efficiency.  
D) Using linker scripts to customize the layout of output segments.  

---

**Question 5: Position-Independent Code (PIC)**
**Which techniques enable Position-Independent Code in shared libraries?**  
A) Using PC-relative addressing for global variables.  
B) Storing absolute addresses in the Procedure Linkage Table (PLT).  
C) Accessing global variables via the Global Offset Table (GOT).  
D) Compiling code with the `-fno-pic` flag to disable relocation.  

---

**Question 6: Library Interpositioning**
**Which methods can be used for library interpositioning?**  
A) Modifying the source code of the target library.  
B) Using the `LD_PRELOAD` environment variable at runtime.  
C) Wrapping functions with the `--wrap` linker flag.  
D) Including interposed functions in a static library linked before the target library.  

---

**Question 7: Global Offset Table (GOT) and Procedure Linkage Table (PLT)**
**Which statements about GOT and PLT are correct?**  
A) The GOT is modified at runtime to resolve addresses of dynamically linked functions.  
B) The PLT contains trampoline code to delay binding until the first function call.  
C) The GOT is used exclusively for data symbols, while the PLT handles function calls.  
D) The PLT entries are resolved during compile-time linking.  

---

**Question 8: Executable and Linkable Format (ELF)**
**Which sections are part of an ELF object file?**  
A) `.text` (executable code).  
B) `.got` (Global Offset Table).  
C) `.plt` (Procedure Linkage Table).  
D) `.debug` (debugging information).  

---

**Question 9: Linker Errors**
**Which scenarios will cause a linker error?**  
A) Multiple strong symbols with the same name in different object files.  
B) An unresolved reference to a weak symbol.  
C) A static library included in the linker command but not used.  
D) A missing entry point (e.g., `main` in a C program).  

---

**Question 10: Relocation Types**
**Which relocation entry types are valid?**  
A) `R_X86_64_PC32` (PC-relative address).  
B) `R_X86_64_COPY` (copy data from a shared library).  
C) `R_X86_64_GLOB_DAT` (global data address).  
D) `R_X86_64_JUMP_SLOT` (lazy binding for functions).  

---

**Question 11: Dynamic Linking**
**Which statements about dynamic linking are true?**  
A) The dynamic linker (`ld.so`) runs before the program’s `main` function.  
B) Lazy binding delays symbol resolution until the first function call.  
C) The `DT_NEEDED` entry in the `.dynamic` section lists required shared libraries.  
D) Shared libraries must be loaded at fixed addresses in memory.  

---

**Question 12: Symbol Visibility**
**Which linker options control symbol visibility?**  
A) `-fvisibility=hidden` to hide symbols by default.  
B) `__attribute__((visibility("default")))` to mark exported symbols.  
C) `-static` to force static linking of all symbols.  
D) `-Bsymbolic` to bind references to definitions within the shared library.  

---

**Question 13: Archive Files (Static Libraries)**
**Which statements about static libraries (*.a files) are true?**  
A) They are collections of relocatable object files.  
B) The linker includes all object files from the library in the executable.  
C) The order of libraries in the linker command does not affect symbol resolution.  
D) Running `ar -t libfoo.a` lists the object files in the archive.  

---

**Question 14: Shared Library Loading**
**Which factors determine the search path for shared libraries at runtime?**  
A) The `LD_LIBRARY_PATH` environment variable.  
B) The `-rpath` linker option embedded in the executable.  
C) The system-wide cache maintained by `ldconfig`.  
D) The order of `#include` directives in the source code.  

---

**Question 15: Linker Scripts**
**Which tasks can be achieved using linker scripts?**  
A) Defining the entry point of the executable.  
B) Merging custom sections into specific memory regions.  
C) Overriding the default layout of text, data, and bss segments.  
D) Enabling position-independent code for shared libraries.  

---

### **Answers and Explanations**

---

**Question 1**  
**Correct Answers:** A, C  
- **A**: Strong symbols (e.g., functions) override weak symbols (e.g., uninitialized globals).  
- **C**: If two strong symbols conflict, the linker issues an error. Weak symbols with different types are allowed.  
- **B**: Incorrect. Multiple weak symbols do not cause errors; the linker picks one arbitrarily.  
- **D**: Incorrect. Weak symbols are resolved by size only if they are uninitialized globals (not "largest").  

**Question 2**  
**Correct Answers:** B, C  
- **B/C**: Relocation resolves absolute/PC-relative addresses using relocation entries.  
- **A**: Merging sections occurs earlier (during "section merging").  
- **D**: GOT is for dynamic linking, not static relocation.  

**Question 3**  
**Correct Answers:** A, B  
- **A**: Static libraries are linked at compile time.  
- **B**: Dynamic libraries save memory via shared pages.  
- **C**: Incorrect. Static linking often produces larger executables.  
- **D**: Incorrect. Dynamic libraries load only necessary parts.  

**Question 4**  
**Correct Answers:** A, D  
- **A/D**: Linkers merge sections and use scripts for custom layouts.  
- **B**: ASLR is handled by the loader, not the linker.  
- **C**: Alignment is done by the assembler, not linker.  

**Question 5**  
**Correct Answers:** A, C  
- **A/C**: PIC uses GOT for globals and PC-relative addressing.  
- **B**: PLT handles function calls, not variables.  
- **D**: `-fno-pic` disables PIC, making code non-relocatable.  

**Question 6**  
**Correct Answers:** B, C  
- **B**: `LD_PRELOAD` injects libraries at runtime.  
- **C**: `--wrap` allows function wrapping.  
- **A/D**: Modifying source or static linking is not interpositioning.  

**Question 7**  
**Correct Answers:** A, B  
- **A/B**: GOT is runtime-modified; PLT enables lazy binding.  
- **C**: GOT handles both data and function addresses.  
- **D**: PLT resolution occurs at runtime.  

**Question 8**  
**Correct Answers:** A, B, C, D  
- All are valid ELF sections. `.debug` is optional.  

**Question 9**  
**Correct Answers:** A, D  
- **A**: Multiple strong symbols cause errors.  
- **D**: Missing `main` causes linker errors (for executables).  
- **B/C**: Unresolved weak symbols or unused libraries do not.  

**Question 10**  
**Correct Answers:** A, C, D  
- **B**: `R_X86_64_COPY` is used for data, not valid in static linking.  

**Question 11**  
**Correct Answers:** A, B, C  
- **D**: Incorrect. Shared libraries use PIC and do not require fixed addresses.  

**Question 12**  
**Correct Answers:** A, B, D  
- **C**: `-static` forces static linking but does not control visibility.  

**Question 13**  
**Correct Answers:** A, D  
- **B**: Linker includes only needed objects.  
- **C**: Order affects resolution (left-to-right).  

**Question 14**  
**Correct Answers:** A, B, C  
- **D**: `#include` is for compile-time headers, not libraries.  

**Question 15**  
**Correct Answers:** A, B, C  
- **D**: PIC is enabled via compiler flags, not linker scripts.  

--- 

These questions test deep understanding of linking concepts, including symbol resolution, relocation, libraries, PIC, and toolchain behavior. The explanations clarify common misconceptions and reinforce key mechanisms in static/dynamic linking.

**After you are done, it is highly recommended to do the exercises from the original book**
