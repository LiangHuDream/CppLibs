 PART II: Distributed Data
### Chapter 5: Replication - Key Concepts and Challenges

1. **Replication Fundamentals**
- **Purpose**: Ensure data availability, improve read performance, and enable geographic distribution.
- **Basic Approaches**:
  - **Leader-based (Primary-Secondary)**: Single writable node (leader) propagates changes to read-only replicas (followers).
  - **Multi-Leader**: Multiple writable nodes accept writes (e.g., cross-datacenter replication).
  - **Leaderless**: No designated leader; nodes coordinate writes using quorums (e.g., Dynamo-style systems).

2. **Synchronous vs. Asynchronous Replication**
- **Synchronous**: 
  - Leader waits for all replicas to confirm writes before acknowledging success.
  - **Advantage**: Strong consistency guarantees.
  - **Disadvantage**: Higher latency; system halts if replicas fail.
- **Asynchronous**:
  - Leader acknowledges writes before replicas confirm.
  - **Advantage**: Lower latency and better fault tolerance.
  - **Disadvantage**: Risk of data loss if leader fails before replicating.

3. **Handling Node Failures**
- **Follower Failure (Catch-up Recovery)**:
  - Followers use the replication log to recover missed updates.
- **Leader Failure (Failover)**:
  - **Automated Steps**:
    1. Detect leader failure (via timeouts).
    2. Elect new leader (via consensus algorithms like Raft).
    3. Reconfigure clients to use new leader.
  - **Challenges**:
    - Split-brain scenarios (multiple leaders).
    - Data loss if asynchronous replication is used.

4. **Replication Logs**
- **Implementation Methods**:
  - **Statement-Based**: Replicate SQL statements (risky due to non-deterministic functions like `NOW()`).
  - **Write-Ahead Log (WAL)**: Replicate low-level storage engine changes (e.g., PostgreSQL).
  - **Logical (Row-Based) Log**: Record row-level changes (decoupled from storage format).
  - **Trigger-Based**: Use application-level triggers for custom replication.

5. **Replication Lag Issues**
- **Read-After-Write Consistency**:
  - Problem: Users may see stale data after a write.
  - Solutions:
    - Read from leader for user-modified data.
    - Track timestamp of last write and enforce reads from up-to-date replicas.
- **Monotonic Reads**:
  - Problem: Users see data moving backward in time.
  - Solution: Route all reads for a user to the same replica.
- **Consistent Prefix Reads**:
  - Problem: Out-of-order writes cause causality violations.
  - Solution: Ensure writes are applied in causal order (e.g., logical clocks).

6. **Multi-Leader Replication**
- **Use Cases**:
  - Multi-datacenter deployments (lower latency for writes).
  - Offline clients (e.g., collaborative editing).
- **Conflict Resolution**:
  - **Avoidance**: Route writes for a record to a single leader.
  - **Convergence**:
    - Last Write Wins (LWW): Use timestamps (risks data loss).
    - Merge Values: Combine conflicting writes (e.g., CRDTs).
    - Version Vectors: Track causal dependencies to resolve conflicts.

7. **Leaderless Replication**
- **Quorums**:
  - Write quorum (`w` nodes), read quorum (`r` nodes), with `w + r > n` (total nodes) for consistency.
  - **Sloppy Quorums**: Allow writes to non-designated nodes during network partitions.
  - **Hinted Handoff**: Temporarily store writes for unreachable nodes; deliver later.
- **Concurrent Writes**:
  - **Versioning**: Use vector clocks or Lamport timestamps to track causality.
  - **Conflict Resolution**:
    - Client-side resolution (e.g., Cassandra’s `LAST_WRITE_WINS`).
    - Application-specific merge logic.

8. **Key Challenges**
- **CAP Theorem Trade-offs**:
  - Leaderless systems favor Availability and Partition Tolerance (AP).
  - Leader-based systems favor Consistency (CP) but may sacrifice Availability during partitions.
- **Network Delays and Partial Failures**:
  - Asynchronous replication introduces eventual consistency.
  - Monitoring and automatic recovery mechanisms are critical.

9. **Practical Takeaways**
- **Consistency Models**:
  - Choose between strong consistency (e.g., banking systems) and eventual consistency (e.g., social media).
