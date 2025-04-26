### Chapters 8 and 9: OCP and LSP

#### **Chapter 8: Open-Closed Principle (OCP)**
**Key Concept**:  
Software entities (classes, modules, etc.) should be **open for extension** but **closed for modification**. New functionality should be added by extending existing code, not modifying it.

---

#### **1. Problem: Violating OCP**
Imagine a system that generates reports in HTML. Adding a PDF report forces changes to existing code.

**Violation Example**:
```cpp
#include <iostream>
#include <string>

class ReportGenerator {
public:
    void generateHTML(const std::string& data) {
        std::cout << "Generating HTML: " << data << std::endl;
    }
};

class ReportController {
public:
    void createReport(ReportGenerator& generator, const std::string& data) {
        generator.generateHTML(data); // Hardcoded to HTML
    }
};

int main() {
    ReportGenerator generator;
    ReportController controller;
    controller.createReport(generator, "Sales Data");
    return 0;
}
```
**Output**:
```
Generating HTML: Sales Data
```

**Issue**: Adding PDF support requires modifying `ReportController`.

---

#### **2. Solution: Adhering to OCP**
Use **abstractions** (interfaces) to decouple high-level logic from low-level details.

**Refactored Code**:
```cpp
#include <iostream>
#include <string>
#include <memory>

// Abstract interface
class ReportGenerator {
public:
    virtual ~ReportGenerator() = default;
    virtual void generate(const std::string& data) = 0;
};

// Concrete implementations
class HTMLGenerator : public ReportGenerator {
public:
    void generate(const std::string& data) override {
        std::cout << "Generating HTML: " << data << std::endl;
    }
};

class PDFGenerator : public ReportGenerator {
public:
    void generate(const std::string& data) override {
        std::cout << "Generating PDF: " << data << std::endl;
    }
};

// High-level controller depends on abstraction
class ReportController {
public:
    void createReport(ReportGenerator& generator, const std::string& data) {
        generator.generate(data);
    }
};

int main() {
    HTMLGenerator html;
    PDFGenerator pdf;
    ReportController controller;

    controller.createReport(html, "Sales Data"); // HTML
    controller.createReport(pdf, "Sales Data");   // PDF (no code change!)
    return 0;
}
```
**Output**:
```
Generating HTML: Sales Data
Generating PDF: Sales Data
```

**Key Points**:
- `ReportController` depends on `ReportGenerator` (abstraction).
- New formats (e.g., PDF) are added by extending `ReportGenerator`.

---

### **Chapter 9: Liskov Substitution Principle (LSP)**
**Key Concept**:  
Subtypes must be substitutable for their base types without altering program correctness.

---

#### **1. Problem: Violating LSP**
A `Square` subclassing `Rectangle` breaks behavior when dimensions change.

**Violation Example**:
```cpp
#include <iostream>
#include <cassert>

class Rectangle {
protected:
    int width, height;
public:
    Rectangle(int w, int h) : width(w), height(h) {}
    virtual void setWidth(int w) { width = w; }
    virtual void setHeight(int h) { height = h; }
    int area() const { return width * height; }
};

class Square : public Rectangle {
public:
    Square(int size) : Rectangle(size, size) {}
    void setWidth(int w) override {
        width = height = w; // Forced to keep square
    }
    void setHeight(int h) override {
        width = height = h;
    }
};

void testArea(Rectangle& rect) {
    rect.setWidth(5);
    rect.setHeight(2);
    assert(rect.area() == 10); // Fails for Square!
}

int main() {
    Rectangle rect(5, 2);
    Square square(5);
    
    testArea(rect);   // Passes
    // testArea(square); // Fails assertion (violates LSP)
    return 0;
}
```
**Issue**: `Square` alters expected behavior of `Rectangle`.

---

#### **2. Solution: Adhering to LSP**
Avoid inheritance hierarchies where subtypes change base behavior. Use **composition** or **interfaces**.

