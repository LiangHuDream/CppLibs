#include <sys/mman.h>
#include <stdio.h>
#include <string.h>

int main() {
    // 分配 1MB 可读写匿名内存（不关联文件）
    size_t size = 1024 * 1024;
    void *mem = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (mem == MAP_FAILED) {
        perror("mmap 失败");
        return 1;
    }

    printf("内存分配成功，地址: %p\n", mem);

    // 使用内存
    const char *message = "Hello, mmap!";
    size_t msg_len = sizeof("Hello, mmap!"); // 编译时计算长度，包含终止符

    if (msg_len > size) {
        fprintf(stderr, "错误：消息长度超过分配的内存\n");
        munmap(mem, size);
        return 1;
    }
    memcpy(mem, message, msg_len); // 安全的内存拷贝

    printf("内存内容: %s\n", (char*)mem);

    // 释放内存
    if (munmap(mem, size) == -1) {
        perror("munmap 失败");
        return 1;
    }

    printf("内存已释放\n");
    return 0;
}