### 《高性能MySQL》第1章 MySQL架构

---

#### **第一章核心知识点总结**
1. **MySQL逻辑架构**  
   - 分层结构：连接层、服务层、存储引擎层  
   - 核心组件：查询解析、优化器、存储引擎API  
   - 存储引擎差异（如InnoDB vs MyISAM）

2. **事务与锁机制**  
   - ACID特性与隔离级别  
   - 锁类型：表锁、行锁、间隙锁（Next-Key Locking）  
   - MVCC（多版本并发控制）实现原理

3. **复制与高可用**  
   - 异步复制、半同步复制、全同步复制  
   - GTID（全局事务标识符）与Binlog日志格式  
   - 主从延迟与故障切换策略

4. **数据文件与存储引擎**  
   - InnoDB表空间（System Tablespace、File-Per-Table）  
   - Redo Log与Undo Log的作用  
   - JSON数据类型的原生支持

5. **性能与扩展性**  
   - 线程池（Thread Pool）与连接管理  
   - 元数据锁（Metadata Locks）与隐式提交  
   - 原子DDL（Atomic DDL）特性

---

#### **多选题**

---

**1. 关于MySQL事务隔离级别，以下哪些描述正确？**  
A. `READ UNCOMMITTED`允许脏读  
B. `REPEATABLE READ`通过间隙锁防止幻读  
C. `SERIALIZABLE`通过锁住所有读取行实现可串行化  
D. `READ COMMITTED`在InnoDB中通过MVCC解决不可重复读  

---

**2. InnoDB存储引擎的锁机制包含哪些特性？**  
A. 行级锁（Row-Level Locking）  
B. 表级锁（Table-Level Locking）  
C. 间隙锁（Gap Locking）  
D. 自适应哈希索引锁（Adaptive Hash Locking）  

---

**3. MySQL复制中，以下哪些情况会导致主从数据不一致？**  
A. 主库执行非事务性表（如MyISAM）的写操作  
B. 从库未启用`read_only`模式  
C. 使用`STATEMENT`格式Binlog且存在UDF  
D. 主从服务器系统时间不一致  

---

**4. 关于InnoDB的MVCC，以下哪些说法正确？**  
A. 每个事务可见的版本由Read View决定  
B. 删除操作通过标记删除而非物理删除  
C. 更新操作会生成新版本记录  
D. MVCC仅适用于`SELECT`查询  

---

**5. MySQL中哪些操作会导致隐式提交？**  
A. 执行`ALTER TABLE`  
B. 启动事务（`START TRANSACTION`）  
C. 执行`LOCK TABLES`  
D. 设置`SET AUTOCOMMIT=0`  

---

**6. InnoDB的Redo Log和Undo Log分别用于什么场景？**  
A. Redo Log用于崩溃恢复  
B. Undo Log用于事务回滚  
C. Redo Log用于事务持久化  
D. Undo Log用于MVCC版本管理  

---

**7. 关于MySQL的复制拓扑，以下哪些设计不合理？**  
A. 主库（Master）与从库（Slave）使用不同Binlog格式  
B. 主库配置`log_slave_updates`以支持链式复制  
C. 从库开启`read_only`但允许SUPER权限用户写入  
D. 使用多源复制（Multi-Source Replication）合并多个数据源  

---

**8. InnoDB的原子DDL特性包含哪些？**  
A. DDL操作自动提交  
B. DDL操作支持回滚  
C. DDL操作生成Binlog事件  
D. DDL操作期间禁止并发DML  

---

**9. 关于MySQL的JSON数据类型，以下哪些操作高效？**  
A. 直接查询JSON字段的路径（如`col->"$.key"`）  
B. 为JSON字段创建全文索引  
C. 提取JSON字段值创建虚拟列并索引  
D. 存储大体积JSON文档（>1MB）  

---

**10. MySQL线程池（Thread Pool）的作用包括哪些？**  
A. 减少线程创建/销毁的开销  
B. 控制并发连接数  
C. 避免锁竞争  
D. 自动优化慢查询  

---

#### 多选题答案
1. **答案：A、B、C**  
**详解：**  
- `READ UNCOMMITTED`允许事务读取未提交数据（脏读）。  
- `REPEATABLE READ`通过间隙锁（Gap Lock）阻止幻读，但需结合MVCC实现。  
- `SERIALIZABLE`对所有读取行加锁，强制串行执行。  
- `READ COMMITTED`在InnoDB中通过快照读（Snapshot Read）解决不可重复读，而非MVCC（MVCC仅在REPEATABLE READ及以上生效）。

2. **答案：A、B、C**  
**详解：**  
- InnoDB支持行级锁（通过索引实现）和表级锁（如`LOCK TABLES`）。  
- 间隙锁用于防止幻读，适用于`REPEATABLE READ`隔离级别。  
- 自适应哈希索引是优化查询的辅助结构，非锁机制。

3. **答案：A、B、C**  
**详解：**  
- 非事务性表的写操作无法回滚，导致主从不一致。  
- 从库未启用`read_only`可能导致应用直接写入，破坏复制。  
- `STATEMENT`格式Binlog在UDF（用户自定义函数）中可能因执行环境差异导致不一致。  
- 系统时间不一致不影响数据逻辑一致性，但可能影响Binlog时间戳。

4. **答案：A、B、C**  
**详解：**  
- MVCC通过Read View控制事务可见性，确保隔离性。  
- 删除操作标记为删除（`deleted_flag`），实际清理由Purge线程完成。  
- 更新操作生成新版本记录，旧版本保留在Undo Log中。  
- MVCC也影响`INSERT`和`UPDATE`的并发控制。

5.**答案：A、B、C**  
**详解：**  
- DDL操作（如`ALTER TABLE`）隐式提交当前事务。  
- `START TRANSACTION`隐式提交未提交的事务。  
- `LOCK TABLES`隐式提交并锁定表。  
- `SET AUTOCOMMIT=0`仅关闭自动提交，不提交事务。

6. **答案：A、B、C、D**  
**详解：**  
- Redo Log记录数据页变更，用于崩溃恢复（Write-Ahead Logging）。  
- Undo Log保存旧版本数据，用于事务回滚和MVCC。  
- Redo Log确保事务持久化，Undo Log支持一致性读。

7. **答案：A、C**  
**详解：**  
- 主从Binlog格式需一致（如ROW或STATEMENT），否则复制可能失败。  
- 从库开启`read_only`后，SUPER权限用户仍可写入，需额外限制。  
- 链式复制（B）和多源复制（D）是合理设计。

8. **答案：B、C**  
**详解：**  
- 原子DDL通过Undo Log实现回滚（如中途失败可恢复）。  
- DDL操作生成Binlog事件，确保主从一致性。  
- DDL自动提交（A）正确，但非原子性特性；DDL期间允许并发DML（D）错误。

9. **答案：C**  
**详解：**  
- 直接查询JSON路径（A）效率较低，需遍历文档。  
- MySQL不支持JSON字段的全文索引（B）。  
- 虚拟列索引（C）可将JSON字段高效索引化。  
- 大体积JSON（D）会占用过多存储，影响性能。

10. **答案：A、B**  
**详解：**  
- 线程池复用线程，减少创建/销毁开销（A）。  
- 通过队列管理连接，控制并发数（B）。  
- 线程池不直接解决锁竞争（C）或优化慢查询（D）。

#### **答案与详解总结**
1. **事务隔离级别**：AC（理解不同隔离级别的锁机制与实现原理）  
2. **InnoDB锁机制**：ABC（区分行锁、表锁、间隙锁的作用场景）  
3. **复制一致性**：ABC（关注Binlog格式与数据类型兼容性）  
4. **MVCC核心**：ABC（掌握Read View与Undo Log的协作机制）  
5. **隐式提交**：ABC（区分DDL与事务控制语句的影响）  
6. **Redo/Undo Log**：ABCD（理解WAL机制与版本管理的关系）  
7. **复制拓扑**：AC（避免主从配置不一致与权限漏洞）  
8. **原子DDL**：BC（利用Undo Log实现回滚与Binlog持久化）  
9. **JSON优化**：C（虚拟列索引是高效查询的关键）  
10. **线程池**：AB（平衡连接效率与资源管理）  

---

**学习建议**：结合MySQL官方文档与Percona Toolkit工具验证配置，通过Percona XtraBackup实践原子DDL与备份恢复流程。

---
### 《高性能MySQL》第2章 可靠性程世界中的监控

#### 核心知识点
1. 服务级别指标（SLI）与服务级别目标（SLO）的定义与区别
2. 客户体验监控的三个核心维度（可用性、延迟、错误）
3. 主动监控与被动监控的差异
4. 复制延迟对业务的影响及监控策略
5. 长期性能度量与业务节奏的关系
6. 连接风暴与线程管理的监控要点
7. 磁盘I/O性能的关键指标
8. 错误类型的优先级分类
9. 内存使用率的预测性监控
10. 备份恢复时间的容量规划

---

#### 多选题

1. 关于服务级别指标（SLI）和服务级别目标（SLO），哪些说法正确？
A. SLI是衡量客户是否满意的具体指标  
B. SLO必须包含具体时间范围和数值目标  
C. SLA是业务部门与客户签订的包含SLO的协议  
D. 99.9%的可用性意味着每年允许8小时停机  

2. 以下哪些属于主动监控的关键指标？（多选）
A. 磁盘空间使用率增长率  
B. 当前活跃数据库连接数  
C. 线程池中等待的查询数量  
D. 复制延迟趋势分析  

3. 关于复制延迟监控，正确的描述是？
A. 只影响从副本读取的业务场景  
B. 是判断是否需要分片的重要前兆  
C. 主节点写入性能不足的直接表现  
D. 必须立即触发随叫随到告警  

4. 以下哪些错误类型需要立即处理？（多选）
A. "Too many connections"  
B. "Aborted connections"突然激增  
C. "Lock wait timeout"偶发出现  
D. 副本节点处于只读模式  

5. 关于长期性能度量，正确的策略包括：
A. 使用平均值评估季度趋势  
B. 关注95%分位的查询延迟  
C. 分析业务高峰期流量模式  
D. 完全依赖工具自动优化  

6. 内存监控的要点包含：
A. 监控memory_summary_by_thread_by_event_name  
B. 通过swap分区使用率判断内存压力  
C. 使用thread_cache_size限制内存泄漏  
D. 关注memory_global_total的波动  

7. 关于连接风暴的特征，正确的描述是：
A. threads_connected与max_connections比值突增  
B. 通常伴随CPU利用率骤降  
C. pt-kill工具可快速缓解  
D. 必须立即重启MySQL实例  

8. 磁盘I/O性能的关键指标包括：
A. iostat输出的%util列  
B. vmstat的si/so列  
C. iowait时间占比超过30%  
D. RAID控制器的缓存命中率  

