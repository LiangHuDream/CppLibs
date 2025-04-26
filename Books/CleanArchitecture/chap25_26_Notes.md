
@[TOC](Layers and Boundaries&The Main Component)
### Chapter 25: Layers and Boundaries

#### Key Concepts
1. **Layered Architecture**:
   - Systems are divided into layers (e.g., UI, business logic, data access).
   - **Dependency Rule**: Higher-level layers (e.g., business logic) should not depend on lower-level layers (e.g., database). Dependencies flow inward.
   - **Boundaries**: Defined via interfaces/abstract classes to decouple layers.

2. **Hunt the Wumpus Example**:
   - Demonstrates separation of UI, game logic, and data layers.
   - Layers communicate through abstract interfaces, allowing independent development and testing.

3. **Cross-Layer Communication**:
   - Use **dependency inversion** to invert control (e.g., business logic defines interfaces, data layer implements them).
   - Avoid circular dependencies between layers.

---

#### Code Example: Layered Architecture
```cpp
#include <iostream>
#include <memory>

// ------------------------------
// Layer 1: Data Access (Low-Level)
// ------------------------------
class IDataStorage {
public:
    virtual std::string fetchData() = 0;
    virtual ~IDataStorage() = default;
};

class Database : public IDataStorage {
public:
    std::string fetchData() override {
        return "Data from database";
    }
};

// ------------------------------
// Layer 2: Business Logic (High-Level)
// ------------------------------
class BusinessService {
private:
    std::shared_ptr<IDataStorage> storage;
public:
    explicit BusinessService(std::shared_ptr<IDataStorage> storage) : storage(storage) {}
    void process() {
        std::string data = storage->fetchData();
        std::cout << "Processing: " << data << std::endl;
    }
};

// ------------------------------
// Layer 3: UI (Highest-Level)
// ------------------------------
class UIManager {
private:
    std::shared_ptr<BusinessService> service;
public:
    explicit UIManager(std::shared_ptr<BusinessService> service) : service(service) {}
    void display() {
        service->process();
    }
};

// ------------------------------
// Main: Dependency Injection
// ------------------------------
int main() {
    auto storage = std::make_shared<Database>();
    auto service = std::make_shared<BusinessService>(storage);
    UIManager ui(service);
    ui.display(); // Output: "Processing: Data from database"
    return 0;
}
```

**Explanation**:
- **Dependency Inversion**: `BusinessService` depends on `IDataStorage` (abstract), not `Database` (concrete).
- **Testability**: Replace `Database` with a mock in tests.
- **Compilation**: Ensure all virtual methods are implemented.

---

### Chapter 26: The Main Component

#### Key Concepts
1. **Main Component as Plugin**:
   - Main component initializes the system (e.g., creates objects, injects dependencies).
   - Acts as the outermost layer, dependent on frameworks (e.g., GUI, web servers).
   - Core logic remains framework-agnostic.

2. **Dependency Injection**:
   - Main component configures and wires dependencies (e.g., creating a `Database` instance and injecting it into `BusinessService`).

3. **Framework Independence**:
   - Core logic (business rules) doesn’t depend on frameworks like Qt or Spring.
   - Main component adapts the core to the framework.

---

#### Code Example: Main Component
```cpp
#include <iostream>
#include <memory>

// ------------------------------
// Core Logic (Independent of Framework)
// ------------------------------
class IRepository {
public:
    virtual std::string getMessage() = 0;
    virtual ~IRepository() = default;
};

class CoreService {
private:
    std::shared_ptr<IRepository> repo;
public:
    explicit CoreService(std::shared_ptr<IRepository> repo) : repo(repo) {}
    void run() {
        std::cout << "Core: " << repo->getMessage() << std::endl;
    }
};

// ------------------------------
// Framework-Specific Adapter (Plugin)
// ------------------------------
class FileRepository : public IRepository {
public:
    std::string getMessage() override {
        return "Message from file";
    }
};

// ------------------------------
// Main Component (Initializes Dependencies)
// ------------------------------
int main() {
    auto repo = std::make_shared<FileRepository>();
    CoreService service(repo);
    service.run(); // Output: "Core: Message from file"
    return 0;
}
```

**Explanation**:
- **Decoupling**: `CoreService` depends on `IRepository`, not `FileRepository`.
- **Testability**: Use a mock `IRepository` in tests.
- **Flexibility**: Switch to a `DatabaseRepository` without changing `CoreService`.

---

### Summary
1. **Chapter 25**:
   - Use layered architecture with strict dependency rules.
   - Define boundaries using interfaces to decouple components.
   - Example: `BusinessService` depends on abstract `IDataStorage`.

2. **Chapter 26**:
   - Main component initializes and wires dependencies.
   - Keep core logic independent of frameworks.
   - Example: `CoreService` uses `IRepository` injected by `main()`.

### Chapter 25: Layers and Boundaries  
**Focus**: Cross-layer communication strategies, boundary enforcement, data flow management.  

### Chapter 26: The Main Component  
**Focus**: Bootstrapping dependencies, dependency inversion, minimalism in system initialization.  

---

### Multiple-Choice Questions  

Question 1: Layered Architecture Boundaries  
Which of the following are valid strategies for enforcing layer boundaries in Clean Architecture?  
A. Using compiler-enforced package visibility rules.  
B. Declaring all layers as public modules with unrestricted access.  
C. Implementing dependency injection containers to isolate layer dependencies.  
D. Relying on documentation to guide developers on layer interactions.  

Question 2: Main Component Responsibilities  
Which responsibilities belong to the Main component? (Choose two)  
A. Business rule validation.  
B. Dependency graph initialization.  
C. HTTP request/response handling.  
D. Configuration loading for infrastructure.  

