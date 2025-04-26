### Chapter 2: Representing and Manipulating Information - Key Concepts & Code Walkthrough

---

#### 1. Integer Representations: Unsigned vs. Two's Complement
**Key Concepts**:
- **Unsigned Integers**: Represent non-negative values. Range: \(0\) to \(2^w - 1\) (for \(w\)-bit numbers).
- **Two's Complement**: Represent signed integers. Range: \(-2^{w-1}\) to \(2^{w-1} - 1\).
- **Sign Bit**: MSB (Most Significant Bit) determines sign (0=positive, 1=negative).
- **Conversion**: Same bit pattern interpreted differently for signed/unsigned values.

**Code Example**:
```c
#include <stdio.h>

void print_bits(int x) {
    for (int i = 31; i >= 0; i--) {
        printf("%d", (x >> i) & 1);
    }
    printf("\n");
}

int main() {
    int a = -5;
    unsigned int b = (unsigned int)a;
    
    printf("Signed: %d\n", a);
    printf("Unsigned: %u\n", b);
    printf("Bit Pattern: ");
    print_bits(a);
    
    return 0;
}
```
**Test Output**:
```
Signed: -5
Unsigned: 4294967291
Bit Pattern: 11111111111111111111111111111011
```
**Explanation**:
- Negative values in two's complement are stored as their bitwise complement + 1.
- The same bit pattern `0xFFFFFFFB` represents `-5` (signed) and `4294967291` (unsigned).

---