9. 关于错误监控的陷阱，正确的有：
A. 所有ER_DEPRECATED错误都应立即修复  
B. "Got error 28 from storage engine"需优先处理  
C. 客户端重试可解决所有"Lost connection"错误  
D. 监控Connection_errors_peer_reset指标  

10.  备份恢复时间规划需要考虑：
A. 数据集的功能分片可行性  
B. 冷备份的存储介质吞吐量  
C. binlog日志的保留周期  
D. 云存储API的调用延迟  

---

#### 答案及解析

1. 正确答案：A,B,C  
解析：  
- D错误，99.9%可用性对应年停机时间为8.76小时（计算方式：365×24×0.1%）。  
- SLO必须明确时间范围和数值（如"月度99.5%请求延迟<200ms"），SLA是包含SLO的正式协议。

2. 正确答案：A,D  
解析：  
- B和C属于被动监控当前状态，A（空间增长预测）和D（长期趋势）属于主动预防。

3. 正确答案：B  
解析：  
- A错误，主从数据不一致可能影响写后读一致性；C错误可能由网络问题导致；D需根据业务影响决定告警级别。

4. 正确答案：A,D  
解析：  
- B可能是网络问题，C可能暂时可容忍，D表明拓扑异常需要立即处理。

5. 正确答案：B,C  
解析：  
- A平均值可能掩盖异常值，D自动优化无法替代人工分析业务模式。

6. 正确答案：A,D  
解析：  
- B swap使用反映OS内存管理，C thread_cache_size控制线程复用。

7. 正确答案：A,C  
解析：  
- B错误风暴伴随CPU飙升，D重启是最后手段。

8. 正确答案：A,C  
解析：  
- B监控swap，D RAID缓存需结合BBU状态判断。

9. 正确答案：B,D  
解析：  
- A旧版本兼容错误可能暂缓，C网络故障需基础设施排查。

10. 正确答案：A,B,D  
解析：  
- C影响时间点恢复，与全量恢复无关。

---

#### 重点巩固方向

1. **SLI/SLO设计**：掌握3个9/4个9的可用性计算（年停机时间=8760×(1-可用性百分比)）
2. **监控分层**：区分被动监控（Threads_running）和主动监控（磁盘增长率）
3. **故障树分析**：例如Aborted connections激增可能指向网络层问题而非DB本身
4. **容量预测**：通过`(当前数据量 × 日增量 × 冗余系数) / 磁盘写入速度`计算备份窗口
5. **指标关联性**：iowait高+%util>70%可能预示HDD阵列性能瓶颈，SSD则需要看queue_depth

### 《高性能MySQL》第3章 Performance Schema

---

#### **第三章核心知识点总结**
1. **Performance Schema架构**  
   - 插桩（Instrument）与消费者表（Consumer）的核心作用  
   - 线程（Thread）与Actor的管理机制  
   - 内存存储与动态配置特性  

2. **关键组件与功能**  
   - **Events_* 表**：记录等待、语句、阶段、事务等事件  
   - **Summary表**：聚合分析（如内存使用、锁争用）  
   - **Metadata Locks监控**：解决DDL阻塞问题  

3. **高级用法**  
   - 与sys Schema的联动（如`sys.ps_check_lost_instrumentation`）  
   - 跟踪内存分配与变量状态  
   - 定位慢查询与锁冲突  

4. **性能与限制**  
   - 插桩对CPU的额外开销  
   - 内存表自动扩容与数据保留策略  
   - 不支持某些存储引擎（如MyISAM）的监控  

---

#### **多选题**

---

**1. 关于Performance Schema的插桩（Instrument），哪些说法正确？**  
A. 插桩粒度可细化到SQL语句、锁等待、内存分配等  
B. 默认启用所有插桩以覆盖所有监控场景  
C. 可通过`setup_instruments`表动态启用/禁用特定插桩  
D. 插桩启用后，数据会永久存储在磁盘上  

---

**2. 如何通过Performance Schema诊断锁争用问题？**  
A. 查看`metadata_locks`表中的锁等待事件  
B. 分析`events_statements_summary_by_digest`中的慢查询  
C. 检查`innodb_lock_wait_timeout`变量值  
D. 监控`events_waits_current`表中的`wait/io/lock`事件  

---

**3. Performance Schema的内存管理特性包括哪些？**  
A. 自动调整消费者表大小（如`events_statements_history`）  
B. 通过`performance_schema_max_*`变量限制总内存占用  
C. 支持将数据持久化到磁盘避免重启丢失  
D. 内存分配粒度可细化到单个线程  

---

**4. 如何通过Performance Schema检测内存泄漏？**  
A. 监控`memory_summary_by_thread_by_event_name`表的`CURRENT_ALLOC`列  
B. 检查`SHOW GLOBAL STATUS LIKE 'Memory_used'`的变化趋势  
C. 启用`performance_schema.memory`插桩并分析`memory_events`表  
D. 通过`information_schema.TABLES`查看`DATA_FREE`值  

---

**5. Performance Schema中哪些表可用于分析慢查询？**  
A. `events_statements_history_long`  
B. `statements_with_runtimes_in_95th_percentile`（sys Schema视图）  
C. `events_waits_current`  
D. `file_summary_by_instance`  

---

**6. 如何通过Performance Schema定位高CPU消耗的查询？**  
A. 分析`events_statements_summary_by_digest`中的`SUM_TIMER_WAIT`  
B. 检查`threads`表中`PROCESSLIST_ID`对应的CPU使用率  
C. 监控`sys.processlist`中的`Time`列  
D. 查看`events_stages_history`中的`STAGE_DURATION`  

---

**7. Performance Schema中哪些配置会影响内存使用？**  
A. `performance_schema_events_statements_history_size`  
B. `performance_schema_max_table_instances`  
C. `innodb_buffer_pool_size`  
D. `max_connections`  

---

**8. 如何通过Performance Schema检测未提交的事务？**  
A. 查询`data_locks`表中`LOCK_STATUS = 'GRANTED'`的记录  
B. 分析`events_transactions_current`表中的`STATE`列  
C. 检查`INNODB_TRX`表（InnoDB引擎）  
D. 监控`performance_schema.metadata_locks`表  

---

**9. Performance Schema中哪些功能需要额外配置才能生效？**  
A. 跟踪存储过程执行  
B. 记录JSON操作  
C. 监控临时表使用  
D. 自动检测死锁  

---

**10. 如何通过Performance Schema优化慢查询的索引使用？**  
A. 分析`events_statements_summary_by_digest`中的`FULL_SCAN`次数  
B. 检查`filesort_summary_by_index_usage`表的`INDEX_NAME`列  
C. 监控`events_waits_history`中的`TABLE_IO_WAITS`事件  
D. 使用`sys.schema_unused_indexes`视图  

#### 答案

1. **答案：A, C**  
**详解：**  
- **A**：正确。Performance Schema支持细粒度插桩，例如`statement/sql/select`（SQL语句）、`wait/io/file/sql/binlog`（Binlog写入）等。  
- **C**：正确。可通过`UPDATE setup_instruments SET ENABLED = 'YES'`动态启用插桩，无需重启MySQL。  
- **B**：错误。默认仅启用部分核心插桩（如`statement`、`wait`），需手动配置其他插桩。  
- **D**：错误。Performance Schema数据存储在内存中，重启后数据丢失。

2. **答案：A, D**  
**详解：**  
- **A**：正确。`metadata_locks`表记录元数据锁的持有者与等待者，可定位DDL阻塞问题。  
- **D**：正确。`events_waits_current`表中`OBJECT_TYPE = 'LOCK'`的记录可直接反映锁竞争。  
- **B**：错误。`events_statements_summary_by_digest`用于分析慢查询，但无法直接定位锁争用。  
- **C**：错误。`innodb_lock_wait_timeout`是InnoDB事务锁的超时参数，与Performance Schema无关。

3. **答案：A, B, D**  
**详解：**  
- **A**：正确。例如`events_statements_history`表默认保留每线程最近10条事件，自动扩容。  
- **B**：正确。可通过`performance_schema_max_table_instances`等变量限制表实例数量。  
- **C**：错误。Performance Schema数据不持久化，重启后清空。  
- **D**：正确。每个线程的内存数据独立存储，避免跨线程干扰。

4. **答案：A, C**  
**详解：**  
- **A**：正确。`memory_summary_by_thread_by_event_name`可按线程和事件类型统计内存分配。  
- **C**：正确。启用`memory`插桩后，`memory_events`表记录内存分配与释放事件。  
- **B**：错误。`SHOW GLOBAL STATUS`中的内存指标不精确，且不关联具体事件。  
- **D**：错误。`DATA_FREE`反映表碎片，与内存无关。

5. **答案：A, B**  
**详解：**  
- **A**：正确。`events_statements_history_long`存储所有线程的慢查询事件（默认保留10,000条）。  
- **B**：正确。sys Schema的`statements_with_runtimes_in_95th_percentile`视图直接展示慢查询。  
- **C**：错误。`events_waits_current`记录等待事件（如锁、I/O），非直接查询性能。  
- **D**：错误。`file_summary_by_instance`分析文件I/O性能，与查询无关。

6. **答案：A, B**  
**详解：**  
- **A**：正确。`events_statements_summary_by_digest`的`SUM_TIMER_WAIT`字段可量化查询耗时。  
- **B**：正确。通过`threads`表关联`PROCESSLIST_ID`，可定位高CPU线程的执行阶段。  
- **C**：错误。`sys.processlist`依赖Performance Schema数据，但无法直接关联CPU消耗。  
- **D**：错误。`events_stages_history`记录查询阶段（如排序、临时表），非直接CPU指标。

7. **答案：A, B**  
**详解：**  
- **A**：正确。控制每个线程保存的历史事件数量，值越大内存占用越高。  
- **B**：正确。限制Performance Schema表的最大实例数，防止内存耗尽。  
- **C**：错误。InnoDB缓冲池与Performance Schema无关。  
- **D**：错误。`max_connections`影响线程池资源，但非直接内存配置。

8. **答案：B, C**  
**详解：**  
- **B**：正确。`events_transactions_current`中`STATE = 'ACTIVE'`表示未提交事务。  
- **C**：正确。`INNODB_TRX`表（需启用InnoDB监控）直接显示未提交事务详情。  
- **A**：错误。`data_locks`表记录锁冲突，无法直接关联事务状态。  
- **D**：错误。`metadata_locks`表监控元数据锁，非事务锁。

9. **答案：A, B**  
**详解：**  
- **A**：正确。需启用`statement/procedure/*`插桩才能跟踪存储过程。  
- **B**：正确。需启用`wait/lock/json/*`插桩才能监控JSON操作。  
- **C**：错误。临时表监控默认启用，无需额外配置。  
- **D**：错误。死锁检测由InnoDB自动完成，与Performance Schema无关。

10. **答案：A, B, D**  
**详解：**  
- **A**：正确。`FULL_SCAN`次数高表明需添加索引避免全表扫描。  
- **B**：正确。`filesort_summary_by_index_usage`显示排序操作是否利用索引。  
- **D**：正确。`schema_unused_indexes`列出未使用的索引，可优化冗余索引。  
- **C**：错误。`TABLE_IO_WAITS`反映I/O等待，非直接索引优化依据。

