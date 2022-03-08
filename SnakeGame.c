#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <windows.h>

int Str2Int(const char * intStr) {
	int num = 0;
	while (*intStr) {
		if ((*intStr) < '0' || (*intStr) > '9') return -1;
		num = num * 10 + (int)((*intStr) - '0');
		++intStr;
	}
	return num;
}

struct Point {
	int x, y;
};

struct Snake {
	struct Point head, tail;
	int length;
} snake;

#define MAX_Y 55
#define MAX_X 55
struct status {
	unsigned char isSnake: 1;
	unsigned char bodyDir: 2;
	unsigned char isFood: 1;
} mapStatus[MAX_X][MAX_Y];
int mapSize = 0, gameSpeed = 0;

void SetCursorVisble(int isVis) { // 设置光标是否可见
	HANDLE handleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(handleOutput, &cursorInfo);
	cursorInfo.bVisible = isVis;
	SetConsoleCursorInfo(handleOutput, &cursorInfo);
	return;
}

void SetConCursorPos(int x, int y) { // 设置光标位置
	COORD pos = {x, y};
	HANDLE handleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(handleOutput, pos);
	return;
}

void PrintField(int width, int height) { // 打印游戏地图
	system("cls");
	int i;
	printf("++");
	for (i = 0; i < width; ++i) {
		printf("==");
	}
	puts("++");
	for (i = 0; i < height; ++i) {
		printf("||");
		SetConCursorPos(width * 2 + 2, i + 1);
		puts("||");
	}
	printf("++");
	for (i = 0; i < width; ++i) {
		printf("==");
	}
	puts("++");
	puts("按 p 可以暂停游戏");
	puts("按 e 可以中途退出游戏");
	return;
}

void PrintBody(struct Point body, char c) {
	SetConCursorPos(body.x * 2 + 2, body.y + 1);
	putchar(c);
	return;
}

char HeadChar(int dir) {
	switch (dir) {
		case 0: {
			return 'V';
		}
		case 1: {
			return 'A';
		}
		case 2: {
			return '>';
		}
		case 3: {
			return '<';
		}
	}
	return 'O';
}

int foodExist = 0;
void setFood(void) {
	if (foodExist) return;
	foodExist = 1;
	struct Point food;
	while (1) {
		food = (struct Point){rand() % mapSize, rand() % mapSize};
		if (!mapStatus[food.x][food.y].isSnake) break;
	}
	PrintBody(food, '*');
	mapStatus[food.x][food.y].isFood = 1;
	return;
}

int dy[] = {1, -1, 0, 0}, dx[] = {0, 0, 1, -1};
int SnakeMov (int dir) {
	int curHeadDir = mapStatus[snake.head.x][snake.head.y].bodyDir;
	if ((dir ^ 1) == curHeadDir || dir > 3) {
		dir = curHeadDir;
	}
	int nxtHeadx = snake.head.x + dx[curHeadDir] >= mapSize ? 0 : (snake.head.x + dx[curHeadDir] < 0 ? mapSize - 1 : snake.head.x + dx[curHeadDir]); // 允许穿墙
	int nxtHeady = snake.head.y + dy[curHeadDir] >= mapSize ? 0 : (snake.head.y + dy[curHeadDir] < 0 ? mapSize - 1 : snake.head.y + dy[curHeadDir]);
	if (mapStatus[nxtHeadx][nxtHeady].isSnake) { // 撞到自己
		return 0;
	}
	mapStatus[nxtHeadx][nxtHeady].bodyDir = dir;
	mapStatus[nxtHeadx][nxtHeady].isSnake = 1; // 给下一个位置的头设置状态
	PrintBody(snake.head, 'O'); // 删除旧头
	struct Point nxtHead = {nxtHeadx, nxtHeady}; // 设置新头
	PrintBody(nxtHead, HeadChar(dir));
	snake.head = nxtHead; // 新头设置完成
	if (mapStatus[nxtHeadx][nxtHeady].isFood) {
		snake.length += 1;
		foodExist = 0;
		return; // 不改变尾的位置
	}
	int curTailDir = mapStatus[snake.tail.x][snake.tail.y].bodyDir;
	int nxtTailx = snake.tail.x + dx[curTailDir] >= mapSize ? 0 : (snake.tail.x + dx[curTailDir] < 0 ? mapSize - 1 : snake.tail.x + dx[curTailDir]); // 和头一样
	int nxtTaily = snake.tail.y + dy[curTailDir] >= mapSize ? 0 : (snake.tail.y + dy[curTailDir] < 0 ? mapSize - 1 : snake.tail.y + dy[curTailDir]); // 允许穿墙
	mapStatus[snake.tail.x][snake.tail.y] = (struct status){0, 0, 0}; // 删除旧的身体信息
	PrintBody(snake.tail, ' '); // 删除旧尾
	struct Point nxtTail = {nxtTailx, nxtTaily}; // 设置新尾
	PrintBody(nxtTail, curTailDir > 1 ? '-' : '|');
	snake.tail = nxtTail; // 完成一次移动
	return 1;
}

