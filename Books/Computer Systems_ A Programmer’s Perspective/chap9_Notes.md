### Chapter 9: Virtual Memory

---

#### 1. Virtual Memory Basics
**Core Concepts**:
- **Address Spaces**: Each process has a _virtual address space_ (illusion of exclusive memory) mapped to _physical memory_ via hardware/OS.
- **Pages**: Virtual/physical memory divided into fixed-size pages (e.g., 4KB). Page tables track mappings.
- **Page Faults**: OS handles missing pages by loading from disk (demand paging).

**Code Example: Page Fault Simulation**
```c
#include <stdio.h>
#include <stdlib.h>
#define SIZE (8 * 1024 * 1024) // 8MB

int main() {
    char *array = malloc(SIZE);
    for (int i = 0; i < SIZE; i += 4096) { // Access every page
        array[i] = 'A'; // Triggers page faults initially
    }
    free(array);
    return 0;
}
```
**Testing**:
- Compile with `gcc -o pagefault pagefault.c`.
- Use `time ./pagefault` to observe initial load delays (page faults).

---

#### 2. Address Translation & TLB
**Core Concepts**:
- **Multi-Level Page Tables**: Reduce memory overhead (e.g., x86-64 uses 4-level tables).
- **TLB**: Cache for page table entries (PTEs) to speed up translation.

**Code Example: TLB Effects**
```c
#include <stdio.h>
#include <time.h>
#define SIZE (1 << 24) // 16MB array

int main() {
    int *array = malloc(SIZE * sizeof(int));
    clock_t start = clock();
    for (int i = 0; i < SIZE; i++) {
        array[i] = i; // Sequential access (TLB-friendly)
    }
    printf("Sequential: %f sec\n", (double)(clock() - start)/CLOCKS_PER_SEC);

    start = clock();
    for (int i = 0; i < SIZE; i += 4096) { // Stride to stress TLB
        array[i] = i;
    }
    printf("Strided: %f sec\n", (double)(clock() - start)/CLOCKS_PER_SEC);
    free(array);
    return 0;
}
```
**Testing**:
- Run to see slower strided access due to TLB misses.

---

#### 3. Dynamic Memory Allocation
**Core Concepts**:
- **Implicit Free List**: Blocks with size/allocated flag in header.
- **Explicit Free List**: Maintain linked list of free blocks for faster coalescing.
- **Boundary Tags**: Store metadata at block edges to enable merging.

**Code Example: Simple Allocator**
```c
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define HEAP_SIZE 4096
static char heap[HEAP_SIZE];
typedef struct block {
    size_t size;
    struct block *next;
    int free;
} Block;

void init_heap() {
    Block *head = (Block *)heap;
    head->size = HEAP_SIZE - sizeof(Block);
    head->next = NULL;
    head->free = 1;
}

void *simple_malloc(size_t size) {
    Block *curr = (Block *)heap;
    while (curr) {
        if (curr->free && curr->size >= size) {
            if (curr->size > size + sizeof(Block)) { // Split block
                Block *new = (Block *)((char *)curr + sizeof(Block) + size);
                new->size = curr->size - size - sizeof(Block);
                new->next = curr->next;
                new->free = 1;
                curr->size = size;
                curr->next = new;
            }
            curr->free = 0;
            return (void *)(curr + 1);
        }
        curr = curr->next;
    }
    return NULL; // Out of memory
}

void simple_free(void *ptr) {
    Block *b = (Block *)ptr - 1;
    b->free = 1;
    // Coalesce with next block if free
    if (b->next && b->next->free) {
        b->size += sizeof(Block) + b->next->size;
        b->next = b->next->next;
    }
}

int main() {
    init_heap();
    int *a = simple_malloc(sizeof(int));
    *a = 42;
    printf("Allocated int: %d\n", *a);
    simple_free(a);
    return 0;
}
```
**Testing**:
- Compile with `gcc -o allocator allocator.c`.
- Verify allocation/free works and inspect heap structure with debugger.

---

#### 4. Garbage Collection (Mark & Sweep)
**Core Concepts**:
- **Roots**: Global variables, stack variables, registers.
- **Mark Phase**: Traverse reachable objects from roots.
- **Sweep Phase**: Reclaim memory from unmarked objects.

