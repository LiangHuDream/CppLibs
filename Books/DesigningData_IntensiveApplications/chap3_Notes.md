### Chapter 3: Storage and Retrieval

#### 1. Data Structures Powering Databases
**Core Idea**: Databases rely on efficient data structures to store and retrieve data.  
- **Hash Indexes**:  
  - **Structure**: Simple in-memory hash tables mapping keys to byte offsets in a data file.  
  - **Use Case**: Good for key-value stores with frequent appends (e.g., Bitcask).  
  - **Limitations**:  
    - Poor performance for range queries.  
    - Memory constraints (entire index must fit in RAM).  
    - Not durable unless backed by write-ahead logs (WAL).  

- **SSTables and LSM-Trees**:  
  - **SSTable (Sorted String Table)**:  
    - Key-value pairs sorted by key and grouped into immutable files.  
    - Merged in the background via compaction to remove duplicates.  
  - **LSM-Tree (Log-Structured Merge-Tree)**:  
    - Combines in-memory memtables (sorted) and on-disk SSTables.  
    - Writes are fast (sequential appends), reads may require checking multiple SSTables.  
    - **Trade-offs**:  
      - Write-optimized (high throughput).  
      - Reads can be slower (require merging from multiple SSTables).  
      - Background compaction impacts I/O and CPU.  

- **B-Trees**:  
  - **Structure**:  
    - Fixed-size blocks/pages (e.g., 4 KB), organized as a balanced tree.  
    - Pages contain keys and pointers to child pages or data.  
  - **Operations**:  
    - Writes require splitting/merging pages to maintain balance (overhead).  
    - Reads are efficient (logarithmic time).  
  - **Trade-offs vs. LSM-Trees**:  
    - Slower writes (due to page splits and write amplification).  
    - Faster reads (no need to check multiple files).  
    - Fragmentation and recovery complexity (WAL required for crash safety).  

---

#### 2. Indexing Structures
- **Clustered vs. Non-Clustered Indexes**:  
  - **Clustered**: Data stored in index order (e.g., primary key in B-Tree).  
  - **Non-Clustered**: Separate index structure pointing to data location (e.g., secondary indexes).  
- **Multi-Column Indexes**:  
  - Concatenated indexes (e.g., (last_name, first_name)) for composite queries.  
  - Limited flexibility for non-prefix queries.  
- **Full-Text Search Indexes**:  
  - Inverted indexes mapping words to document IDs (e.g., Elasticsearch).  

---

#### 3. Transactional Processing vs. Analytics
- **OLTP (Online Transaction Processing)**:  
  - **Characteristics**:  
    - High volume of small, fast reads/writes.  
    - Queries operate on small subsets of data (e.g., user lookup by ID).  
  - **Storage**: Row-oriented (optimized for retrieving entire records).  

- **OLAP (Online Analytical Processing)**:  
  - **Characteristics**:  
    - Complex analytical queries over large datasets (e.g., aggregation, JOINs).  
    - Batch-oriented, read-heavy workloads.  
  - **Storage**: Column-oriented (optimized for scanning specific columns).  

---

#### 4. Data Warehousing and Columnar Storage
- **Data Warehouse**:  
  - Centralized repository for OLAP workloads.  
  - **Star/Snowflake Schemas**:  
    - **Star Schema**: Fact table (metrics) linked to dimension tables (descriptive attributes).  
    - **Snowflake Schema**: Normalized dimensions (reduces redundancy).  

- **Column-Oriented Storage**:  
  - **Structure**: Data stored column-wise (e.g., Parquet, Cassandra’s SASI).  
  - **Advantages**:  
    - Compression efficiency (similar data types in a column).  
      - Techniques: Bitmap encoding, run-length encoding, dictionary encoding.  
    - Vectorized processing (batch operations on column chunks).  
  - **Challenges**:  
    - Slow writes (columns must be updated across multiple files).  
    - Requires sorting and materialized views for query optimization.  

---

#### 5. Key Challenges and Trade-offs
- **Write Amplification**:  
  - **LSM-Trees**: High write throughput but requires compaction.  
  - **B-Trees**: Write amplification due to page splits and WAL.  
- **Read/Write Optimization**:  
  - LSM-Trees favor writes; B-Trees favor reads.  
- **Compression in Column Stores**:  
  - Benefits storage and I/O but complicates updates.  
- **Sort Order**:  
  - Clustered indexes (B-Tree) vs. SSTable sorting (LSM-Tree).  
  - Columnar storage often sorts rows to group similar values for compression.  

---

