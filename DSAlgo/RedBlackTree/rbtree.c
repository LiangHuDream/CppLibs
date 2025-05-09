#include <stdio.h>
#include <stdlib.h>

/* 红黑树颜色常量 */
#define RED     0   // 红色节点标识
#define BLACK   1   // 黑色节点标识

/* 键值类型定义（可根据需求修改为其他类型） */
typedef int KEY_TYPE;

/* 红黑树节点结构体 */
typedef struct _rbtree_node {
    unsigned char color;        // 节点颜色（RED/BLACK）
    struct _rbtree_node *left;  // 左子节点指针
    struct _rbtree_node *right; // 右子节点指针
    struct _rbtree_node *parent;// 父节点指针
    KEY_TYPE key;               // 节点键值（用于比较和排序）
    void *value;                // 节点存储的值（可扩展为任意数据类型）
} rbtree_node;

/* 红黑树容器结构体 */
typedef struct _rbtree {
    rbtree_node *root;  // 树的根节点指针
    rbtree_node *nil;   // 哨兵节点（表示空节点，所有叶子节点都指向它）
} rbtree;

/* 创建新节点
 * @param key    节点键值
 * @param value  节点存储的数据指针
 * @param nil    哨兵节点指针
 * @return 成功返回新节点指针，失败返回NULL
 */
rbtree_node *rbtree_create_node(KEY_TYPE key, void *value, rbtree_node *nil) {
    rbtree_node *node = (rbtree_node*)malloc(sizeof(rbtree_node));
    if (!node) return NULL;
    
    // 初始化节点属性
    node->key = key;
    node->value = value;
    node->color = RED;  // 新节点初始为红色（插入修复可能会调整）
    node->left = nil;   // 左右子节点初始指向哨兵
    node->right = nil;
    node->parent = nil; // 父节点初始指向哨兵
    return node;
}

/* 初始化红黑树
 * @param T 红黑树指针
 * 创建哨兵节点并初始化树结构
 */
void rbtree_init(rbtree *T) {
    // 创建哨兵节点（代表所有空节点）
    T->nil = (rbtree_node*)malloc(sizeof(rbtree_node));
    T->nil->color = BLACK;     // 哨兵节点必须为黑色
    T->root = T->nil;          // 初始时根节点指向哨兵
}

/* 左旋操作（维护平衡的关键操作）
 * @param T 红黑树指针
 * @param x 旋转基准节点
 * 时间复杂度：O(1)
 */
void rbtree_left_rotate(rbtree *T, rbtree_node *x) {
    rbtree_node *y = x->right; // 保存x的右子节点y
    
    // 第一步：将y的左子树变为x的右子树
    x->right = y->left;
    if (y->left != T->nil) {
        y->left->parent = x;   // 更新父指针（如果子树存在）
    }
    
    // 第二步：将y连接到x的父节点
    y->parent = x->parent;
    if (x->parent == T->nil) { // x原本是根节点
        T->root = y;
    } else if (x == x->parent->left) { // x是左子节点
        x->parent->left = y;
    } else { // x是右子节点
        x->parent->right = y;
    }
    
    // 第三步：将x变为y的左子节点
    y->left = x;
    x->parent = y;
}

/* 右旋操作（左旋的对称操作）
 * @param T 红黑树指针
 * @param y 旋转基准节点
 * 时间复杂度：O(1)
 */
void rbtree_right_rotate(rbtree *T, rbtree_node *y) {
    rbtree_node *x = y->left;  // 保存y的左子节点x
    
    // 第一步：将x的右子树变为y的左子树
    y->left = x->right;
    if (x->right != T->nil) {
        x->right->parent = y;  // 更新父指针（如果子树存在）
    }
    
    // 第二步：将x连接到y的父节点
    x->parent = y->parent;
    if (y->parent == T->nil) { // y原本是根节点
        T->root = x;
    } else if (y == y->parent->right) { // y是右子节点
        y->parent->right = x;
    } else { // y是左子节点
        y->parent->left = x;
    }
    
    // 第三步：将y变为x的右子节点
    x->right = y;
    y->parent = x;
}

/* 插入后修复红黑树性质
 * @param T 红黑树指针
 * @param z 新插入的节点
 * 处理三种情况来维护红黑树性质
 */
