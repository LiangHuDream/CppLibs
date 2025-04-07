# Chapter 10: System-Level I/O 

## 1. Unix I/O Fundamentals
### Core Concepts:
- All I/O devices modeled as **files**
- **File descriptors** (int values 0-2 reserved: STDIN=0, STDOUT=1, STDERR=2)
- **open/read/write/close** as basic operations
- **Short counts** (partial read/write) are normal and must be handled

### Code Example: Basic File Read
```c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 1024

int main() {
    int fd = open("test.txt", O_RDONLY);
    if (fd < 0) {
        perror("open failed");
        exit(1);
    }

    char buf[BUFSIZE];
    ssize_t n = read(fd, buf, BUFSIZE-1);
    if (n < 0) {
        perror("read failed");
        close(fd);
        exit(1);
    }
    
    buf[n] = '\0';
    printf("Read %zd bytes:\n%s\n", n, buf);
    close(fd);
    return 0;
}
```
**Test File Creation:**
```bash
echo "Hello, System I/O!" > test.txt
```
**Compile & Run:**
```bash
gcc read_example.c -o read_example
./read_example
```
**Expected Output:**
```
Read 17 bytes:
Hello, System I/O!
```

---

## 2. Robust I/O (RIO Package)
### Key Concepts:
- **Unbuffered I/O** (`rio_readn`, `rio_writen`)
- **Buffered I/O** (`rio_readlineb`, `rio_readnb`)
- Automatically handles short counts
- Thread-safe implementation

### Code Example: Buffered Line Reading
```c
#include "csapp.h"

int main(int argc, char **argv) {
    int fd = Open("lines.txt", O_RDONLY, 0);
    rio_t rio;
    rio_readinitb(&rio, fd);

    char buf[MAXLINE];
    int linecnt = 0;
    
    while (rio_readlineb(&rio, buf, MAXLINE) > 0) {
        printf("Line %d: %s", ++linecnt, buf);
    }
    
    Close(fd);
    return 0;
}
```
**Test File Creation:**
```bash
printf "Line 1\nLine 2\nLine 3" > lines.txt
```
**Output:**
```
Line 1: Line 1
Line 2: Line 2
Line 3: Line 3
```

---

## 3. File Metadata & Permissions
### Key Concepts:
- `stat`/`fstat` functions
- File metadata structure contains:
  - File type (regular, directory, socket)
  - Size
  - Permissions (st_mode)
  - Last access/modification times

### Code Example: File Info
```c
#include <sys/stat.h>
#include <stdio.h>

int main(int argc, char **argv) {
    struct stat st;
    
    if (stat(argv[1], &st) < 0) {
        perror("stat error");
        return 1;
    }

    printf("File: %s\n", argv[1]);
    printf("Size: %lld bytes\n", (long long)st.st_size);
    printf("Permissions: %o\n", st.st_mode & 0777);
    printf("Last modified: %ld\n", (long)st.st_mtime);
    
    return 0;
}
```
**Test Command:**
```bash
gcc fileinfo.c -o fileinfo
./fileinfo /etc/passwd
```
**Sample Output:**
```
File: /etc/passwd
Size: 3000 bytes 
Permissions: 644
Last modified: 1680123456
```

---

## 4. File Sharing & Atomic Operations
### Key Concepts:
- Three kernel data structures:
  1. Descriptor table (per-process)
  2. Open file table (global)
  3. v-node table (global)
- Concurrent writes require synchronization
- `O_APPEND` ensures atomic writes

### Code Example: Shared File Position
```c
#include <csapp.h>

int main() {
    int fd = Open("shared.txt", O_CREAT|O_TRUNC|O_RDWR, 0666);
    pid_t pid = Fork();
    
    if (pid == 0) {  // Child
        Write(fd, "child\n", 6);
    } else {         // Parent
        Write(fd, "parent\n", 7);
    }
    
    Close(fd);
    return 0;
}
```
**Possible Output in File:**
```
parent
child
```
**Or:**
```
child
parent
```
(Demonstrates non-atomic writes without synchronization)

---

## 5. I/O Redirection
### Key Concepts:
- `dup2(oldfd, newfd)` system call
- Overwrites newfd's entry in descriptor table
- Common pattern for shell pipelines

### Code Example: Redirect STDOUT
```c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd = open("output.txt", O_CREAT|O_WRONLY|O_TRUNC, 0666);
    dup2(fd, STDOUT_FILENO);
    close(fd);
    
    printf("This goes to file!\n");
    fprintf(stderr, "This goes to stderr\n");
    return 0;
}
```
**Output File Contents:**
```
This goes to file!
```
**Console Output:**
```
This goes to stderr
```