**Code Example: Conservative GC (Simplified)**
```c
#include <stdio.h>
#include <stdlib.h>
#define MEM_SIZE 1024

typedef struct Object {
    int marked;
    struct Object *next;
} Object;

Object *heap = NULL;
Object *free_list = NULL;

void gc_mark(Object *obj) {
    if (!obj || obj->marked) return;
    obj->marked = 1;
    // Recursively mark children (simplified)
}

void gc_sweep() {
    Object **prev = &heap;
    Object *curr = heap;
    while (curr) {
        if (!curr->marked) {
            *prev = curr->next;
            curr->next = free_list;
            free_list = curr;
        } else {
            curr->marked = 0;
            prev = &curr->next;
        }
        curr = *prev;
    }
}

void gc() {
    // Mark roots (simulate stack/global)
    Object *root = heap;
    gc_mark(root);
    gc_sweep();
}

int main() {
    // Initialize heap with dummy objects
    for (int i = 0; i < 5; i++) {
        Object *obj = malloc(sizeof(Object));
        obj->marked = 0;
        obj->next = heap;
        heap = obj;
    }
    gc();
    return 0;
}
```
**Testing**:
- Run with Valgrind to detect leaks (`valgrind ./gc`).

---

#### 5. Memory-Mapped Files
**Core Concepts**:
- **mmap**: Map file/device into virtual address space for efficient I/O.

**Code Example: File Mapping**
```c
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd = open("test.txt", O_RDWR | O_CREAT, 0666);
    write(fd, "Hello, mmap!", 12);
    char *map = mmap(NULL, 12, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    printf("File content: %s\n", map);
    map[0] = 'h'; // Modify via memory
    munmap(map, 12);
    close(fd);
    return 0;
}
```
**Testing**:
- Create `test.txt`, run to see file modification via memory mapping.

---

### Summary
- **Virtual Memory** enables isolation, efficient memory use, and simplified programming.
- **Dynamic Allocators** manage heap space with trade-offs between speed/fragmentation.
- **Garbage Collection** automates memory reclamation but requires precise root identification.
- **mmap** bridges memory and files for high-performance I/O.


### **Chapter 9: Virtual Memory – Key Concepts and Hard-Level Multiple-Choice Questions**

---

Question 1: Address Translation and Page Tables
**What are the primary purposes of a page table in virtual memory systems? (Select two.)**  
A. Storing cryptographic hashes of physical pages for security.  
B. Mapping virtual page numbers (VPNs) to physical page numbers (PPNs).  
C. Tracking page permissions (read, write, execute).  
D. Storing the contents of swapped-out pages on disk.  

---

Question 2: Translation Lookaside Buffer (TLB)
**Which scenarios cause a TLB miss? (Select two.)**  
A. A process accesses a page that has been recently evicted from the TLB.  
B. The virtual address corresponds to a page not present in physical memory (page fault).  
C. The page table entry (PTE) for the VPN is invalid.  
D. The TLB is fully associative and has no empty entries.  

---

Question 3: Multi-Level Page Tables  
**Why are multi-level page tables more space-efficient than single-level page tables? (Select two.)**  
A. They allocate entries only for regions of the virtual address space that are in use.  
B. They reduce the number of TLB misses during address translation.  
C. They compress metadata for unused pages.  
D. They avoid storing PTEs for large contiguous unused regions of the virtual address space.  

---

Question 4: Memory-Mapped Files  
**Which operations are supported by memory-mapped files in Linux? (Select two.)**  
A. Directly accessing file data through pointers without explicit I/O syscalls.  
B. Guaranteeing atomicity for concurrent writes to the same mapped region.  
C. Sharing read-only data between multiple processes.  
D. Automatically synchronizing mapped memory with the disk file after every write.  

---

Question 5: Dynamic Memory Allocation  
**Which statements about implicit free lists in memory allocators are true? (Select two.)**  
A. They require explicit metadata to link free blocks.  
B. Free blocks are traversed sequentially during allocation.  
C. Coalescing adjacent free blocks requires boundary tags.  
D. The allocator must search the entire heap to find a suitable free block.  

---

Question 6: Fragmentation  
**Which types of fragmentation are reduced by segregated free lists? (Select two.)**  
A. Internal fragmentation.  
B. External fragmentation.  
C. Logical fragmentation.  
D. Compaction-induced fragmentation.  

---

Question 7: Garbage Collection
**In a conservative garbage collector for C programs, which scenarios may lead to memory leaks? (Select two.)**  
A. Integer variables accidentally interpreted as pointers to live objects.  
B. Memory blocks referenced only by stale pointers in deallocated stack frames.  
C. Memory blocks referenced by pointers stored in registers.  
D. Memory blocks with cyclic references.  

---

Question 8: Memory-Related Bugs  
**Which code snippets contain memory-related bugs? (Select two.)**  
```c
// Snippet 1:
int* p = (int*)malloc(sizeof(int));
*p = 42;
free(p);
printf("%d", *p);

// Snippet 2:
int arr[10];
arr[10] = 0;

// Snippet 3:
int* q;
*q = 100;

// Snippet 4:
char* s = "read-only";
s[0] = 'R';
```  
A. Snippet 1  
B. Snippet 2  
C. Snippet 3  
D. Snippet 4  