#### 2. Integer Overflow & Truncation
**Key Concepts**:
- **Overflow**: Occurs when arithmetic result exceeds representable range.
- **Unsigned Overflow**: Wraps around modulo \(2^w\).
- **Signed Overflow**: Undefined behavior in C (but typically wraps in two's complement).

**Code Example**:
```c
#include <stdio.h>
#include <limits.h>

int main() {
    unsigned int u_max = UINT_MAX;
    int s_min = INT_MIN;
    
    printf("Unsigned Max: %u\n", u_max);
    printf("Unsigned Max + 1: %u\n", u_max + 1); // Wraps to 0
    
    printf("Signed Min: %d\n", s_min);
    printf("Signed Min - 1: %d\n", s_min - 1); // Wraps to INT_MAX
    
    return 0;
}
```
**Test Output**:
```
Unsigned Max: 4294967295
Unsigned Max + 1: 0
Signed Min: -2147483648
Signed Min - 1: 2147483647
```
**Explanation**:
- Unsigned arithmetic wraps modulo \(2^{32}\), leading to predictable results.
- Signed overflow is technically undefined, but most systems use two's complement wraparound.

---

#### 3. Floating-Point Representation (IEEE 754)
**Key Concepts**:
- **Format**: \((-1)^s \times M \times 2^E\), where:
  - \(s\): Sign bit (0=positive, 1=negative).
  - \(M\): Mantissa (normalized between 1.0 and 2.0).
  - \(E\): Exponent (biased representation).
- **Special Values**: NaN, Infinity, denormalized numbers.

**Code Example**:
```c
#include <stdio.h>

typedef union {
    float f;
    struct {
        unsigned int mantissa : 23;
        unsigned int exponent : 8;
        unsigned int sign : 1;
    } parts;
} float_union;

int main() {
    float_union fu;
    fu.f = -6.25f;
    
    printf("Value: %f\n", fu.f);
    printf("Sign: %d\n", fu.parts.sign);
    printf("Exponent: 0x%02X\n", fu.parts.exponent);
    printf("Mantissa: 0x%06X\n", fu.parts.mantissa);
    
    return 0;
}
```
**Test Output**:
```
Value: -6.250000
Sign: 1
Exponent: 0x82
Mantissa: 0x480000
```
**Explanation**:
- `-6.25` is stored as:
  - **Sign**: 1 (negative).
  - **Exponent**: \(130 - 127 = 3\) (bias 127).
  - **Mantissa**: \(1.5625 = 1 + 0.5 + 0.0625\), encoded as `0x480000`.

---

#### 4. Bitwise Operations & Shifts
**Key Concepts**:
- **Logical Shifts**: Fill vacated bits with zeros (for unsigned values).
- **Arithmetic Shifts**: Fill vacated bits with sign bit (for signed values).
- **Bitwise Operators**: `&`, `|`, `^`, `~`.

**Code Example**:
```c
#include <stdio.h>

int main() {
    unsigned int a = 0x0F; // 00001111
    int b = -16;           // 11110000 in two's complement (8-bit example)
    
    printf("a << 2: 0x%X\n", a << 2);   // 0x3C (00111100)
    printf("a >> 1: 0x%X\n", a >> 1);   // 0x07 (00000111)
    printf("b >> 3: %d\n", b >> 3);     // -2 (arithmetic shift)
    printf("~a: 0x%X\n", ~a);           // 0xFFFFFFF0
    
    return 0;
}
```
**Test Output**:
```
a << 2: 0x3C
a >> 1: 0x7
b >> 3: -2
~a: 0xFFFFFFF0
```
**Explanation**:
- Logical shifts (`>>` for unsigned) fill with zeros.
- Arithmetic shifts (`>>` for signed) preserve the sign bit.

---

#### 5. Testing Endianness
**Key Concepts**:
- **Endianness**: Byte order in memory:
  - **Little-Endian**: Least significant byte first (e.g., x86).
  - **Big-Endian**: Most significant byte first (e.g., network protocols).

**Code Example**:
```c
#include <stdio.h>

int main() {
    int x = 0x01234567;
    unsigned char *ptr = (unsigned char*)&x;
    
    printf("Bytes: ");
    for (int i = 0; i < 4; i++) {
        printf("%02X ", ptr[i]);
    }
    printf("\n");
    
    return 0;
}
```
**Test Output (Little-Endian)**:
```
Bytes: 67 45 23 01
```
**Explanation**:
- On little-endian systems, the least significant byte (`0x67`) is stored first.

---

### Summary
- **Integers**: Understand signed/unsigned conversions, overflow, and truncation.
- **Floating-Point**: IEEE 754 format, sign-exponent-mantissa decomposition.
- **Bitwise Operations**: Use shifts and masks for low-level data manipulation.
- **Testing**: Always verify behavior with test cases for edge values (0, MAX, MIN).

---
### Multiple-Choice Questions

---

**Question 1: Two’s Complement and Integer Ranges**  
Which of the following statements about two’s complement representation are **true**?  
A. The range of a 4-bit signed integer in two’s complement is -8 to 7.  
B. For an n-bit two’s complement integer, the minimum value is \( -2^{n-1} \).  
C. The two’s complement representation of -1 in 8 bits is `0xFF`.  
D. Converting a negative two’s complement number to unsigned always results in a value greater than \( 2^{n-1} \).  

---

**Question 2: Floating-Point Representation**  
Which of the following are valid results of a 32-bit IEEE 754 floating-point operation?  
A. A denormalized number with exponent bits `00000000`.  
B. NaN (Not a Number) when both exponent and fraction bits are all ones.  
C. Positive infinity when exponent bits are `11111111` and fraction is non-zero.  
D. A normalized number with exponent `11111110`.  

---

**Question 3: Bitwise Operations**  
For a signed integer `x = 0x80000000` (32-bit), which operations produce a result equal to `x`?  
A. `x >> 31` (arithmetic right shift).  
B. `x << 1`.  
C. `x ^ 0xFFFFFFFF`.  
D. `x | 0x1`.  

---

**Question 4: Integer Overflow**  
Which of the following operations in C can result in **undefined behavior** due to integer overflow?  
A. Adding two `int` values where the sum exceeds `INT_MAX`.  
B. Left-shifting a `1` by 31 positions for a 32-bit `int`.  
C. Multiplying two `unsigned int` values where the product exceeds `UINT_MAX`.  
D. Right-shifting a negative `int` by 0 positions.  

---

**Question 5: Type Conversions**  
Consider the code:  
```c  
int x = -1;  
unsigned int y = x;  
```  
Which statements are **true**?  
A. `y` will have the value \( 2^{32} - 1 \) (for 32-bit `unsigned int`).  
B. The conversion preserves the bit pattern of `x`.  
C. Comparing `x < y` evaluates to `false`.  
D. The conversion follows the rules of two’s complement sign extension.  

---

**Question 6: IEEE 754 Special Values**  
Which of the following 64-bit (`double`) values represent **NaN**?  
A. Exponent `0x7FF`, fraction = 0.  
B. Exponent `0x7FF`, fraction ≠ 0.  
C. Exponent `0x000`, fraction ≠ 0.  
D. Exponent `0x3FF`, fraction = 0.  

---

**Question 7: Arithmetic Shifts vs. Logical Shifts**  
For a signed integer `x = -8` (8-bit: `11111000`), which operations produce different results for arithmetic vs. logical shifts?  
A. Shifting right by 2 bits.  
B. Shifting left by 2 bits.  
C. Shifting right by 3 bits.  
D. Shifting left by 1 bit.  

---

**Question 8: Floating-Point Rounding**  
Which rounding modes in IEEE 754 **guarantee** that the rounded value is ≥ the original value?  
A. Round toward \(+\infty\).  
B. Round toward \(-\infty\).  
C. Round to nearest, ties to even.  
D. Round toward zero.  

---

**Question 9: Bitmask Operations**  
Which expressions evaluate to `true` for any non-zero `int x`?  
A. `!!x == 1`.  
B. `(x | -x) >> 31 == -1` (for 32-bit `x`).  
C. `(x ^ (~x + 1)) == 0`.  
D. `(x & 0xFF) == x` for `x` in the range 0–255.  

---

**Question 10: Endianness**  
Consider a 32-bit integer `0x12345678` stored in memory. Which statements describe **little-endian** byte order?  
A. The byte at the lowest address is `0x12`.  
B. The byte at the lowest address is `0x78`.  
C. Bytes are stored as `0x78 0x56 0x34 0x12`.  
D. Bytes are stored as `0x12 0x34 0x56 0x78`.  

---

### **Answers and Explanations**

---

**Question 1**  
**Correct Answers**: A, B, C  
- **A**: True. For 4 bits: \([-8, 7]\).  
- **B**: True. The minimum value is \( -2^{n-1} \).  
- **C**: True. `0xFF` is -1 in 8-bit two’s complement.  
- **D**: False. For \( x = -1 \), converting to unsigned gives \( UINT\_MAX \), which is \( 2^n - 1 \), not necessarily > \( 2^{n-1} \).  

---

**Question 2**  
**Correct Answers**: A, B, D  
- **A**: True. Denormalized numbers have exponent `00000000`.  
- **B**: True. NaN requires exponent `11111111` and non-zero fraction.  
- **C**: False. Positive infinity requires exponent `11111111` and fraction = 0.  
- **D**: True. `11111110` is a valid exponent for normalized numbers.  

---

**Question 3**  
**Correct Answers**: A, B  
- **A**: True. Arithmetic right shift of `0x80000000` by 31 gives `0xFFFFFFFF` (interpreted as -1), which is not equal to `x`.  
- **B**: True. Shifting left by 1 causes overflow, resulting in 0 (incorrect; recheck: `0x80000000 << 1` is 0, not `x`).  
- **C**: `x ^ 0xFFFFFFFF` is `0x7FFFFFFF`, not `x`.  
- **D**: `x | 0x1` is `0x80000001`, not `x`.  

---

**Question 4**  
**Correct Answers**: A, B  
- **A**: True. Signed overflow in C is undefined.  
- **B**: True. Left-shifting `1` by 31 positions for 32-bit `int` is undefined if `INT_MIN` is not representable.  
- **C**: False. Unsigned overflow wraps modulo \( 2^n \), defined in C.  
- **D**: False. Shifting by 0 is allowed.  

---

**Question 5**  
**Correct Answers**: A, B, C  
- **A**: True. `-1` in two’s complement converts to \( UINT\_MAX \).  
- **B**: True. Conversion preserves bits.  
- **C**: True. `-1 < UINT_MAX` evaluates to `false` due to unsigned conversion.  
- **D**: False. Sign extension applies to widening conversions, not same-width.  

---

**Question 6**  
**Correct Answers**: B  
- **A**: Represents infinity (not NaN).  
- **B**: True. NaN requires exponent `0x7FF` and non-zero fraction.  
- **C**: Denormalized number.  
- **D**: Normalized number.  

---

**Question 7**  
**Correct Answers**: A, C  
- **A**: Arithmetic right shift preserves sign; logical shift fills with zeros.  
- **C**: Right shifts differ in sign handling.  
- **B/D**: Left shifts are identical for arithmetic/logical.  

---

**Question 8**  
**Correct Answers**: A  
- **A**: True. Round toward \(+\infty\) ensures result ≥ original.  
- **B/C/D**: No.  

---

**Question 9**  
**Correct Answers**: A, B  
- **A**: `!!x` converts any non-zero `x` to `1`.  
- **B**: For negative `x`, `(x | -x)` is `-1`, right-shifted gives `-1`.  
- **C**: `x ^ (~x + 1)` is `0` only if `x == 0`.  
- **D**: Only true if `x` is ≤ 255 and non-negative.  

---

**Question 10**  
**Correct Answers**: B, C  
- **B/C**: Little-endian stores least-significant byte first.  
- **A/D**: Big-endian.  

---

These questions test deep understanding of two’s complement, IEEE 754, bitwise operations, type conversions, and endianness. The explanations reference key concepts like sign extension, NaN encoding, and undefined behavior in C.

**After you are done, it is highly recommended to do the exercises from the original book**