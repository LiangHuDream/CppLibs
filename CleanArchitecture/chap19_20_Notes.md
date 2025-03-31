### Chapter 19: Policy and Level

#### Key Concepts:
1. **Policy (Business Rules):**  
   - Policies define the core logic/behavior of the system.  
   - High-level policies (e.g., business rules) should not depend on low-level details (e.g., databases, UI).  
   - Example: A payment processing policy should not depend on a specific payment gateway.  

2. **Level:**  
   - Indicates how "close" a component is to inputs/outputs or core logic.  
   - Higher-level components (closer to core logic) should depend on abstractions, not lower-level components.  
   - Example: A business rule (high-level) depends on an interface for data access, not a specific database (low-level).  

#### Code Example:
```cpp
#include <iostream>
#include <vector>

// High-level policy (Level 1)
class OrderProcessor {
public:
    virtual double calculateTotal(const std::vector<double>& prices) const {
        double total = 0.0;
        for (const auto& price : prices) {
            total += price;
        }
        return total;
    }
};

// Lower-level component (Level 2)
class Database {
public:
    std::vector<double> getOrderPrices(int orderId) {
        // Simulate database access
        return {10.5, 20.3, 5.2};
    }
};

// Adapter to invert dependency
class OrderService {
private:
    OrderProcessor& processor;
    Database& db;
public:
    OrderService(OrderProcessor& proc, Database& database) 
        : processor(proc), db(database) {}
    
    double processOrder(int orderId) {
        auto prices = db.getOrderPrices(orderId);
        return processor.calculateTotal(prices);
    }
};

// Test
int main() {
    Database db;
    OrderProcessor processor;
    OrderService service(processor, db);
    double total = service.processOrder(123);
    std::cout << "Total: $" << total << std::endl; // Output: Total: $36.0
    return 0;
}
```

#### Explanation:
- **Policy:** `OrderProcessor` handles business logic (calculating totals).  
- **Level Separation:** `OrderService` acts as an adapter to decouple `OrderProcessor` from `Database`.  
- **Dependency Inversion:** High-level `OrderProcessor` does not depend on low-level `Database`.  

---

### Chapter 20: Business Rules

#### Key Concepts:
1. **Entities:**  
   - Represent core domain objects (e.g., `User`, `Order`).  
   - Contain business logic that is independent of external systems.  

2. **Use Cases:**  
   - Coordinate data flow between entities and external systems (e.g., databases).  
   - Example: A `CreateOrderUseCase` might validate data and save an `Order` entity.  

#### Code Example:
```cpp
#include <iostream>
#include <string>
#include <vector>

// Entity (Business Rule)
class Order {
private:
    std::string orderId;
    std::vector<double> items;
public:
    Order(const std::string& id, const std::vector<double>& items)
        : orderId(id), items(items) {}
    
    double calculateTotal() const {
        double total = 0.0;
        for (const auto& item : items) {
            total += item;
        }
        return total;
    }
};

// Use Case
class CreateOrderUseCase {
public:
    Order execute(const std::string& orderId, const std::vector<double>& items) {
        if (items.empty()) {
            throw std::invalid_argument("Order must have items.");
        }
        return Order(orderId, items);
    }
};

// Test
int main() {
    CreateOrderUseCase useCase;
    Order order = useCase.execute("ORD123", {15.0, 25.5, 10.0});
    std::cout << "Order Total: $" << order.calculateTotal() << std::endl; // Output: Order Total: $50.5
    return 0;
}
```

#### Explanation:
- **Entity:** `Order` encapsulates the business rule for calculating totals.  
- **Use Case:** `CreateOrderUseCase` validates input and creates an `Order`.  
- **Decoupling:** Business rules in `Order` are independent of databases/UI.  

---

### Compilation & Testing:
1. **Compile with GCC:**  
   ```bash
   g++ -std=c++11 policy_level.cpp -o policy_level
   g++ -std=c++11 business_rules.cpp -o business_rules
   ```

2. **Run Tests:**  
   ```bash
   ./policy_level       # Output: Total: $36.0
   ./business_rules     # Output: Order Total: $50.5
   ```

---

