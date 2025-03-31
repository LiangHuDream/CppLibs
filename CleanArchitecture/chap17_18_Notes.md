Boundaries & Boundary Anatomy

---

#### **Key Concepts & Challenges**
1. **Boundary Definition**  
   - **Purpose**: Isolate components to manage change and reduce coupling.  
   - **Mechanisms**: Use interfaces, dependency inversion, or abstract classes to decouple high-level policies from low-level details.  

2. **Dependency Direction**  
   - **Rule**: Dependencies should point toward stable abstractions (e.g., interfaces) to invert control.  

3. **Plugin Architecture**  
   - **Goal**: Allow dynamic extension by isolating core logic from plugins.  

---

### **Code Examples with Tests**

---

#### **Example 1: Interface-Based Boundary (C++)**
**Scenario**: Separate data processing from I/O operations.  
**Boundary**: Define an `IWriter` interface to abstract output mechanisms.

```cpp
// File: iwriter.h
#pragma once
#include <string>

class IWriter {
public:
    virtual ~IWriter() = default;
    virtual void write(const std::string& data) = 0;
};

// File: console_writer.h
#include "iwriter.h"
class ConsoleWriter : public IWriter {
public:
    void write(const std::string& data) override {
        std::cout << "[CONSOLE] " << data << std::endl;
    }
};

// File: data_processor.cpp
#include "iwriter.h"
class DataProcessor {
    IWriter* writer;
public:
    DataProcessor(IWriter* w) : writer(w) {}
    void process() {
        writer->write("Processing complete.");
    }
};

// Test in main.cpp
#include "console_writer.h"
int main() {
    ConsoleWriter cw;
    DataProcessor dp(&cw);
    dp.process();  // Output: [CONSOLE] Processing complete.
    return 0;
}
```

**Compilation**:  
```bash
g++ -std=c++17 main.cpp console_writer.cpp data_processor.cpp -o app && ./app
```

---

#### **Example 2: Dependency Inversion (C)**
**Scenario**: Decouple a `Logger` from its output method.  
**Boundary**: Use function pointers to invert dependencies.

```c
// File: logger.h
#pragma once
typedef void (*LogFunc)(const char*);

struct Logger {
    LogFunc log;
};

void init_logger(struct Logger* l, LogFunc f);
void log_message(struct Logger* l, const char* msg);

// File: logger.c
#include "logger.h"
#include <stdio.h>

void init_logger(struct Logger* l, LogFunc f) {
    l->log = f;
}

void log_message(struct Logger* l, const char* msg) {
    if (l->log) l->log(msg);
}

// File: main.c
#include "logger.h"

void file_log(const char* msg) {
    printf("[FILE] %s\n", msg);
}

int main() {
    struct Logger logger;
    init_logger(&logger, file_log);
    log_message(&logger, "System started");  // Output: [FILE] System started
    return 0;
}
```

**Compilation**:  
```bash
gcc main.c logger.c -o app && ./app
```

---

#### **Example 3: Plugin Architecture (C++)**
**Scenario**: Load plugins dynamically.  
**Boundary**: Define a plugin interface and load implementations at runtime.

```cpp
// File: plugin.h
#pragma once
#include <string>

class IPlugin {
public:
    virtual ~IPlugin() = default;
    virtual std::string execute() = 0;
};

// File: plugin_loader.cpp
#include <dlfcn.h>
#include "plugin.h"

IPlugin* load_plugin(const char* path) {
    void* handle = dlopen(path, RTLD_LAZY);
    if (!handle) return nullptr;
    auto create = (IPlugin*(*)())dlsym(handle, "create_plugin");
    return create ? create() : nullptr;
}

// File: main.cpp
#include "plugin.h"
#include <iostream>

int main() {
    IPlugin* plugin = load_plugin("./libdemo_plugin.so");
    if (plugin) {
        std::cout << plugin->execute() << std::endl;  // Output: Plugin A executed
        delete plugin;
    }
    return 0;
}
```