void init(void) {
	mapSize = 0;
	gameSpeed = 0;
	memset(mapStatus, 0, sizeof(mapStatus));
	srand(time(NULL));
	return;
}

void setSnakeStatus(void) {
	snake.head = (struct Point){0, 1};
	snake.tail = (struct Point){0, 0};
	snake.length = 0;
	return;
}

int main(int argc, char *argv[]) {
	if (argc == 3) {
		mapSize = Str2Int(argv[1]);
		gameSpeed = Str2Int(argv[2]);
		if (mapSize == -1 || gameSpeed == -1 || mapSize < 1 || mapSize > 5 || gameSpeed < 1 || gameSpeed > 5) {
			puts("请输入 1 到 5 的数字作为游戏地图大小和游戏速度");
			return 0;
		}
	} else if (argc == 2 || argc > 3) {
		puts("请用如下格式设置游戏参数:\n\tSnakeGame  mapSize  gameSpeed");
		puts("或者不在命令行中使用参数，而是在游戏中设置参数");
	} else if (argc == 1) {
		while (mapSize < 1 || mapSize > 5) {
			puts("请输入地图大小(1 ~ 5，1最小):");
			scanf("%d", &mapSize);
		}
		while (gameSpeed < 1 || gameSpeed > 5) {
			puts("请输入游戏速度(1 ~ 5，1最慢):");
			scanf("%d", &gameSpeed);
		}
	}
	int firstGame = 1; // 是否第一次游戏
	while (1) {
		if (!firstGame) {
			init();
			system("cls");
			while (mapSize < 1 || mapSize > 5) {
				puts("请输入地图大小(1 ~ 5，1最小):");
				scanf("%d", &mapSize);
			}
			while (gameSpeed < 1 || gameSpeed > 5) {
				puts("请输入游戏速度(1 ~ 5，1最慢):");
				scanf("%d", &gameSpeed);
			}
		}
		SetCursorVisble(0);
		mapSize = 10 + mapSize * 10;
		PrintField(mapSize, mapSize);
		setSnakeStatus();
		PrintBody((struct Point){0, 0}, '|');
		PrintBody((struct Point){0, 1}, 'V');
		firstGame = 0;
		int gameBreak = 20 * (6 - gameSpeed);
		int curTime;
		char dir = 0;
		int exitFlag = 0;
		while (1) {
			curTime = clock();
			setFood();
			if (_kbhit()) {
				dir = getch();
				switch (dir) {
					case 'a': {
						dir = 3;
						break;
					}
					case 'd': {
						dir = 2;
						break;
					}
					case 'w': {
						dir = 1;
						break;
					}
					case 's': {
						dir = 0;			
						break;
					}
					case 'p': {
						SetConCursorPos(mapSize - 2, 0);
						puts("游戏暂停");
						while(1) {
							if (_kbhit()) {
								if (getch() == 'p') break;
								if (getch() == 'e') {
									exitFlag = 1;
									break;
								}
							}
						}
						SetConCursorPos(mapSize - 2, 0);
						puts("========");
						break;
					}
					case 'e': {
						exitFlag = 1;
						break;
					}
					default: {
						break;
					}
				}
			}
			if (exitFlag) break;
			while (clock() - curTime < gameBreak);
			if (!SnakeMov( (int)dir )) break;
		}
		SetCursorVisble(1);
		SetConCursorPos(mapSize - 4, mapSize / 2 - 1);
		puts("++========++");
		SetConCursorPos(mapSize - 4, mapSize / 2);
		puts("||游戏结束||");
		SetConCursorPos(mapSize - 4, mapSize / 2 + 1);
		puts("++========++");
		SetConCursorPos(0, mapSize + 2);
		printf("你的分数是: %4d  \n", snake.length);
		puts("输入 c 可以开始一局新游戏");
		puts("输入 # 退出");
		int gameOver = 0;
		while(1) {
			char c;
			scanf("%c", &c);
			if (c == 'c') break;
			if (c == '#') {
				gameOver = 1;
				break;
			}
		}
		if (gameOver) break;
	}
	return 0;
}