### Summary:
- **Chapter 19:** High-level policies must depend on abstractions (interfaces), not concrete low-level details.  
- **Chapter 20:** Entities encapsulate business rules; use cases orchestrate interactions while keeping core logic decoupled.  
- **Code Design:** Dependency inversion and separation of concerns ensure flexibility and testability.
- ### Summary of Chapters 19 & 20  
**Chapter 19: Policy and Level**  
- Focuses on organizing system architecture by separating policies based on their level of abstraction.  
- **Key Concepts**:  
  1. **Policy**: A set of rules/processes that dictate system behavior.  
  2. **Level**: The distance of a policy from inputs/outputs (higher levels are more abstract, lower levels are implementation details).  
  3. **Stable Abstraction Principle**: High-level policies should not depend on low-level details.  
  4. **Dependency Inversion Principle (DIP)**: High-level modules define interfaces; low-level modules implement them.  

**Chapter 20: Business Rules**  
- Explores structuring business rules as independent entities decoupled from frameworks/databases.  
- **Key Concepts**:  
  1. **Entities**: Encapsulate critical business rules/data structures (e.g., `Order`, `User`).  
  2. **Use Cases**: Orchestrate application-specific workflows (e.g., `PlaceOrderUseCase`).  
  3. **Interface Segregation**: Use narrow interfaces to avoid unnecessary dependencies.  
  4. **Boundaries**: Separate business logic from I/O mechanisms (e.g., databases, UIs).  

---

### 10 Hard Multiple-Choice Questions  

**1. Which statements about "policy levels" are correct?**  
A) Lower-level policies should depend on higher-level policies.  
B) Higher-level policies should define interfaces for lower-level policies to implement.  
C) Changing a low-level policy should never impact high-level policies.  
D) Policy levels are determined by their proximity to frameworks/databases.  

**2. What violates the Stable Abstraction Principle?**  
A) A business rule class inheriting from a database utility.  
B) A use case depending on an abstract `Repository` interface.  
C) An entity class with no dependencies on external frameworks.  
D) A high-level policy directly instantiating a low-level HTTP client.  

**3. How should business rules be structured?**  
A) Tightly coupled with UI frameworks for rapid iteration.  
B) Isolated in entities/use cases with clear input/output boundaries.  
C) Embedded directly in database schema definitions.  
D) Split across multiple microservices for scalability.  

**4. Which code snippet follows the Dependency Inversion Principle?**  
**Snippet 1**:  
```cpp 
class OrderService {
  Database* db; // Depends on concrete DB
};
```  
**Snippet 2**:  
```cpp 
class OrderService {
  IRepository* repo; // Depends on interface
};
```  
A) Only Snippet 1  
B) Only Snippet 2  
C) Both  
D) Neither  

**5. What distinguishes an "Entity" from a "Use Case"?**  
A) Entities handle UI rendering; Use Cases manage workflows.  
B) Entities contain business logic; Use Cases coordinate data flow.  
C) Use Cases depend on frameworks; Entities are framework-agnostic.  
D) Use Cases define database schemas; Entities validate inputs.  

**6. Which violates Interface Segregation Principle (ISP)?**  
A) Splitting a `UserRepository` into `ReadableRepository` and `WritableRepository`.  
B) A `ReportGenerator` interface with `generatePDF()` and `generateCSV()`.  
C) A `PaymentGateway` interface requiring credit card details for refunds.  
D) A `Logger` interface with `logError()`, `logWarning()`, and `logInfo()`.  

**7. How to test business rules independently?**  
A) Mock the database/API calls using interfaces.  
B) Embed test data directly in entity classes.  
C) Deploy a staging environment with real services.  
D) Write end-to-end tests covering all layers.  

**8. Which code correctly separates policy levels?**  
**High-Level Policy**:  
```cpp 
class TaxCalculator {
public: 
  virtual double calculateTax(Order& order) = 0;
};
```  
**Low-Level Policy**:  
```cpp 
class USATaxCalculator : public TaxCalculator {
  double calculateTax(Order& order) override { /*...*/ }
};
```  
A) Correct: High-level defines interface, low-level implements.  
B) Incorrect: High-level depends on low-level details.  
C) Correct: Both levels are at the same abstraction level.  
D) Incorrect: Low-level policy should not inherit from high-level.  

