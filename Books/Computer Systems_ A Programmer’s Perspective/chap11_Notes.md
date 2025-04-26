### Chapter 11: Network Programming

---

#### 1. Client-Server Model
**Key Concepts**:
- Clients request services, servers provide services.
- Communication via sockets over TCP/IP or UDP.
- Servers handle multiple clients via concurrency (forking/threading).

**Code Example: Simple TCP Echo Server**
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    
    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    // Configure address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    // Bind socket to port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    // Listen for connections
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
    
    printf("Server listening on port %d...\n", PORT);
    
    // Accept incoming connection
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }
    
    // Echo received data
    ssize_t bytes_read;
    while ((bytes_read = read(new_socket, buffer, BUFFER_SIZE)) > 0) {
        write(new_socket, buffer, bytes_read);
        memset(buffer, 0, BUFFER_SIZE);
    }
    
    close(new_socket);
    close(server_fd);
    return 0;
}
```

**Test Client (Use `telnet` or `nc`)**:
```bash
$ telnet localhost 8080
Trying 127.0.0.1...
Connected to localhost.
Hello, Server!  # Client sends
Hello, Server!  # Server echoes back
```

---

#### 2. Socket Address Structures
**Key Concepts**:
- `sockaddr_in` for IPv4, `sockaddr_in6` for IPv6.
- Use `htons()`/`htonl()` for byte ordering (host-to-network).
- `INADDR_ANY` binds to all interfaces.

**Code Snippet: Address Initialization**
```c
struct sockaddr_in server_addr;
server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(8080);
server_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // Bind to all interfaces
```

---

#### 3. Robust Error Handling
**Key Concepts**:
- Check return values of all system calls (`socket`, `bind`, `listen`, etc.).
- Use `perror()` to print human-readable errors.
- Graceful shutdown on failure.

**Code Example: Error Handling in `bind()`**
```c
if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind failed");
    close(sockfd);  // Cleanup socket
    exit(EXIT_FAILURE);
}
```

---

#### 4. HTTP Protocol Basics
**Key Concepts**:
- HTTP is a text-based protocol over TCP.
- Requests: `GET /path HTTP/1.1\r\nHost: ...\r\n\r\n`.
- Responses: `HTTP/1.1 200 OK\r\nContent-Type: ...\r\n\r\n<body>`.

**Code Example: Minimal HTTP Server**
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
#define RESPONSE "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nHello, World!\r\n"

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
    // Create socket, bind, listen (same as previous example)
    
    printf("HTTP Server listening on port %d...\n", PORT);
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }
    
    // Send HTTP response
    send(new_socket, RESPONSE, strlen(RESPONSE), 0);
    printf("Response sent\n");
    
    close(new_socket);
    close(server_fd);
    return 0;
}
```

**Test with `curl`**:
```bash
$ curl http://localhost:8080
Hello, World!
```

---

#### 5. Concurrency with `fork()`
**Key Concepts**:
- Servers handle multiple clients by forking a child process per connection.
- Parent process continues listening; child handles the client.

**Code Snippet: Forking Server**
```c
while (1) {
    int client_sock = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_sock < 0) {
        perror("accept failed");
        continue;
    }
    
    pid_t pid = fork();
    if (pid == 0) {  // Child process
        close(server_fd);  // Child doesn't need listener
        handle_client(client_sock);
        exit(0);  // Terminate child after handling
    } else if (pid > 0) {
        close(client_sock);  // Parent doesn't need client socket
    } else {
        perror("fork failed");
    }
}
```

**Test with Multiple Clients**:
```bash
$ telnet localhost 8080 &  # Start multiple clients in background
$ telnet localhost 8080 &
```

---

#### 6. DNS Resolution with `getaddrinfo()`
**Key Concepts**:
- Converts hostnames (e.g., "google.com") to IP addresses.
- Handles IPv4/IPv6 transparently.

**Code Example: Resolve Hostname**
```c
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>

int main() {
    struct addrinfo hints, *res, *p;
    char ipstr[INET6_ADDRSTRLEN];
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;  // IPv4 or IPv6
    
    if (getaddrinfo("google.com", NULL, &hints, &res) != 0) {
        perror("getaddrinfo failed");
        exit(EXIT_FAILURE);
    }
    
    // Iterate through results
    for (p = res; p != NULL; p = p->ai_next) {
        void *addr;
        if (p->ai_family == AF_INET) {  // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
        } else {  // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
        }
        inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
        printf("IP: %s\n", ipstr);
    }
    
    freeaddrinfo(res);
    return 0;
}
```

