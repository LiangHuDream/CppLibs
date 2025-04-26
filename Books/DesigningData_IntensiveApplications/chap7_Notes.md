### Chapter 7: Transactions

---

#### **1. The Slippery Concept of Transactions**
**Core Challenge**:  
Transactions are foundational but inherently complex due to trade-offs between isolation, performance, and correctness. Key questions include:  
- What guarantees do transactions provide?  
- How do isolation levels balance consistency vs. concurrency?  

---

#### **2. ACID Properties Demystified**
**Atomicity**:  
- "All or nothing" execution.  
- Implemented via undo logs (rollback on failure).  

**Consistency**:  
- Misunderstood: Not an inherent property of transactions.  
- Refers to application-level invariants (e.g., account balances ≥ 0).  
- Transactions help preserve consistency but don’t guarantee it.  

**Isolation**:  
- Illusion of serial execution despite concurrent operations.  
- Weak isolation levels trade off consistency for performance.  

**Durability**:  
- Committed writes survive crashes.  
- Achieved via write-ahead logs (WAL) and flushing to non-volatile storage.  

---

#### **3. Weak Isolation Levels & Concurrency Anomalies**
**Key Isolation Levels**:  
1. **Read Committed**:  
   - Prevents dirty reads (reading uncommitted data).  
   - Prevents dirty writes (overwriting uncommitted data).  
   - Implementation: Row-level locks for writes; no locks for reads (uses committed snapshots).  

2. **Snapshot Isolation (Repeatable Read)**:  
   - Transactions read from a consistent snapshot.  
   - Prevents non-repeatable reads but not phantom reads.  
   - **Write Skew**: A concurrency anomaly where two transactions commit based on stale snapshots, violating invariants (e.g., double-booking a room).  

3. **Serializable Isolation**:  
   - True serial execution or equivalent.  
   - Prevents all anomalies but imposes high performance costs.  

**Common Anomalies**:  
- **Dirty Read**: Reading uncommitted data (prevented by Read Committed).  
- **Non-Repeatable Read**: A row’s value changes between two reads in the same transaction (prevented by Snapshot Isolation).  
- **Phantom Read**: New rows appear in subsequent reads (requires range locks/SERIALIZABLE).  
- **Lost Update**: Two concurrent writes overwrite each other (prevented by locks or CAS).  
- **Write Skew**: Transactions commit based on inconsistent snapshots (requires SERIALIZABLE or explicit locking).  

---

#### **4. Implementing Isolation Levels**
**Pessimistic Concurrency Control**:  
- **Two-Phase Locking (2PL)**:  
  - **Growing Phase**: Acquire locks (shared/exclusive).  
  - **Shrinking Phase**: Release locks after transaction completes.  
  - Downsides: Deadlocks, low concurrency.  

**Optimistic Concurrency Control**:  
- **Multi-Version Concurrency Control (MVCC)**:  
  - Maintain multiple versions of data.  
  - Reads access snapshots; writes create new versions.  
  - Used in Snapshot Isolation (e.g., PostgreSQL, MySQL InnoDB).  

**Serializable Snapshot Isolation (SSI)**:  
- Combines MVCC with detection of write skew.  
- Tracks dependencies between transactions.  
- Aborts transactions causing serialization conflicts.  
- Lower overhead than 2PL but higher than weak isolation.  

---

#### **5. Handling Write Skew & Phantoms**
**Write Skew Example**:  
- Two doctors on call: Both see "on-call" status and concurrently take leave, leaving no coverage.  
- Solution: SERIALIZABLE isolation or explicit locks (SELECT FOR UPDATE).  

**Phantom Reads**:  
- Example: Querying available rooms returns 10, but a concurrent insert adds an 11th.  
- Prevention:  
  - Range locks (e.g., SELECT ... FOR UPDATE).  
  - Materializing conflicts (e.g., creating "lock" rows to enforce constraints).  

---

#### **6. Serializable Isolation Strategies**
**Actual Serial Execution**:  
- Single-threaded execution (e.g., Redis, VoltDB).  
- Pros: Simple, no concurrency issues.  
- Cons: Limited scalability; requires stored procedures.  

