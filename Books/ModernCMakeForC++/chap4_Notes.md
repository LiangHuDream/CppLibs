### Chapter 4: Setting Up Your First CMake Project - Key Concepts & Code Walkthrough

---

#### 1. Core CMake Project Structure
**Key Points:**
- **`cmake_minimum_required()`**: Enforces minimum CMake version compatibility.
- **`project()`**: Defines project metadata (name, languages, version).
- **`add_executable()`/`add_library()`**: Declares build targets.
- **Hierarchical structure**: Use `add_subdirectory()` to include subprojects.

**Code Example:**
```cmake
cmake_minimum_required(VERSION 3.26)
project(MyApp 
  VERSION 1.0 
  LANGUAGES CXX
)

add_executable(main_app main.cpp)
add_subdirectory(lib)  # Contains another CMakeLists.txt
```

**Explanation:**
- `cmake_minimum_required` ensures backward compatibility.
- `project()` sets project name, version, and specifies C++ as the language.
- `add_executable` creates an executable target from `main.cpp`.
- `add_subdirectory` includes a subdirectory `lib` with its own CMake logic.

---

#### 2. Variable Scopes & Subdirectory Management
**Key Challenges:**
- Variables in parent scope aren't automatically visible in subdirectories.
- Targets defined in subdirectories need explicit exposure.

**Code Structure:**
```
project-root/
├── CMakeLists.txt
└── lib/
    ├── CMakeLists.txt
    └── utils.cpp
```

**Parent `CMakeLists.txt`:**
```cmake
set(COMMON_FLAGS "-Wall")  # Parent scope variable
add_subdirectory(lib)
```

**Child `lib/CMakeLists.txt`:**
```cmake
add_library(utils utils.cpp)
target_compile_options(utils PRIVATE ${COMMON_FLAGS})  # Error: COMMON_FLAGS not visible

# Fix: Use PARENT_SCOPE or better, target-based approaches
```

**Solution:**
- **Best Practice**: Use `target_*` commands to propagate settings:
  ```cmake
  # Parent scope
  add_library(common_settings INTERFACE)
  target_compile_options(common_settings INTERFACE -Wall)
  
  # Child directory
  target_link_libraries(utils PRIVATE common_settings)
  ```

---

#### 3. Cross-Platform Configuration
**Key Points:**
- Detect OS using `CMAKE_SYSTEM_NAME`.
- Handle compiler differences with generator expressions.

**Code Example:**
```cmake
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  add_compile_definitions(LINUX_PLATFORM)
elseif(WIN32)
  add_compile_definitions(WINDOWS_PLATFORM)
endif()

# Modern approach using generator expressions:
target_compile_definitions(main_app 
  PRIVATE 
    $<$<PLATFORM_ID:Linux>:LINUX>
    $<$<PLATFORM_ID:Windows>:WIN32>
)
```

**Explanation:**
- Conditional blocks check the target platform.
- Generator expressions (`$<...>`) allow platform-specific settings at build time.

---

#### 4. Enforcing C++ Standards
**Key Commands:**
- `set(CMAKE_CXX_STANDARD 20)`: Requests C++20.
- `set(CMAKE_CXX_STANDARD_REQUIRED ON)`: Makes it mandatory.
- `set(CMAKE_CXX_EXTENSIONS OFF)`: Disables compiler-specific extensions.

**Code Example:**
```cmake
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # e.g., Disable GNU's -std=gnu++20

add_executable(app main.cpp)
```

**Common Pitfalls:**
- Forgetting `CMAKE_CXX_STANDARD_REQUIRED` may lead to silent fallback to older standards.
- Extensions (like `-std=gnu++20`) can cause portability issues.

---

#### **5. Out-of-Source Builds**
**Best Practice:**
- Always build in a separate directory to avoid polluting source tree.

**Terminal Commands:**
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

**CMake Enforcement:**
```cmake
# Prevent in-source builds
if(CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
  message(FATAL_ERROR "In-source builds not allowed!")
endif()
```

**Explanation:**
- Creates a clean `build` directory for generated files.
- The fatal error check ensures users follow the correct workflow.

---