**9. What is the primary risk of mixing business logic with I/O?**  
A) Improved performance due to fewer abstractions.  
B) Difficulty testing rules without external systems.  
C) Faster development cycles with tight integration.  
D) Reduced need for documentation.  

**10. Which design ensures entities remain framework-agnostic?**  
A) Injecting a `HttpClient` into an `Order` entity.  
B) Using inheritance to share code between entities and UI components.  
C) Defining data structures in entities and dependencies in use cases.  
D) Directly referencing a database ORM in entity methods.  

---

### Answers & Explanations  

1. **B, C**  
   - B: DIP states high-level modules define interfaces.  
   - C: Low-level changes shouldn’t impact high-level policies (stable abstraction).  
   - *A is wrong (lower should depend on higher via interfaces). D is incorrect (levels are about abstraction, not proximity to I/O).*  

2. **A, D**  
   - A: Entities should not inherit from implementation details.  
   - D: High-level policies should not directly depend on low-level concretions.  
   - *B/C follow the principle.*  

3. **B**  
   - Business rules should be isolated with clear boundaries.  
   - *A/C/D introduce coupling or improper scoping.*  

4. **B**  
   - Snippet 2 depends on an abstract interface (`IRepository`), adhering to DIP.  
   - *Snippet 1 violates DIP by depending on a concrete `Database`.*  

5. **B, C**  
   - B: Entities encapsulate core logic; Use Cases manage flow.  
   - C: Use Cases may depend on frameworks; Entities should not.  
   - *A/D incorrectly assign responsibilities.*  

6. **C**  
   - Requiring credit card details for refunds forces unnecessary dependencies.  
   - *A/B/D follow ISP by providing cohesive interfaces.*  

7. **A**  
   - Mocking dependencies allows isolated unit testing.  
   - *B/C/D mix concerns or require external systems.*  

8. **A**  
   - High-level defines interface; low-level implements it (correct DIP usage).  

9. **B**  
   - Mixing I/O makes testing harder without external systems.  
   - *A/C/D are incorrect or irrelevant.*  

10. **C**  
   - Entities contain data/logic; use cases handle dependencies.  
   - *A/B/D tie entities to frameworks.*  

---

### Test Code Examples (C++)  

**Entity Example (Chapter 20)**  
```cpp 
// Entity: Business rule for validating orders. 
class Order { 
private: 
  double amount; 
  std::string currency; 
public: 
  Order(double amt, std::string curr) : amount(amt), currency(curr) {} 
  bool isValid() const { 
    return amount > 0 && (currency == "USD" || currency == "EUR"); 
  } 
}; 

// Test 
int main() { 
  Order validOrder(100.0, "USD"); 
  Order invalidOrder(-50.0, "JPY"); 
  assert(validOrder.isValid()); 
  assert(!invalidOrder.isValid()); 
  return 0; 
} 
```  

**Use Case with Dependency Injection (Chapters 19-20)**  
```cpp 
// High-level interface 
class IRepository { 
public: 
  virtual void saveOrder(const Order& order) = 0; 
}; 

// Use Case: Coordinates workflow 
class PlaceOrderUseCase { 
private: 
  IRepository* repo; 
public: 
  PlaceOrderUseCase(IRepository* repo) : repo(repo) {} 
  void execute(const Order& order) { 
    if (order.isValid()) repo->saveOrder(order); 
  } 
}; 

// Mock implementation for testing 
class MockRepo : public IRepository { 
public: 
  void saveOrder(const Order& order) override { /*...*/ } 
}; 

// Test 
int main() { 
  MockRepo repo; 
  PlaceOrderUseCase useCase(&repo); 
  Order validOrder(200.0, "EUR"); 
  useCase.execute(validOrder); // Should save 
  return 0; 
} 
```  

These examples demonstrate:  
1. **Entities** encapsulate validation logic without external dependencies.  
2. **Use Cases** depend on abstractions (`IRepository`) and are testable with mocks.  
3. Adherence to DIP and ISP through interface segregation.