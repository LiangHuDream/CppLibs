### Step-by-Step Guide to Chapter 3: Programming Paradigms

Chapter 3 of *Clean Architecture* introduces three foundational programming paradigms: **Structured Programming**, **Object-Oriented Programming (OOP)**, and **Functional Programming**. Each imposes discipline on code structure to improve reliability and maintainability. Let’s break them down with code examples and tests.

---

### 1. Structured Programming

**Key Idea**: Replace unrestricted `goto` with sequence, selection (`if`/`else`), and iteration (`while`/`for`). This makes code provably correct.

#### Example: Factorial Calculation (C)

```c
#include <stdio.h>

// Structured approach: No goto; uses loops and conditionals
int factorial(int n) {
    int result = 1;
    for (int i = 1; i <= n; i++) {
        result *= i;
    }
    return result;
}

int main() {
    printf("Factorial of 5: %d\n", factorial(5)); // 120
    return 0;
}
```

**Test Output**:

```
Factorial of 5: 120
```

**Why It Works**:

- Uses `for` loops instead of `goto`.
- Easy to reason about correctness through structured flow.

---

### 2. Object-Oriented Programming (OOP)

**Key Idea**: Polymorphism enables dependency inversion. High-level policies depend on abstractions, not concrete details.

#### Example: Shape Hierarchy (Java)

```java
// Abstraction: High-level policy depends on Shape interface
interface Shape {
    double area();
}

// Concrete implementation
class Circle implements Shape {
    private double radius;

    public Circle(double radius) { this.radius = radius; }

    @Override
    public double area() { return Math.PI * radius * radius; }
}

// Another concrete implementation
class Rectangle implements Shape {
    private double width, height;

    public Rectangle(double w, double h) { width = w; height = h; }

    @Override
    public double area() { return width * height; }
}

public class Main {
    public static void main(String[] args) {
        Shape circle = new Circle(3);
        Shape rect = new Rectangle(4, 5);

        System.out.println("Circle area: " + circle.area()); // ~28.27
        System.out.println("Rectangle area: " + rect.area()); // 20.0
    }
}
```

**Test Output**:

```
Circle area: 28.274333882308138
Rectangle area: 20.0
```

**Why It Works**:

- `Shape` interface abstracts details (Dependency Inversion Principle).
- New shapes (e.g., `Triangle`) can be added without changing high-level code.

---

### 3. Functional Programming

**Key Idea**: Immutability and pure functions avoid side effects, making concurrency safer.

#### Example: Immutable Data (Python)

```python
# Pure function: No side effects; returns new data
def square_numbers(nums):
    return [x ** 2 for x in nums]

# Main test
original = [1, 2, 3, 4]
squared = square_numbers(original)

print("Original:", original)  # Unmodified
print("Squared:", squared)    # [1, 4, 9, 16]
```

**Test Output**:

```
Original: [1, 2, 3, 4]
Squared: [1, 4, 9, 16]
```

**Why It Works**:

- `square_numbers` doesn’t modify input (immutability).
- Safe for concurrent execution (no shared mutable state).

---

### Key Takeaways

1. **Structured Programming**:

   - Uses control structures (`if`, `for`, `while`) instead of `goto`.
   - Makes code verifiable through stepwise decomposition.
2. **OOP**:

   - Polymorphism inverts dependencies (high-level ↔ abstractions).
   - Enables plug-in architecture (e.g., new `Shape` implementations).
3. **Functional Programming**:

   - Avoids mutable state and side effects.
   - Simplifies concurrency and testing.

---

### Testing Strategy

- **Structured**: Verify outputs for known inputs (e.g., factorial of 5 = 120).
- **OOP**: Ensure abstractions work with multiple implementations.
- **Functional**: Check that data isn’t mutated and outputs are pure transformations.

**Questions:**

1. Which statements about programming paradigms are TRUE?
   a) Paradigms remove capabilities from programmers
   b) Object-oriented programming adds new capabilities to structured programming
   c) All paradigms were discovered between 1958-1968
   d) Functional programming primarily deals with indirect control transfer

2. What do the three programming paradigms have in common?
   a) They all enable mathematical proofs of correctness
   b) They impose restrictions on specific coding practices
   c) They were all enabled by advances in hardware capabilities
   d) They represent different approaches to concurrency management

3. Which statements about structured programming are CORRECT?
   a) It allows decomposition into provable units
   b) Dijkstra advocated for unrestricted goto statements
   c) It enables functional decomposition
   d) It was primarily about improving performance