void rbtree_insert_fixup(rbtree *T, rbtree_node *z) {
    // 当父节点为红色时循环处理（违反红色节点不能有红色父节点性质）
    while (z->parent->color == RED) {
        // 父节点是祖父的左子节点的情况
        if (z->parent == z->parent->parent->left) {
            rbtree_node *y = z->parent->parent->right; // 叔节点
            
            // Case 1: 叔节点为红色
            if (y->color == RED) {
                z->parent->color = BLACK;      // 父节点变黑
                y->color = BLACK;              // 叔节点变黑
                z->parent->parent->color = RED; // 祖父变红
                z = z->parent->parent;         // 上移检查点到祖父节点
            } else {
                // Case 2: z是右子节点（形成三角关系）
                if (z == z->parent->right) {
                    z = z->parent;             // 上移检查点到父节点
                    rbtree_left_rotate(T, z);  // 左旋转换为Case3
                }
                // Case 3: z是左子节点（直线关系）
                z->parent->color = BLACK;      // 父节点变黑
                z->parent->parent->color = RED; // 祖父变红
                rbtree_right_rotate(T, z->parent->parent); // 右旋祖父节点
            }
        } else { // 对称处理父节点是右子节点的情况
            rbtree_node *y = z->parent->parent->left;
            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    rbtree_right_rotate(T, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rbtree_left_rotate(T, z->parent->parent);
            }
        }
    }
    T->root->color = BLACK; // 确保根节点始终为黑色
}

/* 插入节点
 * @param T 红黑树指针
 * @param z 要插入的新节点
 * 标准二叉搜索树插入，然后调用修复函数
 */
void rbtree_insert(rbtree *T, rbtree_node *z) {
    rbtree_node *y = T->nil; // 跟踪插入位置的父节点
    rbtree_node *x = T->root;// 用于遍历的当前节点
    
    // 步骤1：标准BST插入，查找插入位置
    while (x != T->nil) {
        y = x;
        x = (z->key < x->key) ? x->left : x->right;
    }
    
    // 连接新节点与父节点
    z->parent = y;
    if (y == T->nil) {       // 树为空的情况
        T->root = z;
    } else if (z->key < y->key) {
        y->left = z;
    } else {
        y->right = z;
    }
    
    // 初始化新节点属性
    z->left = T->nil;
    z->right = T->nil;
    z->color = RED;          // 新节点总是红色
    
    // 步骤2：修复可能破坏的红黑树性质
    rbtree_insert_fixup(T, z);
}

/********************​ 删除相关操作 ********************​/

/* 查找子树最小节点
 * @param T 红黑树指针
 * @param x 起始节点
 * @return 最小节点指针
 */
rbtree_node *rbtree_minimum(rbtree *T, rbtree_node *x) {
    while (x->left != T->nil) {
        x = x->left;
    }
    return x;
}

/* 查找指定键的节点
 * @param T  红黑树指针
 * @param key 要查找的键值
 * @return 找到返回节点指针，未找到返回NULL
 */
rbtree_node *rbtree_search(rbtree *T, KEY_TYPE key) {
    rbtree_node *node = T->root;
    while (node != T->nil) {
        if (key < node->key) {
            node = node->left;
        } else if (key > node->key) {
            node = node->right;
        } else {
            return node; // 找到匹配节点
        }
    }
    return NULL; // 未找到
}

/* 子树替换操作（删除辅助函数）
 * @param T 红黑树指针
 * @param u 被替换的节点
 * @param v 替换节点
 */
void rbtree_transplant(rbtree *T, rbtree_node *u, rbtree_node *v) {
    if (u->parent == T->nil) {       // u是根节点
        T->root = v;
    } else if (u == u->parent->left) { // u是左子节点
        u->parent->left = v;
    } else {                         // u是右子节点
        u->parent->right = v;
    }
    v->parent = u->parent; // 即使v是哨兵也更新父指针
}

/* 删除后修复红黑树性质
 * @param T 红黑树指针
 * @param x 需要修复的起始节点
 * 处理四种情况来维护红黑树性质
 */
void rbtree_delete_fixup(rbtree *T, rbtree_node *x) {
    while (x != T->root && x->color == BLACK) {
        if (x == x->parent->left) { // x是左子节点的情况
            rbtree_node *w = x->parent->right; // 兄弟节点
            
            // Case 1: 兄弟节点为红色
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                rbtree_left_rotate(T, x->parent);
                w = x->parent->right;
            }
            // Case 2: 兄弟的两个子节点都是黑色
            if (w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED;
                x = x->parent; // 上移处理点
            } else {
                // Case 3: 兄弟的右子节点是黑色
                if (w->right->color == BLACK) {
                    w->left->color = BLACK;
                    w->color = RED;
                    rbtree_right_rotate(T, w);
                    w = x->parent->right;
                }
                // Case 4: 最终调整
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                rbtree_left_rotate(T, x->parent);
                x = T->root; // 结束循环
            }
        } else { // 对称处理x是右子节点的情况
            rbtree_node *w = x->parent->left;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                rbtree_right_rotate(T, x->parent);
                w = x->parent->left;
            }
            if (w->right->color == BLACK && w->left->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->left->color == BLACK) {
                    w->right->color = BLACK;
                    w->color = RED;
                    rbtree_left_rotate(T, w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                rbtree_right_rotate(T, x->parent);
                x = T->root;
            }
        }
    }
    x->color = BLACK; // 确保根节点为黑色
}