**Refactored Code**:
```cpp
#include <iostream>
#include <memory>

// Shape interface
class Shape {
public:
    virtual ~Shape() = default;
    virtual int area() const = 0;
};

// Concrete implementations
class Rectangle : public Shape {
    int width, height;
public:
    Rectangle(int w, int h) : width(w), height(h) {}
    void setWidth(int w) { width = w; }
    void setHeight(int h) { height = h; }
    int area() const override { return width * height; }
};

class Square : public Shape {
    int size;
public:
    Square(int s) : size(s) {}
    void setSize(int s) { size = s; }
    int area() const override { return size * size; }
};

void printArea(const Shape& shape) {
    std::cout << "Area: " << shape.area() << std::endl;
}

int main() {
    Rectangle rect(5, 2);
    Square square(5);

    printArea(rect);   // Area: 10
    printArea(square); // Area: 25
    return 0;
}
```
**Output**:
```
Area: 10
Area: 25
```

**Key Points**:
- `Rectangle` and `Square` implement `Shape` without inheritance.
- No unexpected side effects when substituting types.

---

### **Summary**
- **OCP**: Depend on abstractions to avoid modifying existing code.
- **LSP**: Ensure substitutability by preserving behavioral contracts.
- Use interfaces and composition to decouple components.
### Chapter 8 & 9 Focus Areas:
- **Open-Closed Principle (OCP)**: Architectural strategies for extension vs. modification
- **Liskov Substitution Principle (LSP)**: Interface contracts and substitution validity
- Dependency inversion patterns
- Component hierarchy design
- Interface segregation techniques

---

### Multiple-Choice Questions on OCP and LSP 

---

#### **Question 1**  
Which of the following code snippets **violate** the Open-Closed Principle (OCP)?  

```java
// Option A
interface Shape { double area(); }
class Circle implements Shape { /*...*/ }
class Square implements Shape { /*...*/ }

// Option B
class ReportGenerator {
    void generatePDF(Data data) { /*...*/ }
    void generateCSV(Data data) { /*...*/ } // Added later
}

// Option C
interface Report { void generate(Data data); }
class PDFReport implements Report { /*...*/ }
class CSVReport implements Report { /*...*/ }

// Option D
class PaymentProcessor {
    void process(Payment payment) {
        if (payment.type == "CreditCard") { /*...*/ }
        else if (payment.type == "PayPal") { /*...*/ }
    }
}
```

---

#### **Question 2**  
Which scenarios describe a **valid application** of the Liskov Substitution Principle (LSP)?  

```java
// Option A: Square extends Rectangle
class Rectangle { 
    void setWidth(int w) { /*...*/ } 
    void setHeight(int h) { /*...*/ } 
}
class Square extends Rectangle {
    void setWidth(int w) { setHeight(w); }
}

// Option B: Ostrich extends Bird (Bird has fly())
class Ostrich extends Bird { 
    void fly() { throw new UnsupportedOperationException(); } 
}

// Option C: ReadOnlyList implements List
class ReadOnlyList implements List {
    void add(Object item) { throw new UnsupportedException(); }
}

// Option D: ElectricCar extends Car
class Car { void refuel() { /*...*/ } }
class ElectricCar extends Car { void refuel() { recharge(); } }
```

---

#### **Question 3**  
Which design patterns help **enforce the OCP**?  

1. Strategy Pattern  
2. Singleton Pattern  
3. Template Method Pattern  
4. Facade Pattern  

---

#### **Question 4**  
Which code snippets **preserve LSP** when substituting a subclass for its superclass?  

```java
// Option A
class Animal { void speak() { /* default silence */ } }
class Dog extends Animal { void speak() { bark(); } }

// Option B
class Stack extends ArrayList {
    void push(Object item) { add(item); }
    Object pop() { return remove(size()-1); }
}

// Option C
class ImmutableList extends List {
    void add(Object item) { throw new UnsupportedException(); }
}

// Option D
class Bird { void fly() { /*...*/ } }
class Penguin extends Bird { void fly() { /* do nothing */ } }
```

---

#### **Question 5**  
How does violating LSP impact **architectural boundaries**?  

1. Increases modularity  
2. Introduces unexpected runtime errors  
3. Forces changes across multiple layers  
4. Reduces test coverage  

---

