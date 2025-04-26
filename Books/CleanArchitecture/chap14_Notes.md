### Step-by-Step Guide to Chapter 14: Component Coupling

---

### 1. **Key Concepts**
Chapter 14 focuses on component coupling principles to ensure maintainability:
- **Acyclic Dependencies Principle (ADP)**: Avoid cyclic dependencies between components.
- **Stable Dependencies Principle (SDP)**: Depend on components that are more stable.
- **Stable Abstractions Principle (SAP)**: Stable components should be abstract; unstable ones concrete.

---

### 2. **1. Acyclic Dependencies Principle (ADP)**
**Problem**: Cyclic dependencies create maintenance nightmares.
**Solution**: Break cycles using interfaces or dependency inversion.

**Example 1: Violating ADP**
Two components depend on each other, creating a cycle:

**File: `User.h`**
```cpp
#pragma once
#include "Account.h"

class User {
  Account* account;
public:
  User(Account* acc) : account(acc) {}
  void updateProfile() { /* ... */ }
};
```

**File: `Account.h`**
```cpp
#pragma once
#include "User.h"  // Cyclic dependency!

class Account {
  User* user;
public:
  Account(User* u) : user(u) {}
  void resetPassword() { /* ... */ }
};
```

**Result**: Fails to compile due to cyclic inclusion.

---

**Solution: Introduce an Interface**
**File: `IAccount.h` (Abstract Interface)**
```cpp
#pragma once
class IAccount {
public:
  virtual void resetPassword() = 0;
  virtual ~IAccount() = default;
};
```

**File: `User.h`**
```cpp
#pragma once
#include "IAccount.h"

class User {
  IAccount* account;
public:
  User(IAccount* acc) : account(acc) {}
  void updateProfile() { /* ... */ }
};
```

**File: `Account.h`**
```cpp
#pragma once
#include "User.h"
#include "IAccount.h"

class Account : public IAccount {
  User* user;
public:
  Account(User* u) : user(u) {}
  void resetPassword() override { /* ... */ }
};
```

**Test Case**:
```cpp
#include "User.h"
#include "Account.h"

int main() {
  User user(nullptr);
  Account account(&user);
  // No cyclic dependency!
}
```

---

### 3. **Stable Dependencies Principle (SDP)**
**Rule**: Less stable components should depend on more stable ones.

**Stability Metric**:  
`Instability (I) = Fan-out / (Fan-in + Fan-out)`  
- **Fan-out**: Number of outgoing dependencies.  
- **Fan-in**: Number of incoming dependencies.  

**Example: Violating SDP**
**Unstable Component `PaymentProcessor`** (high fan-out):
```cpp
// PaymentProcessor.h
#include "CreditCardGateway.h"
#include "PayPalGateway.h"

class PaymentProcessor {
  CreditCardGateway ccGateway;
  PayPalGateway ppGateway;
  // High fan-out = unstable
};
```

**Stable Component `Database`** (high fan-in):
```cpp
// Database.h
#include "UserRepository.h"
#include "OrderRepository.h"

class Database { /* ... */ };  // High fan-in = stable
```

**Problem**: `PaymentProcessor` (unstable) depends on `Database` (stable), violating SDP.

---

**Solution: Invert Dependency**
**File: `IDatabase.h` (Abstract)**
```cpp
class IDatabase {
public:
  virtual void saveTransaction() = 0;
  virtual ~IDatabase() = default;
};
```

**File: `PaymentProcessor.h`**
```cpp
#include "IDatabase.h"

class PaymentProcessor {
  IDatabase* db;
public:
  PaymentProcessor(IDatabase* db) : db(db) {}
};
```

**Now**: `PaymentProcessor` depends on abstraction (`IDatabase`), aligning with SDP.

---

### 4. **Stable Abstractions Principle (SAP)**
**Rule**: Stable components should be abstract; unstable ones concrete.

**Abstractness Metric**:  
`A = Abstract Classes / Total Classes`

**Example: Violating SAP**
**Stable but Concrete Component `Logger`**:
```cpp
// Logger.h
class Logger {  // Stable (high fan-in) but concrete
public:
  void log(const std::string& msg) { /* ... */ }
};
```

**Problem**: Changes to `Logger` will propagate widely.

---

**Solution: Make Stable Component Abstract**
**File: `ILogger.h`** (Abstract):
```cpp
class ILogger {
public:
  virtual void log(const std::string& msg) = 0;
  virtual ~ILogger() = default;
};
```

**File: `FileLogger.h`** (Concrete):
```cpp
#include "ILogger.h"
class FileLogger : public ILogger {
public:
  void log(const std::string& msg) override { /* ... */ }
};
```

**Test Case**:
```cpp
#include "ILogger.h"
#include "FileLogger.h"

int main() {
  ILogger* logger = new FileLogger();
  logger->log("Test message");  // Depends on abstraction
  delete logger;
}
```

---

### 5. **Putting It All Together**
**Component Diagram**:
```
           +---------------+
           |   IDatabase   | (Abstract, Stable)
           +---------------+
                   ^
                   |
+-------------------+-------------------+
|                   |                   |
|         +-------------------+         |
|         |  PaymentProcessor|         |
|         +-------------------+         |
|                   |                   |
|         +-------------------+         |
|         |    FileLogger     | (Concrete, Unstable)
|         +-------------------+         |
+---------------------------------------+
```