/* 删除节点
 * @param T 红黑树指针
 * @param z 要删除的节点
 * 处理三种情况并调用修复函数
 */
void rbtree_delete(rbtree *T, rbtree_node *z) {
    rbtree_node *y = z;        // 实际被删除的节点
    rbtree_node *x;            // 需要修复的节点
    int y_original_color = y->color; // 保存原始颜色
    
    // Case 1/2: 最多一个子节点
    if (z->left == T->nil) {
        x = z->right;
        rbtree_transplant(T, z, z->right);
    } else if (z->right == T->nil) {
        x = z->left;
        rbtree_transplant(T, z, z->left);
    } else { // Case 3: 有两个子节点
        y = rbtree_minimum(T, z->right); // 找后继节点
        y_original_color = y->color;
        x = y->right;
        if (y->parent == z) {
            x->parent = y;
        } else {
            rbtree_transplant(T, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        rbtree_transplant(T, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }
    
    // 如果删除的是黑色节点，需要修复
    if (y_original_color == BLACK) {
        rbtree_delete_fixup(T, x);
    }
    free(z); // 释放节点内存
}

/********************​ 测试与验证 ********************​/

/* 中序遍历打印（验证有序性）
 * @param T 红黑树指针
 * @param node 当前节点
 */
void rbtree_inorder(rbtree *T, rbtree_node *node) {
    if (node != T->nil) {
        rbtree_inorder(T, node->left);
        printf("%d(%s) ", node->key, node->color == RED ? "R" : "B");
        rbtree_inorder(T, node->right);
    }
}

/* 递归验证红黑树性质
 * @param T 红黑树指针
 * @param node 当前检查节点
 * @param black_count 当前路径黑节点计数
 * @param path_black 基准黑高度（初始-1）
 * @return 符合性质返回1，否则返回0
 */
int verify_rbtree_properties(rbtree *T, rbtree_node *node, 
                            int black_count, int* path_black) {
    if (node == T->nil) {
        if (*path_black == -1) { // 首次到达叶子节点
            *path_black = black_count;
        }
        return (black_count == *path_black); // 验证黑高一致
    }
    
    // 检查红色节点的子节点必须为黑色
    if (node->color == RED && 
       (node->left->color == RED || node->right->color == RED)) {
        return 0;
    }
    
    // 累计黑节点数量
    int new_count = black_count + (node->color == BLACK);
    // 递归验证左右子树
    return verify_rbtree_properties(T, node->left, new_count, path_black) &&
           verify_rbtree_properties(T, node->right, new_count, path_black);
}

/* 综合测试函数 */
void test_rbtree() {
    rbtree T;
    rbtree_init(&T);
    
    // 测试数据（包含各种插入顺序）
    int test_data[] = {10,5,15,3,8,12,18,2,4,7,9};
    int size = sizeof(test_data)/sizeof(int);
    
    printf("=== 插入测试 ===\n");
    for (int i = 0; i < size; i++) {
        rbtree_node *node = rbtree_create_node(test_data[i], NULL, T.nil);
        if (!node) {
            fprintf(stderr, "内存分配失败\n");
            exit(EXIT_FAILURE);
        }
        rbtree_insert(&T, node);
    }
    
    // 验证根节点性质
    printf("根节点颜色: %s\n", T.root->color == BLACK ? "黑" : "红");
    
    // 验证中序有序性
    printf("中序遍历结果: ");
    rbtree_inorder(&T, T.root);
    printf("\n");
    
    // 全面验证红黑树性质
    int path_black = -1;
    int valid = verify_rbtree_properties(&T, T.root, 0, &path_black);
    printf("红黑树性质验证: %s\n", valid ? "通过" : "失败");
    
    // 删除测试
    printf("\n=== 删除测试 ===\n");
    int delete_seq[] = {5,15,10}; // 测试不同位置的删除
    for (int i = 0; i < 3; i++) {
        rbtree_node *node = rbtree_search(&T, delete_seq[i]);
        if (node) {
            printf("\n删除节点 %d 后:\n", delete_seq[i]);
            rbtree_delete(&T, node);
            
            printf("中序遍历结果: ");
            rbtree_inorder(&T, T.root);
            printf("\n");
            
            // 重新验证性质
            path_black = -1;
            valid = verify_rbtree_properties(&T, T.root, 0, &path_black);
            printf("红黑树性质验证: %s\n", valid ? "通过" : "失败");
        }
    }
}

int main() {
    test_rbtree();
    return 0;
}