**Plugin Implementation (demo_plugin.cpp)**:
```cpp
#include "plugin.h"
extern "C" IPlugin* create_plugin() { return new DemoPlugin(); }

class DemoPlugin : public IPlugin {
    std::string execute() override { return "Plugin A executed"; }
};
```

**Compilation**:  
```bash
g++ -shared -fPIC demo_plugin.cpp -o libdemo_plugin.so
g++ -std=c++17 main.cpp plugin_loader.cpp -ldl -o app && ./app
```

---

### **Critical Analysis**
1. **Boundary Enforcement**:  
   - Use **interfaces/abstract classes** to define contracts.  
   - **Example 1** shows how `DataProcessor` depends only on `IWriter`, not concrete implementations.  

2. **Testability**:  
   - **Example 2** uses dependency injection to mock I/O in unit tests.  

3. **Runtime Flexibility**:  
   - **Example 3** demonstrates dynamic loading, allowing plugins to be added without recompiling the core system.  

4. **Compilation & Linking**:  
   - Ensure all dependencies are resolved (e.g., `-ldl` for dynamic loading in C++).  

---

### **Common Pitfalls**
- **Leaky Abstractions**: Avoid exposing internal details across boundaries.  
- **Over-Engineering**: Use boundaries judiciously; premature separation adds complexity.  
- **Circular Dependencies**: Ensure boundaries form a **DAG** (no cycles).  

---

### **Multiple-Choice Questions**  

**Question 1**  
**What are the primary goals of defining boundaries in a software system?**  
A. To enforce strict coding standards.  
B. To decouple components for independent development and deployment.  
C. To minimize the number of lines of code.  
D. To isolate volatile implementation details from stable policies.  
E. To centralize logging and monitoring.  

---

**Question 2**  
**Which patterns are effective for implementing boundaries between components?**  
A. Using global variables for shared state.  
B. Adapter pattern to translate interfaces.  
C. Plugin architecture with dependency inversion.  
D. Direct database access across components.  
E. RESTful APIs for service communication.  

---

**Question 3**  
**When should architectural boundaries be explicitly defined?**  
A. Only after the system becomes unmanageable.  
B. When components have different reasons to change.  
C. During initial design phases for all possible future requirements.  
D. When deploying to a distributed environment.  
E. When adhering to regulatory compliance.  

---

**Question 4**  
**Which communication mechanisms are suitable for crossing boundaries?**  
A. In-process method calls.  
B. Shared memory for high performance.  
C. Local processes with IPC.  
D. RESTful services over HTTP.  
E. Direct file system access.  

---

**Question 5**  
**What are the risks of poorly defined boundaries?**  
A. Increased compile-time dependencies.  
B. Difficulty in scaling the system horizontally.  
C. Inability to replace components without system-wide changes.  
D. Improved testability of individual modules.  
E. Tight coupling leading to ripple effects during modifications.  

---

**Question 6**  
**How does the Plugin Architecture pattern enforce boundary separation?**  
A. By allowing plugins to modify core system code.  
B. By defining interfaces owned by high-level policies.  
C. By using reflection to dynamically load modules.  
D. By compiling all plugins into a single binary.  
E. By requiring plugins to depend on core abstractions.  

---

**Question 7**  
**Which principles guide the direction of dependencies across boundaries?**  
A. Stable Dependencies Principle (depend on stable components).  
B. Common Closure Principle (group components that change together).  
C. Dependency Inversion Principle (depend on abstractions).  
D. Single Responsibility Principle (one reason to change).  
E. Interface Segregation Principle (avoid fat interfaces).  

---

**Question 8**  
**What distinguishes a service boundary from a local process boundary?**  
A. Services always use HTTP for communication.  
B. Local processes share the same memory space.  
C. Services enforce network-based decoupling.  
D. Local processes are limited to single-machine deployment.  
E. Services require serialization/deserialization of data.  