Question 3: Dependency Inversion in Layers  
Which patterns help implement the Dependency Inversion Principle between layers? (Choose two)  
A. Using abstract factories for service instantiation.  
B. Directly instantiating concrete classes in higher layers.  
C. Defining interface contracts in the domain layer.  
D. Embedding database schemas in UI components.  

Question 4: Boundary Violations  
What issues arise from improper layer boundary enforcement? (Choose two)  
A. Improved testability due to tight coupling.  
B. Circular dependencies between components.  
C. Reduced deployment flexibility.  
D. Faster compilation times.  

Question 5: Data Flow Management  
Which strategies prevent data schema coupling across layers? (Choose two)  
A. Sharing DTOs across all layers.  
B. Defining layer-specific data models.  
C. Using protocol buffers for inter-layer communication.  
D. Exposing database entities directly to the UI.  

Question 6: Main Component Testing  
How can the Main component be tested effectively? (Choose two)  
A. Mocking all infrastructure dependencies.  
B. Executing end-to-end tests only.  
C. Validating dependency injection configurations.  
D. Testing business logic within Main.  

Question 7: Bootstrapping Dependencies  
Which techniques align with Clean Architecture for dependency initialization? (Choose two)  
A. Hardcoding concrete implementations in Main.  
B. Using a DI container to resolve dependencies.  
C. Injecting dependencies via constructor parameters.  
D. Embedding configuration logic in domain entities.  

Question 8: Layer Isolation Anti-Patterns  
Which practices violate layer isolation? (Choose two)  
A. UI layer referencing domain interfaces only.  
B. Domain layer importing database adapter code.  
C. Infrastructure layer implementing domain interfaces.  
D. Presentation layer containing business logic.  

Question 9: Cross-Layer Communication  
Which mechanisms allow safe cross-layer communication? (Choose two)  
A. Direct method calls between unrelated layers.  
B. Event-driven architecture with domain events.  
C. Shared global variables for data transfer.  
D. RESTful APIs between bounded contexts.  

Question 10: Main Component Minimalism  
Why should the Main component remain minimal? (Choose two)  
A. To centralize business logic for clarity.  
B. To avoid coupling with volatile infrastructure.  
C. To simplify deployment configurations.  
D. To enable framework-agnostic core logic.  

---

### Answers & Explanations  

1. **A, C**  
   - Compiler-enforced rules (A) and DI containers (C) codify boundaries. Public modules (B) and documentation (D) are ineffective.  

2. **B, D**  
   - Main initializes dependencies (B) and loads configs (D). Business rules (A) and HTTP handling (C) belong to other layers.  

3. **A, C**  
   - Abstract factories (A) and domain interfaces (C) invert dependencies. Direct instantiation (B) and DB schemas in UI (D) violate DIP.  

4. **B, C**  
   - Circular dependencies (B) and inflexible deployment (C) result from poor boundaries. Tight coupling harms testability (A).  

5. **B, C**  
   - Layer-specific models (B) and protocol buffers (C) decouple schemas. Shared DTOs (A) and exposed entities (D) create coupling.  

6. **A, C**  
   - Mocking dependencies (A) and validating DI (C) test Main. Business logic (D) belongs elsewhere.  

7. **B, C**  
   - DI containers (B) and constructor injection (C) align with Clean Architecture. Hardcoding (A) creates rigidity.  

8. **B, D**  
   - Domain layer importing infrastructure (B) and UI logic in presentation layer (D) are anti-patterns.  

9. **B, D**  
   - Event-driven (B) and REST APIs (D) enforce boundaries. Direct calls (A) and globals (C) create coupling.  

10. **B, D**  
    - Minimal Main avoids infrastructure coupling (B) and keeps core framework-agnostic (D).  

---

### Test Code Examples  

Example 1: Layered Data Models  
```java  
// Domain Layer  
public class Order { 
    private String id; 
    private BigDecimal amount; 
    // Getters/setters omitted
}  

// Infrastructure Layer DTO  
public class OrderDocument { 
    private String _id; 
    private String amount; 
    // MongoDB-specific fields
}  

// Adapter converting between domain and infrastructure  
public class OrderMongoMapper { 
    public OrderDocument toDocument(Order order) { /*...*/ } 
    public Order toDomain(OrderDocument doc) { /*...*/ } 
}  
```

Example 2: Main Component with DI  
```java  
public class Main { 
    public static void main(String[] args) { 
        // Initialize dependencies
        OrderRepository repo = new MongoOrderRepository(); 
        PaymentGateway gateway = new StripeGateway(); 
        
        // Inject into core component  
        OrderService service = new OrderService(repo, gateway); 
        service.processOrder(); 
    } 
}  

// Domain interface  
public interface OrderRepository { 
    void save(Order order); 
}  

// Infrastructure implementation  
public class MongoOrderRepository implements OrderRepository { 
    public void save(Order order) { 
        OrderDocument doc = new OrderMongoMapper().toDocument(order); 
        // Save to MongoDB
    } 
}  
```

Example 3: Event-Driven Communication  
```java  
// Domain event  
public class OrderPlacedEvent { 
    private Order order; 
    // Constructor/getter omitted
}  

// Infrastructure listener  
public class EmailNotificationListener { 
    @Subscribe 
    public void handle(OrderPlacedEvent event) { 
        // Send confirmation email
    } 
}  

// Main initializes event bus  
public class Main { 
    public static void main(String[] args) { 
        EventBus bus = new EventBus(); 
        bus.register(new EmailNotificationListener()); 
        
        OrderService service = new OrderService(bus); 
        service.placeOrder(new Order()); 
    } 
}  
```