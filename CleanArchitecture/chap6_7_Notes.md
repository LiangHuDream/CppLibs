### Chapter 6: Functional Programming

#### Key Concepts:
1. **Immutability**: Data cannot be modified after creation; new copies are created instead.
2. **Pure Functions**: Functions with no side effects (same input ⇒ same output).
3. **Higher-Order Functions**: Functions that take other functions as arguments.
4. **Lazy Evaluation**: Delay computation until needed.

---

Code Example 1: Immutable Data Structure (Java)
```java
final class ImmutableOrder {
    private final String id;
    private final double amount;

    public ImmutableOrder(String id, double amount) {
        this.id = id;
        this.amount = amount;
    }

    public ImmutableOrder updateAmount(double newAmount) {
        return new ImmutableOrder(this.id, newAmount);
    }

    public String getId() { return id; }
    public double getAmount() { return amount; }
}

public class Main {
    public static void main(String[] args) {
        ImmutableOrder order = new ImmutableOrder("ORD-123", 100.0);
        ImmutableOrder updatedOrder = order.updateAmount(150.0);
        
        System.out.println("Original: " + order.getAmount()); // 100.0
        System.out.println("Updated: " + updatedOrder.getAmount()); // 150.0
    }
}
```

**Explanation**:
- `ImmutableOrder` cannot be modified after creation.
- `updateAmount()` returns a **new object** instead of mutating state.
- **Test Output** confirms immutability.

---

Code Example 2: Pure Function (Python)
```python
def calculate_tax(price, tax_rate):
    return price * tax_rate  # No side effects

if __name__ == "__main__":
    print(calculate_tax(100, 0.15))  # 15.0 (Always returns same result)
```

**Explanation**:
- No dependency on external state.
- Same input ⇒ predictable output.

---

Code Example 3: Higher-Order Function (JavaScript)
```javascript
// Higher-order function: takes a function as argument
const processOrders = (orders, processor) => orders.map(processor);

const applyDiscount = order => ({ ...order, amount: order.amount * 0.9 });

const orders = [{ id: "A", amount: 100 }, { id: "B", amount: 200 }];
console.log(processOrders(orders, applyDiscount));
// Output: [ { id: 'A', amount: 90 }, { id: 'B', amount: 180 } ]
```

**Explanation**:
- `processOrders` accepts a function (`applyDiscount`) to modify data.

---

### Chapter 7: Single Responsibility Principle (SRP)

Key Concepts:
1. **One Reason to Change**: A class should have only one responsibility.
2. **Separation of Concerns**: Split classes that handle multiple tasks.

---

Code Example 1: Violating SRP (Java)
```java
// Anti-pattern: Handles report generation AND data storage
class Employee {
    private String name;
    private String role;

    public void saveToDatabase() { /* DB logic */ }
    public String generateReport() { /* Report logic */ }
}

public class Main {
    public static void main(String[] args) {
        Employee emp = new Employee();
        emp.saveToDatabase();  // Mixes persistence and reporting
        System.out.println(emp.generateReport());
    }
}
```

**Problem**:
- `Employee` handles **data persistence** AND **report generation**.
- Changing the database schema would force changes to report logic.

---

Code Example 2: Complying with SRP (Java)
```java
// Separate responsibilities into distinct classes
class Employee {
    private String name;
    private String role;
    // Getters only
}

class EmployeeRepository {
    public void save(Employee emp) { /* DB logic */ }
}

class ReportGenerator {
    public String generateReport(Employee emp) { /* Report logic */ }
}

public class Main {
    public static void main(String[] args) {
        Employee emp = new Employee();
        EmployeeRepository repo = new EmployeeRepository();
        ReportGenerator reporter = new ReportGenerator();
        
        repo.save(emp);
        System.out.println(reporter.generateReport(emp));
    }
}
```

**Explanation**:
- `Employee`: Data model only.
- `EmployeeRepository`: Handles **data persistence**.
- `ReportGenerator`: Handles **report logic**.
- Changes to one class don’t affect others.

---

Compilation & Testing

1. **Java**: Use `javac Main.java && java Main` to compile/run.
2. **Python**: Run `python script.py`.
3. **JavaScript**: Execute with Node.js (`node script.js`).

**Output Verification**:
- Immutability examples show unchanged original data.
- SRP examples demonstrate isolated responsibilities.

---

### Summary

- **Functional Programming** emphasizes avoiding mutable state and side effects.
- **SRP** ensures classes have a single responsibility, reducing coupling.
- Always test with `main()` to validate behavior and design.
### Multiple-Choice Questions (Hard Difficulty)

**1. Which of the following are valid strategies to handle mutability in functional programming systems?**  
A. Use global mutable variables for shared state  
B. Segregate components into mutable and immutable layers  
C. Apply event sourcing to store state transitions  
D. Implement all business logic in mutable classes  

**2. Which scenarios indicate a violation of the Single Responsibility Principle (SRP)?**  
A. A class `Employee` with methods `calculatePay()`, `generateReport()`, and `saveToDatabase()`  
B. A module `Logger` handling log formatting and sending logs to cloud storage  
C. A service `PaymentProcessor` that only validates credit card transactions  
D. A class `UserManager` that handles authentication, profile updates, and email notifications  

