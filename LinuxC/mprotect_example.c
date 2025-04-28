#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

static void *g_mem = NULL;
static size_t g_size = 0;

void safe_cleanup() {
    if (g_mem != MAP_FAILED && g_mem != NULL) {
        munmap(g_mem, g_size);
        g_mem = NULL;
    }
}

void segv_handler(int sig) {
    fprintf(stderr, "\n捕获段错误 (SIGSEGV)，执行安全清理\n");
    safe_cleanup();
    _exit(EXIT_FAILURE);
}

int main() {
    struct sigaction sa;
    int ret = EXIT_FAILURE;
    const char *msg = "Hello, mprotect!";
    size_t msg_len = strlen(msg) + 1;

    // 初始化信号处理器
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = segv_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESETHAND;

    if (sigaction(SIGSEGV, &sa, NULL) == -1) {
        perror("sigaction 失败");
        return EXIT_FAILURE;
    }

    // 分配内存
    g_size = 4096;
    g_mem = mmap(NULL, g_size, PROT_READ | PROT_WRITE, 
                MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (g_mem == MAP_FAILED) {
        perror("mmap 失败");
        return EXIT_FAILURE;
    }

    // 写入数据
    if (msg_len > g_size) {
        fprintf(stderr, "错误：消息长度超过内存容量\n");
        safe_cleanup();
        return EXIT_FAILURE;
    }
    memcpy(g_mem, msg, msg_len);

    // 修改权限
    if (mprotect(g_mem, g_size, PROT_READ) == -1) {
        perror("mprotect 失败");
        safe_cleanup();
        return EXIT_FAILURE;
    }

    // 验证可读性
    printf("内存内容: %s\n", (char*)g_mem);

    // 触发测试
    printf("尝试写入只读内存...\n");
    volatile char *test = (char*)g_mem;
    *test = 'X';  // 触发SIGSEGV

    // 正常退出
    safe_cleanup();
    ret = EXIT_SUCCESS;

    return ret;
}