#### **Question 6**  
Which principles are **directly related** to OCP?  

1. Dependency Inversion Principle  
2. Interface Segregation Principle  
3. Single Responsibility Principle  
4. Composite Reuse Principle  

---

#### **Question 7**  
Which code changes **adhere to OCP**?  

```java
// Original
class Logger {
    void logToFile(String msg) { /*...*/ }
}

// Option A: Add a new method logToDatabase()
class Logger {
    void logToFile(String msg) { /*...*/ }
    void logToDatabase(String msg) { /*...*/ }
}

// Option B: Extract an interface
interface Logger { void log(String msg); }
class FileLogger implements Logger { /*...*/ }
class DatabaseLogger implements Logger { /*...*/ }

// Option C: Modify logToFile() to support encryption
class Logger {
    void logToFile(String msg, boolean encrypt) { /*...*/ }
}
```

---

#### **Question 8**  
Which statements about **LSP violations** are true?  

1. They always cause compile-time errors.  
2. They can lead to broken preconditions/postconditions.  
3. They are acceptable if the subclass is rarely used.  
4. They complicate polymorphism.  

---

#### **Question 9**  
Which code uses **abstractions to enforce OCP**?  

```java
// Option A
class PaymentProcessor {
    void process(PaymentStrategy strategy) { strategy.execute(); }
}

// Option B
class ReportService {
    private PDFGenerator pdfGenerator = new PDFGenerator();
    void generate() { pdfGenerator.generate(); }
}

// Option C
class DataExporter {
    void export(Format format) {
        if (format == Format.CSV) { /*...*/ }
        else if (format == Format.XML) { /*...*/ }
    }
}
```

---

#### **Question 10**  
Which examples **preserve LSP**?  

```java
// Option A: Covariant return types
class Super { Animal getPet() { return new Animal(); } }
class Sub extends Super { Dog getPet() { return new Dog(); } }

// Option B: Strengthened preconditions
class Super { void save(int value) { /*...*/ } }
class Sub extends Super { void save(int value) { 
    if (value < 0) throw new Exception(); 
    super.save(value);
}}

// Option C: Weakened postconditions
class Super { List<String> getItems() { return unmodifiableList; } }
class Sub extends Super { List<String> getItems() { return mutableList; } }

// Option D: No exception throwing in subclass
class Super { void load() throws IOException { /*...*/ } }
class Sub extends Super { void load() { /*...*/ } }
```

---

### Answers and Explanations  

1. **B, D**  
   - **B** violates OCP by modifying `ReportGenerator` to add `generateCSV()`.  
   - **D** violates OCP by using conditional logic for payment types.  

2. **A, D**  
   - **A**: `Square` violates LSP if `setWidth()` changes height (breaks invariants).  
   - **D**: `ElectricCar` safely substitutes `Car` if `recharge()` is equivalent to `refuel()`.  

3. **1, 3**  
   - Strategy and Template Method patterns allow extending behavior without modifying existing code.  

4. **A**  
   - **A**: `Dog` strengthens `Animal` without violating contracts. **B/C/D** violate LSP by breaking superclass invariants.  

5. **2, 3**  
   - LSP violations propagate errors across layers and force cascading changes.  

6. **1, 2**  
   - Dependency Inversion and Interface Segregation directly support OCP.  

7. **B**  
   - Extracting an interface (`Logger`) allows new implementations without modifying existing code.  

8. **2, 4**  
   - LSP violations break contracts and complicate polymorphic behavior.  

9. **A**  
   - `PaymentProcessor` depends on the `PaymentStrategy` abstraction.  

10. **A, D**  
    - **A**: Covariant return types are allowed. **D**: Subclass removes exception (weaker postcondition).  

---

### Test Cases (Java)  

#### LSP Test for `ElectricCar`:  
```java
public class Main {
    public static void main(String[] args) {
        Car car = new ElectricCar();
        car.refuel(); // Should call recharge()
    }
}
```

#### OCP Test for `Logger`:  
```java
public class Main {
    public static void main(String[] args) {
        Logger logger = new DatabaseLogger();
        logger.log("test"); // No changes to existing code
    }
}
``` 
