PART III: Derived Data
### Chapter 10: Batch Processing - Key Concepts and Deep Dive

1. Fundamentals of Batch Processing
- **Definition**: 
  - Processing large volumes of data in discrete, finite batches (non-real-time).
  - Suited for scenarios requiring comprehensive analysis of historical data (e.g., daily ETL jobs, analytics).
- **Key Characteristics**:
  - High throughput over low latency.
  - Fault tolerance through data recomputation or checkpointing.
  - Scalability via parallelization across distributed systems.

---

2. Unix Tools and Batch Processing Philosophy
- **Unix Philosophy**:
  - Small, composable programs that do one thing well.
  - Data flows through pipelines (e.g., `grep`, `sort`, `awk`).
- **Example Workflow**:
  - Log analysis using pipes: `cat log.txt | grep "ERROR" | sort | uniq -c`.
- **Limitations**:
  - Single-node processing (no distributed scalability).
  - Limited to text-based data; struggles with complex data formats.

---

3. MapReduce and Distributed Batch Processing
- **Core Idea**:
  - Break computation into **map** (process key-value pairs) and **reduce** (aggregate results) phases.
  - Parallelize across a cluster using distributed storage (e.g., HDFS).
- **Execution Steps**:
  1. **Input Splitting**: Divide input data into chunks (e.g., 128MB blocks in HDFS).
  2. **Map Phase**: Process each chunk in parallel, emitting intermediate key-value pairs.
  3. **Shuffle & Sort**: Group intermediate data by key and distribute to reducers.
  4. **Reduce Phase**: Aggregate values for each key.
- **Fault Tolerance**:
  - Re-execute failed tasks rather than repairing in-memory state.
  - Relies on immutable input data and deterministic operations.

---

4. Joins in Batch Processing
- **Reduce-Side Joins**:
  - **Process**: 
    - Mappers tag records with their source (e.g., "user" or "order").
    - Reducers perform a Cartesian product of records with the same key.
  - **Use Case**: General-purpose joins with no assumptions about data organization.
  - **Drawbacks**: High network traffic during shuffle; requires sorting.
- **Map-Side Joins**:
  - **Broadcast Join**: Small dataset replicated to all mappers (e.g., hash joins).
  - **Partitioned Join**: Pre-shuffle datasets to align partitions (e.g., sorted merge join).
  - **Use Case**: Optimized for large datasets with known partitioning/sorting.
  - **Limitations**: Requires careful data preprocessing (e.g., co-partitioning).

---

5. Output of Batch Workflows
- **Materialization**:
  - Write output to distributed storage (e.g., HDFS, cloud storage).
  - Immutable and versioned for reproducibility.
- **Key Output Types**:
  - **Derived Datasets**: Precomputed aggregates (e.g., daily sales reports).
  - **Search Indexes**: Batch-built indexes (e.g., inverted indexes for full-text search).
  - **Model Training**: Batch-generated machine learning models.

---

6. Hadoop vs. Distributed Databases
- **Hadoop (MapReduce)**:
  - **Strengths**:
    - Handles unstructured/semi-structured data (e.g., logs, JSON).
    - Flexible schema-on-read (no upfront schema definition).
    - Cost-effective scaling with commodity hardware.
  - **Weaknesses**:
    - High latency due to disk I/O and batch scheduling.
    - Verbose API compared to SQL.
- **MPP Databases (e.g., Teradata, Redshift)**:
  - **Strengths**:
    - Optimized for structured data and SQL queries.
    - Faster for interactive queries via in-memory processing.
  - **Weaknesses**:
    - Less flexible for complex data pipelines.
    - Expensive scaling for petabyte-scale data.

---

7. Evolution Beyond MapReduce
- **Limitations of Classic MapReduce**:
  - High disk I/O due to materializing intermediate results.
  - Inflexible for iterative algorithms (e.g., machine learning).
- **Spark and In-Memory Processing**:
  - **RDDs (Resilient Distributed Datasets)**: 
    - Immutable, fault-tolerant collections processed in memory.
    - Enables iterative algorithms (e.g., PageRank, gradient descent).
  - **DAG Scheduler**: Optimizes execution plans to minimize data shuffling.
- **Flink and Unified Batch/Streaming**:
  - Treats batch as a special case of streaming (bounded streams).
  - Enables hybrid pipelines (e.g., Lambda architecture unification).

---

8. High-Level APIs and Declarative Processing
- **Pig Latin**:
  - Scripting language for data flows (e.g., `JOIN`, `GROUP`, `FILTER`).
  - Compiles to MapReduce jobs but abstracts low-level code.
- **Hive**:
  - SQL-like interface (HiveQL) for batch processing.
  - Metadata storage via Hive Metastore (table schemas, partitions).