---

#### **答案与详解总结**
1. **插桩与消费者表**：动态配置、细粒度监控  
2. **锁争用诊断**：元数据锁表、等待事件表  
3. **内存管理**：自动扩容、线程隔离、配置限制  
4. **慢查询分析**：摘要表、sys视图、执行阶段跟踪  
5. **索引优化**：全表扫描检测、排序索引利用率、冗余索引识别  


---

通过以上题目，可深入理解Performance Schema的核心机制与实战应用。

### 《高性能MySQL》第4章 操作系统和硬件优化

第四章聚焦于**操作系统和硬件优化**，核心是通过底层系统调优提升MySQL性能。以下是重难点提炼及10道高难度多选题：

---

#### **重难点解析**
1. **硬件选型**  
   - CPU：多核高频CPU更适合高并发场景，NUMA架构需合理配置内存分配策略  
   - 存储：SSD（NVMe）是主流选择，HDD仅适用于冷数据或超大容量场景  
   - 网络：万兆网卡+RDMA技术可降低延迟  

2. **文件系统优化**  
   - **XFS**：支持大文件、高吞吐，推荐用于MySQL数据目录（`innodb_file_per_table`开启时）  
   - **EXT4**：适合小规模数据，但存在元数据锁竞争问题  

3. **RAID配置**  
   - **RAID 10**：读写性能均衡，适合写密集型场景  
   - **RAID 5/6**：存储效率高，但写惩罚显著（需计算奇偶校验）  
   - **RAID控制器缓存**：必须启用BBU电池保护，否则禁用写缓存  

4. **操作系统调优**  
   - **I/O调度器**：数据库推荐`noop`或`deadline`（SSD用`noop`，HDD用`deadline`）  
   - **内存管理**：`vm.swappiness=0`禁用交换分区，避免性能抖动  
   - **大页面（HugePages）**：减少TLB miss，需结合`innodb_large_prefix`使用  

5. **性能监控工具**  
   - `iostat -x 1`：监控磁盘IOPS、吞吐量和饱和度  
   - `pidstat -d 1`：跟踪进程级磁盘I/O行为  

---

#### **多选题**

---

**1. 关于RAID配置的选择，以下哪些说法正确？**  
A. RAID 5适合写密集型业务  
B. RAID 10的重建速度比RAID 5快  
C. RAID 0提供冗余能力  
D. RAID控制器缓存需搭配电池保护  

---

**2. MySQL数据目录挂载XFS文件系统的优势包括？**  
A. 支持大文件（>16TB）  
B. 减少元数据锁竞争  
C. 自动优化SSD磨损均衡  
D. 兼容EXT4工具链  

---

**3. 以下I/O调度器配置适用于SSD的是？**  
A. `noop`  
B. `deadline`  
C. `cfq`  
D. `kyber`  

---

**4. 关于内存管理参数`vm.swappiness`，正确的说法是？**  
A. 值越大越倾向于使用物理内存  
B. 设置为0可完全禁用交换分区  
C. MySQL建议设置为10-20  
D. 默认值60适合数据库服务器  

---

**5. 以下哪些硬件特性对MySQL性能影响显著？**  
A. CPU多核与超线程  
B. 内存通道数  
C. 磁盘转速（RPM）  
D. 网卡多队列  

---

**6. NUMA架构下MySQL优化的关键点包括？**  
A. 绑定MySQL进程到单一NUMA节点  
B. 开启`innodb_numa_interleave`  
C. 禁用透明大页面（Transparent Huge Pages）  
D. 增加`innodb_buffer_pool_instances`  

---

**7. RAID缓存策略中，以下哪些描述正确？**  
A. `write-back`模式需电池保护  
B. `write-through`模式无写惩罚  
C. RAID 5的`write-back`性能优于`write-through`  
D. 控制器缓存大小应设置为内存的10%-20%  

---

**8. 监控磁盘I/O性能时，需关注哪些指标？**  
A. `iops`  
B. `await`（平均等待时间）  
C. `%util`（利用率）  
D. `svctm`（服务时间）  

---

**9. 以下关于大页面（HugePages）的配置，正确的是？**  
A. 需在BIOS中启用内存页大小设置  
B. MySQL参数`innodb_large_pages=ON`  
C. 大页面大小通常为2MB或1GB  
D. 与`innodb_buffer_pool_size`无关  

---

**10. 优化操作系统TCP/IP栈时，哪些参数需调整？**  
A. `net.core.somaxconn`  
B. `tcp_tw_reuse`  
C. `tcp_max_syn_backlog`  
D. `fs.file-max`  

---

#### 答案
1. **答案：B, D**  
**解析**：  
- RAID 5的写惩罚高（需计算奇偶校验），仅适合读密集型场景（A错）  
- RAID 10重建仅需复制存活磁盘数据，RAID 5需重建校验块（B对）  
- RAID 0无冗余（C错）  
- RAID控制器缓存需电池保护防止断电数据丢失（D对）  

2. **答案：A, B, D**  
**解析**：  
- XFS支持最大16EB文件（A对）  
- XFS采用分配组设计，减少元数据锁竞争（B对）  
- SSD磨损均衡需通过`discard`或`fstrim`手动触发（C错）  
- XFS兼容EXT4工具（如`mkfs.xfs`与`mkfs.ext4`语法相似）（D对）  

3. **答案：A, D**  
**解析**：  
- `noop`直接将I/O请求传递给驱动，适合SSD（A对）  
- `kyber`是Linux 5.0+的SSD优化调度器（D对）  
- `deadline`适合HDD（B错）  
- `cfq`已废弃（C错）  

4. **答案：C**  
**解析**：  
- `swappiness=0`表示尽可能不交换（非完全禁用）（B错）  
- MySQL推荐`swappiness=1`（D错），但部分场景可设为10-20（C对）  
- 值越大越倾向交换（A错）  

5. **答案：A, B, C, D**  
**解析**：  
- 多核CPU提升并行处理能力（A对）  
- 内存通道数影响带宽（如DDR4 4通道）（B对）  
- 高转速HDD（15k RPM）降低随机I/O延迟（C对）  
- 网卡多队列（RSS）提升网络吞吐（D对）  

6. **答案：A, B, C**  
**解析**：  
- NUMA绑核避免跨节点内存访问延迟（A对）  
- `innodb_numa_interleave`强制内存交错分配（B对）  
- THP（Transparent Huge Pages）会导致内存碎片（C对）  
- `innodb_buffer_pool_instances`与NUMA无关（D错）  

7. **答案：A, C**  
**解析**：  
- `write-back`依赖缓存加速，需电池防断电（A对）  
- `write-through`无缓存，写性能低（B错）  
- RAID 5的`write-back`通过缓存合并写操作提升性能（C对）  
- 缓存大小通常为硬盘容量的1%-2%（D错）  

8. **答案：A, B, C, D**  
**解析**：  
- `iops`反映I/O吞吐能力  
- `await`体现响应延迟  
- `%util`接近100%表示饱和  
- `svctm`显示单次I/O处理时间  

9. **答案：A, B, C**  
**解析**：  
- BIOS需支持并启用大页面（A对）  
- MySQL需显式开启`innodb_large_pages`（B对）  
- 大页面常见大小为2MB（C对）  
- `innodb_buffer_pool_size`需大于大页面最小值（D错）  

10. **答案：A, B, C, D**  
**解析**：  
- `somaxconn`限制监听队列长度（A对）  
- `tcp_tw_reuse`允许复用TIME_WAIT连接（B对）  
- `tcp_max_syn_backlog`增大SYN队列（C对）  
- `fs.file-max`提升文件描述符上限（D对）  

#### **答案与详解总结**
1. **RAID配置**：B, D  
2. **XFS优势**：A, B, D  
3. **SSD调度器**：A, D  
4. **内存管理**：C  
5. **硬件特性**：A, B, C, D  
6. **NUMA优化**：A, B, C  
7. **RAID缓存**：A, C  
8. **I/O监控**：A, B, C, D  
9. **大页面**：A, B, C  
10. **TCP参数**：A, B, C, D  

通过底层系统调优，可显著提升MySQL在高并发、大数据量场景下的稳定性与性能。

---

### 《高性能MySQL》第5章 优化服务器设置

第五章聚焦 **MySQL服务器配置优化**，涵盖从基础参数调优到高级InnoDB设置的全方位策略。核心重难点包括：

1. **内存管理**  
   - InnoDB缓冲池（innodb_buffer_pool_size）的合理分配
   - 线程缓存（thread_cache_size）与连接管理
   - 操作系统缓存与MySQL缓存的协同

2. **I/O优化**  
   - 日志文件大小（innodb_log_file_size）与写入性能
   - 事务日志刷新策略（innodb_flush_log_at_trx_commit）
   - 磁盘调度器与RAID配置

3. **并发控制**  
   - 线程并发度（innodb_thread_concurrency）
   - 锁机制与死锁处理
   - 并行复制（多线程复制）配置

4. **安全与可靠性**  
   - 最大连接数（max_connections）与资源限制
   - 安全参数（max_connect_errors、sql_mode）
   - 备份与恢复策略

5. **高级InnoDB特性**  
   - 自适应哈希索引（innodb_adaptive_hash_index）
   - 严格模式（innodb_strict_mode）
   - 文件格式与页大小配置

---

#### 多选题

1. 关于InnoDB缓冲池（innodb_buffer_pool_size）的配置，以下哪些说法正确？
A. 应设置为服务器物理内存的50%-75%
B. 大小会影响数据页的缓存效率
C. 修改后需重启MySQL生效
D. 可动态调整无需重启
---

2. 以下哪些参数与MySQL事务日志（redo log）相关？
A. innodb_log_file_size
B. innodb_flush_log_at_trx_commit
C. sync_binlog
D. max_connections
---

3. 以下哪些场景适合使用`innodb_file_per_table`？
A. 需要频繁删除大表释放空间
B. 需要跨表空间优化存储
C. 需要减少文件系统碎片
D. 需要提升备份效率
---

4. 关于线程缓存（thread_cache_size），以下哪些描述正确？
A. 默认值通常足够，无需调整
B. 缓存空闲线程以加速新连接
C. 过大可能导致内存浪费
D. 仅适用于短连接场景
---

5. 以下哪些参数会影响MySQL的I/O性能？
A. innodb_flush_method
B. innodb_io_capacity
C. innodb_read_io_threads
D. max_connections
---

6. 以下哪些是MySQL安全加固的推荐配置？
A. 设置`max_connect_errors=1000`
B. 禁用`local_infile`
C. 启用`sql_mode=STRICT_TRANS_TABLES`
D. 设置`skip_networking`
---

7. 关于InnoDB的锁机制，以下哪些说法正确？
A. 行级锁通过索引实现
B. 共享锁（S锁）允许并发读取
C. 意向锁用于表级锁定
D. 死锁检测默认关闭
---