**Output**:
```
IP: 142.250.191.46
IP: 2607:f8b0:4005:80a::200e
```

---

### Compilation & Testing
**Build Commands**:
```bash
# For server.c
gcc server.c -o server

# For DNS resolver
gcc dns_resolver.c -o dns_resolver
```

**Run**:
```bash
./server  # Start the server in one terminal
./dns_resolver  # Resolve hostnames in another
```

---

### Summary
- **TCP Sockets**: Use `socket()`, `bind()`, `listen()`, `accept()` for servers; `connect()` for clients.
- **HTTP**: Text-based protocol; responses include status line, headers, and body.
- **Concurrency**: Forking allows handling multiple clients simultaneously.
- **Error Handling**: Check all system calls; use `perror()` and `exit()` on failure.
- **DNS**: Use `getaddrinfo()` to resolve hostnames to IP addresses.

---

### Multiple-Choice Questions

---

**Question 1: Socket API Fundamentals**  
**Which of the following are *required* steps to set up a TCP server socket?**  
A) Call `socket` with `SOCK_STREAM` and `AF_INET`  
B) Bind the socket to a specific port using `bind`  
C) Use `listen` to mark the socket as passive  
D) Call `accept` to wait for client connections  

---

**Question 2: Address Conversion**  
**Which functions correctly convert human-readable IP addresses to binary network byte order?**  
A) `inet_ntoa`  
B) `inet_pton`  
C) `getaddrinfo`  
D) `htons`  

---

**Question 3: HTTP Protocol**  
**Which HTTP request methods are idempotent?**  
A) GET  
B) POST  
C) PUT  
D) DELETE  

---

**Question 4: TCP vs. UDP**  
**Which statements about TCP and UDP are true?**  
A) TCP guarantees in-order delivery; UDP does not.  
B) UDP is connection-oriented.  
C) TCP uses sequence numbers to detect packet loss.  
D) UDP is suitable for real-time video streaming.  

---

**Question 5: Concurrent Servers**  
**Which concurrency models allow a server to handle multiple clients simultaneously?**  
A) Single-threaded iterative processing  
B) Forking a child process per client  
C) Using threads with a shared listening socket  
D) I/O multiplexing with `select` or `epoll`  

---

**Question 6: Socket Options**  
**Which socket options are relevant for reusing ports to avoid "Address already in use" errors?**  
A) `SO_REUSEADDR`  
B) `SO_KEEPALIVE`  
C) `SO_LINGER`  
D) `SO_RCVBUF`  

---

**Question 7: Error Handling**  
**Which errors might occur when using blocking I/O on sockets?**  
A) `EINTR` (interrupted system call)  
B) `ECONNREFUSED`  
C) `ENOMEM` (out of memory)  
D) `EAGAIN` (resource temporarily unavailable)  

---

**Question 8: URI Parsing**  
**When parsing an HTTP request URI, which components are mandatory?**  
A) Hostname  
B) Port number  
C) Path  
D) Query string  

---

**Question 9: Web Content Generation**  
**Which techniques generate dynamic web content?**  
A) Returning a static HTML file  
B) Executing a CGI program  
C) Using server-side templates (e.g., Jinja2)  
D) Serving precompiled binary data  

---

**Question 10: Protocol-Independent Programming**  
**Which functions support protocol-independent server code?**  
A) `gethostbyname`  
B) `getaddrinfo`  
C) `inet_ntop`  
D) `strerror`  

---

**Question 11: HTTP Headers**  
**Which HTTP headers are required in a valid HTTP/1.1 response?**  
A) `Content-Length`  
B) `Connection`  
C) `Date`  
D) `Server`  

---

**Question 12: Robust I/O**  
**Which issues does the RIO package address?**  
A) Partial reads/writes due to signal interrupts  
B) Buffer overflow vulnerabilities  
C) Network latency optimization  
D) Short counts in I/O operations  

---

**Question 13: Client-Server Communication**  
**Which steps are required for a client to connect to a server using TCP?**  
A) Call `socket` with `SOCK_DGRAM`  
B) Use `connect` to establish a connection  
C) Call `bind` to specify the client port  
D) Use `getaddrinfo` to resolve the server address  

