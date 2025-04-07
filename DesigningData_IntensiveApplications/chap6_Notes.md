### Chapter 6: Partitioning 

#### 1. Purpose of Partitioning
- **Scalability**: Distribute data across nodes to handle larger datasets/higher throughput.
- **Performance**: Reduce query latency by parallelizing operations.
- **Fault tolerance**: Isolate failures to specific partitions (when combined with replication).

---

#### 2. Partitioning Strategies
a. **Key-Range Partitioning**
- **Mechanism**: Assign contiguous ranges of keys to partitions (e.g., `[a-f]`, `[g-m]`).
- **Pros**:
  - Efficient range queries (keys in the same partition are physically close).
  - Natural for ordered datasets (e.g., time-series data).
- **Cons**:
  - Risk of **hotspots** if access patterns are skewed (e.g., recent timestamps).
  - Requires careful key selection to balance load.

b. Hash-Based Partitioning
- **Mechanism**: Apply a hash function to keys, then modulo to assign partitions.
- **Pros**:
  - Even data distribution reduces hotspots (assuming a good hash function).
  - Simplified load balancing.
- **Cons**:
  - Range queries become inefficient (keys are scattered across partitions).
  - Example: Dynamo, Cassandra.

c. Skewed Workloads & Hotspot Mitigation
- **Problem**: Uneven access patterns (e.g., celebrity accounts in social media).
- **Solutions**:
  - **Salting**: Append a random suffix to keys (e.g., `user123_0`, `user123_1`).
  - **Secondary Indexing**: Distribute index entries across partitions.

---

#### 3. Partitioning and Secondary Indexes
a. **Document-Partitioned Indexes**
- **Mechanism**: Store index entries locally within each partition (e.g., local indexes in MongoDB).
- **Pros**: Fast writes (indexes updated within the same partition).
- **Cons**: Requires **scatter/gather** for queries (inefficient for global searches).

b. **Term-Partitioned Indexes**
- **Mechanism**: Global index partitioned by term (e.g., Elasticsearch).
- **Pros**: Efficient global searches (query only relevant partitions).
- **Cons**: Slower writes (requires cross-partition coordination).

---

#### 4. Rebalancing Partitions
a. **Goals**
- **Balance load**: Ensure data/query load is evenly distributed.
- **Minimize disruption**: Avoid moving large amounts of data during rebalancing.

b. **Strategies**
- **Fixed Number of Partitions**:
  - Predefine more partitions than nodes (e.g., 10x nodes).
  - Assign multiple partitions per node (e.g., Kafka).
  - Nodes added/removed by redistributing partitions.
- **Dynamic Partitioning**:
  - Automatically split/merge partitions based on size (e.g., HBase, Bigtable).
- **Manual Partitioning**:
  - Requires administrative intervention (rarely used in modern systems).

c. **Challenges**
- **Network overhead**: Data movement during rebalancing.
- **Consistency**: Ensure minimal impact on read/write operations during rebalancing.

---

#### 5. Request Routing
- **Problem**: Clients need to know which node to query for a specific key.
- **Solutions**:
  - **Coordination Service**: Use a separate service (e.g., ZooKeeper) to track partition-node mappings.
  - **Routing Layer**: Deploy a proxy layer that routes requests (e.g., Redis Cluster).
  - **Client Awareness**: Clients directly communicate with nodes using partition metadata (e.g., Dynamo).

a. **Consistency in Metadata**
- **Challenge**: Ensuring all clients/nodes agree on partition assignments.
- **Approach**: Use consensus algorithms (e.g., Raft) or versioned metadata.

---

#### 6. **Cross-Partition Operations**
- **Joins**: 
  - Difficult in partitioned systems; often handled via batch processing (e.g., MapReduce).
- **Transactions**:
  - Multi-partition transactions require coordination (e.g., 2PC), which impacts performance.

---

#### 7. **Key Takeaways**
- **Trade-offs**:
  - Hash partitioning balances load but sacrifices range queries.
  - Key-range partitioning optimizes for locality but risks hotspots.