8. 以下哪些配置会影响MySQL的复制延迟？
A. `binlog_group_commit_sync_delay`
B. `innodb_flush_log_at_trx_commit=2`
C. `slave_parallel_workers=4`
D. `max_connections=1000`
---

9. 以下哪些场景适合使用`pt-online-schema-change`工具？
A. 修改大表的索引
B. 添加全文索引
C. 在线变更主键
D. 删除无用列
---

10.  关于MySQL内存分配，以下哪些说法正确？
A. `innodb_buffer_pool_size`应尽可能大
B. `key_buffer_size`对InnoDB无效
C. `sort_buffer_size`建议设置为16MB
D. `read_rnd_buffer_size`影响排序性能
---

#### 答案
1. **答案：A、B、C**

**详解：**
- A：InnoDB缓冲池是MySQL内存占用最高的组件，通常建议占服务器内存的50%-75%（需扣除操作系统和其他应用内存）。
- B：缓冲池缓存数据和索引页，大小直接影响缓存命中率，进而影响查询性能。
- C：缓冲池大小需在MySQL启动时通过配置文件设置，修改后需重启生效。
- D：错误，动态调整需使用`SET GLOBAL`，但重启后仍会失效。

2. **答案：A、B**

**详解：**
- A：`innodb_log_file_size`控制单个日志文件大小，影响写入性能和恢复时间。
- B：`innodb_flush_log_at_trx_commit`决定事务日志的刷新策略（0/1/2），影响数据持久性与性能。
- C：`sync_binlog`控制二进制日志的同步频率，与事务日志无关。
- D：`max_connections`限制最大连接数，与日志无关。

3. **答案：A、B、C**

**详解：**
- A：启用后每个表独立存储，删除表时可直接删除`.ibd`文件，释放空间。
- B：不同表可存储在不同目录或存储设备，优化I/O分布。
- C：减少单个大文件的碎片问题，提升存储效率。
- D：错误，备份效率通常与表空间无关，取决于备份工具。

4. **答案：B、C**

**详解：**
- B：线程缓存保存空闲线程，减少创建新线程的开销，提升连接效率。
- C：若设置过大（如数万），可能占用过多内存，尤其在内存有限的服务器中。
- A：错误，默认值（如80）可能不足，需根据连接负载调整。
- D：错误，长连接同样受益于线程缓存，减少频繁创建线程的开销。

5. **答案：A、B、C**

**详解：**
- A：`innodb_flush_method`控制数据文件与日志的写入方式（如O_DIRECT），影响I/O效率。
- B：`innodb_io_capacity`指定InnoDB的I/O吞吐量上限，需与硬件匹配。
- C：`innodb_read_io_threads`和`innodb_write_io_threads`控制读写线程数，影响并发I/O能力。
- D：`max_connections`限制连接数，与I/O无直接关系。

6. **答案：B、C**

**详解：**
- B：禁用`local_infile`防止通过LOAD DATA INFILE注入攻击。
- C：`STRICT_TRANS_TABLES`模式严格检查数据合法性，避免无效数据写入。
- A：`max_connect_errors`默认100较小，但设置为1000可能掩盖恶意攻击，需谨慎。
- D：`skip_networking`禁用TCP/IP连接，仅允许本地Socket连接，但可能影响远程管理。

7. **答案：A、B、C**

**详解：**
- A：InnoDB行锁通过索引记录实现，无索引可能导致表锁。
- B：共享锁（S锁）允许多个事务读取同一行。
- C：意向锁（如IS锁）用于表级锁定，表示事务即将获取行级锁。
- D：错误，InnoDB默认启用死锁检测，超时后主动回滚其中一个事务。

8. **答案：A、B、C**

**详解：**
- A：`binlog_group_commit_sync_delay`控制组提交的延迟时间，增大可提升吞吐但增加延迟。
- B：`innodb_flush_log_at_trx_commit=2`异步刷盘，提升性能但可能增加复制延迟。
- C：`slave_parallel_workers`启用多线程复制，减少延迟。
- D：`max_connections`限制连接数，与复制延迟无直接关系。


9. **答案：A、B、C、D**

**详解：**
- `pt-online-schema-change`支持在线修改表结构，避免锁表，适用于大表操作。
- 包括添加/删除索引、修改列、变更主键等场景。

10. **答案：B、D**

**详解：**
- B：`key_buffer_size`仅对MyISAM引擎有效，InnoDB使用缓冲池。
- D：`read_rnd_buffer_size`用于随机读取排序，增大可减少排序时间。
- A：错误，缓冲池过大会导致操作系统缓存不足，通常建议不超过总内存的75%。
- C：`sort_buffer_size`建议根据查询负载调整，盲目增大可能导致内存碎片。


---
### 《高性能MySQL》第6章 schema设计与管理


---

#### **第六章核心知识点**

1. **数据类型优化原则**  
   - 选择最小存储空间类型（如`TINYINT`优于`INT`）
   - 避免`NULL`列（影响索引统计和查询优化）
   - 使用整型存储IP地址（`INET_ATON()`/`INET_NTOA()`）
   - 优先使用`ENUM`替代固定值字符串（如状态字段）

2. **范式与反范式设计**  
   - 反范式化适用于读多写少场景（如冗余高频查询字段）
   - 避免过度反范式化（如牺牲写入性能存储大字段）

3. **索引设计**  
   - 覆盖索引（`Using index`）避免回表
   - 前缀索引优化长文本（如`VARCHAR(255)`的前5个字符）
   - 多列索引顺序遵循最左前缀原则
   - 冗余索引检测（如`(a,b)`与`(a)`重复）

4. **Schema管理**  
   - 在线DDL工具（如`pt-online-schema-change`避免锁表）
   - 源码控制与自动化部署（如Git管理DDL变更）
   - 避免DDL高峰期执行（如业务低峰期操作）

5. **特殊场景设计**  
   - JSON类型优化（避免存储大对象，使用虚拟列索引）
   - 分区表设计（按时间或范围分区分摊查询压力）

---

#### **多选题**

---

**1. 关于MySQL数据类型选择，以下哪些说法正确？**  
A. 使用`VARCHAR(255)`作为默认字符串长度  
B. 整数类型应优先选择最小存储空间（如`TINYINT`）  
C. `ENUM`适合存储固定值列表（如性别、状态）  
D. `TEXT`类型适合存储频繁更新的短文本  

---

**2. 以下哪些场景适合反范式设计？**  
A. 用户表存储冗余的`country_name`（避免JOIN）  
B. 订单表存储`product_price`历史值（避免关联价格表）  
C. 日志表存储`user_id`和`username`（减少关联查询）  
D. 商品表存储`category_id`和`category_name`（避免频繁JOIN）  

---

**3. 索引设计中，哪些情况会导致索引失效？**  
A. 索引列使用`LIKE 'abc%'`  
B. 索引列包含`NULL`值  
C. 索引列在查询条件中使用`OR`连接  
D. 索引列是字符串但查询条件未加引号  

---

**4. 关于在线DDL操作，以下哪些描述正确？**  
A. `ALGORITHM=INPLACE`无需重建表  
B. `LOCK=NONE`允许读写操作  
C. 所有DDL操作都支持在线模式  
D. `pt-online-schema-change`通过触发器实现无锁变更  

---

**5. 以下哪些情况适合使用前缀索引？**  
A. `VARCHAR(255)`的URL字段（取前10个字符）  
B. `TEXT`类型字段的全文搜索  
C. 高基数字段（如`email`）的精确查询  
D. 频繁更新的字段（减少索引维护成本）  

---

**6. 关于JSON数据类型，以下哪些说法正确？**  
A. JSON字段适合存储结构化数据（如配置信息）  
B. JSON索引通过虚拟列实现（如`column->"$.key"`）  
C. JSON类型比传统表结构查询更快  
D. 大JSON对象应存储在文件系统（如对象存储）  

---

**7. 以下哪些属于Schema设计陷阱？**  
A. 过度使用`VARCHAR(255)`  
B. 在`WHERE`条件中使用函数（如`WHERE YEAR(date) = 2023`）  
C. 为每个查询单独创建索引  
D. 使用`ENUM`存储动态扩展的枚举值  

---

**8. 关于分区表设计，以下哪些描述正确？**  
A. 范围分区适合按时间归档历史数据  
B. 列表分区适合枚举值分片（如省份）  
C. 分区表自动优化查询性能  
D. 分区键必须包含在主键中  

---

**9. 以下哪些工具可用于在线Schema变更？**  
A. `pt-online-schema-change`  
B. `gh-ost`  
C. `FLUSH TABLES WITH READ LOCK`  
D. `ALTER TABLE ... ALGORITHM=INSTANT`  

---

**10. 关于Schema管理，以下哪些实践是正确的？**  
A. 将Schema变更纳入版本控制（如Git）  
B. 生产环境直接执行未经测试的DDL  
C. 使用`pt-online-schema-change`避免锁表  
D. 所有DDL操作在业务高峰期执行  

---

#### **答案与详解总结**
1. **答案：B, C**  
**解析：**  
- B：更小的整数类型减少存储空间和内存占用（如`TINYINT`仅需1字节）。  
- C：`ENUM`通过整数存储固定值，比字符串更节省空间且查询更快。  
- A：`VARCHAR(255)`默认长度浪费空间，应按实际需求设置。  
- D：`TEXT`类型适合大文本，频繁更新会导致碎片化。

2. **答案：A, B, C, D**  
**解析：**  
反范式化适用于高频读场景，通过冗余字段减少JOIN操作。所有选项均通过冗余提升查询性能，但需权衡写入成本。

3. **答案：B, D**  
**解析：**  
- B：`NULL`值使索引统计不准确，可能导致优化器放弃使用索引。  
- D：字符串未加引号会导致类型隐式转换，使索引失效（如`WHERE id = 123`对字符串`id`无效）。  
- A：`LIKE 'abc%'`可使用前缀索引。  
- C：`OR`条件需所有字段都有索引才会生效，否则可能全表扫描。

4. **答案：A, B, D**  
**解析：**  
- A：`INPLACE`算法直接修改数据文件，无需重建表（如添加列）。  
- B：`LOCK=NONE`允许并发读写（部分操作仍需短暂锁）。  
- C：复杂操作（如修改字符集）不支持在线模式。  
- D：`pt-online-schema-change`通过触发器和影子表实现无锁变更。

5. **答案：A, D**  
**解析：**  
- A：长文本取前缀减少索引大小，适合查询前缀匹配。  
- D：前缀索引减少索引维护成本（如频繁更新的`status`字段）。  
- B：全文搜索应使用`FULLTEXT`索引。  
- C：高基数字段需完整索引保证选择性。

