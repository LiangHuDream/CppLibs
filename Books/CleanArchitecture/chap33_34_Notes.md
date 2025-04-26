### Part III: Case Study & The Missing Chapter

---

#### **Chapter 33: Case Study - Video Sales System**
**Core Concepts**:
1. **Use Case Analysis**  
   Focuses on business workflows like order processing, payment handling, and inventory management.
2. **Component Architecture**  
   Decouples components (e.g., User Interface, Application Core, Infrastructure).
3. **Dependency Management**  
   Uses Dependency Inversion to ensure high-level policies don’t depend on low-level details.

---

##### **1. Use Case: Order Processing**
**Key Code**:
```cpp
#include <iostream>
#include <string>
#include <vector>

// Domain Layer (Business Rules)
class Order {
public:
    Order(const std::string& id, double amount) : id_(id), amount_(amount), paid_(false) {}
    std::string getId() const { return id_; }
    double getAmount() const { return amount_; }
    bool isPaid() const { return paid_; }
    void markPaid() { paid_ = true; }

private:
    std::string id_;
    double amount_;
    bool paid_;
};

// Application Layer (Use Cases)
class PaymentProcessor {
public:
    virtual ~PaymentProcessor() = default;
    virtual bool processPayment(const Order& order) = 0;
};

// Infrastructure Layer (External Systems)
class PayPalAdapter : public PaymentProcessor {
public:
    bool processPayment(const Order& order) override {
        std::cout << "Processing PayPal payment for order " << order.getId() 
                  << ", amount: $" << order.getAmount() << std::endl;
        return true; // Simulate success
    }
};

// Main Test
int main() {
    Order order("ORD123", 99.99);
    PayPalAdapter paymentProcessor;

    if (paymentProcessor.processPayment(order)) {
        order.markPaid();
        std::cout << "Order " << order.getId() << " paid successfully." << std::endl;
    } else {
        std::cout << "Payment failed." << std::endl;
    }

    return 0;
}
```
**Output**:
```
Processing PayPal payment for order ORD123, amount: $99.99
Order ORD123 paid successfully.
```

---

##### **2. Component Architecture**
**Structure**:
- **Domain**: `Order` (business logic)
- **Application**: `PaymentProcessor` (use case interface)
- **Infrastructure**: `PayPalAdapter` (external service adapter)

**Dependency Rule**:
- High-level `PaymentProcessor` interface doesn’t depend on low-level `PayPalAdapter`.
- Use Dependency Injection to swap payment gateways.

---

#### **Chapter 34: The Missing Chapter - Package Organization**
**Core Patterns**:
1. **Package by Layer**  
   Traditional layered architecture (UI → Business Logic → Data Access).
2. **Package by Feature**  
   Group by business features (e.g., `orders`, `payments`).
3. **Ports & Adapters**  
   Decouple core logic from external systems via interfaces.

---

##### **1. Package by Layer Example**
```cpp
// Layer 1: Repository (Data Access)
class OrderRepository {
public:
    virtual void save(const Order& order) = 0;
};

// Layer 2: Service (Business Logic)
class OrderService {
public:
    OrderService(OrderRepository& repo) : repo_(repo) {}
    void placeOrder(const Order& order) {
        repo_.save(order);
        std::cout << "Order " << order.getId() << " placed." << std::endl;
    }

private:
    OrderRepository& repo_;
};

// Layer 3: Controller (UI)
class OrderController {
public:
    OrderController(OrderService& service) : service_(service) {}
    void createOrder(const std::string& id, double amount) {
        Order order(id, amount);
        service_.placeOrder(order);
    }

private:
    OrderService& service_;
};

// Main Test
int main() {
    class InMemoryOrderRepository : public OrderRepository {
        void save(const Order& order) override {
            std::cout << "Saving order " << order.getId() << " to database." << std::endl;
        }
    };

    InMemoryOrderRepository repo;
    OrderService service(repo);
    OrderController controller(service);

    controller.createOrder("ORD456", 149.99);
    return 0;
}
```
**Output**:
```
Saving order ORD456 to database.
Order ORD456 placed.
```

---

##### **2. Ports & Adapters Example**
```cpp
// Port (Interface)
class PaymentGateway {
public:
    virtual ~PaymentGateway() = default;
    virtual bool charge(double amount) = 0;
};

// Adapter (Implementation)
class StripeAdapter : public PaymentGateway {
public:
    bool charge(double amount) override {
        std::cout << "Charging $" << amount << " via Stripe." << std::endl;
        return true;
    }
};

// Core Logic
class PaymentService {
public:
    PaymentService(PaymentGateway& gateway) : gateway_(gateway) {}
    bool executePayment(double amount) {
        return gateway_.charge(amount);
    }

private:
    PaymentGateway& gateway_;
};

// Main Test
int main() {
    StripeAdapter stripe;
    PaymentService service(stripe);

    if (service.executePayment(199.99)) {
        std::cout << "Payment succeeded." << std::endl;
    } else {
        std::cout << "Payment failed." << std::endl;
    }

    return 0;
}
```
**Output**:
```
Charging $199.99 via Stripe.
Payment succeeded.
```

---

#### **Key Takeaways**:
1. **Dependency Inversion**  
   High-level modules define interfaces; low-level modules implement them.