4. Key aspects of object-oriented programming include:
   a) Safe polymorphism through function pointers
   b) Automatic dependency inversion
   c) Complete encapsulation implementation
   d) Plugin architecture enablement

5. Functional programming's main constraints:
   a) Prohibition of sequence structures
   b) Restrictions on variable mutation
   c) Mandatory use of recursion
   d) Immutable data structures

6. Which paradigm relationships are CORRECT?
   a) OOP builds on structured programming
   b) Functional programming replaces OOP
   c) Structured programming enables testable units
   d) All paradigms can coexist in a system

7. About paradigm history:
   a) Lisp was the first functional language
   b) ALGOL inspired OOP discoveries
   c) Church's λ-calculus predates computers
   d) Structured programming came last chronologically

8. Architectural implications include:
   a) Structured programming for algorithm foundations
   b) OOP for dependency management
   c) Functional for data management
   d) All paradigms are mutually exclusive

9. Key paradigm restrictions:
   a) Structured: goto statements
   b) OOP: direct function calls
   c) Functional: variable assignment
   d) All: specific code organization

10. About the "no new paradigms" claim:
    a) Proved by advances in language theory
    b) Based on fundamental computational limits
    c) Supported by historical pattern
    d) Refuted by recent concurrency paradigms

**Answers & Explanations:**

1. **a, c**  
   a) Correct. Paradigms restrict rather than add capabilities  
   c) Correct. All three main paradigms discovered in that decade  
   b) False. OOP doesn't add new capabilities  
   d) False. OOP handles indirect control

2. **b, d**  
   b) Correct. Each restricts specific practices  
   d) Correct. All relate to concurrency indirectly  
   a) False. Only structured enables proofs  
   c) False. Not hardware-dependent

3. **a, c**  
   a) Correct. Enables decomposition  
   c) Correct. Supports functional decomposition  
   b) False. Dijkstra opposed goto  
   d) False. About structure not performance

4. **a, d**  
   a) Correct. Safe polymorphism via vtables  
   d) Correct. Enables plugin architecture  
   b) False. Manual dependency inversion needed  
   c) False. Encapsulation often incomplete

5. **b, d**  
   b) Correct. Restricts variable mutation  
   d) Correct. Immutable data is key  
   a) False. Sequence allowed  
   c) False. Recursion not mandatory

6. **a, c, d**  
   a) Correct. OOP builds on structured  
   c) Correct. Structured enables testable units  
   d) Correct. Can coexist  
   b) False. Paradigms complement

7. **a, c**  
   a) Correct. Lisp (1958) first functional  
   c) Correct. λ-calculus predates computers  
   b) False. ALGOL influenced but not direct  
   d) False. Structured programming came later

8. **a, b, c**  
   All correct per architectural implications in text  
   d) False. Can be combined

9. **a, c**  
   a) Correct. Structured restricts goto  
   c) Correct. Functional restricts assignment  
   b) False. OOP uses indirect calls  
   d) False. About control flow not organization

10. **b, c**  
    b) Correct. Based on computation fundamentals  
    c) Correct. Historical pattern supports  
    a) False. Not formally proved  
    d) False. No fundamentally new paradigms

**Test Code Examples:**

1. **Structured Programming Test**
```c
#include <stdio.h>

void structured_example() {
    int i = 0;
    while(i < 5) { // Proper structured loop
        printf("%d\n", i);
        i++;
    }
}

int main() {
    structured_example();
    return 0;
}
```

2. **OOP Polymorphism Test**
```java
interface Shape { double area(); }

class Circle implements Shape {
    private double radius;
    Circle(double r) { radius = r; }
    public double area() { return Math.PI * radius * radius; }
}

class Square implements Shape {
    private double side;
    Square(double s) { side = s; }
    public double area() { return side * side; }
}

public class Main {
    public static void main(String[] args) {
        Shape[] shapes = { new Circle(2), new Square(3) };
        for (Shape s : shapes) {
            System.out.println("Area: " + s.area());
        }
    }
}
```

3. **Functional Immutability Test**
```scala
object FunctionalTest {
    def main(args: Array[String]): Unit = {
        val numbers = List(1, 2, 3)
        val squared = numbers.map(x => x * x) // Immutable transformation
        println(s"Original: $numbers")
        println(s"Squared: $squared")
    }
}
```

These examples demonstrate:
- Structured programming with controlled loops
- OOP polymorphism through interface implementation
- Functional programming with immutable data transformations