- **Conflict Handling**:
  - Design idempotent writes and conflict-resolution strategies upfront.
- **Monitoring**:
  - Track replication lag, node health, and consistency metrics.

#### Common Misconceptions
- **Myth**: "Asynchronous replication always leads to data loss."
  - Reality: Proper quorum configurations and durability settings (e.g., fsync) mitigate risks.
- **Myth**: "Leaderless systems are inherently less consistent."
  - Reality: Quorum configurations can enforce strong consistency if needed (`w = n, r = 1`).


### Multiple-Choice Questions

---

**Question 1:**  
Which replication strategies are **synchronous by default** in their basic form?  
A) Single-leader replication  
B) Multi-leader replication  
C) Leaderless replication  
D) Chain replication  

**Question 2:**  
In asynchronous leader-follower replication, which of the following are **true**?  
A) Followers can become leaders automatically during failover.  
B) The leader waits for followers to acknowledge writes before confirming to the client.  
C) The system guarantees strong consistency.  
D) Followers may serve stale reads if replication lag exists.  

**Question 3:**  
Which techniques are used to handle **write conflicts** in multi-leader replication?  
A) Last-write-wins (LWW) with synchronized clocks  
B) Conflict-free Replicated Data Types (CRDTs)  
C) Version vectors to track causality  
D) Two-phase commit (2PC)  

**Question 4:**  
Which scenarios are **suitable for multi-leader replication**?  
A) A single data center with high write throughput  
B) Collaborative editing applications  
C) Offline clients that sync data later  
D) Systems requiring strict linearizability  

**Question 5:**  
In leaderless replication, which mechanisms ensure data consistency during node failures?  
A) Hinted handoff  
B) Quorum reads and writes  
C) Conflict resolution using timestamps  
D) Automatic leader election  

**Question 6:**  
Which replication log implementations allow **cross-database compatibility**?  
A) Write-ahead log (WAL)  
B) Trigger-based replication  
C) Logical (row-based) replication  
D) Statement-based replication  

**Question 7:**  
Which issues are caused by **replication lag** in asynchronous systems?  
A) Lost writes due to network partitions  
B) Stale reads for followers  
C) Violation of read-after-write consistency  
D) Split-brain scenarios  

**Question 8:**  
Which methods help achieve **monotonic reads** in a distributed database?  
A) Routing all reads to the leader  
B) Using version vectors to track client sessions  
C) Hashing client IDs to specific replicas  
D) Storing read timestamps in client cookies  

**Question 9:**  
In **dynamo-style leaderless systems**, which are true about sloppy quorums?  
A) They prioritize availability over consistency during network partitions.  
B) They require strict adherence to quorum size (e.g., W + R > N).  
C) They allow writes to non-reachable nodes using hinted handoff.  
D) They guarantee linearizability.  

**Question 10:**  
Which strategies detect **concurrent writes** in leaderless systems?  
A) Vector clocks  
B) Lamport timestamps  
C) Two-phase locking (2PL)  
D) Merkle trees  

**Question 11:**  
Which replication topologies are **prone to write ordering issues** in multi-leader systems?  
A) All-to-all topology  
B) Star topology  
C) Circular topology  
D) Single-leader topology  

**Question 12:**  
Which statements about **quorum consistency** are correct?  
A) A write quorum (W) and read quorum (R) must satisfy W + R > N.  
B) It guarantees sequential consistency.  
C) It allows stale reads if W + R ≤ N.  
D) It prevents split-brain scenarios.  

**Question 13:**  
Which techniques are used for **anti-entropy** in leaderless systems?  
A) Read repair  
B) Merkle trees for efficient comparison  
C) Gossip protocols  
D) Leader election  

**Question 14:**  
Which **failure scenarios** can cause split-brain in a single-leader system?  
A) Network partition isolating the leader from followers  
B) Asynchronous replication with long lag  
C) Faulty leader election after a leader failure  
D) Clock drift between nodes  

**Question 15:**  
In **multi-datacenter replication**, which factors favor multi-leader over single-leader?  
A) Lower latency for local writes  
B) Strong consistency across datacenters  
C) Tolerance for temporary network partitions  
D) Simplified conflict resolution  