---

## 6. Standard I/O vs. Unix I/O
### Key Differences:
| Feature          | Standard I/O       | Unix I/O          |
|------------------|--------------------|-------------------|
| Buffering        | Full buffering     | No buffering      |
| Short counts     | Handled            | Manual handling   |
| Thread safety    | Non-thread-safe    | Safe with RIO     |
| File types       | FILE* streams      | Integer descriptors|
| Error handling   | errno + perror()  | Return NULL/EOF   |

### Code Example: Buffering Effects
```c
#include <stdio.h>
#include <unistd.h>

int main() {
    printf("Standard I/O");
    write(STDOUT_FILENO, "Unix I/O\n", 9);
    
    // If program crashes here:
    // abort();
    
    return 0;
}
```
**Output:**
```
Unix I/O
Standard I/O
```
(Demonstrates buffer flushing differences)

---

## Key Takeaways:
1. **Always handle short counts** - Use RIO wrappers for robustness
2. **Understand file descriptor sharing** - Crucial for concurrent access
3. **Choose I/O method wisely** - Standard I/O for text, Unix I/O for networks
4. **Metadata matters** - Check permissions and file types
5. **Redirection is powerful** - Fundamental for shell operations

---

### Multiple-Choice Questions

---

**Question 1: Unix I/O Fundamentals**  
Which statements about Unix file descriptors are **true**?  
A) File descriptors are integers that uniquely identify open files in a process.  
B) File descriptors 0, 1, and 2 are reserved for standard input, output, and error by default.  
C) Closing a file descriptor automatically flushes its buffer to disk.  
D) File descriptors can be shared between processes through inheritance.  

---

**Question 2: File Opening Modes**  
Which `open` flags are valid when opening a file in **non-blocking** mode?  
A) `O_RDONLY | O_NONBLOCK`  
B) `O_WRONLY | O_CREAT`  
C) `O_APPEND | O_SYNC`  
D) `O_RDWR | O_NONBLOCK`  

---

**Question 3: Robust I/O (RIO) Package**  
The RIO package addresses which issues in Unix I/O?  
A) Automatically retrying incomplete reads/writes due to signals.  
B) Handling short counts in network I/O.  
C) Providing thread-safe buffered I/O operations.  
D) Supporting asynchronous I/O operations.  

---

**Question 4: File Metadata**  
Which functions can retrieve metadata about a file (e.g., size, permissions)?  
A) `stat`  
B) `fstat`  
C) `lseek`  
D) `access`  

---

**Question 5: Directory Operations**  
Which system calls are used to read directory contents?  
A) `readdir`  
B) `opendir`  
C) `getcwd`  
D) `closedir`  

---

**Question 6: File Sharing Mechanisms**  
Which mechanisms allow processes to share file descriptors?  
A) Forking a child process.  
B) Using `dup2` to redirect descriptors.  
C) Passing file descriptors via Unix domain sockets.  
D) Using `mmap` to map the same file into memory.  

---

**Question 7: I/O Redirection**  
How can I/O redirection (e.g., `>` in shells) be implemented programmatically?  
A) Using `freopen` to reassign `stdout`.  
B) Closing the original file descriptor and opening a new file with the same descriptor.  
C) Modifying the `FILE` struct’s internal buffer.  
D) Using `dup2` to copy a file descriptor to another.  

---

**Question 8: Standard I/O Limitations**  
Which are limitations of the C Standard I/O library?  
A) Not thread-safe by default.  
B) Buffering can introduce delays in real-time output.  
C) Cannot handle binary files larger than 2 GB.  
D) Poor support for network sockets.  

---

**Question 9: Buffered vs. Unbuffered I/O**  
Which statements about buffering are **true**?  
A) Line-buffered streams flush automatically on newline characters.  
B) Fully buffered streams flush only when the buffer is full.  
C) Unbuffered I/O is faster for small, frequent writes.  
D) `setvbuf` can change the buffering mode of a stream.  

---

**Question 10: Concurrent File Access**  
Which scenarios risk race conditions when multiple processes access the same file?  
A) Using `O_APPEND` to write to the end of a file.  
B) Reading a file without locking while another process writes to it.  
C) Using `flock` to enforce exclusive access.  
D) Writing to disjoint regions of a file mapped with `mmap`.  

---

