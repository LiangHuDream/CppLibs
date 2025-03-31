### Chapter 23: Presenters and Humble Objects

**Key Concepts**
1. **Humble Object Pattern**:
   - Separates logic that is hard to test (e.g., UI, I/O) from business logic.
   - Core idea: Move testable logic into "humble" components with minimal behavior.

2. **Presenter**:
   - Acts as an intermediary between business logic and UI.
   - Converts business data into a format suitable for display (e.g., HTML, JSON).
   - Ensures UI remains "humble" (no business logic).

---

**Code Example: Presenter + View**
```cpp
#include <iostream>
#include <string>

// Model (Business Data)
struct Order {
    std::string id;
    double amount;
};

// View Interface (Humble Object)
class OrderView {
public:
    virtual void display(const std::string& formattedData) = 0;
    virtual ~OrderView() = default;
};

// Presenter (Converts Model to View-friendly format)
class OrderPresenter {
public:
    explicit OrderPresenter(OrderView& view) : view(view) {}

    void present(const Order& order) {
        std::string formatted = "Order ID: " + order.id + ", Amount: $" + std::to_string(order.amount);
        view.display(formatted);
    }

private:
    OrderView& view;
};

// Concrete View Implementation (Console)
class ConsoleOrderView : public OrderView {
public:
    void display(const std::string& formattedData) override {
        std::cout << "Displaying Order: " << formattedData << std::endl;
    }
};

// Test Case
int main() {
    ConsoleOrderView view;
    OrderPresenter presenter(view);
    Order order{"12345", 99.99};
    presenter.present(order);  // Output: "Displaying Order: Order ID: 12345, Amount: $99.99"
    return 0;
}
```

**Explanation**
- **Humble Object**: `ConsoleOrderView` has no business logic; it only displays formatted data.
- **Presenter**: `OrderPresenter` converts `Order` to a string and delegates display to the view.
- **Testability**: Business logic (formatting) is tested via `OrderPresenter`, while `ConsoleOrderView` is trivially simple.

---

### Chapter 24: Partial Boundaries

**Key Concepts**
1. **Partial Boundaries**:
   - Trade-off between full architectural boundaries (costly) and no boundaries (inflexible).
   - Strategies:
     - **Skipping the Last Step**: Define interfaces but defer full implementation.
     - **Facades**: Create simplified interfaces to subsystems.

2. **Use Cases**:
   - Early project phases where full boundaries are premature.
   - Integrating third-party systems incrementally.

---

**Code Example: Facade Pattern (Partial Boundary)**
```cpp
#include <iostream>
#include <memory>

// Subsystem: Payment Processing
class PaymentProcessor {
public:
    virtual void processPayment(double amount) = 0;
    virtual ~PaymentProcessor() = default;
};

// Partial Boundary: Facade for Payment + Logistics (incomplete)
class OrderFacade {
public:
    explicit OrderFacade(std::unique_ptr<PaymentProcessor> processor)
        : paymentProcessor(std::move(processor)) {}

    void executeOrder(double amount) {
        paymentProcessor->processPayment(amount);
        // Logistics not yet implemented (partial boundary)
        std::cout << "Logistics pending..." << std::endl;
    }

private:
    std::unique_ptr<PaymentProcessor> paymentProcessor;
};

// Concrete Payment Processor
class PayPalProcessor : public PaymentProcessor {
public:
    void processPayment(double amount) override {
        std::cout << "Processing $" << amount << " via PayPal." << std::endl;
    }
};

// Test Case
int main() {
    auto processor = std::make_unique<PayPalProcessor>();
    OrderFacade facade(std::move(processor));
    facade.executeOrder(150.0);  // Output: "Processing $150 via PayPal. Logistics pending..."
    return 0;
}
```

**Explanation**
- **Partial Boundary**: `OrderFacade` integrates payment processing but defers logistics (a placeholder).
- **Flexibility**: Allows incremental development without full upfront costs.
- **Testability**: Payment logic is testable; logistics can be mocked/stubbed.

---

**Testing & Compilation**
1. **Compilation**:
   - Use `g++ -std=c++17 -o example example.cpp` (adjust filenames as needed).
2. **Test Outputs**:
   - **Chapter 23**: Validates formatted string generation and display.
   - **Chapter 24**: Confirms payment processing works while logistics are pending.

**Summary**
- **Chapter 23**: Decouple UI from business logic using Presenters; keep views "humble."
- **Chapter 24**: Use facades or deferred implementations to manage complexity in evolving systems.

---

### Multiple-Choice Questions (Hard Difficulty)

1. **Humble Object Pattern Application**
Which components are **valid examples** of the Humble Object pattern?  
A) A database gateway that executes raw SQL queries  
B) A Presenter class converting domain data to UI models  
C) A View component rendering HTML without business logic  
D) A service listener handling HTTP request parsing  

**Code Snippet**:
```java
// Humble Object: Simple View
class ConsoleView {
    void display(String data) {
        System.out.println(data);
    }
}
```

---

2. **Presenter Responsibilities**
What are **valid responsibilities** of a Presenter?  
A) Executing business rules for payment processing  
B) Converting domain entities to JSON for API responses  
C) Validating user input formats  
D) Formatting dates for UI display  

**Code Snippet**:
```python
# Presenter converts data for View
class ReportPresenter:
    def format_report(self, report_data):
        return f"Report: {report_data['title']} (${report_data['amount']})"
```

---