---

### Answers and Explanations

---

**Question 1:**  
**Correct Answers:** D  
- **D) Chain replication** is synchronous by design, as writes propagate sequentially through nodes.  
- **A/B/C** are incorrect: Single-leader, multi-leader, and leaderless replications are typically asynchronous in their basic forms.  

**Question 2:**  
**Correct Answers:** D  
- **D)** Followers may serve stale reads due to replication lag in asynchronous systems.  
- **A)** Automatic failover requires additional mechanisms (e.g., consensus algorithms).  
- **B)** Describes synchronous replication.  
- **C)** Asynchronous replication cannot guarantee strong consistency.  

**Question 3:**  
**Correct Answers:** A, B, C  
- **A)** LWW uses timestamps (clocks must be synchronized).  
- **B)** CRDTs allow conflict-free merging of concurrent writes.  
- **C)** Version vectors track causal dependencies.  
- **D)** 2PC is for transactions, not conflict resolution.  

**Question 4:**  
**Correct Answers:** B, C  
- **B)** Collaborative editing requires conflict resolution for concurrent writes.  
- **C)** Offline clients sync asynchronously with a local leader.  
- **A/D)** Multi-leader is unsuitable for single-datacenter high throughput (scalability issues) or strict linearizability.  

**Question 5:**  
**Correct Answers:** A, B  
- **A)** Hinted handoff stores writes temporarily during node failures.  
- **B)** Quorums ensure consistency (e.g., W + R > N).  
- **C/D)** Conflict resolution and leader election are unrelated to failure handling.  

**Question 6:**  
**Correct Answers:** B, C  
- **B)** Trigger-based replication uses application-level logic.  
- **C)** Logical replication is decoupled from storage format.  
- **A/D)** WAL and statement-based logs are database-specific.  

**Question 7:**  
**Correct Answers:** B, C  
- **B)** Followers return outdated data if lag exists.  
- **C)** Read-after-write consistency violations occur if a client reads from a lagging replica.  
- **A/D)** Lost writes and split-brain are unrelated to replication lag.  

**Question 8:**  
**Correct Answers:** B, C  
- **B)** Version vectors ensure a client sees progressively newer data.  
- **C)** Hashing ensures a client consistently reads from the same replica.  
- **A/D)** Routing to the leader is unnecessary, and cookies are unreliable.  

**Question 9:**  
**Correct Answers:** A, C  
- **A)** Sloppy quorums prioritize availability during partitions.  
- **C)** Hinted handoff writes to reachable nodes during failures.  
- **B/D)** Sloppy quorums relax quorum rules and do not guarantee linearizability.  

**Question 10:**  
**Correct Answers:** A, B  
- **A)** Vector clocks track causal relationships.  
- **B)** Lamport timestamps order events.  
- **C/D)** 2PL and Merkle trees are unrelated to concurrency detection.  

**Question 11:**  
**Correct Answers:** B, C  
- **B)** Star topologies introduce central bottlenecks.  
- **C)** Circular topologies may lose writes if a node fails.  
- **A/D)** All-to-all and single-leader avoid ordering issues.  

**Question 12:**  
**Correct Answers:** A, C  
- **A)** W + R > N ensures overlap for consistency.  
- **C)** If W + R ≤ N, stale reads are possible.  
- **B/D)** Quorums do not guarantee sequential consistency or prevent split-brain.  

**Question 13:**  
**Correct Answers:** A, B, C  
- **A)** Read repair fixes inconsistencies during reads.  
- **B)** Merkle trees efficiently compare data ranges.  
- **C)** Gossip protocols propagate updates.  
- **D)** Leader election is unrelated.  

**Question 14:**  
**Correct Answers:** A, C  
- **A)** A network partition can lead to dual leaders.  
- **C)** Faulty elections may elect multiple leaders.  
- **B/D)** Replication lag and clock drift do not cause split-brain.  

**Question 15:**  
**Correct Answers:** A, C  
- **A)** Multi-leader allows local writes with lower latency.  
- **C)** Multi-leader tolerates partitions between datacenters.  
- **B/D)** Multi-leader sacrifices consistency and complicates conflict resolution.