---

Question 9: mmap and munmap  
**What are valid use cases for `mmap`? (Select two.)**  
A. Allocating small blocks of memory for short-lived objects.  
B. Implementing demand-paging for large files.  
C. Creating shared memory regions between processes.  
D. Reducing the overhead of frequent `malloc`/`free` calls.  

---

Question 10: Copy-on-Write (COW)  
**Which operations trigger copy-on-write in a process fork? (Select two.)**  
A. Reading data from a page shared between parent and child.  
B. Writing data to a page shared between parent and child.  
C. Executing code from a read-only text segment.  
D. Allocating new pages in the child process.  

---

Question 11: Page Replacement Policies  
**Which policies are vulnerable to Belady’s anomaly? (Select two.)**  
A. FIFO  
B. LRU  
C. Optimal  
D. Clock (Second Chance)  

---

Question 12: Memory Aliasing  
**What causes memory aliasing in virtual memory systems? (Select two.)**  
A. Two virtual addresses mapping to the same physical address.  
B. A single virtual address mapped to multiple physical addresses.  
C. Using large pages (e.g., 2MB) instead of small pages (4KB).  
D. Shared libraries loaded into multiple processes.  

---

Question 13: Allocator Metadata  
**Which allocator metadata fields are typically stored in boundary tags? (Select two.)**  
A. Size of the block.  
B. Pointers to adjacent free blocks.  
C. Allocation status (free/allocated).  
D. Timestamp of the last access.  

---

Question 14: Demand Paging
**Which events occur during a page fault? (Select two.)**  
A. The OS evicts a page from physical memory to disk.  
B. The TLB is flushed to ensure consistency.  
C. The page table entry (PTE) is updated with a valid physical page number.  
D. The interrupted process resumes execution immediately after the fault is handled.  

---

Question 15: Memory Protection  
**Which mechanisms enforce memory protection in virtual memory? (Select two.)**  
A. Page table entries with read/write/execute permission bits.  
B. Kernel-mode vs. user-mode CPU execution levels.  
C. Checksums stored in TLB entries.  
D. Segmented addressing with base/bound registers.  

---

---

### Answers and Explanations

1. **B, C**  
   - Page tables map VPNs to PPNs (B) and store permissions (C). D is incorrect because swapped-out pages are tracked in the swap space, not the page table. A is unrelated to page tables.  

2. **A, B**  
   - A TLB miss occurs when the VPN is not cached (A). A page fault (B) also causes a TLB miss since the PTE is invalid. C refers to invalid PTEs (handled by page faults), not TLB misses. D is incorrect because associativity affects replacement, not misses.  

3. **A, D**  
   - Multi-level tables save space by omitting unused regions (A/D). B and C are unrelated to multi-level efficiency.  

4. **A, C**  
   - Memory-mapped files allow pointer-based file access (A) and shared read-only regions (C). B/D are not guaranteed by `mmap`.  

5. **B, C**  
   - Implicit lists traverse blocks sequentially (B) and use boundary tags for coalescing (C). A/D describe explicit free lists.  

6. **A, B**  
   - Segregated lists reduce external fragmentation (B) by size classes and internal fragmentation (A) via block size matching.  

7. **A, B**  
   - Conservative GC may mistake integers for pointers (A) and miss stale references (B). C/D are handled by precise GC.  

8. **B, C, D**  
   - Snippet 2: off-by-one (B). Snippet 3: dereferencing uninitialized pointer (C). Snippet 4: writing to read-only memory (D).  

9. **B, C**  
   - `mmap` is used for demand-paging (B) and shared memory (C). A/D are better handled by `malloc`.  

10. **B, C**  
    - COW triggers on writes (B). Executing code (C) uses read-only pages, avoiding COW.  

11. **A, D**  
    - FIFO (A) and Clock (D) suffer from Belady’s anomaly. LRU (B) and Optimal (C) do not.  

12. **A, D**  
    - Aliasing occurs when multiple VPNs map to the same PPN (A) or via shared libraries (D).  

13. **A, C**  
    - Boundary tags store size (A) and allocation status (C). B is for explicit lists.  

14. **A, C**  
    - Page faults trigger eviction (A) and PTE updates (C). B/D are incorrect (TLB not flushed; process resumes after handling).  

15. **A, B**  
    - Permissions in PTEs (A) and CPU modes (B) enforce protection. D is segmentation, not virtual memory.  

---
**After you are done, it is highly recommended to do the exercises from the original book**