### Step-by-Step Explanation of Chapters 10 & 11

---

### **Chapter 10: Interface Segregation Principle (ISP)**

#### **Key Concept**
- **ISP Definition**: Clients should not be forced to depend on interfaces they do not use. Split large interfaces into smaller, focused ones.
- **Violation Risks**: Forcing clients to implement unused methods creates "fat interfaces" and increases coupling.

---

#### **Code Example: Printer Scenario**
**Problem**: A `MultiFunctionPrinter` interface forces clients to implement unnecessary methods.

##### **1. Violation of ISP**
```cpp
#include <iostream>

// BAD: Fat interface
class MultiFunctionPrinter {
public:
    virtual void print() = 0;
    virtual void scan() = 0;
    virtual void fax() = 0;
    virtual ~MultiFunctionPrinter() = default;
};

// Client forced to implement unused methods
class BasicPrinter : public MultiFunctionPrinter {
public:
    void print() override { std::cout << "Printing...\n"; }
    void scan() override { throw std::runtime_error("Scan not supported"); }
    void fax() override { throw std::runtime_error("Fax not supported"); }
};

int main() {
    BasicPrinter printer;
    printer.print();  // Works
    // printer.scan(); // Throws error (violates ISP)
    return 0;
}
```
**Compile & Test**:
```bash
g++ -std=c++17 isp_violation.cpp -o isp_violation && ./isp_violation
```

##### **2. ISP-Compliant Solution**
```cpp
#include <iostream>

// Split interfaces
class IPrinter {
public:
    virtual void print() = 0;
    virtual ~IPrinter() = default;
};

class IScanner {
public:
    virtual void scan() = 0;
    virtual ~IScanner() = default;
};

class IFax {
public:
    virtual void fax() = 0;
    virtual ~IFax() = default;
};

// Clients implement only needed interfaces
class BasicPrinter : public IPrinter {
public:
    void print() override { std::cout << "Printing...\n"; }
};

class AdvancedPrinter : public IPrinter, public IScanner {
public:
    void print() override { std::cout << "Printing...\n"; }
    void scan() override { std::cout << "Scanning...\n"; }
};

int main() {
    BasicPrinter basic;
    basic.print();  // OK
    
    AdvancedPrinter advanced;
    advanced.print(); // OK
    advanced.scan();  // OK
    
    return 0;
}
```
**Compile & Test**:
```bash
g++ -std=c++17 isp_compliant.cpp -o isp_compliant && ./isp_compliant
```

---

### **Chapter 11: Dependency Inversion Principle (DIP)**

#### **Key Concept**
- **DIP Definition**: 
  - High-level modules should not depend on low-level modules. Both should depend on abstractions.
  - Abstractions should not depend on details; details should depend on abstractions.
- **Violation Risks**: Tight coupling makes systems rigid and hard to modify.

---

#### **Code Example: Payment Processor**

##### **1. Violation of DIP**
```cpp
#include <iostream>

// Low-level module
class PayPalPayment {
public:
    void processPayment(double amount) {
        std::cout << "Processing PayPal payment: $" << amount << "\n";
    }
};

// High-level module depends directly on low-level
class PaymentProcessor {
    PayPalPayment paypal;
public:
    void pay(double amount) { paypal.processPayment(amount); }
};

int main() {
    PaymentProcessor processor;
    processor.pay(100.0);  // Tightly coupled to PayPal
    return 0;
}
```
**Compile & Test**:
```bash
g++ -std=c++17 dip_violation.cpp -o dip_violation && ./dip_violation
```

##### **2. DIP-Compliant Solution**
```cpp
#include <iostream>
#include <memory>

// Abstraction
class IPaymentGateway {
public:
    virtual void processPayment(double amount) = 0;
    virtual ~IPaymentGateway() = default;
};

// Low-level modules implement abstraction
class PayPal : public IPaymentGateway {
public:
    void processPayment(double amount) override {
        std::cout << "Processing PayPal payment: $" << amount << "\n";
    }
};

class Stripe : public IPaymentGateway {
public:
    void processPayment(double amount) override {
        std::cout << "Processing Stripe payment: $" << amount << "\n";
    }
};

// High-level module depends on abstraction
class PaymentProcessor {
    std::unique_ptr<IPaymentGateway> gateway;
public:
    explicit PaymentProcessor(std::unique_ptr<IPaymentGateway> gw) 
        : gateway(std::move(gw)) {}
    
    void pay(double amount) { gateway->processPayment(amount); }
};

int main() {
    PaymentProcessor paypalProcessor(std::make_unique<PayPal>());
    paypalProcessor.pay(100.0);  // Uses PayPal
    
    PaymentProcessor stripeProcessor(std::make_unique<Stripe>());
    stripeProcessor.pay(200.0);   // Uses Stripe
    
    return 0;
}
```
**Compile & Test**:
```bash
g++ -std=c++17 dip_compliant.cpp -o dip_compliant && ./dip_compliant
```