- **Rebalancing**: Automate where possible to handle cluster scaling.
- **Secondary Indexes**: Choose between query efficiency (term-partitioned) and write efficiency (document-partitioned).
- **System Design**: Combine partitioning with replication (Chapter 5) for fault tolerance.

---

#### 8. **Common Pitfalls**
- **Underestimating Hotspots**: Even hash-based systems can develop skewed access patterns.
- **Over-Partitioning**: Too many partitions increase metadata overhead.
- **Ignoring Network Costs**: Rebalancing and cross-partition queries can degrade performance.

---
### Multiple Choice Questions

---

**Question 1: Partitioning Strategies for Key-Value Data**  
Which of the following statements about partitioning strategies are true?  
A) Range partitioning preserves ordering but may lead to skewed workloads.  
B) Hash partitioning guarantees equal data distribution across all partitions.  
C) Consistent hashing reduces rebalancing overhead compared to hash mod N.  
D) Key-range partitioning is optimal for range queries (e.g., time-series data).  

---

**Question 2: Handling Hotspots**  
Which techniques effectively mitigate hotspot issues caused by skewed workloads?  
A) Adding more partitions to distribute load.  
B) Adding a random prefix/suffix to hotspot keys.  
C) Using a hash function with higher entropy.  
D) Manually reassigning hotspot keys to underutilized nodes.  

---

**Question 3: Secondary Index Partitioning**  
When partitioning secondary indexes by document (local indexing):  
A) Queries requiring cross-partition scans are eliminated.  
B) Index entries are co-located with their corresponding data records.  
C) Write operations require updating multiple partitions.  
D) Range queries on secondary keys are optimized.  

---

**Question 4: Rebalancing Partitions**  
Which rebalancing strategies minimize data movement during scaling?  
A) Fixed number of partitions with hash-based assignment.  
B) Dynamic partitioning based on partition size thresholds.  
C) Using the modulo operation (hash mod N) for partition assignment.  
D) Virtual nodes (e.g., consistent hashing with tokens).  

---

**Question 5: Request Routing**  
Which approaches are valid for routing client requests to the correct partition?  
A) Clients contact any node, which proxies the request to the appropriate partition.  
B) A centralized routing tier (e.g., ZooKeeper) maintains partition assignment metadata.  
C) Clients use a gossip protocol to discover partition locations.  
D) Partition assignments are hardcoded in client libraries.  

---

**Question 6: Partitioning by Hash of Key**  
What are limitations of hash-based partitioning?  
A) Range queries require scatter-gather across all partitions.  
B) It inherently prevents hotspots for high-frequency keys.  
C) Rebalancing requires redistributing a large fraction of data.  
D) Ordering of keys is preserved within partitions.  

---

**Question 7: Partitioning Secondary Indexes by Term**  
When partitioning secondary indexes by term (global indexing):  
A) Index entries for the same term are stored in the same partition.  
B) Writes to a single document may update multiple partitions.  
C) Queries on secondary keys can be served from a single partition.  
D) The index is always consistent with the primary data partitions.  

---

**Question 8: Trade-offs in Partitioning Schemes**  
Which statements are true about partitioning trade-offs?  
A) Hash partitioning simplifies range queries but complicates writes.  
B) Range partitioning enables efficient scans but risks skewed data.  
C) Dynamic partitioning avoids manual intervention but adds metadata overhead.  
D) Consistent hashing ensures minimal data movement during node failures.  

---

**Question 9: Handling Write Conflicts**  
In a multi-leader partitioned system, which techniques address write conflicts?  
A) Last-write-wins (LWW) using synchronized clocks.  
B) Version vectors to track causal dependencies.  
C) Conflict-free replicated data types (CRDTs).  
D) Quorum writes to enforce strict consistency.  

---

**Question 10: System Design for Partitioning**  
Which factors are critical when designing a partitioned system?  
A) The choice of partition key to avoid correlated access patterns.  
B) The use of composite primary keys to combine range and hash partitioning.  
C) Ensuring all secondary indexes are co-located with primary data.  
D) Leveraging workload patterns to optimize partition boundaries.  