6. **答案：A, B, D**  
**解析：**  
- A：JSON适合非结构化或半结构化数据（如用户偏好设置）。  
- B：MySQL 5.7+支持虚拟列索引（如`ALTER TABLE t ADD INDEX idx_key ((data->"$.key"))`）。  
- C：JSON查询通常比传统表结构慢（需解析JSON路径）。  
- D：大JSON对象存储在文件系统可减少数据库体积。


7. **答案：A, B, C, D**  
**解析：**  
- A：浪费存储空间且影响索引效率。  
- B：函数操作导致索引失效。  
- C：索引过多影响写入性能。  
- D：动态枚举需`ALTER TABLE`，建议改用关联表。

8. **答案：A, B, D**  
**解析：**  
- A：范围分区按时间范围分片，便于清理旧数据。  
- B：列表分区按离散值分片（如`PARTITION BY LIST (province)`）。  
- C：分区表需配合查询条件才能生效（如`WHERE date BETWEEN ...`）。  
- D：分区键必须为主键的一部分。

9. **答案：A, B, D**  
**解析：**  
- A：Percona工具，通过触发器和影子表实现无锁变更。  
- B：GitHub工具，基于binlog的异步变更。  
- C：锁表操作，不支持在线变更。  
- D：MySQL 8.0+支持部分操作的即时变更。

10. **答案：A, C**  
**解析：**  
- A：版本控制确保变更可追溯和回滚。  
- B：未经测试的DDL可能导致生产事故。  
- C：`pt-online-schema-change`通过触发器实现无锁变更。  
- D：高峰期执行DDL会导致性能下降。


--- 

通过这些问题，可巩固第六章关于数据类型、索引、Schema设计的核心难点。

---


### 《高性能MySQL》第7章 创建性能的索引

---

#### **第七章核心重难点总结**
1. **索引类型与底层原理**  
   - B-tree索引（InnoDB默认）的存储结构与最左前缀匹配原则  
   - 覆盖索引的原理与优势（减少回表操作）  
   - 全文索引（FULLTEXT）与空间索引（SPATIAL）的适用场景  
   - 聚簇索引（InnoDB）与非聚簇索引（MyISAM）的区别  

2. **索引设计策略**  
   - 最左前缀原则与复合索引设计（列顺序影响查询效率）  
   - 冗余索引的识别与避免（如`(a,b)`与`(a)`的冗余性）  
   - 前缀索引在长文本字段（如VARCHAR(255)）中的应用  
   - 索引选择性（Cardinality）的优化（高选择性列优先建索引）  

3. **索引优化技巧**  
   - 索引覆盖扫描（Covering Index）的适用场景  
   - 利用索引排序（避免`filesort`）  
   - 联表查询中的索引优化（JOIN顺序与索引选择）  
   - 索引下推（Index Condition Pushdown, ICP）的原理  

4. **索引维护与诊断**  
   - 索引统计信息的维护（`ANALYZE TABLE`的作用）  
   - 碎片化索引的检测与优化（`OPTIMIZE TABLE` vs. `ALTER TABLE`）  
   - 锁机制对索引性能的影响（如间隙锁在RR隔离级别下的行为）  

---

#### **多选题**

---

**题目1：关于B-tree索引的描述，哪些是正确的？**  
A. InnoDB的聚簇索引将数据与索引存储在同一结构中  
B. B-tree索引支持范围查询（如`BETWEEN`和`>`操作符）  
C. 索引列的顺序不影响查询效率  
D. B-tree索引的叶子节点直接存储数据行  

---

**题目2：以下哪些情况会导致索引失效？**  
A. 在索引列上使用`LIKE 'abc%'`  
B. 索引列上使用函数（如`LOWER(column)`）  
C. 索引列是字符串类型，但查询条件未加引号（如`WHERE col=123`）  
D. 使用复合索引时，查询条件仅包含最左列  

---

**题目3：关于覆盖索引的描述，哪些正确？**  
A. 覆盖索引可以避免回表操作  
B. 覆盖索引必须包含查询的所有列  
C. 覆盖索引只能用于`SELECT *`查询  
D. 聚簇索引天然支持覆盖索引  

---

**题目4：关于索引合并（Index Merge）的描述，哪些正确？**  
A. 索引合并总是比单索引查询更高效  
B. 索引合并适用于`OR`条件或`UNION`查询  
C. 索引合并可能导致更高的CPU和内存消耗  
D. 索引合并仅适用于InnoDB引擎    

---

**题目5：关于全文索引（FULLTEXT）的描述，哪些正确？**  
A. 全文索引支持精确匹配（如`=`操作符）  
B. 全文索引只能用于`MATCH ... AGAINST`查询  
C. 全文索引的停用词列表可通过配置自定义  
D. MyISAM和InnoDB均原生支持全文索引  

---

**题目6：关于索引对排序（ORDER BY）的影响，哪些正确？**  
A. 索引顺序必须与`ORDER BY`完全一致才能避免`filesort`  
B. `ASC`和`DESC`混合排序时，索引无法生效  
C. 覆盖索引可以避免排序操作  
D. 索引对`GROUP BY`的优化原理与`ORDER BY`相同  

---

**题目7：关于冗余索引的描述，哪些正确？**  
A. 联合索引`(a,b)`与`(a)`构成冗余  
B. 冗余索引会降低写操作性能  
C. 主键索引与唯一索引必然冗余  
D. 通过`pt-duplicate-key-checker`工具可检测冗余索引  

---

**题目8：关于锁与索引的关系，哪些正确？**  
A. 非唯一索引的间隙锁范围小于唯一索引  
B. 聚簇索引的锁冲突概率高于非聚簇索引  
C. 索引覆盖查询可减少锁竞争  
D. `READ COMMITTED`隔离级别下不会使用间隙锁  

---

**题目9：关于索引统计信息的描述，哪些正确？**  
A. `ANALYZE TABLE`会锁表并重新收集统计信息  
B. 索引基数（Cardinality）越高，选择性越好  
C. InnoDB的统计信息基于采样，可能不精确  
D. 索引统计信息过期会导致优化器选择次优执行计划  

---

**题目10：关于多列索引设计的描述，哪些正确？**  
A. 多列索引列顺序应遵循高选择性优先原则  
B. `(a,b,c)`索引可优化`WHERE a=val1 AND c=val3`查询  
C. 索引列顺序对范围查询效率无影响  
D. 前缀索引适用于高基数字符串列的联合索引  

#### 答案
1. **答案：A, B**  
**解析：**  
- A正确：InnoDB的聚簇索引将主键和数据行存储在一起，其他索引存储主键值。  
- B正确：B-tree索引天然支持范围查询，因其有序性。  
- C错误：索引列顺序影响最左前缀匹配，顺序错误会导致索引失效。  
- D错误：B-tree索引的叶子节点存储数据行指针（非聚簇索引）或数据行本身（聚簇索引）。  

2. **答案：B, C**  
**解析：**  
- B正确：函数操作会导致索引无法使用（如`LOWER(column)`）。  
- C正确：隐式类型转换（如字符串列与数字比较）会导致索引失效。  
- A错误：`LIKE 'abc%'`可使用前缀索引（若列是字符串类型）。  
- D错误：复合索引最左列单独使用时有效（最左前缀原则）。  

3. **答案：A, D**  
**解析：**  
- A正确：覆盖索引通过索引直接返回数据，无需回表。  
- B错误：覆盖索引只需包含查询所需列的子集（非全部）。  
- C错误：覆盖索引适用于任何返回列被索引覆盖的查询（如`SELECT col1, col2`）。  
- D正确：聚簇索引的叶子节点存储数据行，天然支持覆盖索引。  

4. **答案：B, C**  
**解析：**  
- B正确：索引合并用于处理多索引条件（如`WHERE a=1 OR b=2`）。  
- C正确：索引合并需要扫描多个索引并合并结果，增加资源消耗。  
- A错误：索引合并可能比单索引更慢（需额外合并操作）。  
- D错误：索引合并是MySQL优化器的通用策略，不限于InnoDB。

5. **答案：B, C**  
**解析：**  
- B正确：全文索引仅支持`MATCH ... AGAINST`语法。  
- C正确：MySQL允许自定义停用词列表（通过`ft_stopword_file`配置）。  
- A错误：全文索引不支持精确匹配，仅支持自然语言或布尔搜索。  
- D错误：InnoDB从MySQL 5.6开始支持全文索引，MyISAM原生支持。  

6. **答案：C, D**  
**解析：**  
- C正确：覆盖索引直接返回有序数据，无需额外排序。  
- D正确：`GROUP BY`和`ORDER BY`均依赖索引有序性，优化原理类似。  
- A错误：部分索引顺序匹配（如前缀匹配）可减少排序开销。  
- B错误：MySQL 8.0支持混合排序（如`ORDER BY a ASC, b DESC`）的索引优化。  

7. **答案：A, B, D**  
**解析：**  
- A正确：`(a)`是`(a,b)`的前缀，构成冗余。  
- B正确：冗余索引增加维护成本（插入、更新时需更新多个索引）。  
- C错误：主键与唯一索引可能覆盖不同列，不一定冗余。  
- D正确：Percona Toolkit的`pt-duplicate-key-checker`可自动化检测。  

8. **答案：B, C**  
**解析：**  
- B正确：聚簇索引的行锁可能升级为间隙锁（如RR隔离级别）。  
- C正确：覆盖索引减少数据页访问，降低锁竞争概率。  
- A错误：非唯一索引的间隙锁范围可能更大（需防止幻读）。  
- D错误：`READ COMMITTED`仍可能使用间隙锁（如`UPDATE ... FOR UPDATE`）。  

9. **答案：B, C, D**  
**解析：**  
- B正确：基数表示唯一值比例，越高选择性越好。  
- C正确：InnoDB采用随机采样，统计信息可能偏差。  
- D正确：统计信息不准确会导致优化器误判（如低估扫描行数）。  
- A错误：`ANALYZE TABLE`在InnoDB中为在线操作（不锁表）。  

10.  **答案：A, B**  
**解析：**  
- A正确：高选择性列前置可过滤更多无效数据。  
- B正确：多列索引支持最左前缀匹配（如`a=val1`和`a=val1 AND c=val3`）。  
- C错误：范围查询会终止索引使用（如`WHERE a>10 AND b=val`仅用`a`列）。  
- D错误：前缀索引适用于单列长文本，不适用于联合索引。  

---

**注：** 每题解析均结合第七章核心知识点，涵盖索引原理、设计策略、优化技巧与维护实践。

---
### 《高性能MySQL》第8章 查询性能优化

---
#### **多选题**

题目1：关于EXPLAIN执行计划中的type字段，以下哪些描述正确？
A. "const"表示通过主键或唯一索引查询单条记录  
B. "index"表示全索引扫描，需要扫描整棵索引树  
C. "ALL"表示全表扫描，通常需要优化  
D. "range"表示范围查询，只能使用B-Tree索引  
E. "ref"表示使用非唯一索引进行等值查询  