3. **Partial Boundary Tradeoffs**
Which statements about partial boundaries are **true**?  
A) They eliminate the need for future architectural changes.  
B) They reduce initial development costs but increase technical debt.  
C) Facade patterns are a type of partial boundary implementation.  
D) They provide the same flexibility as full boundaries.  

**Code Snippet**:
```csharp
// Facade as a partial boundary
public class OrderFacade {
    private OrderService _service = new OrderService();
    public void ProcessOrder(Order order) {
        _service.Validate(order);
        _service.Save(order);
    }
}
```

---

4. **Testable Architecture Design**
Which strategies align with testable architectures?  
A) Keeping business logic in Humble Objects  
B) Using Presenters to isolate view-related transformations  
C) Embedding SQL queries directly in UI controllers  
D) Mocking database gateways during unit tests  

**Code Snippet**:
```java
// Testable Presenter with mocked dependencies
@Test
void testReportPresenter() {
    var presenter = new ReportPresenter();
    String result = presenter.formatReport(new ReportData("Q1", 5000));
    assertEquals("Report: Q1 ($5000)", result);
}
```

---

5. **Boundary Identification**
Which scenarios **require** a full architectural boundary?  
A) A module needing cross-team collaboration  
B) A component with high volatility and frequent changes  
C) A third-party library with a stable API  
D) A legacy system requiring future replacement  

---

6. **Facade Pattern Limitations**
What are **limitations** of using Facade for partial boundaries?  
A) Tight coupling between Facade and external systems  
B) Inability to support future extensibility  
C) Increased runtime performance overhead  
D) Hidden dependencies on concrete implementations  

**Code Snippet**:
```python
# Facade with hidden dependency on PaymentService
class PaymentFacade:
    def __init__(self):
        self._payment_service = PaymentService()
    
    def process(self, amount):
        self._payment_service.charge(amount)
```

---

7. **Humble Object Characteristics**
Which are **true** about Humble Objects?  
A) They contain complex state management logic.  
B) They minimize dependencies on external frameworks.  
C) They are primary targets for unit testing.  
D) They delegate business logic to other components.  

---

8. **When to Use Partial Boundaries**
Partial boundaries are **most appropriate** when:  
A) The system requires immediate full scalability.  
B) Development time is limited, and upfront costs must be minimized.  
C) The team anticipates frequent changes to component interfaces.  
D) Long-term maintenance is the highest priority.  

---

9. **Dependency Risks in Partial Boundaries**
Which risks arise from one-dimensional boundaries?  
A) Inability to test components in isolation  
B) Implicit coupling between components  
C) Increased deployment complexity  
D) Difficulty replacing underlying implementations  

**Code Snippet**:
```java
// One-dimensional boundary with direct dependency
class OrderProcessor {
    private ShippingService shippingService = new FedExShippingService();
    void process(Order order) {
        shippingService.ship(order);
    }
}
```

---

10. **Refactoring Partial Boundaries**
Which techniques help transition partial boundaries to full boundaries?  
A) Introducing dependency injection containers  
B) Replacing facades with abstract factories  
C) Splitting modules into independent deployable units  
D) Adding interface layers between components  

---

### Answers & Explanations

1. **B, C, D**  
   - **B/C/D**: Presenters, Views, and service listeners are Humble Objects.  
   - **A**: Database gateways handle complex logic (not Humble Objects).  

2. **B, D**  
   - Presenters format data for views but don’t execute business rules (A) or validate inputs (C).  

3. **B, C**  
   - Partial boundaries save costs but add technical debt (B). Facades are a valid implementation (C).  

4. **B, D**  
   - Presenters enable testing (B), and mocking dependencies (D) is key.  

5. **A, B, D**  
   - High volatility (B) and cross-team work (A) demand full boundaries. Legacy systems (D) may need isolation.  

6. **A, D**  
   - Facades hide concrete dependencies (D) and create coupling (A).  

7. **B, D**  
   - Humble Objects avoid frameworks (B) and delegate logic (D).  

8. **B**  
   - Partial boundaries suit limited resources (B), not long-term needs (D).  

9. **B, D**  
   - One-dimensional boundaries create coupling (B) and hinder replacement (D).  

10. **A, C, D**  
    - Dependency injection (A), modular splits (C), and interfaces (D) enable full boundaries.  

---

### Compilable Test Code Examples

1. Humble Object (Java)
```java
interface DataExporter {
    String export(String data);
}

class CSVExporter implements DataExporter {
    public String export(String data) {
        return "CSV: " + data; // Simple formatting
    }
}

class ReportGenerator {
    private DataExporter exporter;
    public ReportGenerator(DataExporter exporter) {
        this.exporter = exporter;
    }
    public String generateReport(String rawData) {
        return exporter.export(rawData.toUpperCase());
    }
}
```

2. Facade Pattern (Python)
```python
class InventoryService:
    def check_stock(self, product_id):
        return 100  # Simulated stock check

class OrderFacade:
    def __init__(self):
        self.inventory = InventoryService()
    
    def place_order(self, product_id, quantity):
        if self.inventory.check_stock(product_id) >= quantity:
            return "Order placed"
        else:
            return "Insufficient stock"
```

3. Partial Boundary with Interface (C#)
```csharp
public interface IShippingService {
    void Ship(Order order);
}

public class FedExShipping : IShippingService {
    public void Ship(Order order) {
        Console.WriteLine($"Shipping via FedEx: {order.Id}");
    }
}

public class OrderProcessor {
    private IShippingService shipper;
    public OrderProcessor(IShippingService shipper) {
        this.shipper = shipper;
    }
    public void Process(Order order) {
        shipper.Ship(order);
    }
}
```
