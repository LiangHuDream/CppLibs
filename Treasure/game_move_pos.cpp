#include <iostream>
#include <vector>
 
// 定义世界大小
const int WORLD_WIDTH = 20;
const int WORLD_HEIGHT = 10;
 
// 定义方块类型
enum BlockType {
    AIR,
    GRASS,
    DIRT,
    STONE
};
 
// 定义世界
std::vector<std::vector<BlockType>> world(WORLD_HEIGHT, std::vector<BlockType>(WORLD_WIDTH, AIR));
 
// 生成地形
void generateTerrain() {
    for (int x = 0; x < WORLD_WIDTH; ++x) {
        int groundLevel = WORLD_HEIGHT / 2;
        world[groundLevel][x] = GRASS;
        for (int y = groundLevel + 1; y < WORLD_HEIGHT; ++y) {
            if (y < groundLevel + 3) {
                world[y][x] = DIRT;
            } else {
                world[y][x] = STONE;
            }
        }
    }
}
 
// 打印世界
void printWorld(int playerX, int playerY) {
    for (int y = 0; y < WORLD_HEIGHT; ++y) {
        for (int x = 0; x < WORLD_WIDTH; ++x) {
            if (x == playerX && y == playerY) {
                std::cout << 'P';
            } else {
                switch (world[y][x]) {
                    case AIR:
                        std::cout << ' ';
                        break;
                    case GRASS:
                        std::cout << 'G';
                        break;
                    case DIRT:
                        std::cout << 'D';
                        break;
                    case STONE:
                        std::cout << 'S';
                        break;
                }
            }
        }
        std::cout << std::endl;
    }
}
 
int main() {
    generateTerrain();
 
    int playerX = WORLD_WIDTH / 2;
    int playerY = WORLD_HEIGHT / 2 - 1;
 
    while (true) {
        system("cls"); // 清屏，在 Linux 系统中使用 system("clear");
        printWorld(playerX, playerY);
 
        std::cout << "使用 w(上), s(下), a(左), d(右) 移动，q 退出: ";
        char input;
        std::cin >> input;
 
        if (input == 'q') {
            break;
        }
 
        int newX = playerX;
        int newY = playerY;
 
        switch (input) {
            case 'w':
                newY--;
                break;
            case 's':
                newY++;
                break;
            case 'a':
                newX--;
                break;
            case 'd':
                newX++;
                break;
        }
 
        // 检查新位置是否合法
        if (newX >= 0 && newX < WORLD_WIDTH && newY >= 0 && newY < WORLD_HEIGHT) {
            playerX = newX;
            playerY = newY;
        }
    }
 
    return 0;
}    