题目2：以下哪些操作会导致MySQL使用临时表？
A. GROUP BY子句中的列无索引  
B. ORDER BY子句中的列与GROUP BY列不一致  
C. 使用DISTINCT且无法通过索引优化  
D. 查询包含UNION ALL操作  
E. 使用覆盖索引的查询  

题目3：关于覆盖索引（Covering Index），以下哪些说法正确？
A. 可以避免回表查询，减少磁盘I/O  
B. 需要包含查询中所有SELECT的列  
C. 对COUNT(*)操作有显著优化效果  
D. 仅适用于等值查询，不适用于范围查询  
E. 索引列顺序不影响覆盖索引的效果  

题目4：以下哪些是优化JOIN操作的合理策略？
A. 尽量使用小表作为驱动表  
B. 为JOIN条件的列添加合适索引  
C. 使用STRAIGHT_JOIN强制指定表连接顺序  
D. 将子查询改写为JOIN操作  
E. 增加join_buffer_size参数以优化Block Nested-Loop  

题目5：关于索引条件下推（Index Condition Pushdown, ICP），以下哪些正确？
A. 仅适用于InnoDB存储引擎  
B. 将WHERE条件从Server层下推到存储引擎层  
C. 可以减少回表次数  
D. 需要满足索引覆盖查询的条件  
E. 在EXPLAIN中通过"Using index condition"标识  

题目6：以下哪些情况可能导致MySQL选择错误的执行计划？
A. 统计信息不准确  
B. 存在强制索引提示（FORCE INDEX）  
C. 查询中包含无法下推的函数（如YEAR()）  
D. 使用LIMIT子句限制返回行数  
E. 存在多列索引的前缀列参与查询  

题目7：关于MySQL的查询重写优化，以下哪些是有效方法？
A. 将OR条件改写为UNION ALL  
B. 将IN子查询改写为EXISTS子查询  
C. 使用派生表（Derived Table）代替临时表  
D. 利用延迟关联（Deferred Join）优化分页查询  
E. 将NOT IN改写为LEFT JOIN加IS NULL  

题目8：以下哪些是使用EXPLAIN分析时的关键字段？
A. possible_keys  
B. filtered  
C. Extra  
D. partitions  
E. ref  

题目9：关于索引选择性（Selectivity），以下哪些正确？
A. 选择性越高，索引效果越好  
B. 计算方式为唯一值数量除以总行数  
C. 多列索引的选择性由第一列决定  
D. 低选择性的列不适合单独建立索引  
E. 对VARCHAR列的前缀索引选择性可能更高  

题目10：以下哪些属于MySQL优化器的局限性？
A. 无法自动选择最优的JOIN顺序  
B. 对复杂表达式索引的支持不完善  
C. 忽略索引中的列顺序导致全索引扫描  
D. 统计信息更新不及时影响执行计划  
E. 无法利用覆盖索引优化包含TEXT列的查询  

---

#### 答案与解析

题目1答案：ABCE  
解析：  
- D错误，"range"可用于B-Tree和SPATIAL索引。  
- 其他选项均符合EXPLAIN的type字段定义（《高性能MySQL》第8章查询执行计划分析）。

题目2答案：ABC  
解析：  
- D错误，UNION ALL不会创建临时表，UNION会去重可能使用临时表。  
- E错误，覆盖索引避免临时表。  
- 参考第8章临时表与文件排序优化部分。

题目3答案：ACE  
解析：  
- B错误，只需包含WHERE和SELECT所需列，无需所有列。  
- D错误，覆盖索引适用于范围查询。  
- E错误，索引列顺序影响是否覆盖查询条件（最左前缀原则）。

题目4答案：ABCDE  
解析：  
- 所有选项均为JOIN优化策略（驱动表选择、索引、执行顺序控制、子查询改写、缓冲区调整）。

题目5答案：BCE  
解析：  
- A错误，ICP适用于支持ICP的引擎如InnoDB和MyISAM。  
- D错误，ICP不需要覆盖索引，而是过滤索引中的条件。

题目6答案：AC  
解析：  
- B错误，强制索引可能纠正而非导致错误计划。  
- D/E不直接影响优化器选择错误。

题目7答案：ACDE  
解析：  
- B错误，EXISTS和IN的性能差异取决于具体场景，非绝对优化方法。

题目8答案：ABCE  
解析：  
- D（partitions）仅在分区表时相关，非通用关键字段。

题目9答案：ABDE  
解析：  
- C错误，多列索引的选择性由所有列组合决定。

题目10答案：ABD  
解析：  
- C错误，优化器会考虑列顺序。  
- E错误，覆盖索引可包含TEXT列（但需前缀索引）。

---

#### 重难点总结  
1. **执行计划分析**：理解EXPLAIN的type、Extra字段含义及临时表触发条件。  
2. **索引优化**：覆盖索引、ICP、索引选择性计算、多列索引设计。  
3. **JOIN与子查询**：驱动表选择、执行顺序控制、子查询改写策略。  
4. **优化器限制**：统计信息准确性、复杂表达式处理、索引选择策略。  
5. **查询重写技巧**：OR转UNION、延迟关联、NOT IN优化等。  

题目设计覆盖了索引设计、执行计划解读、优化器行为等核心难点，需结合书中案例与原理深入掌握。

### 《高性能MySQL》第9章 复制

---

#### **第九章核心知识点总结**
1. **复制格式**  
   - 基于语句（Statement-Based Replication, SBR）  
   - 基于行（Row-Based Replication, RBR）  
   - 混合模式（Mixed Replication）  
   - **重点**：RBR是MySQL 5.6+默认格式，解决SBR的不确定性问题（如非确定性函数、触发器）。

2. **全局事务标识符（GTID）**  
   - 通过`server_uuid`和事务ID唯一标识事务。  
   - 解决传统复制位点管理的痛点（如日志切换、主从切换）。  
   - **关键配置**：`gtid_mode=ON`，`enforce_gtid_consistency=ON`。

3. **半同步复制（Semi-Synchronous Replication）**  
   - 至少一个从库确认接收事务后主库才提交。  
   - 提升数据安全性，但可能增加延迟。  
   - **参数**：`rpl_semi_sync_master_enabled=1`。

4. **多线程复制**  
   - **LOGICAL_CLOCK模式**：基于组提交并行重放事务。  
   - **DATABASE模式**：按数据库分线程（仅适用于多库场景）。  
   - **配置**：`slave_parallel_workers=N`（N≥2）。

5. **延迟复制（Delayed Replication）**  
   - 通过`CHANGE MASTER TO MASTER_DELAY=N`设置延迟时间（单位：秒）。  
   - 用于灾难恢复（如误删表后可回退到延迟时间点）。

6. **复制拓扑**  
   - 主从复制（Master-Slave）  
   - 主主复制（Master-Master）  
   - 主从从链式复制（Chain Replication）  
   - **推荐架构**：主动-被动模式（Active-Passive）或主动-只读池模式（Active-ReadOnly Pool）。

7. **故障切换与高可用**  
   - 使用VIP（虚拟IP）或中间件（如Orchestrator）自动化切换。  
   - 切换步骤：停止写入→验证从库同步→提升从库为主库→重定向流量。

8. **复制过滤**  
   - `replicate-do-db`/`replicate-ignore-db`（按数据库过滤）  
   - `replicate-do-table`/`replicate-ignore-table`（按表过滤）  
   - **注意**：过滤可能导致主从数据不一致，慎用。

---

#### 多选题

---

**1. 关于GTID的描述，哪些是正确的？**  
A. GTID由`server_uuid`和递增的事务ID组成  
B. GTID模式必须关闭二进制日志（Binary Log）  
C. GTID可简化主从切换流程  
D. GTID不兼容传统基于位置的复制  

---

**2. 半同步复制的主要作用是什么？**  
A. 提升复制性能  
B. 确保至少一个从库接收事务  
C. 防止主库崩溃导致数据丢失  
D. 自动修复主从数据不一致  

---

**3. 多线程复制的适用场景包括哪些？**  
A. 单库写入，多库读取  
B. 高并发事务的混合读写  
C. 复制拓扑中存在大量延迟节点  
D. 需要最大化利用多核CPU  

---

**4. 下列哪些配置可提升复制可靠性？**  
A. `sync_binlog=1`  
B. `innodb_flush_log_at_trx_commit=0`  
C. `relay_log_recovery=ON`  
D. `expire_logs_days=7`  

---

**5. 主从复制延迟的可能原因包括哪些？**  
A. 主库写入压力过大  
B. 从库硬件性能不足  
C. 网络带宽不足  
D. 使用基于语句的复制（SBR）  

---

**6. 关于复制过滤器的描述，哪些正确？**  
A. `binlog_do_db`基于当前默认数据库过滤  
B. `replicate-ignore-table`在从库过滤指定表  
C. 复制过滤器可防止误删表  
D. 过滤器可能导致主从数据不一致  

---

**7. 主从切换时需要考虑哪些风险？**  
A. 数据丢失  
B. 短暂服务中断  
C. 缓冲池（Buffer Pool）污染  
D. 自动化切换工具的可靠性  

---

**8. 哪些场景适合使用环形复制（Circular Replication）？**  
A. 多数据中心灾备  
B. 高可用集群  
C. 跨区域数据同步  
D. 避免单点故障  

---

**9. 多源复制（Multi-Source Replication）的用途包括哪些？**  
A. 聚合多个数据源到单一从库  
B. 实现跨实例数据同步  
C. 替代分片（Sharding）  
D. 提升单实例写入性能  

---

**10. 关于复制拓扑设计的最佳实践，哪些正确？**  
A. 主从复制使用主动-被动模式  
B. 主主复制需配置`auto_increment_increment`  
C. 延迟复制用于备份和容灾  
D. 所有从库配置为读写分离  

---

#### 答案与详解汇总
1. **答案：A, C**  
**详解**：  
- A正确：GTID唯一标识事务，格式为`server_uuid:transaction_id`。  
- C正确：GTID自动跟踪复制进度，切换时无需手动找位点。  
- B错误：GTID必须启用二进制日志。  
- D错误：GTID与基于位置的复制可通过`MASTER_AUTO_POSITION=1`兼容。

2. **答案：B, C**  
**详解**：  
- B正确：半同步确保事务提交前至少一个从库已接收日志。  
- C正确：减少主从数据丢失风险（至少有一个从库有最新数据）。  
- A错误：半同步可能增加延迟。  
- D错误：半同步不修复不一致，需人工介入。

3. **答案：A, D**  
**详解**：  
- A正确：LOGICAL_CLOCK模式可并行不同库的事务。  
- D正确：通过多线程提升从库重放速度（依赖多核CPU）。  
- B错误：混合读写场景需串行化，无法并行。  
- C错误：延迟节点需单独优化，多线程不适用。

4. **答案：A, C**  
**详解**：  
- A正确：每次事务提交同步二进制日志到磁盘。  
- C正确：从库崩溃后自动清理损坏的中继日志。  
- B错误：设置为0会降低事务持久性。  
- D错误：控制日志过期时间，与可靠性无关。