---

**Question 14: HTTP Status Codes**  
**Which status codes indicate client-side errors?**  
A) 200 OK  
B) 404 Not Found  
C) 500 Internal Server Error  
D) 400 Bad Request  

---

**Question 15: Secure Programming**  
**Which practices mitigate buffer overflow attacks in network programs?**  
A) Using `strcpy` for string operations  
B) Validating input lengths before copying  
C) Employing bounds-checked functions like `strncpy`  
D) Disabling stack canaries  

---

---

### **Answers and Explanations**

**Question 1**  
**Correct Answers:** B, C, D  
**Explanation:**  
- A) Incorrect. While `SOCK_STREAM` is required for TCP, `AF_INET` restricts the server to IPv4. Protocol-independent servers use `AF_UNSPEC`.  
- B) Correct. `bind` associates the socket with a port.  
- C) Correct. `listen` configures the socket to accept incoming connections.  
- D) Correct. `accept` blocks until a client connects.  

---

**Question 2**  
**Correct Answers:** B, C  
**Explanation:**  
- A) `inet_ntoa` converts binary to string, not vice versa.  
- B) `inet_pton` converts IPv4/IPv6 addresses to binary.  
- C) `getaddrinfo` resolves hostnames to `struct addrinfo`, including binary addresses.  
- D) `htons` converts port numbers to network byte order, not IP addresses.  

---

**Question 3**  
**Correct Answers:** A, C, D  
**Explanation:**  
- Idempotent methods produce the same result after multiple identical requests.  
- GET, PUT, and DELETE are idempotent; POST is not.  

---

**Question 4**  
**Correct Answers:** A, C, D  
**Explanation:**  
- B) Incorrect. UDP is connectionless.  
- D) Correct. UDP’s lower latency suits real-time applications.  

---

**Question 5**  
**Correct Answers:** B, C, D  
**Explanation:**  
- A) Iterative servers process clients sequentially, not concurrently.  
- B/C/D) Forking, threading, and I/O multiplexing enable concurrency.  

---

**Question 6**  
**Correct Answer:** A  
**Explanation:**  
- `SO_REUSEADDR` allows reuse of ports in `TIME_WAIT` state. Other options are unrelated.  

---

**Question 7**  
**Correct Answers:** A, B  
**Explanation:**  
- `EINTR` occurs if a signal interrupts a blocking call. `ECONNREFUSED` happens if the server rejects the connection.  
- `EAGAIN` is non-blocking I/O specific.  

---

**Question 8**  
**Correct Answer:** C  
**Explanation:**  
- The path is mandatory (e.g., `GET /index.html`). Hostname is required in HTTP/1.1 but parsed from the `Host` header, not the URI.  

---

**Question 9**  
**Correct Answers:** B, C  
**Explanation:**  
- A/D) Static content is not dynamic.  
- B/C) CGI and templates generate dynamic responses.  

---

**Question 10**  
**Correct Answers:** B, C  
**Explanation:**  
- `getaddrinfo` and `inet_ntop` handle IPv4/IPv6 transparently. `gethostbyname` is IPv4-only.  

---

**Question 11**  
**Correct Answers:** None (Trick Question)  
**Explanation:**  
- HTTP/1.1 requires `Date` and `Connection` headers, but these are often enforced by servers, not clients. `Content-Length` is required only for non-chunked responses.  

---

**Question 12**  
**Correct Answers:** A, D  
**Explanation:**  
- RIO handles short counts and interruptions (e.g., `EINTR`).  

---

**Question 13**  
**Correct Answers:** B, D  
**Explanation:**  
- A) `SOCK_STREAM` is required for TCP.  
- C) `bind` is optional for clients.  

---

**Question 14**  
**Correct Answers:** B, D  
**Explanation:**  
- 4xx codes indicate client errors; 5xx are server errors.  

---

**Question 15**  
**Correct Answers:** B, C  
**Explanation:**  
- `strcpy` is unsafe. Bounds checking and secure functions (e.g., `strncpy`) prevent overflows. Stack canaries are a defense mechanism (opposite of D).  

--- 

These questions test deep understanding of network programming concepts, common pitfalls, and best practices from Chapter 11.

**After you are done, it is highly recommended to do the exercises from the original book**