- **TensorFlow Extended (TFX)**:
  - Batch-oriented ML pipelines (data validation, model training).

---

9. Challenges and Trade-offs
- **Intermediate State Materialization**:
  - **Pros**: Simplifies fault recovery and debugging.
  - **Cons**: Increases I/O overhead; slows down iterative jobs.
- **Data Skew**:
  - Uneven data distribution causes stragglers (slow tasks).
  - Mitigation: Salting keys, dynamic partitioning.
- **Cost of Serialization**:
  - Text formats (e.g., CSV) are human-readable but inefficient.
  - Optimize with binary formats (Avro, Parquet) for faster I/O.

---

10. Summary of Key Takeaways
1. **Batch Processing Paradigm**:
   - Prioritizes throughput and fault tolerance over latency.
   - Built on distributed storage (HDFS) and parallel computation (MapReduce/Spark).
2. **Joins and Shuffling**:
   - Reduce-side joins are flexible but costly; map-side joins require preprocessing.
3. **Hadoop Ecosystem**:
   - Evolved from MapReduce to Spark/Flink for in-memory and stream-batch unification.
4. **Design Trade-offs**:
   - Materialization vs. compute-on-read, scalability vs. complexity.
5. **Modern Use Cases**:
   - Data warehousing, ML training, and large-scale ETL remain dominant applications.

---
### Chapter 10: Batch Processing – 10 Hard Multiple-Choice Questions

---

**Question 1: Key Differences Between Hadoop and MPP Databases**  
Which of the following are **true** differences between Hadoop-based systems and Massively Parallel Processing (MPP) databases?  
A) Hadoop supports unstructured data processing, while MPP databases require structured schemas.  
B) Hadoop uses disk-based execution, while MPP databases rely on in-memory computation.  
C) Hadoop’s fault tolerance relies on task retries, while MPP databases use checkpointing or transactions.  
D) Hadoop is optimized for low-latency queries, while MPP databases focus on high-throughput batch jobs.  

---

**Question 2: Advantages of MapReduce Over Distributed Databases**  
Which advantages does MapReduce **specifically** offer compared to traditional distributed databases?  
A) Native support for complex data transformations (e.g., joins, aggregations).  
B) Better performance for interactive ad-hoc queries.  
C) Flexibility to process unstructured or semi-structured data.  
D) Strong consistency guarantees for transactional workloads.  

---

**Question 3: Intermediate State Materialization**  
What are **valid criticisms** of materializing intermediate state in batch workflows?  
A) Increases complexity of debugging failed jobs.  
B) Reduces fault tolerance by creating single points of failure.  
C) Slows down workflow execution due to I/O overhead.  
D) Makes iterative processing (e.g., machine learning) less efficient.  

---

**Question 4: Map-Side Joins**  
Which scenarios are **appropriate** for using map-side joins in batch processing?  
A) When both input datasets are sorted and partitioned by the same key.  
B) When one dataset is small enough to fit in memory.  
C) When the join key is highly skewed (e.g., power-law distribution).  
D) When the datasets are stored in a column-oriented format.  

---

**Question 5: Output Characteristics of Batch Systems**  
Which statements about the **output** of batch processing workflows are **correct**?  
A) Batch jobs typically generate immutable datasets as output.  
B) Outputs must always overwrite existing data to ensure atomicity.  
C) Outputs can be used to update indexes incrementally.  
D) Outputs are designed for real-time streaming consumption.  

---

**Question 6: Graph Processing in Batch Systems**  
Which properties are **true** for graph processing frameworks like Pregel?  
A) They use bulk synchronous parallel (BSP) execution models.  
B) They require all graph data to fit in memory for fast access.  
C) They prioritize low-latency updates over high-throughput batch processing.  
D) They perform iterative computations by propagating messages between nodes.  

---

**Question 7: Fault Tolerance in Batch Systems**  
Which mechanisms do Hadoop and Spark use for **fault tolerance**?  
A) Write-ahead logging (WAL) for all intermediate steps.  
B) Re-running failed tasks from lineage information.  
C) Checkpointing intermediate state to persistent storage.  
D) Using ACID transactions for atomic job outputs.  

---

**Question 8: Column-Oriented Storage**  
Which optimizations are enabled by **column-oriented storage** in batch processing?  
A) Efficient compression due to homogeneous data types per column.  
B) Faster row-level updates for transactional workloads.  
C) Reduced I/O for queries reading only a subset of columns.  
D) Support for nested data structures like JSON or XML.  

---