5. **答案：A, B, C, D**  
**详解**：  
- A/B/C/D均正确：高写入压力、低性能从库、网络延迟、SBR的不确定性均会导致延迟。

6. **答案：A, B, D**  
**详解**：  
- A正确：`binlog_do_db`根据主库当前数据库过滤。  
- B正确：`replicate-ignore-table`在从库过滤表。  
- C错误：过滤器无法防止误操作，需结合权限控制。  
- D正确：过滤可能导致主从数据不一致（如部分表未复制）。

7. **答案：A, B, C, D**  
**详解**：  
- A/B/C/D均正确：切换可能导致短暂数据丢失、服务中断、缓存失效，需验证工具可靠性。

8. **答案：A, C**  
**详解**：  
- A/C正确：环形复制适用于多中心灾备和跨区域同步。  
- B/D错误：环形复制复杂度高，不适合高可用集群或单点故障规避。

9. **答案：A, B**  
**详解**：  
- A/B正确：多源复制用于合并多实例数据或跨实例同步。  
- C/D错误：多源复制不替代分片，也无法提升单实例写入性能。

10. **答案：A, B, C**  
**详解**：  
- A正确：主动-被动避免脑裂。  
- B正确：主主需设置自增步长避免冲突。  
- C正确：延迟复制可用于数据恢复。  
- D错误：从库应设为只读，避免数据不一致。


### 《高性能MySQL》第10章 备份与恢复

**核心知识点：**
1. 逻辑备份 vs 物理备份的优缺点
2. 在线热备份的实现方式及工具选择
3. 二进制日志在时间点恢复中的核心作用
4. InnoDB崩溃恢复机制与REDO/UNDO日志
5. 备份一致性保障（FLUSH TABLES WITH READ LOCK应用）
6. 延迟复制在数据修复中的应用场景
7. 云环境下的备份存储策略设计
8. 备份验证方法论（checksum验证、抽样恢复测试）
9. 并行备份恢复的优化技巧
10. 容灾方案中的备份层级设计

---

#### 多选题

**1. 关于InnoDB崩溃恢复机制，以下哪两项描述正确？**
A) 依赖REDO日志进行前滚操作  
B) UNDO日志用于保证事务原子性  
C) CHECKPOINT机制会清理所有已提交事务  
D) 崩溃恢复阶段会重建整个缓冲池

**2. 使用Percona XtraBackup时，必须满足哪两个条件才能执行增量备份？**
A) 启用GTID模式  
B) 前一次完整备份的LSN信息  
C) 使用独立表空间文件  
D) 关闭二进制日志记录  

**3. 关于时间点恢复(PITR)，正确的操作步骤包含：**
A) 恢复最新全量备份后直接应用所有binlog  
B) 必须保留从备份开始到故障点的所有binlog  
C) 使用mysqlbinlog工具按时间范围过滤日志  
D) 需要同步恢复所有从库的relay log  

**4. 在云数据库环境中实施备份策略时，应重点考虑：**
A) 跨区域存储副本  
B) 对象存储的版本控制功能  
C) 备份文件加密算法选择  
D) 定期执行备份文件完整性校验  

**5. 关于延迟复制技术的应用场景，正确的描述是：**
A) 可替代常规备份方案  
B) 有效防止误操作导致数据丢失  
C) 需要配合CHANGE MASTER TO配置  
D) 延迟时间应设置为业务低峰期时长  

**6. 使用mysqldump进行逻辑备份时，可能造成锁定的操作包括：**
A) --single-transaction参数  
B) --lock-all-tables参数  
C) 备份InnoDB表时默认加锁  
D) 使用--master-data参数  

**7. 验证备份完整性的有效方法有：**
A) 对比备份前后checksum值  
B) 在沙箱环境执行完整恢复流程  
C) 定期进行备份文件大小比对  
D) 使用备份验证专用存储过程  

**8. 关于二进制日志管理策略，正确的做法是：**
A) 设置expire_logs_days自动清理  
B) 所有从库启用log_slave_updates  
C) 使用PURGE BINARY LOGS手动删除  
D) 将binlog与数据文件分盘存储  

**9. 在大型实例中优化备份性能的手段包括：**
A) 采用文件系统快照技术  
B) 增加innodb_buffer_pool_size  
C) 使用--parallel参数并行导出  
D) 关闭AUTOCOMMIT模式  

**10. 设计容灾方案时，必须包含的备份层级是：**
A) 实时同步的异地灾备  
B) 每日增量备份  
C) 季度全量归档备份  
D) 交易日志持续传输  

---

#### 答案与解析

**1. AB**  
解析：A正确，REDO日志用于重做已提交事务；B正确，UNDO保证事务回滚。C错误，CHECKPOINT不会清理已提交事务；D错误，缓冲池通过正常加载恢复。

**2. BC**  
解析：B正确，增量依赖LSN；C正确，需要file-per-table。A无关GTID；D错误，二进制日志不影响物理备份。

**3. BC**  
解析：B正确，需完整binlog序列；C正确，时间过滤必要。A错误，不能直接应用所有；D错误，从库恢复无关。

**4. AD**  
解析：A正确，地域容灾需求；D正确，完整性验证关键。B版本控制非必须；C加密算法非云环境独有。

**5. BC**  
解析：B正确，防误删有效；C正确，延迟复制配置。A错误，不能替代备份；D时间设置与业务周期无关。

**6. BD**  
解析：B锁定全表；D获取binlog位置时短暂锁。A使用事务不锁定；C InnoDB默认无锁。

**7. AB**  
解析：A正确，校验值比对；B正确，实际恢复验证。C大小变化不说明问题；D无专用存储过程。

**8. AD**  
解析：A自动清理必要；D分盘存储优化IO。B非所有从库需要；C手动删除有风险。

**9. AC**  
解析：A快照提升速度；C并行加速。B缓冲池影响有限；D无关备份性能。

**10. ABD**  
解析：A异地容灾核心；B增量保证恢复点；D日志连续保障。C季度归档非必须层级。

### 《高性能MySQL》第11章 扩展MySQL

---

**1. 关于水平分片的优缺点，以下哪些描述是正确的？**
A) 减少单节点负载压力  
B) 天然支持跨分片事务  
C) 分片键选择不当易导致热点  
D) 降低应用层代码复杂度  
E) 支持灵活的多表JOIN操作  

---

**2. 选择分片键时需重点考虑哪些因素？**
A) 字段值的高基数性  
B) 字段的频繁更新频率  
C) 查询模式中的过滤条件  
D) 字段的存储空间大小  
E) 跨分片关联的可能性  

---

**3. 跨分片查询可能引发哪些问题？**
A) 网络延迟显著增加  
B) 事务一致性难以保证  
C) 分片中间件负载激增  
D) 单分片索引失效  
E) 数据冗余存储成本上升  

---

**4. 关于数据迁移至分片集群的挑战，哪些正确？**
A) 迁移期间需保证零停机  
B) 增量数据同步难以处理  
C) 分片规则变更导致数据重分布  
D) 应用层无需修改查询逻辑  
E) 跨分片唯一ID生成复杂  

---

**5. 分片中间件的核心功能包括？**
A) 自动负载均衡  
B) SQL查询解析路由  
C) 分布式事务协调  
D) 物理存储引擎优化  
E) 跨分片结果集合并  

---

**6. 分片与复制结合使用时，哪些场景适合？**
A) 分片内实现读写分离  
B) 跨分片强一致性事务  
C) 分片副本提升高可用性  
D) 分片间数据实时同步  
E) 副本延迟影响跨分片查询  

---

**7. 分片集群监控应关注哪些指标？**
A) 各分片QPS均衡度  
B) 中间件连接池利用率  
C) 单分片锁等待时间  
D) 跨分片查询响应延迟  
E) 分片键哈希冲突率  

---

**8. 分片节点故障处理步骤包括？**
A) 自动切换至备用节点  
B) 手动修改分片路由表  
C) 临时关闭整个集群  
D) 数据回补与一致性校验  
E) 优先修复故障节点  

---

**9. 分片自动化管理工具的作用是？**
A) 动态调整分片数量  
B) 自动优化分片键选择  
C) 弹性扩缩容分片资源  
D) 自动重构跨分片事务  
E) 智能路由复杂JOIN查询  

---

**10. 云环境下分片设计的特殊考虑包括？**
A) 利用对象存储降低成本  
B) 跨可用区部署容灾  
C) 依赖SDK实现分片逻辑  
D) 按需付费的弹性扩展  
E) 避免使用托管数据库服务  

---

---

#### **答案与解析**

1. **AC**  
   - A正确，分片分散负载；C正确，分片键不合理会导致数据倾斜；B错误，跨分片事务需额外处理；D错误，应用需适配分片逻辑；E错误，JOIN难度增加。

2. **ACE**  
   - A（高基数避免热点）、C（查询过滤匹配分片键）、E（减少跨分片关联）正确；B（频繁更新破坏路由）、D（存储大小无关）错误。

3. **ABC**  
   - A（多节点通信延迟）、B（事务难保证）、C（中间件负载高）正确；D（索引仍有效）、E（冗余非必然结果）错误。

4. **BCE**  
   - B（增量同步复杂）、C（规则变更需重分布）、E（唯一ID生成难）正确；A（零停机难实现）、D（需修改逻辑）错误。

5. **ABE**  
   - A（负载均衡）、B（查询路由）、E（结果合并）正确；C（部分中间件支持，非核心）、D（存储引擎无关）错误。

6. **AC**  
   - A（分片内读写分离）、C（副本高可用）正确；B（强一致需额外机制）、D（分片间不同步）、E（延迟非必然）错误。

7. **ABD**  
   - A（负载均衡）、B（中间件性能）、D（跨分片延迟）正确；C（单分片锁无关分片）、E（哈希冲突率非常规指标）错误。

8. **ABDE**  
   - A（自动切换）、B（路由调整）、D（数据校验）、E（修复节点）正确；C（关闭集群不可行）错误。

9. **AC**  
   - A（动态扩缩）、C（弹性资源）正确；B（分片键需人工设计）、D（事务需业务处理）、E（JOIN非自动化）错误。

10. **ABD**  
    - A（对象存储降低成本）、B（跨可用区容灾）、D（按需扩展）正确；C（分片逻辑多在中间件）、E（云托管可支持分片）错误。

---

**解析重点**：  
1. **分片策略**：哈希分片（均衡但难范围查询）与范围分片（易热点但支持范围）的权衡。  
2. **分片键选择**：需结合查询模式和高基数，避免频繁更新字段。  
3. **跨分片事务**：通常牺牲强一致性，采用最终一致性或业务补偿。  
4. **中间件角色**：路由、合并结果，但不处理存储引擎优化。  
5. **云环境特性**：弹性扩展和跨区部署是核心优势，需结合云服务特性设计。

### 《高性能MySQL》第12章 云端的MySQL