**Two-Phase Locking (2PL)**:  
- Strict locking for conflict prevention.  
- Downsides: Deadlocks, poor throughput under contention.  

**Serializable Snapshot Isolation (SSI)**:  
- Optimistic approach: Allow concurrent execution but detect conflicts.  
- Tracks read/write sets and aborts conflicting transactions.  
- Used in PostgreSQL 9.1+, CockroachDB.  

---

#### **7. Trade-offs & Practical Considerations**
- **Performance vs. Correctness**:  
  Weaker isolation (e.g., Read Committed) offers higher concurrency but risks anomalies.  
  Serializable ensures correctness but may require application redesign.  

- **Application-Level Checks**:  
  Even with SERIALIZABLE, some invariants (e.g., uniqueness) need database constraints.  

- **Database Support**:  
  Not all databases implement isolation levels as per the SQL standard (e.g., Oracle’s "serializable" is snapshot isolation).  

---

#### **8. Summary of Key Points**
1. **ACID ≠ Absolute Guarantees**: Durability and atomicity are clear; consistency and isolation depend on levels.  
2. **Weak Isolation Dominates**: Most systems use Read Committed/Snapshot Isolation for performance.  
3. **Write Skew is Subtle**: Requires SERIALIZABLE or explicit locks even under Snapshot Isolation.  
4. **SSI Balances Safety & Speed**: Emerging as a viable alternative to 2PL for serializability.  

---

**Further Study**:  
- Experiment with isolation levels in your database (e.g., `SET TRANSACTION ISOLATION LEVEL`).  
- Analyze real-world scenarios (e.g., banking, inventory) to identify required isolation levels.  
- Explore database-specific behaviors (e.g., PostgreSQL’s SSI vs. MySQL’s gap locks).

### Chapter 7 (Transactions) Hard Difficulty Multiple-Choice Questions

---

**Question 1: ACID Properties**  
Which of the following statements about ACID properties are true? (Select two)  
A. Atomicity ensures that transactions are executed as a single indivisible operation.  
B. Consistency guarantees that concurrent transactions do not interfere with each other.  
C. Isolation ensures the database remains in a valid state after a transaction.  
D. Durability ensures committed transactions survive system crashes.  

---

**Question 2: Isolation Levels**  
Which isolation levels prevent **dirty reads**? (Select two)  
A. Read Uncommitted  
B. Read Committed  
C. Repeatable Read  
D. Serializable  

---

**Question 3: Lost Update Prevention**  
Which methods effectively prevent **lost updates**? (Select two)  
A. Using atomic operations (e.g., `UPDATE table SET value = value + 1`).  
B. Enabling Snapshot Isolation.  
C. Explicit row-level locking (e.g., `SELECT FOR UPDATE`).  
D. Using Read Committed isolation.  

---

**Question 4: Write Skew**  
Which scenarios are examples of **write skew**? (Select two)  
A. Two transactions concurrently incrementing the same bank account balance.  
B. Two doctors marking themselves as "on call" when the other is off-duty, violating a constraint.  
C. Two transactions reading overlapping ranges and inserting new rows, causing phantom reads.  
D. Two transactions updating different rows based on a condition that becomes invalid after both commit.  

---

**Question 5: Serializable Snapshot Isolation (SSI)**  
How does SSI detect conflicts? (Select two)  
A. By tracking read and write sets and checking for overlaps.  
B. By using two-phase locking (2PL) to enforce serial execution.  
C. By aborting transactions that read stale versions of data.  
D. By monitoring for "dangerous structures" in the transaction dependency graph.  

---

**Question 6: Phantom Reads**  
Which isolation levels prevent **phantom reads**? (Select two)  
A. Read Committed  
B. Repeatable Read  
C. Snapshot Isolation  
D. Serializable  

---

**Question 7: Two-Phase Locking (2PL)**  
What are drawbacks of 2PL? (Select two)  
A. Risk of deadlocks requiring detection and resolution.  
B. Limited concurrency due to long-held locks.  
C. Inability to prevent write skew anomalies.  
D. No support for atomic multi-object transactions.  

---

