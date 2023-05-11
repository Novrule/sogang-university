﻿#include "tetris.h"

static struct sigaction act, oact;

int main(void) {
	int exit = 0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);

	srand((unsigned int)time(NULL));

	while (!exit) {
		clear();
		switch (menu()) {
		case MENU_PLAY: play(); break;
		case MENU_EXIT: exit=1; break;
		default: break;
		}
	}

	endwin();
	system("clear");

	return 0;
}

void InitTetris() {
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			field[i][j] = 0;
		}
	}

	nextBlock[0] = rand() % 7;
	nextBlock[1] = rand() % 7;
	blockRotate = 0;
	blockY = -1;
	blockX = WIDTH / 2 - 2;
	score = 0;
	gameOver = 0;
	timed_out = 0;

	DrawOutline();
	DrawField();
	DrawBlock(blockY, blockX, nextBlock[0], blockRotate, ' ');
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline() {
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0, 0, HEIGHT, WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2, WIDTH + 10);
	printw("NEXT BLOCK");
	DrawBox(3, WIDTH + 10, 4, 8);

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(9, WIDTH + 10);
	printw("SCORE");
	DrawBox(10, WIDTH + 10, 1, 8);
}

int GetCommand() {
	int command;

	command = wgetch(stdscr);
	switch (command) {
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	} 

	return command;
}

int ProcessCommand(int command) {
	int ret = 1;
	int drawFlag = 0;

	switch (command) {
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if ((drawFlag = CheckToMove(field, nextBlock[0], (blockRotate + 1) % 4, blockY, blockX))) {
			blockRotate = (blockRotate + 1) % 4;
		}
		break;
	case KEY_DOWN:
		if ((drawFlag = CheckToMove(field, nextBlock[0], blockRotate, blockY + 1, blockX))) {
			blockY++;
		}
		break;
	case KEY_RIGHT:
		if ((drawFlag = CheckToMove(field, nextBlock[0], blockRotate, blockY, blockX + 1))) {
			blockX++;
		}
		break;
	case KEY_LEFT:
		if ((drawFlag = CheckToMove(field, nextBlock[0], blockRotate, blockY, blockX - 1))) {
			blockX--;
		}
		break;
	default:
		break;
	}
	if (drawFlag) {
		DrawChange(field, command, nextBlock[0], blockRotate, blockY, blockX);
	}

	return ret;
}

