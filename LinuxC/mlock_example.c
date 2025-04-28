#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <strings.h>
#include <sys/resource.h>

typedef struct {
    void *address;     // 内存地址
    size_t size;       // 内存大小
    int is_locked;     // 锁定状态
} SecureMemory;

// 安全擦除内存内容
void secure_erase(void *mem, size_t size) {
    if (mem && size > 0) {
        explicit_bzero(mem, size);
    }
}

// 分步释放资源
void secure_cleanup(SecureMemory *sm) {
    if (sm->address == MAP_FAILED) return;

    // 擦除敏感数据
    secure_erase(sm->address, sm->size);

    // 解锁内存（忽略错误但记录）
    if (sm->is_locked) {
        if (munlock(sm->address, sm->size) == -1) {
            perror("警告：内存解锁异常");
        }
    }

    // 释放内存
    if (munmap(sm->address, sm->size) == -1) {
        perror("严重错误：内存释放失败");
    } else {
        sm->address = MAP_FAILED; // 标记为已释放
    }
}

// 初始化内存分配
int memory_allocate(SecureMemory *sm, size_t size) {
    sm->address = mmap(NULL, size, PROT_READ | PROT_WRITE, 
                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (sm->address == MAP_FAILED) {
        perror("内存分配失败");
        return 0;
    }
    sm->size = size;
    sm->is_locked = 0;
    return 1;
}

// 锁定内存操作
int memory_lock(SecureMemory *sm) {
    // Linux系统预检查锁定限制
#ifdef __linux__
    struct rlimit rlim;
    if (getrlimit(RLIMIT_MEMLOCK, &rlim) == 0) {
        if (rlim.rlim_cur < sm->size) {
            fprintf(stderr, "错误：当前内存锁定限制为 %zu KB (需要至少 %zu KB)\n",
                   (size_t)rlim.rlim_cur/1024, sm->size/1024);
            return 0;
        }
    }
#endif

    if (mlock(sm->address, sm->size) == -1) {
        perror("内存锁定失败（需要 root 权限或 CAP_IPC_LOCK）");
        return 0;
    }
    sm->is_locked = 1;
    return 1;
}

int main() {
    SecureMemory sm = { MAP_FAILED, 0, 0 };
    const char *secret = "Hello, mlock!";
    const size_t secret_len = sizeof("Hello, mlock!"); // 包含终止符

    // 步骤1：分配内存
    if (!memory_allocate(&sm, 4096)) {
        return 1;
    }

    // 步骤2：锁定内存
    if (!memory_lock(&sm)) {
        secure_cleanup(&sm);
        return 1;
    }

    // 步骤3：安全写入数据
    if (secret_len > sm.size) {
        fprintf(stderr, "致命错误：数据长度超过内存容量\n");
        secure_cleanup(&sm);
        return 1;
    }
    memcpy(sm.address, secret, secret_len);
    printf("内存初始化成功，内容：%s\n", (char*)sm.address);

    // 步骤4：安全释放资源
    secure_cleanup(&sm);
    printf("内存已安全释放\n");
    return 0;
}