**Metrics**:
- `IDatabase`: High stability (`I=0.2`), High abstractness (`A=1.0`).
- `PaymentProcessor`: Lower stability (`I=0.6`), depends on abstractions.
- `FileLogger`: High instability (`I=0.8`), low abstractness (`A=0.0`).

---

### 6. **Key Takeaways**
1. **Avoid Cycles**: Use interfaces to break dependency loops.
2. **Stability Hierarchy**: Unstable components depend downward on stable ones.
3. **Abstract Stability**: Keep stable components abstract to minimize ripple effects.
---

### Multiple-Choice Questions

Question 1: Cyclic Dependencies
Which scenarios violate the **Acyclic Dependencies Principle (ADP)**?  
A) Component A → Component B → Component C → Component A  
B) Component X depends on both Y and Z, while Y and Z depend on each other  
C) A self-dependent component D with `D → D`  
D) Two independent components M and N with no dependencies between them  

Question 2: Stable Dependencies Principle
Which design adheres to the **Stable Dependencies Principle (SDP)**?  
A) A volatile UI component depending on a stable core business rules component  
B) A stable database component depending on a volatile logging utility  
C) Two highly stable components with mutual dependencies  
D) A volatile plugin depending on an abstract interface from a stable component  

Question 3: Stable Abstractions Principle
Which configuration violates the **Stable Abstractions Principle (SAP)**?  
A) A stable component with 80% abstract classes/interfaces  
B) A highly unstable component containing only concrete implementations  
C) A stable component with all concrete classes and no abstractions  
D) An abstract component with high afferent coupling  

Question 4: Dependency Metrics
Given Component P metrics:  
- Afferent Coupling (Ca) = 5  
- Efferent Coupling (Ce) = 2  
- Instability (I) = Ce/(Ca+Ce) = 0.28  
Which statements are true?  
A) P is stable and should contain abstractions per SAP  
B) P is unstable and should depend on stable components  
C) P violates the Stable Dependencies Principle  
D) P has high reusability potential  

Question 5: Component Relationships
Which dependency graph represents good design?  
A)  
```
[Abstract Policy] ← [Concrete Mechanism]
     ↑
[UI Layer]
```  
B)  
```
[Database] → [Business Logic] → [Reporting]
     ↓___________________________↑
```  
C)  
```
[Core Algorithms] ← [Plugin Interface]
     ↑               ↓
[Utilities] ← [ThirdPartyAdapter]
```  
D)  
```
[Web Framework] → [Payment Gateway]
     ↓______________↑
```  

---

### Answers & Explanations

**Q1 Answer:** A, B  
- **A:** Direct cyclic dependency chain (A→B→C→A).  
- **B:** Indirect cycle through Y↔Z.  
- **C:** Self-dependency is allowed (though unusual).  
- **D:** Independent components are valid.  

**Q2 Answer:** D  
- **D:** Volatile plugin depends on stable abstraction.  
- **A:** Violates SDP (volatile depends on stable).  
- **B:** Stable depends on volatile (wrong direction).  
- **C:** Mutual dependencies violate ADP.  

**Q3 Answer:** C  
- **C:** Stable component must be abstract per SAP.  
- **A:** Follows SAP.  
- **B:** Unstable components can be concrete.  
- **D:** High Ca + abstract = good.  

**Q4 Answer:** A, B  
- **A:** Low instability (0.28) = stable → needs abstraction.  
- **B:** Low I = stable → should have dependents.  
- **C:** Doesn’t violate SDP.  
- **D:** High stability = lower reusability.  

**Q5 Answer:** A, C  
- **A:** Abstract policy guides concrete implementations.  
- **C:** Stable core with plugin abstraction.  
- **B:** Cyclic dependency (Database→Reporting).  
- **D:** Framework ↔ Gateway cycle.  

---

### Code Test Cases (C++ Example)

```cpp
// Question 1 Cyclic Dependency Example
// File: ComponentA.cpp
#include "ComponentB.hpp" // A → B
class ComponentA { /*...*/ };

// File: ComponentB.cpp 
#include "ComponentC.hpp" // B → C
class ComponentB { /*...*/ };

// File: ComponentC.cpp
#include "ComponentA.hpp" // C → A → CYCLIC!
class ComponentC { /*...*/ };

// Test Case Compilation Command:
// g++ -c ComponentA.cpp ComponentB.cpp ComponentC.cpp
// Error: cyclic dependency detected
```

```cpp
// Question 5 Valid Design (Option C)
// File: CoreAlgorithms.hpp (stable abstract)
class CoreAlgorithm {
public:
    virtual void execute() = 0;
};

// File: PluginInterface.hpp (abstract)
class Plugin {
public:
    virtual void adapt() = 0;
};

// File: ThirdPartyAdapter.cpp (concrete)
#include "PluginInterface.hpp"
#include "CoreAlgorithms.hpp"
class ThirdPartyAdapter : public Plugin {
    CoreAlgorithm& algo;
public:
    void adapt() override { algo.execute(); }
};
// Compiles with directed dependencies
```