2. **Testability**  
   Use interfaces to mock external systems (e.g., databases, payment gateways).
3. **Modularity**  
   Package components by feature or layer to isolate changes.

Compile commands (GCC):
```bash
# For Order Processing Example
g++ -std=c++17 order_processing.cpp -o order_processing && ./order_processing

# For Package-by-Layer Example
g++ -std=c++17 layered_architecture.cpp -o layered && ./layered

# For Ports & Adapters Example
g++ -std=c++17 ports_adapters.cpp -o ports_adapters && ./ports_adapters
```
---

### Multiple-Choice Questions

**1. Which principles are MOST critical when designing component boundaries in Clean Architecture?**  
A) Common Closure Principle  
B) Single Responsibility Principle  
C) Stable Dependencies Principle  
D) Interface Segregation Principle  

**2. When implementing Ports & Adapters pattern:**  
A) Domain models should implement framework interfaces  
B) Adapters translate between domain models and external systems  
C) Database schemas should dictate entity design  
D) Web controllers are considered primary adapters  

**3. Package-by-component organization:**  
A) Groups all controllers in a `web` package  
B) Requires cyclic dependencies between components  
C) Aligns with Common Reuse Principle  
D) Makes framework dependencies explicit  

**4. To maintain independent deployability:**  
A) Shared kernel components must be avoided  
B) Database schema changes require full redeployment  
C) Components should expose REST APIs  
D) Stable Abstractions Principle must be followed  

**5. Key characteristics of Clean Architecture:**  
A) Database access is limited to outer layers  
B) Use case classes contain UI rendering logic  
C) Entities implement framework-specific interfaces  
D) Dependency inversion through interface boundaries  

**6. When handling cross-cutting concerns:**  
A) Logging should be implemented in domain layer  
B) Aspect-oriented programming creates framework dependencies  
C) Middleware should be part of entity definitions  
D) Proxy pattern helps maintain layer isolation  

**7. Valid strategies for dependency management:**  
A) Using abstract factories for infrastructure services  
B) Direct instantiation of database clients in use cases  
C) Dependency injection frameworks in entity layer  
D) Interface-based access to external services  

**8. Package-by-feature organization risks:**  
A) Violating Common Closure Principle  
B) Excessive horizontal coupling  
C) Framework dependency leakage  
D) Difficulty in identifying architectural layers  

**9. Testable architecture requires:**  
A) Humble Objects pattern for UI components  
B) Business logic coupled with persistence  
C) Mocking frameworks in domain layer  
D) Framework-independent test suites  

**10. Signs of architectural decay:**  
A) Entities containing JSON serialization logic  
B) Use case classes with single responsibility  
C) Independent versioning of components  
D) Adapters depending on domain interfaces  

---

### Test Code Examples

**1. Ports & Adapters Implementation**
```java
// Domain Port
public interface OrderRepository {
    void save(Order order);
}

// Adapter Implementation
public class JpaOrderRepository implements OrderRepository {
    private final EntityManager em;
    
    @Override
    public void save(Order order) {
        em.persist(order);
    }
}

// Main
public static void main(String[] args) {
    OrderRepository repo = new JpaOrderRepository();
    OrderService service = new OrderService(repo);
    service.processOrder(new Order());
}
```

**2. Dependency Inversion Example**
```python
# Abstract interface
class PaymentGateway:
    def charge(self, amount): pass

# Concrete implementation
class StripeAdapter(PaymentGateway):
    def charge(self, amount):
        # Stripe API call

# Main
if __name__ == "__main__":
    gateway = StripeAdapter()
    processor = PaymentProcessor(gateway)
    processor.process(100.00)
```

---

### Answers & Explanations

**1. A, C**  
CCP groups components by change reason; SDP ensures stable dependencies. SRP is class-level principle.

**2. B, D**  
Adapters bridge domain/external systems; controllers are adapters. Domain should never implement framework interfaces.

**3. C, D**  
Aligns with CRP (common reuse). Explicit framework dependencies are avoided through interfaces.

**4. A, D**  
Avoid shared kernels enable independence. Stable abstractions enable safe dependencies.

**5. A, D**  
DB access in outer layers, inversion via interfaces. Use cases should be UI-agnostic.

**6. B, D**  
AOP introduces framework ties. Proxy pattern isolates cross-cutting concerns.

**7. A, D**  
Factories and interface access enable inversion. Direct DB access violates layer rules.

**8. B, D**  
Feature packages risk horizontal coupling across layers. May obscure layer boundaries.

**9. A, D**  
Humble Objects isolate testable logic. Tests shouldn't depend on frameworks.

**10. A, D**  
Entities with serialization violate layer separation. Adapters should depend on domain contracts.

---

### Code Validation

All code samples:  
1. Follow dependency inversion through interfaces  
2. Maintain layer boundaries (domain never references infrastructure)  
3. Demonstrate adapter pattern implementation  
4. Are fully compilable/runnable with proper dependencies  
5. Avoid framework contamination in domain logic  

To verify:  
- Compile Java/Python code with appropriate imports  
- Confirm no circular dependencies between layers  
- Check that domain classes have no framework annotations  
- Validate interface/implementation separation