#include "tetris.h"

static struct sigaction act, oact;

int main(void) {
	int exit = 0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);
	createRankList();

	srand((unsigned int)time(NULL));

	while (!exit) {
		clear();
		switch (menu()) {
		case MENU_PLAY: play(); break;
		case MENU_RANK: rank(); break;
		case MENU_RECCOMEND_PLAY: recommendedPlay(); break;
		case MENU_EXIT: exit = 1; break;
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
	nextBlock[2] = rand() % 7;
	blockRotate = 0;
	blockY = -1;
	blockX = WIDTH / 2 - 2;
	score = 0;
	gameOver = 0;
	timed_out = 0;
	size = 0;
	maketime = 0;

	root = (tree*)malloc(sizeof(tree));
	root->level = 0;
	root->score = 0;
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			root->field[i][j] = field[i][j];
		}
	}
	recommendR = 0;
	recommendY = 0;
	recommendX = 0;
	modified_recommend(root);

	DrawOutline();
	DrawField();
	DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate);
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void RecommendInitTetris() {
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			field[i][j] = 0;
		}
	}

	nextBlock[0] = rand() % 7;
	nextBlock[1] = rand() % 7;
	nextBlock[2] = rand() % 7;
	blockRotate = 0;
	blockY = -1;
	blockX = WIDTH / 2 - 2;
	score = 0;
	gameOver = 0;
	timed_out = 0;
	size = 0;
	maketime = 0;

	root = (tree*)malloc(sizeof(tree));
	root->level = 0;
	root->score = 0;
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			root->field[i][j] = field[i][j];
		}
	}
	recommendR = 0;
	recommendY = 0;
	recommendX = 0;
	modified_recommend(root);

	DrawOutline();
	DrawField();
	DrawRecommend(recommendY, recommendX, nextBlock[0], recommendR);
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
	DrawBox(9, WIDTH + 10, 4, 8);

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(15, WIDTH + 10);
	printw("SCORE");
	DrawBox(16, WIDTH + 10, 1, 8);
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