---

### **Answers & Explanations**  

---

**Question 1**  
**Correct Answers**: A, C, D  
- **A**: Range partitioning preserves order but may skew data (e.g., all "A*" keys in one partition).  
- **C**: Consistent hashing reduces data movement by assigning partitions to a ring of tokens.  
- **D**: Key-range partitioning supports efficient range scans (e.g., timestamp ranges).  
- **B**: Hash partitioning does *not* guarantee equal distribution (e.g., skewed key frequencies).  

---

**Question 2**  
**Correct Answers**: B, D  
- **B**: Salting keys (e.g., appending random bits) distributes writes across partitions.  
- **D**: Manual reassignment targets hotspots but requires operational effort.  
- **A/C**: More partitions or better hashing spreads load but doesn’t fix skewed *individual* keys.  

---

**Question 3**  
**Correct Answers**: B, A  
- **B**: Local indexes store entries with their data, avoiding cross-partition writes.  
- **A**: Queries on non-partitioned secondary keys still require scatter-gather.  
- **C/D**: Local indexes do *not* optimize cross-partition queries or range scans.  

---

**Question 4**  
**Correct Answers**: A, B, D  
- **A**: Fixed partitions with hash assignment minimize movement (only reassign partitions).  
- **B**: Dynamic partitioning splits/merges partitions based on size (e.g., Bigtable).  
- **D**: Virtual nodes (consistent hashing) limit data movement during node changes.  
- **C**: Modulo requires redistributing almost all data when N changes.  

---

**Question 5**  
**Correct Answers**: A, B  
- **A**: Some systems (e.g., Dynamo) use node proxies for routing.  
- **B**: ZooKeeper or etcd can track partition assignments centrally.  
- **C**: Gossip is used for membership, not typically for routing.  
- **D**: Hardcoding limits flexibility during rebalancing.  

---

**Question 6**  
**Correct Answers**: A, C  
- **A**: Hash destroys ordering, forcing scatter-gather for ranges.  
- **C**: Changing the number of partitions (hash mod N) redistributes most keys.  
- **B**: Hotspots still occur if certain keys are accessed frequently.  
- **D**: Ordering is *not* preserved in hash partitioning.  

---

**Question 7**  
**Correct Answers**: A, C  
- **A**: Global indexes group entries by term (e.g., all "user:123" in one partition).  
- **C**: Term-partitioned indexes allow single-partition queries for specific terms.  
- **B**: Writes to a document may update *multiple* global index partitions.  
- **D**: Global indexes may lag behind primary data (eventual consistency).  

---

**Question 8**  
**Correct Answers**: B, C, D  
- **B**: Range partitioning risks skew (e.g., "A*" keys overloading a partition).  
- **C**: Dynamic partitioning automates scaling but requires metadata management.  
- **D**: Consistent hashing minimizes data movement when nodes join/leave.  
- **A**: Hash partitioning complicates range queries, not writes.  

---

**Question 9**  
**Correct Answers**: B, C  
- **B**: Version vectors track causality to detect conflicts.  
- **C**: CRDTs (e.g., counters, sets) resolve conflicts automatically.  
- **A**: LWW risks data loss if clocks are unsynchronized.  
- **D**: Quorums enforce consistency but don’t resolve conflicts.  

---

**Question 10**  
**Correct Answers**: A, B, D  
- **A**: Poor partition keys (e.g., user_id for social graphs) cause hotspots.  
- **B**: Composite keys (e.g., `(tenant_id, user_id)`) combine range/hash benefits.  
- **D**: Workload analysis informs partition boundaries (e.g., time-based sharding).  
- **C**: Co-locating secondary indexes is impractical for global queries.  

--- 

These questions test nuanced understanding of partitioning trade-offs, rebalancing strategies, and system design considerations covered in Chapter 6. The explanations tie back to key concepts like hotspot mitigation, secondary indexing, and consistency models.