#### 6. Real-World Implications
- **Choosing Between LSM and B-Tree**:  
  - **LSM**: Use for write-heavy workloads (e.g., logging, time-series data).  
  - **B-Tree**: Use for read-heavy, transactional systems (e.g., relational databases).  
- **Hybrid Approaches**:  
  - Some systems (e.g., CockroachDB) combine LSM and B-Tree features.  
- **Columnar Storage in Analytics**:  
  - Tools like Apache Parquet and Google BigQuery leverage columnar storage for fast aggregation.  

---

### Summary of Key Takeaways
1. **Storage Engines**:  
   - LSM-Trees excel in write-heavy scenarios but require compaction.  
   - B-Trees offer faster reads but suffer from write amplification.  
2. **OLTP vs. OLAP**:  
   - Row vs. column storage depends on workload (transactional vs. analytical).  
3. **Columnar Optimization**:  
   - Enables compression and efficient analytics but complicates writes.  
4. **Indexing**:  
   - Secondary indexes and specialized structures (e.g., inverted indexes) expand query capabilities at a storage cost.  

### Chapter 3: Storage and Retrieval - Multiple Choice Questions (Hard Difficulty)

---

**Question 1: LSM-Trees vs. B-Trees**  
Which of the following statements are TRUE about LSM-Trees compared to B-Trees?  
A) LSM-Trees have lower write amplification.  
B) LSM-Trees require periodic compaction to merge SSTables.  
C) B-Trees provide faster point reads due to fewer disk seeks.  
D) LSM-Trees are better suited for write-heavy workloads.  
E) B-Trees use immutable on-disk structures for writes.

---

**Question 2: SSTables**  
What are key characteristics of SSTables?  
A) Keys are sorted within each SSTable.  
B) SSTables use in-place updates for existing keys.  
C) Merging SSTables during compaction is efficient due to sorted order.  
D) SSTables store all data in a single immutable file per level.  
E) Deletes require rewriting the entire SSTable.

---

**Question 3: Column-Oriented Storage**  
Which benefits are specific to column-oriented storage?  
A) Efficient compression due to repeated values in columns.  
B) Faster row-level updates compared to row-oriented storage.  
C) Better performance for OLTP workloads.  
D) Reduced I/O for analytical queries reading specific columns.  
E) Support for transactional ACID guarantees.

---

**Question 4: Write Amplification**  
Which factors contribute to write amplification in storage engines?  
A) Frequent compaction in LSM-Trees.  
B) Copy-on-write semantics in B-Trees.  
C) Storing data in sorted order.  
D) Log-structured append-only writes.  
E) Redundant writes due to RAID configurations.

---

**Question 5: OLAP vs. OLTP**  
Which statements differentiate OLAP from OLTP?  
A) OLAP workloads prioritize high-throughput batch reads.  
B) OLTP systems typically use row-oriented storage.  
C) OLAP queries often involve aggregations over large datasets.  
D) OLTP systems are optimized for low-latency point queries.  
E) Star and snowflake schemas are used in OLTP databases.

---

**Question 6: Data Warehousing**  
What are characteristics of a star schema?  
A) A central fact table linked to dimension tables.  
B) Normalized dimension tables to reduce redundancy.  
C) Denormalized dimension tables for query efficiency.  
D) Hierarchical relationships between dimension tables.  
E) Optimized for real-time transactional processing.

---

**Question 7: Column Compression**  
Which techniques are used for column compression in analytical databases?  
A) Bitmap encoding for low-cardinality columns.  
B) Delta encoding for sorted columns.  
C) Run-length encoding for contiguous repeated values.  
D) Huffman coding for variable-length strings.  
E) In-place updates for frequently modified columns.

---

**Question 8: Materialized Views**  
What is true about materialized views in data systems?  
A) They precompute and store query results for faster access.  
B) They are automatically updated synchronously with base tables.  
C) They are used to optimize read-heavy analytical queries.  
D) They increase write latency due to additional computation.  
E) They replace the need for indexing in OLTP systems.

---

**Question 9: Log-Structured Storage**  
Which storage engines use log-structured append-only writes?  
A) LSM-Trees (e.g., LevelDB).  
B) B-Trees (e.g., InnoDB).  
C) Bitcask (hash table with log).  
D) Columnar databases (e.g., Cassandra).  
E) Heap files with in-place updates.

---

**Question 10: Sorting in Column Stores**  
Why is sorting beneficial in column-oriented storage?  
A) Improves compression ratios by grouping similar values.  
B) Enables efficient range queries on sorted columns.  
C) Reduces write amplification during data ingestion.  
D) Simplifies transaction management in OLTP systems.  
E) Allows faster joins using merge operations.

---

### Answers and Explanations

---