#### **重点内容**  
1. **分片（Sharding）策略**：水平分片与功能分片的区别、分片键设计原则、数据路由机制。  
2. **读写分离与负载均衡**：中间件（ProxySQL/Vitess）的作用、复制拓扑的优化。  
3. **分布式数据一致性**：CAP理论的应用、最终一致性方案（如异步复制）、分布式事务（XA/2PC）。  
4. **自动化运维**：分片扩容/缩容、自动化故障转移、监控指标（如复制延迟、分片负载）。  
5. **云原生扩展**：云数据库的弹性扩展、Serverless架构适配。  

#### **难点内容**  
1. **分片键选择的权衡**：业务查询模式与分片键的关联性，避免热点问题。  
2. **跨分片事务的复杂性**：分布式锁、补偿事务的实现。  
3. **在线分片迁移**：不停机数据迁移工具（如gh-ost）的使用与风险控制。  
4. **全局唯一ID生成**：Snowflake算法、数据库自增ID的局限性。  
5. **多租户架构设计**：资源隔离与数据隔离的平衡。  

---

#### **10道Hard难度多选题**  

1. **关于MySQL分片策略，以下哪些说法正确？**  
   A. 水平分片通过将数据按行分布到不同节点实现扩展。  
   B. 功能分片适用于按业务模块划分数据的场景。  
   C. 分片键应选择高基数字段以避免数据倾斜。  
   D. 所有分片策略均需支持跨分片事务。  

2. **以下哪些中间件适合用于MySQL分片管理？**  
   A. Vitess  
   B. ProxySQL  
   C. Redis  
   D. MyCat  

3. **在分片扩容过程中，可能面临哪些挑战？**  
   A. 数据迁移期间的服务停机。  
   B. 分片键需要动态调整。  
   C. 跨分片查询性能下降。  
   D. 全局唯一ID生成冲突。  

4. **关于MySQL复制拓扑的可扩展性设计，正确的是？**  
   A. 链式复制（Chain Replication）能减少主节点负载。  
   B. 半同步复制确保所有副本数据强一致。  
   C. 多源复制适用于聚合多个数据源的场景。  
   D. 环形复制（Ring Replication）可提高写吞吐量。  

5. **分片键设计时，需重点考虑哪些因素？**  
   A. 字段的查询频率。  
   B. 字段的数据类型大小。  
   C. 业务事务的隔离级别。  
   D. 数据分布的均匀性。  

6. **以下哪些方法可用于实现分布式事务？**  
   A. XA协议（两阶段提交）。  
   B. 基于消息队列的最终一致性。  
   C. 使用全局锁（如Redis分布式锁）。  
   D. 单库事务的串行化隔离级别。  

7. **分片环境下，以下哪些查询可能无法高效执行？**  
   A. 按分片键等值查询。  
   B. 跨分片的JOIN操作。  
   C. 基于分片键的范围查询。  
   D. 单分片内的聚合统计。  

8. **云数据库（如AWS Aurora）的扩展性优势包括？**  
   A. 自动读写分离。  
   B. 存储与计算层分离。  
   C. 内置分片管理功能。  
   D. 按需弹性扩容。  

9. **监控分片集群时，应关注哪些关键指标？**  
   A. 单个分片的QPS/TPS。  
   B. 跨分片查询的响应时间。  
   C. 分片间的数据一致性延迟。  
   D. 中间件的连接池使用率。  

10. **关于分片后的数据备份与恢复，正确的是？**  
    A. 每个分片需独立备份。  
    B. 逻辑备份（如mysqldump）比分片物理备份更高效。  
    C. 恢复时需确保所有分片的时间点一致性。  
    D. 云数据库的Snapshot功能可替代分片备份。  

---

#### **答案与详解**  

1. **AB**  
   - A正确：水平分片按行拆分数据。  
   - B正确：功能分片按业务模块划分（如用户库、订单库）。  
   - C错误：高基数字段不一定避免倾斜，需结合查询模式。  
   - D错误：部分场景可避免跨分片事务（如本地分片操作）。  

2. **ABD**  
   - A正确：Vitess是K8s原生分片管理工具。  
   - B正确：ProxySQL支持读写分离和分片路由。  
   - D正确：MyCat是开源分片中间件。  
   - C错误：Redis是缓存数据库，不直接管理分片。  

3. **ACD**  
   - A正确：传统扩容需停机，但在线工具（如Vitess）可避免。  
   - C正确：跨分片查询需聚合结果，性能可能下降。  
   - D正确：扩容后新分片ID生成需兼容旧逻辑。  
   - B错误：分片键一般固定，动态调整需重构数据。  

4. **AC**  
   - A正确：链式复制减少主节点同步压力。  
   - C正确：多源复制用于聚合数据（如分析库）。  
   - B错误：半同步仅保证至少一个副本接收日志，非强一致。  
   - D错误：环形复制易导致数据冲突，极少使用。  

5. **AD**  
   - A正确：查询频率影响路由效率。  
   - D正确：均匀分布避免热点。  
   - B错误：数据类型大小影响存储，非分片键核心因素。  
   - C错误：隔离级别是事务属性，与分片键无关。  

6. **ABC**  
   - A正确：XA协议是标准分布式事务方案。  
   - B正确：消息队列实现最终一致性（如Saga模式）。  
   - C正确：全局锁可协调跨分片操作。  
   - D错误：单库事务无法解决跨分片问题。  

7. **B**  
   - B正确：跨分片JOIN需合并数据，性能低下。  
   - A/C/D错误：分片键相关查询可路由到单分片，效率高。  

8. **ABD**  
   - A正确：云数据库自动分离读写流量。  
   - B正确：存储与计算解耦（如Aurora共享存储）。  
   - D正确：按需扩展计算节点。  
   - C错误：分片管理仍需应用层设计。  

9. **ABCD**  
   - 所有选项均为分片集群监控重点。  

10. **AC**  
    - A正确：分片独立备份便于恢复。  
    - C正确：恢复需全局一致性（如GTID）。  
    - B错误：逻辑备份对大数据量分片效率低。  
    - D错误：云Snapshot可能不保证分片间一致性。

### 《高性能MySQL》第13章 MySQL的合规性

#### 章节重点内容
1. **合规性框架**（GDPR、CCPA等核心要求）
2. **数据加密策略**（透明加密、传输层加密）
3. **审计日志管理**（记录范围、存储周期）
4. **数据生命周期控制**（分类、保留策略）
5. **分片与数据主权冲突**
6. **数据脱敏技术**
7. **访问控制与权限管理**
8. **跨境数据流动限制**
9. **合规性监控体系**
10. **数据备份与恢复的合规性**

---

#### 多选题（答案及解析见最后）

1. **关于GDPR合规性要求，以下哪些描述正确？**  
   A) 要求数据最小化原则  
   B) 用户有权要求永久删除个人数据  
   C) 所有数据库必须启用透明数据加密(TDE)  
   D) 禁止跨境传输未脱敏的欧盟公民数据  
   E) 数据保留时间由业务需求决定即可  

2. **MySQL中实现数据加密的正确方法包括？**  
   A) 使用InnoDB的透明数据加密(TDE)  
   B) 通过SSL/TLS加密客户端-服务器通信  
   C) 在应用层实现字段级加密  
   D) 依赖文件系统级的全盘加密  
   E) 使用`AES_ENCRYPT()`函数直接加密列数据  

3. **审计日志应记录哪些关键操作？**  
   A) 用户登录失败尝试  
   B) 所有SELECT查询的执行计划  
   C) 数据库Schema变更(DDL)  
   D) 敏感表的访问时间戳  
   E) 慢查询日志中的性能指标  

4. **数据生命周期管理的关键步骤包括？**  
   A) 按数据类型分类标签  
   B) 动态延长所有数据的保留时间  
   C) 制定自动化数据删除策略  
   D) 将过期数据迁移到低成本存储  
   E) 实时监控CPU使用率  

5. **分片策略可能引发的合规性问题有？**  
   A) 跨分片查询导致性能下降  
   B) 不同分片所在地区的数据主权冲突  
   C) 分片备份的完整性难以验证  
   D) 单分片故障影响全局可用性  
   E) 分片间数据一致性难以保障  

6. **设计数据保留策略时需优先考虑？**  
   A) 法律法规要求的最短保留时间  
   B) 业务系统的历史数据分析需求  
   C) 存储介质的硬件成本  
   D) 用户的个人偏好设置  
   E) 数据库版本升级计划  

7. **有效的数据脱敏技术包括？**  
   A) 对身份证号部分字段掩码(如`310***********1234`)  
   B) 使用不可逆哈希算法处理手机号  
   C) 将日期泛化为季度级别(如`2023-Q2`)  
   D) 通过正则表达式替换敏感关键词  
   E) 对加密后的数据进行Base64编码  

8. **访问控制的最佳实践包括？**  
   A) 遵循最小权限原则分配账户权限  
   B) 定期审查已离职员工的数据库账户  
   C) 为所有开发者共享只读管理员账户  
   D) 启用多因素认证(MFA)  
   E) 记录所有权限变更操作  

9. **数据主权问题可能涉及？**  
   A) 数据存储的物理位置是否符合当地法律  
   B) 备份数据跨境传输的合法性  
   C) 第三方云服务商的合规认证状态  
   D) 数据库查询响应时间的SLA  
   E) 索引优化策略的选择  

10. **合规性监控系统的核心功能应包括？**  
    A) 自动检测未加密的敏感数据字段  
    B) 定期生成数据访问审计报告  
    C) 实时拦截所有高风险SQL操作  
    D) 动态调整InnoDB缓冲池大小  
    E) 对比当前策略与最新法规差异  

---

#### 答案与解析

1. **ABD**  
   - C错误：GDPR未强制要求所有数据库使用TDE，但建议加密敏感数据。  
   - E错误：保留时间需明确法律依据，不能仅由业务决定。

2. **ABC**  
   - D错误：文件系统加密不保护MySQL内部数据逻辑。  
   - E错误：`AES_ENCRYPT()`需妥善管理密钥，不符合安全最佳实践。

3. **ACD**  
   - B错误：SELECT执行计划属于性能优化，非审计重点。  
   - E错误：慢查询日志属于性能监控，非合规审计。

4. **ACD**  
   - B错误：动态延长可能违反最小化原则。  
   - E错误：CPU监控属于资源管理，非生命周期控制。

5. **BCE**  
   - A错误：性能问题是技术挑战，非直接合规问题。  
   - D错误：分片设计通常避免单点故障。

6. **ABC**  
   - D错误：用户偏好不可凌驾法律要求。  
   - E错误：版本升级与保留策略无关。

7. **ABCD**  
   - E错误：Base64是可逆编码，非脱敏。

8. **ABDE**  
   - C错误：共享账户违反最小权限原则。

9. **ABC**  
   - D错误：响应时间属于性能，非主权问题。  
   - E错误：索引策略是技术选择。

10. **ABE**  
    - C错误：实时拦截可能影响业务，通常通过审计追溯。  
    - D错误：缓冲池调整属于性能优化。