void DrawField(){
	int i, j;

	for (j = 0; j < HEIGHT; j++) {
		move(j + 1, 1);
		for (i = 0; i < WIDTH; i++) {
			if (field[j][i] == 1) {
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else {
				printw(".");
			}
		}
	}
}


void PrintScore(int score) {
	move(11, WIDTH + 11);
	printw("%8d", score);
}

void DrawNextBlock(int* nextBlock) {
	int i, j;

	for (i = 0; i < BLOCK_HEIGHT; i++) {
		move(4 + i, WIDTH + 13);
		for (j = 0; j < BLOCK_WIDTH; j++) {
			if (block[nextBlock[1]][0][i][j] == 1) {
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else {
				printw(" ");
			}
		}
	}
}

void DrawBlock(int y, int x, int blockID, int blockRotate, char tile) {
	int i, j;

	for (i = 0; i < BLOCK_HEIGHT; i++) {
		for (j = 0; j < BLOCK_WIDTH; j++) {
			if (block[blockID][blockRotate][i][j] == 1 && i + y >= 0) {
				move(i + y + 1, j + x + 1);
				attron(A_REVERSE);
				printw("%c", tile);
				attroff(A_REVERSE);
			}
		}
	}
	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y, int x, int height, int width) {
	int i, j;

	move(y, x);
	addch(ACS_ULCORNER);
	for (i = 0; i < width; i++) {
		addch(ACS_HLINE);
	}
	addch(ACS_URCORNER);
	for (j = 0; j < height; j++) {
		move(y + j + 1, x);
		addch(ACS_VLINE);
		move(y + j + 1, x + width + 1);
		addch(ACS_VLINE);
	}
	move(y + j + 1, x);
	addch(ACS_LLCORNER);
	for (i = 0; i < width; i++) {
		addch(ACS_HLINE);
	}
	addch(ACS_LRCORNER);
}

void play() {
	int command;

	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM, &act, &oact);
	InitTetris();

	do {
		if (timed_out == 0) {
			alarm(1);
			timed_out = 1;
		}

		command = GetCommand();
		if (ProcessCommand(command) == QUIT) {
			alarm(0);
			DrawBox(HEIGHT / 2 - 1, WIDTH / 2 - 5, 1, 10);
			move(HEIGHT / 2, WIDTH / 2 - 4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	} while (!gameOver);
	
	alarm(0);
	getch();
	DrawBox(HEIGHT / 2 - 1, WIDTH / 2 - 5, 1, 10);
	move(HEIGHT / 2, WIDTH / 2 - 4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

char menu() {
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

int CheckToMove(char f[HEIGHT][WIDTH], int currentBlock, int blockRotate, int blockY, int blockX) {
	for (int i = 0; i < BLOCK_HEIGHT; i++) {
		for (int j = 0; j < BLOCK_WIDTH; j++) {
			if (block[currentBlock][blockRotate][i][j] == 1) {
				if (blockY + i >= HEIGHT) {
					return 0;
				}
				else if (blockX + j >= WIDTH || blockX + j < 0) {
					return 0;
				}
				else if (f[blockY + i][blockX + j]) {
					return 0;
				}
			}
		}
	}

	return 1;
}

void DrawChange(char f[HEIGHT][WIDTH], int command, int currentBlock, int blockRotate, int blockY, int blockX) {
	int prevRotate = blockRotate;
	int prevX = blockX;
	int prevY = blockY;

	switch (command) {
	case KEY_UP:
		prevRotate--;
		if (prevRotate == -1) {
			prevRotate = 3;
		}
		break;
	case KEY_DOWN:
		prevY--;
		break;
	case KEY_RIGHT:
		prevX--;
		break;
	case KEY_LEFT:
		prevX++;
		break;
	default:
		break;
	}

	for (int i = 0; i < BLOCK_HEIGHT; i++) {
		for (int j = 0; j < BLOCK_WIDTH; j++) {
			if (block[currentBlock][prevRotate][i][j] == 1 && i + prevY >= 0) {
				move(i + prevY + 1, j + prevX + 1);
				printw(".");
			}
		}
	}

	DrawBlock(blockY, blockX, currentBlock, blockRotate, ' ');
}

void BlockDown(int sig) {
	int temp;

	if (CheckToMove(field, nextBlock[0], blockRotate, blockY + 1, blockX)) {
		blockY++;
		DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, blockY, blockX);
	}
	else {
		if (blockY == -1) {
			gameOver = 1;
		}

		AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);

		temp = DeleteLine(field);
		if (temp != 0) {
			score += temp;
		}

		nextBlock[0] = nextBlock[1];
		nextBlock[1] = rand() % 7;
		blockRotate = 0;
		blockX = WIDTH / 2 - 2;
		blockY = -1;

		DrawNextBlock(nextBlock);
		PrintScore(score);
		DrawField();
	}

	timed_out = 0;
}

void AddBlockToField(char f[HEIGHT][WIDTH], int currentBlock, int blockRotate, int blockY, int blockX) {
	for (int i = 0; i < BLOCK_HEIGHT; i++) {
		for (int j = 0; j < BLOCK_WIDTH; j++) {
			if (block[currentBlock][blockRotate][i][j] == 1) {
				if (blockX + j >= 0 && blockX + j < WIDTH && blockY + i >= 0 && blockY + i < HEIGHT) {
					f[blockY + i][blockX + j] = 1;
				}
			}
		}
	}
}

int DeleteLine(char f[HEIGHT][WIDTH]) {
	int temp = 0;
	int flag = 1;

	for (int i = 0; i < HEIGHT; i++) {
		flag = 1;

		for (int j = 0; j < WIDTH; j++) {
			if (f[i][j] == 0) {
				flag = 0;
			}
		}

		if (flag == 1) {
			temp++;
			i--;

			for (int k = i; k >= 0; k--) {
				for (int l = 0; l < WIDTH; l++) {
					f[k + 1][l] = f[k][l];
				}
			}
			for (int k = 0; k < WIDTH; k++) {
				f[0][k] = 0;
			}
		}
	}

	return temp * temp * 100;
}

///////////////////////////////////////////////////////////////////////////

void DrawShadow(int y, int x, int blockID, int blockRotate) {
	// user code
}

void createRankList() {
	// user code
}

void rank() {
	// user code
}

void writeRankFile() {
	// user code
}

void newRank(int score) {
	// user code
}

void DrawRecommend(int y, int x, int blockID, int blockRotate) {
	// user code
}

int recommend(RecNode* root) {
	int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수

	// user code

	return max;
}

void recommendedPlay() {
	// user code
}