void DrawField() {
	for (int i = 0; i < HEIGHT; i++) {
		move(i + 1, 1);
		for (int j = 0; j < WIDTH; j++) {
			if (field[i][j] == 1) {
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
	move(17, WIDTH + 11);
	printw("%8d", score);
}

void DrawNextBlock(int* nextBlock) {
	for (int i = 0; i < BLOCK_HEIGHT; i++) {
		move(i + 4, WIDTH + 13);
		for (int j = 0; j < BLOCK_WIDTH; j++) {
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

	for (int i = 0; i < BLOCK_HEIGHT; i++) {
		move(10 + i, WIDTH + 13);
		for (int j = 0; j < BLOCK_WIDTH; j++) {
			if (block[nextBlock[2]][0][i][j] == 1) {
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
	for (int i = 0; i < BLOCK_HEIGHT; i++) {
		for (int j = 0; j < BLOCK_WIDTH; j++) {
			if (block[blockID][blockRotate][i][j] == 1 && i + y >= 0) {
				move(i + y + 1, j + x + 1);
				attron(A_REVERSE);
				printw("%c", tile);
				attroff(A_REVERSE);
			}
		}
	}
	move(HEIGHT, WIDTH + 10);
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

	DrawField();
	DrawBlockWithFeatures(blockY, blockX, currentBlock, blockRotate);
	move(HEIGHT, WIDTH + 10);
}

void BlockDown(int sig) {
	int temp;

	if (CheckToMove(field, nextBlock[0], blockRotate, blockY + 1, blockX) == 1) {
		blockY++;
		DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, blockY, blockX);
	}
	else {
		if (blockY == -1) {
			gameOver = 1;
		}

		temp = AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);
		if (temp != 0) {
			score += temp;
		}
		temp = DeleteLine(field);
		if (temp != 0) {
			score += temp;
		}

		nextBlock[0] = nextBlock[1];
		nextBlock[1] = nextBlock[2];
		nextBlock[2] = rand() % 7;
		blockRotate = 0;
		blockY = -1;
		blockX = WIDTH / 2 - 2;

		root = (tree*)malloc(sizeof(tree));
		root->level = 0;
		root->score = 0;
		for (int i = 0; i < HEIGHT; i++) {
			for (int j = 0; j < WIDTH; j++) {
				root->field[i][j] = field[i][j];
			}
		}
		recommendR = 0;
		recommendY = 0;
		recommendX = 0;
		modified_recommend(root);

		DrawField();
		DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate);
		DrawNextBlock(nextBlock);
		PrintScore(score);
	}

	timed_out = 0;
}

void AutoDown(int sig) {
	int temp;

	if (CheckToMove(field, nextBlock[0], blockRotate, blockY, blockX) == 0) {
		gameOver = 1;
	}
	else {
		temp = AddBlockToField(field, nextBlock[0], recommendR, recommendY, recommendX);
		if (temp != 0) {
			score += temp;
		}
		temp = DeleteLine(field);
		if (temp != 0) {
			score += temp;
		}

		nextBlock[0] = nextBlock[1];
		nextBlock[1] = nextBlock[2];
		nextBlock[2] = rand() % 7;
		blockRotate = 0;
		blockY = -1;
		blockX = WIDTH / 2 - 2;

		root = (tree*)malloc(sizeof(tree));
		root->level = 0;
		root->score = 0;
		for (int i = 0; i < HEIGHT; i++) {
			for (int j = 0; j < WIDTH; j++) {
				root->field[i][j] = field[i][j];
			}
		}
		recommendR = 0;
		recommendY = 0;
		recommendX = 0;
		modified_recommend(root);

		DrawField();
		DrawBlock(blockY, blockX, nextBlock[0], blockRotate, ' ');
		DrawRecommend(recommendY, recommendX, nextBlock[0], recommendR);
		DrawNextBlock(nextBlock);
		PrintScore(score);
	}

	timed_out = 0;
}

int AddBlockToField(char f[HEIGHT][WIDTH], int currentBlock, int blockRotate, int blockY, int blockX) {
	int touched = 0;

	for (int i = 0; i < BLOCK_HEIGHT; i++) {
		for (int j = 0; j < BLOCK_WIDTH; j++) {
			if (block[currentBlock][blockRotate][i][j] == 1) {
				if (blockX + j >= 0 && blockX + j < WIDTH && blockY + i >= 0 && blockY + i < HEIGHT) {
					f[blockY + i][blockX + j] = 1;

					if (f[blockY + i + 1][blockX + j] == 1 || blockY + i == HEIGHT - 1) {
						touched++;
					}
				}
			}
		}
	}

	return touched * 10;
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
	int shadowY;

	while (y < HEIGHT && CheckToMove(field, blockID, blockRotate, y + 1, x) == 1) {
		y++;
	}
	shadowY = y;
	
	DrawBlock(shadowY, x, blockID, blockRotate, '/');
}

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate) {
	DrawRecommend(recommendY, recommendX, blockID, recommendR);
	DrawShadow(y, x, blockID, blockRotate);
	DrawBlock(y, x, blockID, blockRotate, ' ');
}

void createRankList() {
	first = (node*)malloc(sizeof(node));
	node* temp = first;

	FILE* fp = fopen("rank.txt", "r");
	fscanf(fp, "%d", &num);

	for (int i = 0; i < num; i++) {
		node* cur = (node*)malloc(sizeof(node));
		fscanf(fp, "%s %d", cur->name, &(cur->score));
		cur->link = NULL;
		temp->link = cur;
		temp = cur;
	}

	fclose(fp);
}

void rank() {
	node* cur = first;

	clear();
	printw("1. list ranks from X to Y\n");
	printw("2. list ranks by a specific name\n");
	printw("3. delete a specific rank\n");

	switch (wgetch(stdscr)) {
		case '1': {
			int X = 1;
			int Y = num;

			echo();
			printw("X : ");
			scanw("%d", &X);
			if (X < 1) {
				X = 1;
			}
			else if (X > num) {
				X = num;
			}
	
			printw("Y : ");
			scanw("%d", &Y);
			if (Y < 1) {
				Y = 1;
			}
			else if (Y > num) {
				Y = num;
			}
			noecho();
			
			if (X > Y) {
				printw("search failure : no rank in the list\n");
			}
			else {
				printw("        name        |     score     \n");
				printw("------------------------------------\n");
		
				for (int i = 1; i <= Y; i++) {
					cur = cur->link;
	
					if (cur == NULL) {
						break;
					}
					if (i >= X) {
						printw(" %-19s| %-13d\n", cur->name, cur->score);
					}
				}
			}
			getch();
	
			break;
		}
		case '2': {
			int print_flag = 0;
			int search_flag = 0;
			char* find = (char*)malloc(sizeof(char) * (NAMELEN + 1));
	
			echo();
			printw("input the name : ");
			scanw("%s", find);
			noecho();
	
			for (int i = 0; i < num; i++) {
				cur = cur->link;
	
				if (strcmp(find, cur->name) == 0) {
					if (print_flag == 0) {
						printw("        name        |     score     \n");
						printw("------------------------------------\n");
						print_flag++;
					}

					printw(" %-19s| %-13d\n", cur->name, cur->score);
					search_flag++;
				}
			}

			if (search_flag == 0) {
				printw("search failure : no name in the list\n");
			}
			getch();

			free(find);
			break;
		}
		case '3': {
			int del;
			node* prev = NULL;

			echo();
			printw("input the rank : ");
			scanw("%d", &del);
			noecho();

			if (del < 1 || del > num) {
				printw("search failure : the rank not in the list\n");
			}
			else {
				for (int i = 0; i < del; i++) {
					prev = cur;
					cur = cur->link;
				}

				prev->link = cur->link;
				free(cur);
				num--;
				printw("result : the rank deleted\n");
				writeRankFile();
			}
			getch();

			break;
		}
		default: {
			break;
		}
	}
}

void writeRankFile() {
	node* cur = first;
	FILE* fp = fopen("rank.txt", "w");

	fprintf(fp, "%d\n", num);
	for (int i = 0; i < num; i++) {
		cur = cur->link;
		fprintf(fp, "%s %d\n", cur->name, cur->score);
	}

	fclose(fp);
}

void newRank(int score) {
	node* cur = (node*)malloc(sizeof(node));

	clear();
	echo();
	printw("your name : ");
	scanw("%s", cur->name);
	cur->score = score;
	cur->link = NULL;
	noecho();
	
	if (first->link == NULL) {
		first->link = cur;
	}
	else if (first->link->score < cur->score) {
		cur->link = first->link;
		first->link = cur;
	}
	else {
		node* temp = first->link;
		node* prev = first;

		while (temp->score >= cur->score && temp->link != NULL) {
			prev = temp;
			temp = temp->link;
		}

		if (temp->score >= cur->score && temp->link == NULL) {
			temp->link = cur;
		}
		else {
			cur->link = prev->link;
			prev->link = cur;
		}
	}
	num++;

	writeRankFile();
}

int recommend(tree* root) {
	int R, X, Y, val;
	int max = -1;
	tree* cur = (tree*)malloc(sizeof(tree));

	for (R = 0; R < NUM_OF_ROTATE; R++) {
		for (X = -3; X <= 13; X++) {
			if (CheckToMove(root->field, nextBlock[root->level], R, 0, X) == 1) {
				val = 0;

				for (int i = 0; i < HEIGHT; i++) {
					for (int j = 0; j < WIDTH; j++) {
						cur->field[i][j] = root->field[i][j];
					}
				}
				cur->level = root->level + 1;

				Y = 0;
				while (Y < HEIGHT && CheckToMove(cur->field, nextBlock[root->level], R, Y + 1, X) == 1) {
					Y++;
				}

				val += AddBlockToField(cur->field, nextBlock[root->level], R, Y, X);
				val += DeleteLine(cur->field);
				if (cur->level < MAX_LEVEL) {
					val += recommend(cur);
				}

				if (max <= val && root->level == 0) {
					max = val;
					recommendR = R;
					recommendY = Y;
					recommendX = X;
				}
			}
		}
	}

	return max;
}

int modified_recommend(tree* root) {
	int R, X, Y, val, start, end;
	int max = -1;
	tree* cur = (tree*)malloc(sizeof(tree));

	start = time(NULL);
	size += sizeof(tree);
	for (R = 0; R < NUM_OF_ROTATE; R++) {
		for (X = -3; X <= 13; X++) {
			if (CheckToMove(root->field, nextBlock[root->level], R, 0, X) == 1) {
				val = 0;

				for (int i = 0; i < HEIGHT; i++) {
					for (int j = 0; j < WIDTH; j++) {
						cur->field[i][j] = root->field[i][j];
					}
				}
				cur->level = root->level + 1;

				Y = 0;
				while (Y < HEIGHT && CheckToMove(cur->field, nextBlock[root->level], R, Y + 1, X) == 1) {
					Y++;
				}

				val += AddBlockToField(cur->field, nextBlock[root->level], R, Y, X);
				val += DeleteLine(cur->field);
				val += Y * 30;
				if (cur->level < MAX_LEVEL) {
					val += modified_recommend(cur);
				}

				if (max <= val) {
					max = val;
					if (root->level == 0 && recommendY <= Y) {
						recommendR = R;
						recommendY = Y;
						recommendX = X;
					}
				}
			}
		}
	}
	end = time(NULL);
	maketime += (double)difftime(end, start);

	return max;
}

void DrawRecommend(int y, int x, int blockID, int blockRotate) {
	DrawBlock(y, x, blockID, blockRotate, 'R');
}

void recommendedPlay() {
	time_t start, end;
	double runtime;
	int command;

	start = time(NULL);
	clear();
	act.sa_handler = AutoDown;
	sigaction(SIGALRM, &act, &oact);
	RecommendInitTetris();

	do {
		if (timed_out == 0) {
			alarm(1);
			timed_out = 1;
		}

		command = GetCommand();
		if (command == QUIT) {
			break;
		}
	} while (!gameOver);

	end = time(NULL);
	runtime = (double)difftime(end, start);
	alarm(0);
	clear();
	move(2, 3);
	printw("runningtime     : %10.2f  s", runtime);
	move(4, 3);
	printw("score           : %10d     ", score);
	move(6, 3);
	printw("time            : %10.2f  s", maketime);
	move(8, 3);
	printw("space           : %10.2f KB", (double)size / 1024);
	move(10, 3);
	printw("score per time  : %10.2f   ", score / maketime);
	move(12, 3);
	printw("score per space : %10.2f   ", score / ((double)size / 1024));
	refresh();
	getch();
}