**3. How does immutability improve architectural design?**  
A. Eliminates all race conditions in distributed systems  
B. Reduces side effects, making code easier to test and debug  
C. Forces all data changes through explicit state transitions  
D. Guarantees 100% thread safety without additional mechanisms  

**4. Which code snippets comply with SRP?**  
A.  
```java
class ReportGenerator {
    void generateReport() { /* ... */ }
    void saveToPDF() { /* ... */ }
}
```  
B.  
```java
class DataFetcher {
    Data fetchData() { /* ... */ }
}
class ReportExporter {
    void exportToPDF(Data data) { /* ... */ }
}
```  
C.  
```java
class TaskHandler {
    void processTask() { /* ... */ }
    void logActivity() { /* ... */ }
}
```  
D.  
```java
class PaymentValidator {
    boolean validatePayment() { /* ... */ }
}
class PaymentExecutor {
    void executePayment() { /* ... */ }
}
```  

**5. What are the risks of violating SRP?**  
A. Increased code duplication  
B. Easier debugging due to centralized logic  
C. Frequent merge conflicts in version control  
D. Improved component reusability  

**6. How does event sourcing align with functional programming principles?**  
A. It relies entirely on mutable database records  
B. It avoids state mutation by storing a log of events  
C. It requires all events to be processed synchronously  
D. It enables rebuilding state without side effects  

**7. Which patterns help invert dependencies to comply with DIP?**  
A. Directly instantiating concrete classes in high-level modules  
B. Using dependency injection frameworks to provide implementations  
C. Defining interfaces in high-level modules implemented by low-level modules  
D. Inheriting from volatile concrete classes  

**8. What makes a class "closed for modification" under OCP?**  
A. Allowing new functionality via inheritance/extension  
B. Requiring changes to existing code for every new feature  
C. Using private methods to hide implementation details  
D. Avoiding interfaces to prevent coupling  

**9. Which practices reduce concurrency issues in functional architectures?**  
A. Using `synchronized` blocks liberally  
B. Isolating mutable state in transactional boundaries  
C. Relying on immutable data structures for core logic  
D. Allowing all threads to modify shared global state  

**10. How does SRP relate to architectural boundaries?**  
A. It encourages grouping code by technical layers (UI, DB)  
B. It mandates separate components for different business actors  
C. It allows mixing unrelated features in a module for convenience  
D. It prevents business rules from depending on frameworks  

---

### Answers & Explanations

**1. B, C**  
- **B** segregates mutability to minimize risks. **C** uses event history for state reconstruction.  
- A violates immutability; D contradicts functional principles.  

**2. A, D**  
- **A** mixes payroll, reporting, and persistence. **D** combines auth, profile, and notifications.  
- B/C have cohesive responsibilities.  

**3. B, C**  
- **B** simplifies testing. **C** ensures controlled state changes.  
- A/D overstate immutability's guarantees.  

**4. B, D**  
- **B/D** separate data fetching, exporting, validation, and execution.  
- A/C bundle unrelated responsibilities.  

**5. A, C**  
- **A** arises from duplicated logic. **C** occurs when multiple teams modify the same class.  
- B/D are incorrect outcomes.  

**6. B, D**  
- **B** uses event logs instead of mutable state. **D** rebuilds state without side effects.  
- A/C contradict event sourcing.  

**7. B, C**  
- **B** injects abstractions. **C** inverts dependencies via interfaces.  
- A/D violate DIP.  

**8. A, C**  
- **A** allows extension. **C** hides details to minimize changes.  
- B/D oppose OCP.  

**9. B, C**  
- **B** confines mutations. **C** uses immutability to prevent races.  
- A/D increase concurrency risks.  

**10. B, D**  
- **B** aligns components with business actors. **D** decouples core logic.  
- A/C violate SRP goals.  

---

### Test Code Examples

**1. Immutable Data Structure (Chapter 6):**  
```java
public final class ImmutablePoint {
    private final double x;
    private final double y;

    public ImmutablePoint(double x, double y) {
        this.x = x;
        this.y = y;
    }

    public double getX() { return x; }
    public double getY() { return y; }

    public static void main(String[] args) {
        ImmutablePoint p = new ImmutablePoint(3, 4);
        System.out.println("Point: (" + p.getX() + ", " + p.getY() + ")");
    }
}
```

**2. SRP-Compliant Classes (Chapter 7):**  
```java
interface DataFetcher {
    String fetchData();
}

class ReportGenerator {
    private DataFetcher fetcher;
    
    public ReportGenerator(DataFetcher fetcher) {
        this.fetcher = fetcher;
    }
    
    public String generate() {
        return "Report: " + fetcher.fetchData();
    }
}

public class Main {
    public static void main(String[] args) {
        DataFetcher fetcher = () -> "Sample Data";
        ReportGenerator generator = new ReportGenerator(fetcher);
        System.out.println(generator.generate());
    }
}
```