---

### **Summary of Key Points**

1. **Interface Segregation Principle (ISP)**:
   - Avoid "fat interfaces" by splitting them into role-specific interfaces.
   - Example: A basic printer shouldn’t implement fax/scan methods.
   
2. **Dependency Inversion Principle (DIP)**:
   - Decouple high-level and low-level modules using abstractions.
   - Example: Payment processor depends on an abstract gateway, not PayPal/Stripe directly.

### Multiple-Choice Questions 

---

**Question 1**  
**Which statements about the Interface Segregation Principle (ISP) are correct?**  
A) ISP suggests splitting large interfaces into smaller, client-specific interfaces.  
B) ISP applies only to statically typed languages like Java.  
C) Violating ISP can force unnecessary recompilation of modules.  
D) ISP is irrelevant at the architectural level.  

---

**Question 2**  
**How does the Dependency Inversion Principle (DIP) improve system flexibility?**  
A) By allowing high-level policies to depend on concrete implementations.  
B) By ensuring source code dependencies point toward abstractions, not concretions.  
C) By requiring all modules to depend on stable platform libraries.  
D) By using factories to decouple object creation from usage.  

---

**Question 3**  
**Which scenarios violate the ISP?**  
A) A class implements an interface but leaves some methods unimplemented.  
B) A client depends on an interface with methods it does not use.  
C) A REST service requires clients to handle unused query parameters.  
D) A module uses only 2 out of 10 methods from a utility class.  

---

**Question 4**  
**What are valid strategies to adhere to the DIP?**  
A) Define all dependencies using abstract classes or interfaces.  
B) Use dependency injection frameworks to resolve concrete implementations.  
C) Directly instantiate low-level modules in high-level policies.  
D) Create factories to manage the lifecycle of concrete classes.  

---

**Question 5**  
**Which statements about stable abstractions are true?**  
A) Abstract interfaces are more volatile than concrete implementations.  
B) Stable components should be abstract to avoid frequent changes.  
C) The `String` class in Java is a valid exception to the DIP.  
D) Architectural boundaries are often defined by stable abstractions.  

---

**Question 6**  
**How do ISP and DIP interact in a layered architecture?**  
A) ISP ensures modules depend only on necessary interfaces, while DIP inverts dependencies to abstractions.  
B) DIP enforces interface segregation to reduce recompilation costs.  
C) ISP prevents high-level modules from depending on low-level details, complementing DIP.  
D) Both principles are irrelevant in microservices architectures.  

---

**Question 7**  
**Which code snippets adhere to the DIP?**  

**Snippet 1:**  
```java
// High-level module
class PaymentProcessor {
    private PaymentGateway gateway;
    PaymentProcessor(PaymentGateway gateway) {
        this.gateway = gateway;
    }
}
```

**Snippet 2:**  
```python
# High-level module
class ReportGenerator:
    def __init__(self):
        self.database = MySQLDatabase()
```

A) Only Snippet 1  
B) Only Snippet 2  
C) Both  
D) Neither  

---

**Question 8**  
**What are the consequences of violating the ISP in a distributed system?**  
A) Increased risk of runtime errors due to unused interface methods.  
B) Forced redeployment of client code when unrelated interface methods change.  
C) Improved performance due to shared dependencies.  
D) Reduced coupling between services.  

---

**Question 9**  
**Which patterns are commonly used to implement the DIP?**  
A) Strategy Pattern  
B) Singleton Pattern  
C) Factory Pattern  
D) Adapter Pattern  

---

**Question 10**  
**In the context of DIP, why are frameworks like Spring or Guice useful?**  
A) They enforce ISP by validating interface segregation.  
B) They automate dependency injection, decoupling clients from concrete implementations.  
C) They generate abstract interfaces from low-level modules.  
D) They reduce the need for stable abstractions.  

---

### **Answers & Explanations**  

---

**Answer 1**  
**Correct:** A, C  
- **A)** Correct. ISP advocates splitting large interfaces to avoid forcing clients to depend on unused methods.  
- **C)** Correct. ISP violations (e.g., depending on a bloated interface) can trigger unnecessary recompilation.  
- **B)** Incorrect. ISP applies to dynamically typed languages too (e.g., unused methods in a Ruby module still create implicit dependencies).  
- **D)** Incorrect. ISP is critical at the architectural level (e.g., avoiding dependencies on frameworks with extraneous features).  

