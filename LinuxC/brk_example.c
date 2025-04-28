#include <unistd.h>
#include <stdio.h>

int main() {
    // 初始堆顶地址
    void *initial_brk = sbrk(0);
    printf("初始堆顶地址: %p\n", initial_brk);

    // 扩展堆内存 4096 字节（4KB）
    if (sbrk(4096) == (void*)-1) {
        perror("sbrk 失败");
        return 1;
    }

    // 新的堆顶地址
    void *new_brk = sbrk(0);
    printf("扩展后堆顶地址: %p (增加了 %ld 字节)\n", new_brk, (char*)new_brk - (char*)initial_brk);

    // 收缩堆内存到初始位置
    if (brk(initial_brk) == -1) {
        perror("brk 失败");
        return 1;
    }

    printf("堆内存已释放\n");
    return 0;
}