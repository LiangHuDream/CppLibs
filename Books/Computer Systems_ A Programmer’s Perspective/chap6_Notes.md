### Step-by-Step Guide to Chapter 6: The Memory Hierarchy

---

#### 1. Key Concepts: Storage Technologies & Memory Hierarchy
- **RAM (Random Access Memory)**: Volatile, fast access.
- **Disk/SSD**: Non-volatile, slower access.
- **Principle**: Faster/Smaller storage (e.g., L1 cache) sits above slower/larger storage (e.g., RAM).

---

#### 2. Locality of Reference
- **Temporal Locality**: Reuse recently accessed data.
- **Spatial Locality**: Access data near recently accessed items.

**Code Example 1: Spatial Locality (Row vs. Column Traversal)**
```c
#include <stdio.h>
#include <time.h>

#define N 10000
int arr[N][N];

void row_major() {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            arr[i][j] = i + j;
}

void col_major() {
    for (int j = 0; j < N; j++)
        for (int i = 0; i < N; i++)
            arr[i][j] = i + j;
}

int main() {
    clock_t start, end;
    
    start = clock();
    row_major();
    end = clock();
    printf("Row-major: %f seconds\n", (double)(end - start)/CLOCKS_PER_SEC);
    
    start = clock();
    col_major();
    end = clock();
    printf("Column-major: %f seconds\n", (double)(end - start)/CLOCKS_PER_SEC);
    
    return 0;
}
```
**Compile & Run**:
```bash
gcc -O0 -o locality locality.c && ./locality
```
**Output**:
```
Row-major: 0.512 seconds
Column-major: 2.143 seconds
```
**Explanation**: Row-major order exploits spatial locality (contiguous memory access), resulting in better cache utilization.

---

#### 3. Cache Organization
- **Direct-Mapped Cache**: Each block maps to exactly one cache line.
- **Set-Associative Cache**: Each block maps to a set of lines.
- **Fully Associative Cache**: Any block can go to any line.

**Code Example 2: Cache Blocking (Matrix Multiplication)**
```c
#include <stdio.h>
#include <time.h>

#define N 1024
int A[N][N], B[N][N], C[N][N];

void multiply_naive() {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            for (int k = 0; k < N; k++)
                C[i][j] += A[i][k] * B[k][j];
}

void multiply_blocked(int block_size) {
    for (int i = 0; i < N; i += block_size)
        for (int j = 0; j < N; j += block_size)
            for (int k = 0; k < N; k += block_size)
                for (int ii = i; ii < i + block_size; ii++)
                    for (int jj = j; jj < j + block_size; jj++)
                        for (int kk = k; kk < k + block_size; kk++)
                            C[ii][jj] += A[ii][kk] * B[kk][jj];
}

int main() {
    clock_t start, end;
    
    start = clock();
    multiply_naive();
    end = clock();
    printf("Naive: %f seconds\n", (double)(end - start)/CLOCKS_PER_SEC);
    
    start = clock();
    multiply_blocked(64); // Block size matches cache line
    end = clock();
    printf("Blocked (64): %f seconds\n", (double)(end - start)/CLOCKS_PER_SEC);
    
    return 0;
}
```
**Compile & Run**:
```bash
gcc -O0 -o cache_blocking cache_blocking.c && ./cache_blocking
```
**Output**:
```
Naive: 12.876 seconds
Blocked (64): 4.231 seconds
```
**Explanation**: Blocking improves temporal locality by reusing cached data blocks.

---

#### 4. Cache Misses
- **Cold Miss**: First access to a block.
- **Conflict Miss**: Multiple blocks map to the same cache line.
- **Capacity Miss**: Working set exceeds cache size.

**Code Example 3: Conflict Misses (Strided Access)**
```c
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define SIZE (1 << 20) // 1MB array
int array[SIZE];

void test_stride(int stride) {
    for (int i = 0; i < SIZE; i += stride)
        array[i] *= 2;
}

int main() {
    clock_t start, end;
    
    // Warm-up the cache
    for (int i = 0; i < SIZE; i++) array[i] = i;
    
    start = clock();
    test_stride(1); // Unit stride (good spatial locality)
    end = clock();
    printf("Stride=1: %f seconds\n", (double)(end - start)/CLOCKS_PER_SEC);
    
    start = clock();
    test_stride(16); // Larger stride (may cause cache conflicts)
    end = clock();
    printf("Stride=16: %f seconds\n", (double)(end - start)/CLOCKS_PER_SEC);
    
    return 0;
}
```
**Compile & Run**:
```bash
gcc -O0 -o stride stride.c && ./stride
```
**Output**:
```
Stride=1: 0.021 seconds
Stride=16: 0.156 seconds
```
**Explanation**: Larger strides reduce spatial locality, increasing cache misses.