**Question 1: LSM-Trees vs. B-Trees**  
**Correct Answers**: A, B, D  
- **A**: LSM-Trees have lower write amplification because they batch writes in memory and append to SSTables.  
- **B**: SSTables require periodic compaction to merge overlapping ranges and remove obsolete data.  
- **D**: LSM-Trees handle write-heavy workloads better due to sequential writes and buffering.  
- **C (False)**: B-Trees may require multiple disk seeks for point reads (e.g., tree traversal), while LSM-Trees may check multiple SSTables.  
- **E (False)**: B-Trees use mutable on-disk structures with in-place updates.

---

**Question 2: SSTables**  
**Correct Answers**: A, C  
- **A**: SSTables store keys in sorted order for efficient range queries.  
- **C**: Merging sorted SSTables is efficient (similar to mergesort).  
- **B (False)**: SSTables are immutable; updates are appended as new entries.  
- **D (False)**: LSM-Trees use multiple SSTables per level (e.g., LevelDB’s levels).  
- **E (False)**: Deletes are handled via tombstones, not SSTable rewrites.

---

**Question 3: Column-Oriented Storage**  
**Correct Answers**: A, D  
- **A**: Columns often contain repetitive values, enabling compression (e.g., run-length encoding).  
- **D**: Analytical queries read specific columns, reducing I/O by skipping unrelated columns.  
- **B (False)**: Row-oriented storage is better for frequent row-level updates.  
- **C (False)**: OLTP workloads favor row-oriented storage.  
- **E (False)**: Column stores often sacrifice transactional guarantees for analytical performance.

---

**Question 4: Write Amplification**  
**Correct Answers**: A, B, E  
- **A**: Compaction in LSM-Trees rewrites data multiple times.  
- **B**: B-Trees split and rebalance nodes, causing redundant writes.  
- **E**: RAID redundancy writes data to multiple disks.  
- **C (False)**: Sorting doesn’t directly cause write amplification.  
- **D (False)**: Log-structured writes reduce amplification by appending.

---

**Question 5: OLAP vs. OLTP**  
**Correct Answers**: A, B, C, D  
- **A**: OLAP focuses on batch reads (e.g., aggregations).  
- **B**: OLTP uses row storage for fast row access.  
- **C**: OLAP queries aggregate large datasets (e.g., SUM, COUNT).  
- **D**: OLTP optimizes for low-latency point queries (e.g., primary key lookups).  
- **E (False)**: Star/snowflake schemas are for OLAP, not OLTP.

---

**Question 6: Data Warehousing**  
**Correct Answers**: A, C  
- **A**: Star schemas have a central fact table connected to denormalized dimension tables.  
- **C**: Denormalized dimensions avoid joins for faster queries.  
- **B (False)**: Snowflake schemas normalize dimensions; star schemas denormalize.  
- **D (False)**: Snowflake schemas use hierarchical dimensions.  
- **E (False)**: Star schemas are for OLAP, not OLTP.

---

**Question 7: Column Compression**  
**Correct Answers**: A, B, C  
- **A**: Bitmap encoding is effective for columns with few distinct values (e.g., enums).  
- **B**: Delta encoding stores differences between sorted values (e.g., timestamps).  
- **C**: Run-length encoding compresses contiguous repeated values.  
- **D (False)**: Huffman coding is generic and less common in column stores.  
- **E (False)**: Column stores use immutable files; updates append new data.

---

**Question 8: Materialized Views**  
**Correct Answers**: A, C, D  
- **A**: Materialized views store precomputed results (e.g., aggregated data).  
- **C**: They optimize read-heavy analytical queries by avoiding recomputation.  
- **D**: Maintaining materialized views adds overhead during writes.  
- **B (False)**: Updates are often asynchronous to avoid write latency.  
- **E (False)**: Materialized views complement indexes but don’t replace them.

---

**Question 9: Log-Structured Storage**  
**Correct Answers**: A, C  
- **A**: LSM-Trees use log-structured writes (e.g., LevelDB).  
- **C**: Bitcask appends writes to a log and uses a hash table for keys.  
- **B (False)**: B-Trees use in-place updates.  
- **D (False)**: Cassandra uses a hybrid approach (SSTables with memtables).  
- **E (False)**: Heap files allow in-place updates.

---

**Question 10: Sorting in Column Stores**  
**Correct Answers**: A, B, E  
- **A**: Sorting improves compression by clustering similar values.  
- **B**: Sorted columns enable efficient range scans (e.g., time ranges).  
- **E**: Sorted columns allow merge joins without hashing.  
- **C (False)**: Sorting increases write amplification during ingestion.  
- **D (False)**: Sorting is unrelated to OLTP transaction management.