---

**Answer 2**  
**Correct:** B, D  
- **B)** Correct. DIP mandates that dependencies target abstractions (interfaces/abstract classes).  
- **D)** Correct. Factories decouple object creation from usage (e.g., `PaymentGatewayFactory`).  
- **A)** Incorrect. DIP forbids high-level policies from depending on concretions.  
- **C)** Incorrect. DIP isn’t about platform libraries but avoiding volatile dependencies.  

---

**Answer 3**  
**Correct:** B, C  
- **B)** Correct. Clients depending on unused interface methods violate ISP.  
- **C)** Correct. Requiring clients to handle unused parameters forces unnecessary dependencies.  
- **A)** Incorrect. Unimplemented methods are a language-specific issue, not an ISP violation.  
- **D)** Incorrect. Using a subset of methods from a utility class is acceptable if the utility is stable (e.g., `String`).  

---

**Answer 4**  
**Correct:** A, B, D  
- **A)** Correct. DIP requires dependencies to be abstract.  
- **B)** Correct. DI frameworks (e.g., Spring) inject concretions without exposing them to clients.  
- **D)** Correct. Factories centralize object creation, preserving abstraction.  
- **C)** Incorrect. Direct instantiation of low-level modules violates DIP.  

---

**Answer 5**  
**Correct:** B, C, D  
- **B)** Correct. Stable components (e.g., core business rules) should be abstract to avoid frequent changes.  
- **C)** Correct. The `String` class is a stable concretion, making it a valid exception.  
- **D)** Correct. Architectural boundaries (e.g., `PaymentGateway` interface) rely on stable abstractions.  
- **A)** Incorrect. Abstractions are *less* volatile than concretions.  

---

**Answer 6**  
**Correct:** A, C  
- **A)** Correct. ISP reduces interface bloat, while DIP inverts dependencies.  
- **C)** Correct. Both principles ensure high-level modules remain decoupled from low-level details.  
- **B)** Incorrect. DIP doesn’t enforce ISP; they are complementary.  
- **D)** Incorrect. Both principles are critical in microservices (e.g., defining service contracts).  

---

**Answer 7**  
**Correct:** A  
- **Snippet 1** follows DIP: `PaymentGateway` is an abstraction.  
- **Snippet 2** violates DIP: Direct dependency on `MySQLDatabase` (a concretion).  

---

**Answer 8**  
**Correct:** B  
- **B)** Correct. Changing an unused method in a shared interface forces redeployment.  
- **A)** Incorrect. Unused methods don’t cause runtime errors if not called.  
- **C/D)** Incorrect. Violating ISP increases coupling and reduces flexibility.  

---

**Answer 9**  
**Correct:** A, C, D  
- **A/C/D)** Correct. Strategy, Factory, and Adapter patterns help decouple abstractions from concretions.  
- **B)** Incorrect. Singleton often introduces hidden dependencies, violating DIP.  

---

**Answer 10**  
**Correct:** B  
- **B)** Correct. DI frameworks (e.g., Spring) resolve dependencies at runtime, adhering to DIP.  
- **A/C/D)** Incorrect. These frameworks don’t enforce ISP or reduce the need for abstractions.  

---

### **Code Test Cases**  

Example for DIP (Question 7):  
```java
// Abstraction
interface PaymentGateway {
    void processPayment(double amount);
}

// Concrete implementation
class PayPalGateway implements PaymentGateway {
    public void processPayment(double amount) {
        System.out.println("Processing $" + amount + " via PayPal.");
    }
}

// Factory
class PaymentGatewayFactory {
    public static PaymentGateway create() {
        return new PayPalGateway(); // Configurable via dependency injection
    }
}

// High-level module
class PaymentProcessor {
    private PaymentGateway gateway;
    public PaymentProcessor(PaymentGateway gateway) {
        this.gateway = gateway;
    }
    public void executePayment(double amount) {
        gateway.processPayment(amount);
    }
}

// Test
public class Main {
    public static void main(String[] args) {
        PaymentGateway gateway = PaymentGatewayFactory.create();
        PaymentProcessor processor = new PaymentProcessor(gateway);
        processor.executePayment(100.0);
    }
}
```

**Output:**  
```plaintext
Processing $100.0 via PayPal.
```

This code adheres to DIP:  
- `PaymentProcessor` depends on the `PaymentGateway` interface.  
- The factory decouples object creation from usage.  
- Concretions (e.g., `PayPalGateway`) are injectable and replaceable.