**Question 9: High-Level APIs in Batch Systems**  
What are **benefits** of using high-level APIs (e.g., Hive, Pig) over raw MapReduce?  
A) Automatic optimization of execution plans by the query planner.  
B) Reduced boilerplate code for common operations (e.g., joins, filters).  
C) Improved performance for low-latency interactive queries.  
D) Built-in support for stream processing with event-time semantics.  

---

**Question 10: Iterative Processing**  
Which techniques are used to optimize **iterative processing** (e.g., machine learning) in batch systems?  
A) Caching intermediate data in memory across iterations.  
B) Using hash partitioning to minimize data shuffling.  
C) Terminating iterations when convergence criteria are met.  
D) Converting iterative jobs into real-time streaming pipelines.  

---

### **Answers & Explanations**  

---

**Question 1**  
**Correct Answers**: A, C  
- **A**: Hadoop handles unstructured/semi-structured data (e.g., logs, text), while MPP databases (e.g., Teradata) enforce schemas.  
- **C**: Hadoop retries failed tasks, while MPP databases use checkpointing (e.g., Greenplum) or transactional guarantees.  
- **B**: False – Both systems use disk-based execution; MPP databases may use memory for intermediate results.  
- **D**: False – Hadoop targets batch processing, while MPP databases handle interactive queries.  

---

**Question 2**  
**Correct Answers**: C  
- **C**: MapReduce processes unstructured data (e.g., raw text), while databases require structured schemas.  
- **A**: Distributed databases natively support joins/aggregations via SQL.  
- **B/D**: False – MapReduce is not optimized for low latency or transactions.  

---

**Question 3**  
**Correct Answers**: A, C, D  
- **A**: Materialized state complicates debugging by requiring inspection of intermediate files.  
- **C**: Writing intermediate data to disk introduces I/O delays.  
- **D**: Iterative algorithms (e.g., PageRank) suffer from repeated I/O when materializing state.  
- **B**: False – Materialization does not reduce fault tolerance.  

---

**Question 4**  
**Correct Answers**: A, B  
- **A**: Sorted and partitioned datasets enable efficient map-side joins (e.g., merge join).  
- **B**: Broadcast joins (a type of map-side join) require one dataset to fit in memory.  
- **C**: Skewed keys are better handled with reduce-side joins.  
- **D**: Columnar storage is unrelated to join strategy.  

---

**Question 5**  
**Correct Answers**: A, C  
- **A**: Batch outputs (e.g., HDFS files) are typically immutable for reproducibility.  
- **C**: Batch jobs can update indexes (e.g., search indexes) atomically.  
- **B**: False – Outputs can be written to new directories (e.g., atomic rename).  
- **D**: False – Batch outputs are not designed for real-time consumption.  

---

**Question 6**  
**Correct Answers**: A, D  
- **A**: Pregel uses BSP, where computation proceeds in synchronized supersteps.  
- **D**: Nodes exchange messages during iterations (e.g., PageRank updates).  
- **B**: False – Graph data is stored on disk (e.g., Giraph).  
- **C**: False – Graph frameworks focus on batch-style high-throughput processing.  

---

**Question 7**  
**Correct Answers**: B, C  
- **B**: Spark reconstructs lost data via lineage (e.g., RDD dependencies).  
- **C**: Checkpointing (e.g., in Spark Streaming) persists state to recover from failures.  
- **A**: False – Write-ahead logging is used in streaming (e.g., Kafka), not batch.  
- **D**: False – Batch systems (e.g., Hadoop) lack transactional guarantees.  

---

**Question 8**  
**Correct Answers**: A, C  
- **A**: Columns with homogeneous data (e.g., integers) compress efficiently.  
- **C**: Reading fewer columns reduces I/O (e.g., Parquet).  
- **B**: False – Column storage is inefficient for row-level updates.  
- **D**: False – Nested data is better handled with row-based formats (e.g., Avro).  

---

**Question 9**  
**Correct Answers**: A, B  
- **A**: High-level APIs (e.g., Hive) optimize execution plans (e.g., predicate pushdown).  
- **B**: Declarative queries (e.g., SQL) reduce code complexity.  
- **C**: False – High-level APIs do not improve latency; interactive queries require MPP.  
- **D**: False – Stream processing is separate from batch APIs.  

---

**Question 10**  
**Correct Answers**: A, B, C  
- **A**: Spark caches RDDs in memory to avoid reloading data per iteration.  
- **B**: Hash partitioning reduces shuffling in iterative algorithms.  
- **C**: Convergence checks (e.g., gradient descent) terminate iterations early.  
- **D**: False – Iterative processing remains batch-oriented, not streaming.  

--- 

These questions test deep understanding of Hadoop/MapReduce design tradeoffs, fault tolerance mechanisms, and optimizations for batch workflows. Key themes include differences between batch and MPP systems, intermediate state management, and iterative processing patterns.