#### 6. Dependency Management Between Targets
**Key Concepts:**
- Declare dependencies with `target_link_libraries()`.
- Header file locations with `target_include_directories()`.

**Code Structure:**
```
project-root/
├── CMakeLists.txt
├── main.cpp
└── lib/
    ├── CMakeLists.txt
    ├── utils.h
    └── utils.cpp
```

**Parent `CMakeLists.txt`:**
```cmake
add_subdirectory(lib)

add_executable(main_app main.cpp)
target_link_libraries(main_app PRIVATE utils)  # Link to 'utils' library
```

**Child `lib/CMakeLists.txt`:**
```cmake
add_library(utils utils.cpp)
target_include_directories(utils 
  PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}  # Makes utils.h visible to users
)
```

**Explanation:**
- `target_include_directories` with `PUBLIC` propagates include paths to dependent targets.
- `target_link_libraries` establishes a dependency chain.

---

### Common Mistakes & Fixes
1. **Undefined Variables in Subdirectories**  
   - *Mistake*: Assuming parent-scope variables are visible in children.  
   - *Fix*: Use `PARENT_SCOPE` or better, use `target_*` properties.

2. **Implicit Compiler Standard Fallback**  
   - *Mistake*: Omitting `CMAKE_CXX_STANDARD_REQUIRED` leading to unexpected standard versions.  
   - *Fix*: Always set `..._REQUIRED ON`.

3. **Platform-Specific Hardcoding**  
   - *Mistake*: Using `#ifdef WIN32` in code without CMake checks.  
   - *Fix*: Define platform macros through CMake as shown in Section 3.

---

### **Conclusion**
Chapter 4 emphasizes building a robust foundation for CMake projects. Key takeaways include:
- Proper project structuring with hierarchical `CMakeLists.txt` files.
- Strict C++ standard enforcement and cross-platform configuration.
- Target-based dependency management over global variables.
- Prevention of common workflow errors (e.g., in-source builds).


---

### Multiple-Choice Questions

---

**Question 1: Basic Directives and Project Metadata**  
Which of the following are **required** in the top-level `CMakeLists.txt` for a valid CMake project? (Select two)  
A) `cmake_minimum_required()`  
B) `project()`  
C) `add_executable()`  
D) `add_subdirectory()`  

---

**Question 2: Project Structure and Subdirectories**  
When using `add_subdirectory(subdir)`, which of the following statements are **true**? (Select two)  
A) Variables defined in the parent scope are automatically inherited by the subdirectory.  
B) The subdirectory's `CMakeLists.txt` runs in a new variable scope.  
C) Targets defined in the subdirectory are globally visible to the parent scope.  
D) `CMAKE_CURRENT_SOURCE_DIR` in the subdirectory refers to the parent directory.  

---

**Question 3: Scoping and Variable Visibility**  
Which practices ensure proper variable scoping when using nested projects? (Select two)  
A) Use `PARENT_SCOPE` to propagate variables to the parent scope.  
B) Use `CACHE` variables for cross-directory communication.  
C) Avoid `add_subdirectory()` and use `ExternalProject` instead.  
D) Define all variables as `GLOBAL` to bypass scoping rules.  

---

**Question 4: External Project Handling**  
Which statements about external projects are **correct**? (Select two)  
A) `ExternalProject_Add()` integrates external projects during the configure stage.  
B) `FetchContent` downloads dependencies at build time.  
C) Nested `add_subdirectory()` is preferred for external projects with their own CMakeLists.  
D) Keeping external projects in separate build trees avoids polluting the main project.  

---

**Question 5: Environment Detection**  
Which CMake variables/methods can detect **system architecture**? (Select two)  
A) `CMAKE_SYSTEM_PROCESSOR`  
B) `CMAKE_SIZEOF_VOID_P`  
C) `CMAKE_SYSTEM_NAME`  
D) `CMAKE_CXX_COMPILER_ID`  

---

**Question 6: Toolchain Configuration**  
To enforce C++20 standard compliance, which steps are **necessary**? (Select two)  
A) `set(CMAKE_CXX_STANDARD 20)`  
B) `set(CMAKE_CXX_STANDARD_REQUIRED ON)`  
C) `target_compile_features(my_target PRIVATE cxx_std_20)`  
D) `add_definitions(-std=c++20)`  