**Question 8: Snapshot Isolation**  
Which anomalies are **not prevented** by Snapshot Isolation? (Select two)  
A. Dirty reads  
B. Lost updates  
C. Write skew  
D. Phantom reads  

---

**Question 9: Multi-Version Concurrency Control (MVCC)**  
Which statements about MVCC are true? (Select two)  
A. Writers never block readers.  
B. Each transaction sees a consistent snapshot of the database at the start time.  
C. It requires explicit row-level locking to avoid conflicts.  
D. It is incompatible with Serializable isolation.  

---

**Question 10: Concurrency Control Trade-offs**  
Which techniques allow **high concurrency** but may sacrifice strict isolation? (Select two)  
A. Two-Phase Locking (2PL)  
B. Serializable Snapshot Isolation (SSI)  
C. Optimistic Concurrency Control (OCC)  
D. Snapshot Isolation (SI)  

---

### **Answers & Explanations**

---

**Question 1**  
**Correct Answers**: A, D  
- **A**: Atomicity ensures all operations in a transaction succeed or none do.  
- **D**: Durability ensures committed data persists after crashes.  
- **B/C Incorrect**: Consistency refers to data validity (app-level invariants), not concurrency. Isolation handles interference between transactions.  

---

**Question 2**  
**Correct Answers**: B, D  
- **B**: Read Committed prevents dirty reads by reading only committed data.  
- **D**: Serializable also prevents dirty reads.  
- **A Incorrect**: Read Uncommitted allows dirty reads.  
- **C**: Repeatable Read prevents non-repeatable reads but not necessarily dirty reads (though some implementations do).  

---

**Question 3**  
**Correct Answers**: A, C  
- **A**: Atomic operations apply changes without interleaving.  
- **C**: Explicit locking prevents concurrent modifications.  
- **B/D Incorrect**: Snapshot Isolation and Read Committed do not inherently prevent lost updates.  

---

**Question 4**  
**Correct Answers**: B, D  
- **B**: Write skew occurs when transactions make decisions based on stale data, violating constraints (e.g., on-call doctors).  
- **D**: Updating different rows based on outdated conditions is write skew.  
- **A**: Lost update, not write skew.  
- **C**: Phantom reads, not write skew.  

---

**Question 5**  
**Correct Answers**: A, D  
- **A**: SSI tracks read/write sets to detect conflicting access.  
- **D**: Dangerous structures (e.g., cycles in dependency graphs) indicate conflicts.  
- **B/C Incorrect**: 2PL and stale data detection are unrelated to SSI.  

---

**Question 6**  
**Correct Answers**: C, D  
- **C**: Snapshot Isolation prevents phantoms by using a fixed snapshot.  
- **D**: Serializable prevents all anomalies, including phantoms.  
- **A/B Incorrect**: Repeatable Read in some systems prevents phantoms via range locks, but standard SQL Repeatable Read does not.  

---

**Question 7**  
**Correct Answers**: A, B  
- **A**: 2PL risks deadlocks due to locking order.  
- **B**: Long-held locks reduce concurrency.  
- **C**: Write skew is possible under 2PL if locks are not applied correctly.  
- **D**: 2PL supports atomicity but not directly related to this question.  

---

**Question 8**  
**Correct Answers**: B, C  
- **B**: Snapshot Isolation does not prevent lost updates unless additional measures (e.g., explicit locking) are used.  
- **C**: Write skew is possible under Snapshot Isolation.  
- **A/D Incorrect**: Snapshot Isolation prevents dirty reads and phantoms.  

---

**Question 9**  
**Correct Answers**: A, B  
- **A**: MVCC allows readers to access older versions, avoiding writer blocks.  
- **B**: Transactions see a snapshot at their start time.  
- **C/D Incorrect**: MVCC does not require explicit locking and works with Serializable (via SSI).  

---

**Question 10**  
**Correct Answers**: C, D  
- **C**: Optimistic Concurrency Control allows high concurrency but may abort transactions.  
- **D**: Snapshot Isolation allows high concurrency but permits write skew.  
- **A/B Incorrect**: 2PL and SSI enforce stricter isolation, reducing concurrency.  

--- 

These questions test deep understanding of transaction mechanics, isolation levels, and trade-offs in concurrency control mechanisms.