---

**Question 9**  
**Why is it critical to minimize transitive dependencies across boundaries?**  
A. To reduce binary size.  
B. To avoid unintended side effects during component replacement.  
C. To speed up compilation times.  
D. To enforce the Single Responsibility Principle.  
E. To simplify debugging.  

---

**Question 10**  
**Which code snippet demonstrates a boundary using dependency inversion?**  
**Snippet 1**:  
```cpp  
class PaymentService {  
public:  
  void process(PaymentGateway& gateway) {  
    gateway.charge();  
  }  
};  
```
**Snippet 2**:  
```cpp  
class PaymentService {  
  PayPalGateway gateway;  
public:  
  void process() { gateway.charge(); }  
};  
```
A. Snippet 1 only.  
B. Snippet 2 only.  
C. Both snippets.  
D. Neither snippet.  

---

### **Answers & Explanations**  

1. **B, D**  
   - Boundaries decouple components (B) and isolate stable policies from volatile details (D).  
   - A, C, E are unrelated to boundary goals.  

2. **B, C, E**  
   - Adapters (B), plugins (C), and REST APIs (E) enforce boundaries.  
   - Global variables (A) and direct DB access (D) increase coupling.  

3. **B, D**  
   - Boundaries are needed when components change independently (B) or for deployment (D).  
   - A delays necessary work; C is over-engineering.  

4. **A, C, D**  
   - In-process (A), IPC (C), and REST (D) are valid.  
   - Shared memory (B) and direct FS access (E) bypass boundaries.  

5. **C, E**  
   - Poor boundaries cause replacement difficulty (C) and tight coupling (E).  
   - B is a scalability issue but not directly due to boundaries.  

6. **B, E**  
   - High-level interfaces (B) and plugin dependencies on abstractions (E) define the pattern.  
   - A and C are implementation details, not principles.  

7. **A, C**  
   - Stable Dependencies (A) and Dependency Inversion (C) guide dependency direction.  
   - B, D, E are cohesion principles.  

8. **C, E**  
   - Services use network decoupling (C) and require data serialization (E).  
   - B and D describe local processes.  

9. **B**  
   - Transitive dependencies cause side effects during component replacement (B).  
   - A, C, D, E are secondary concerns.  

10. **A**  
    - Snippet 1 depends on an abstraction (`PaymentGateway`), demonstrating inversion (A).  
    - Snippet 2 hardcodes `PayPalGateway`, violating inversion.  

---

### **Test Code**  

**Testing Plugin Architecture (Q6)**  
```cpp  
// Interface (owned by high-level policy)  
class PaymentGateway {  
public:  
  virtual void charge() = 0;  
  virtual ~PaymentGateway() = default;  
};  

// Plugin Implementation  
class PayPalGateway : public PaymentGateway {  
public:  
  void charge() override { /* PayPal-specific logic */ }  
};  

// Core system using abstraction  
class PaymentService {  
  PaymentGateway& gateway;  
public:  
  PaymentService(PaymentGateway& g) : gateway(g) {}  
  void process() { gateway.charge(); }  
};  

// Main function  
int main() {  
  PayPalGateway paypal;  
  PaymentService service(paypal);  
  service.process();  
  return 0;  
}  
```

**Testing Service Boundary (Q8)**  
```cpp  
// REST Client (simplified)  
#include <curl/curl.h>  
#include <string>  

class HttpClient {  
public:  
  void post(const std::string& url, const std::string& data) {  
    // Use libcurl to send HTTP POST  
  }  
};  

// Service Client  
class OrderService {  
  HttpClient client;  
public:  
  void createOrder(const std::string& data) {  
    client.post("http://api/orders", data);  
  }  
};  

int main() {  
  OrderService service;  
  service.createOrder("{ \"item\": \"book\" }");  
  return 0;  
}  
```

Both examples compile with proper dependencies (e.g., `libcurl`) and demonstrate boundary implementation.