---

**Question 7: In-Source Build Prevention**  
Which methods effectively **disable in-source builds**? (Select two)  
A) Adding `if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)` with `message(FATAL_ERROR)`.  
B) Using `cmake -B build` to specify an out-of-source build directory.  
C) Setting `CMAKE_DISABLE_SOURCE_CHANGES=ON`.  
D) Configuring `CMakePresets.json` to enforce out-of-source builds.  

---

**Question 8: Compiler Feature Checks**  
Which CMake commands are valid for **checking compiler features**? (Select two)  
A) `check_cxx_compiler_flag()`  
B) `try_compile()`  
C) `target_compile_definitions()`  
D) `include(CheckCXXSourceCompiles)`  

---

**Question 9: Cross-Compilation**  
When configuring cross-compilation, which variables **must** be set? (Select two)  
A) `CMAKE_SYSTEM_NAME`  
B) `CMAKE_CROSSCOMPILING`  
C) `CMAKE_C_COMPILER`  
D) `CMAKE_HOST_SYSTEM_PROCESSOR`  

---

**Question 10: Project Partitioning**  
Which scenarios justify using **nested projects** (`project()` in subdirectories)? (Select two)  
A) Building a standalone library with its own versioning.  
B) Isolating unit tests from the main codebase.  
C) Enabling different languages (e.g., C and C++) in subdirectories.  
D) Simplifying variable scope management.  

---

### **Answers and Explanations**

---

**Question 1**  
**Correct Answers**: A, B  
**Explanation**:  
- `cmake_minimum_required()` is mandatory to define compatibility (Chapter 4, "Basic Directives").  
- `project()` initializes the project name and languages. Other commands like `add_executable` are optional.  

---

**Question 2**  
**Correct Answers**: B, C  
**Explanation**:  
- Subdirectories create new scopes (B).  
- Targets are globally visible by default (C). `CMAKE_CURRENT_SOURCE_DIR` refers to the subdirectory (D is false).  

---

**Question 3**  
**Correct Answers**: A, B  
**Explanation**:  
- `PARENT_SCOPE` propagates variables upward (A).  
- `CACHE` variables persist across scopes (B). `GLOBAL` variables are discouraged for scoping (D).  

---

**Question 4**  
**Correct Answers**: B, D  
**Explanation**:  
- `FetchContent` downloads at configure time (B).  
- Separate build trees avoid pollution (D). `ExternalProject_Add` runs at build time (A is false).  

---

**Question 5**  
**Correct Answers**: A, B  
**Explanation**:  
- `CMAKE_SYSTEM_PROCESSOR` gives the target CPU (A).  
- `CMAKE_SIZEOF_VOID_P` indicates 32/64-bit (B). `CMAKE_SYSTEM_NAME` detects OS (C).  

---

**Question 6**  
**Correct Answers**: A, B  
**Explanation**:  
- `CMAKE_CXX_STANDARD` sets the standard (A).  
- `CMAKE_CXX_STANDARD_REQUIRED` enforces compliance (B). `target_compile_features` is an alternative (C is valid but not "necessary").  

---

**Question 7**  
**Correct Answers**: A, B  
**Explanation**:  
- The `if` check blocks in-source builds (A).  
- `-B build` enforces out-of-source builds (B). `CMAKE_DISABLE_SOURCE_CHANGES` is not a valid variable (C).  

---

**Question 8**  
**Correct Answers**: A, D  
**Explanation**:  
- `check_cxx_compiler_flag()` tests flags (A).  
- `CheckCXXSourceCompiles` validates code snippets (D). `try_compile` is lower-level (B is valid but not a command).  

---

**Question 9**  
**Correct Answers**: A, C  
**Explanation**:  
- `CMAKE_SYSTEM_NAME` defines the target OS (A).  
- `CMAKE_C_COMPILER` specifies the cross-compiler (C). `CMAKE_CROSSCOMPILING` is auto-set (B).  

---

**Question 10**  
**Correct Answers**: A, B  
**Explanation**:  
- Nested projects are useful for standalone components (A) and isolated tests (B). They complicate scoping (D is false).  

--- 