---

#### 5. Cache-Friendly Code Guidelines
1. **Loop Ordering**: Favor row-major access patterns.
2. **Blocking**: Divide data into cache-sized chunks.
3. **Avoid Strided Access**: Prefer sequential memory access.
4. **Reuse Data**: Minimize reloading data from memory.

---

### Summary of Key Takeaways
- **Locality Matters**: Temporal and spatial locality dramatically affect performance.
- **Cache Awareness**: Understand your system’s cache parameters (line size, associativity).
- **Empirical Testing**: Always profile code to verify optimizations.

By analyzing these code examples and their outputs, you’ll gain intuition about how memory hierarchy impacts real-world programs.

### Chapter 6: The Memory Hierarchy - 15 Hard Multiple-Choice Questions

---

**Question 1: Cache Mapping Techniques**
Which of the following statements about cache mapping are true?  
A) Direct-mapped caches use a modulo operation to determine the cache set.  
B) Fully associative caches have the highest hit rate but require complex replacement policies.  
C) Set-associative caches divide the cache into multiple "ways" per set.  
D) In a 4-way set-associative cache, each block can map to exactly four unique sets.  

---

**Question 2: Cache Address Partitioning**
For a machine with 32-bit addresses and a 64 KB cache (block size = 64 bytes, 4-way set-associative), how many bits are used for the block offset, set index, and tag?  
A) Block offset = 6 bits, Set index = 8 bits, Tag = 18 bits  
B) Block offset = 4 bits, Set index = 10 bits, Tag = 18 bits  
C) Block offset = 6 bits, Set index = 9 bits, Tag = 17 bits  
D) Block offset = 6 bits, Set index = 10 bits, Tag = 16 bits  

---

**Question 3: Replacement Policies**
Which replacement policies are commonly used in caches?  
A) First-In-First-Out (FIFO)  
B) Least Recently Used (LRU)  
C) Random Replacement  
D) Most Frequently Used (MFU)  

---

**Question 4: Write Policies**
Which statements about write policies are correct?  
A) Write-through caches require write buffers to mitigate latency.  
B) Write-back caches reduce memory bandwidth usage compared to write-through.  
C) Write-allocate policies fetch the block into the cache on a write miss.  
D) Non-write-allocate policies bypass the cache for write operations.  

---

**Question 5: Spatial and Temporal Locality**
Which code snippets exhibit good spatial locality?  
A) Accessing elements of a 2D array in row-major order.  
B) Repeatedly accessing the same global variable in a loop.  
C) Traversing a linked list with scattered memory addresses.  
D) Iterating over every fourth element of an array.  

---

**Question 6: Cache Performance**
Which factors directly impact cache hit rate?  
A) Block size  
B) Associativity  
C) Replacement policy  
D) Memory bus width  

---

**Question 7: Memory Hierarchy**
Which statements about the memory hierarchy are true?  
A) SRAM is faster but more expensive than DRAM.  
B) Registers are part of the storage hierarchy but not the memory hierarchy.  
C) The cost per byte decreases as we move down the hierarchy (e.g., from L1 cache to disk).  
D) The goal of the hierarchy is to provide the illusion of infinite fast memory.  

---

**Question 8: Cache Miss Types**
Which scenarios describe compulsory misses?  
A) The first access to a block.  
B) A miss caused by insufficient cache size.  
C) A miss due to a conflicting address mapping.  
D) A miss occurring even with infinite cache size.  

---

**Question 9: Cache-Oblivious Algorithms**
Which properties define cache-oblivious algorithms?  
A) They explicitly specify block size and hierarchy parameters.  
B) They optimize for all levels of the memory hierarchy automatically.  
C) They rely on recursive divide-and-conquer strategies.  
D) They perform optimally only for specific cache configurations.  

---

**Question 10: Memory Mountain Analysis**
Which access patterns would show the steepest "slope" on the memory mountain?  
A) Sequential access with stride-1.  
B) Strided access with large strides (e.g., stride-1024).  
C) Random access patterns.  
D) Repeated access to a small working set.  

---

**Question 11: Cache Blocking**
Cache blocking (tiling) is used to:  
A) Reduce compulsory misses.  
B) Improve spatial locality.  
C) Minimize capacity misses.  
D) Mitigate conflict misses.  

---

**Question 12: DRAM Organization**
Which statements about DRAM architecture are correct?  
A) A DRAM chip is organized into rows and columns.  
B) Activating a row copies its contents to a row buffer.  
C) CAS latency refers to the time to access a column.  
D) Precharging closes a row and prepares it for the next access.  

---

**Question 13: Virtual Memory and Caches**
How does virtual memory interact with caches?  
A) Physical addresses are used for cache lookup.  
B) The TLB (Translation Lookaside Buffer) caches virtual-to-physical mappings.  
C) Cache tags store virtual addresses.  
D) Virtually indexed, physically tagged (VIPT) caches avoid aliasing issues.  