**Question 11: Performance Optimization**  
Which techniques improve I/O performance for large files?  
A) Increasing buffer size for `read`/`write` operations.  
B) Using memory-mapped I/O (`mmap`).  
C) Disabling kernel caching with `O_DIRECT`.  
D) Using synchronous writes (`O_SYNC`).  

---

**Question 12: Error Handling**  
Which system calls can be interrupted by signals, requiring careful error handling?  
A) `read`  
B) `write`  
C) `open`  
D) `close`  

---

**Question 13: Asynchronous I/O**  
Which statements about asynchronous I/O (`aio_*` functions) are **true**?  
A) It allows overlapping I/O and computation.  
B) It requires non-blocking file descriptors.  
C) Completion can be checked via signals or callbacks.  
D) It is supported uniformly across all Unix-like systems.  

---

**Question 14: File Position Pointers**  
Which operations modify the file position pointer?  
A) `read`  
B) `lseek`  
C) `write`  
D) `fsync`  

---

**Question 15: Choosing I/O Functions**  
When should you prefer Unix I/O over Standard I/O?  
A) When working with network sockets.  
B) When portability across platforms is critical.  
C) When atomicity of operations is required (e.g., `O_APPEND`).  
D) When buffering is needed for performance.  

---

### **Answers and Explanations**

---

**Question 1**  
**Correct Answers**: A, B, D  
- **A/B/D**: File descriptors are process-specific integers (A). 0/1/2 are stdin/stdout/stderr (B). They can be inherited by child processes (D).  
- **C**: Closing a descriptor does **not** guarantee flushing; `fsync` is required.  

---

**Question 2**  
**Correct Answers**: A, D  
- `O_NONBLOCK` can be combined with any access mode (`O_RDONLY`, `O_WRONLY`, `O_RDWR`).  

---

**Question 3**  
**Correct Answers**: A, B  
- RIO handles short counts (B) and retries interrupted syscalls (A). It does not provide thread safety (C) or async I/O (D).  

---

**Question 4**  
**Correct Answers**: A, B  
- `stat` and `fstat` retrieve metadata. `lseek` adjusts file offsets; `access` checks permissions but not metadata.  

---

**Question 5**  
**Correct Answers**: A, B, D  
- `opendir`, `readdir`, and `closedir` are directory functions. `getcwd` returns the current working directory path.  

---

**Question 6**  
**Correct Answers**: A, C  
- Child processes inherit descriptors after `fork` (A). Descriptors can be sent via sockets (C). `dup2` duplicates within a process (B); `mmap` shares memory, not descriptors (D).  

---

**Question 7**  
**Correct Answers**: B, D  
- `dup2` copies descriptors (D). Closing and reopening with the same descriptor achieves redirection (B). `freopen` is a Stdio function (A).  

---

**Question 8**  
**Correct Answers**: A, B, D  
- Stdio is not thread-safe (A), buffering delays output (B), and it’s unsuitable for sockets (D). Modern systems support >2GB files (C is false).  

---

**Question 9**  
**Correct Answers**: A, B, D  
- Line buffering flushes on `\n` (A), full buffering on buffer full (B), and `setvbuf` configures modes (D). Unbuffered I/O is slower for small writes (C is false).  

---

**Question 10**  
**Correct Answers**: B, D  
- Concurrent writes without synchronization (B) and `mmap` without locks (D) risk races. `O_APPEND` ensures atomic writes (A). `flock` prevents races (C).  

---

**Question 11**  
**Correct Answers**: A, B  
- Larger buffers reduce syscall overhead (A). `mmap` bypasses user-kernel copies (B). `O_DIRECT` and `O_SYNC` hurt performance (C/D).  

---

**Question 12**  
**Correct Answers**: A, B  
- `read` and `write` can return `EINTR` if interrupted by signals.  

---

**Question 13**  
**Correct Answers**: A, C  
- Async I/O overlaps I/O and computation (A), uses completion mechanisms (C). It does not require non-blocking mode (B). Support varies (D is false).  

---

**Question 14**  
**Correct Answers**: A, B, C  
- `read`/`write` advance the pointer (A/C); `lseek` explicitly sets it (B). `fsync` flushes data but doesn’t move the pointer (D).  

---

**Question 15**  
**Correct Answers**: A, C  
- Unix I/O is better for sockets (A) and atomic operations like `O_APPEND` (C). Stdio is more portable (B) and buffered (D).  

--- 

**After you are done, it is highly recommended to do the exercises from the original book**