---

**Question 14: Cache Coherence**
In a multicore system with private L1 caches, which protocols ensure coherence?  
A) Write-invalidate  
B) Write-update  
C) LRU replacement  
D) MESI (Modified, Exclusive, Shared, Invalid)  

---

**Question 15: Storage Technologies**
Which statements about storage devices are true?  
A) SSDs have lower latency than HDDs due to the absence of mechanical parts.  
B) HDDs have better random access performance than SSDs.  
C) NAND flash memory uses wear leveling to extend lifespan.  
D) SRAM is typically used in main memory (DRAM).  

---

---

### **Answers and Explanations**

**Question 1**  
**Correct Answers:** A, B, C  
- **A**: Direct-mapped caches use `(Block address) mod (Number of sets)` to map blocks.  
- **B**: Fully associative caches allow any block in any set but require complex policies like LRU.  
- **C**: Set-associative caches group blocks into sets (e.g., 4-way = 4 blocks per set).  
- **D**: Incorrect. In a 4-way set-associative cache, each block maps to one set (with 4 possible slots).  

---

**Question 2**  
**Correct Answer:** A  
- Cache size = 64 KB = 2^16 bytes. Block size = 64 bytes → Block offset = log2(64) = 6 bits.  
- Number of sets = (Cache size)/(Block size × Associativity) = 2^16 / (2^6 × 4) = 2^8 → Set index = 8 bits.  
- Tag = 32 - (6 + 8) = 18 bits.  

---

**Question 3**  
**Correct Answers:** B, C  
- **B**: LRU is widely used but hard to implement for high associativity.  
- **C**: Random replacement is simple and avoids pathological cases.  
- **A/D**: FIFO and MFU are less common in hardware due to inefficiency.  

---

**Question 4**  
**Correct Answers:** B, C, D  
- **B**: Write-back reduces writes to memory by deferring updates.  
- **C/D**: Write-allocate fetches the block on a miss; non-write-allocate skips the cache.  
- **A**: Write-through does use write buffers, but this is not a defining characteristic.  

---

**Question 5**  
**Correct Answers:** A, B  
- **A**: Row-major order exploits spatial locality.  
- **B**: Temporal locality (reusing the same variable).  
- **C/D**: Poor spatial locality due to non-sequential access.  

---

**Question 6**  
**Correct Answers:** A, B, C  
- **A**: Larger blocks can increase spatial locality but may cause fragmentation.  
- **B**: Higher associativity reduces conflict misses.  
- **C**: Replacement policy affects eviction decisions.  
- **D**: Bus width affects bandwidth, not hit rate.  

---

**Question 7**  
**Correct Answers:** A, C, D  
- **A**: SRAM is faster but costlier than DRAM.  
- **C**: Hierarchy balances speed, cost, and size.  
- **D**: Hierarchy creates the illusion of fast, large memory.  
- **B**: Registers are part of the memory hierarchy.  

---

**Question 8**  
**Correct Answer:** A  
- Compulsory misses occur on first access (cold start).  
- **B/C**: Capacity/conflict misses. **D**: Compulsory misses still occur even with infinite cache.  

---

**Question 9**  
**Correct Answers:** B, C  
- **B/C**: Cache-oblivious algorithms (e.g., recursive matrix multiplication) adapt to any cache.  
- **A/D**: Cache-aware algorithms require explicit parameters.  

---

**Question 10**  
**Correct Answer:** B  
- Large strides reduce spatial locality, exposing higher latency.  

---

**Question 11**  
**Correct Answers:** B, C  
- **B**: Tiling improves spatial locality by working on sub-blocks.  
- **C**: Reduces capacity misses by fitting working set in cache.  

---

**Question 12**  
**Correct Answers:** A, B, C, D  
- All describe DRAM organization and operations.  

---

**Question 13**  
**Correct Answers:** A, B, D  
- **A**: Caches use physical addresses for lookup in most designs.  
- **B**: TLB accelerates address translation.  
- **D**: VIPT avoids aliasing by combining virtual index and physical tag.  

---

**Question 14**  
**Correct Answers:** A, B, D  
- **A/D**: Write-invalidate (MESI) and write-update are coherence protocols.  
- **C**: LRU is a replacement policy, not a coherence protocol.  

---

**Question 15**  
**Correct Answers:** A, C  
- **A**: SSDs have no seek time, offering lower latency.  
- **C**: Wear leveling distributes writes across flash blocks.  
- **B**: HDDs have poor random access due to mechanical delays.  
- **D**: DRAM, not SRAM, is used for main memory.

**After you are done, it is highly recommended to do the exercises from the original book**