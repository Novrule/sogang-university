#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <ctime>

using namespace std;

void write_edge(FILE *fp, int WIDTH) {
	fprintf(fp, "+");
	for (int i = 0; i < WIDTH; i++) {
		fprintf(fp, "-+");
	}
	fprintf(fp, "\n");
}

void write_room(FILE* fp, int WIDTH, int* wall) {
	fprintf(fp, "|");
	for (int i = 0; i < WIDTH - 1; i++) {
		fprintf(fp, " ");
		if (wall[i] == 1) {
			fprintf(fp, "|");
		}
		else {
			fprintf(fp, " ");
		}
	}
	fprintf(fp, " |\n");
}

void write_wall(FILE *fp, int WIDTH, int* wall, int flag) {
	fprintf(fp, "+");
	for (int i = 0; i < WIDTH - 1; i++) {
		if (wall[i] == 1) {
			fprintf(fp, "-+");
		}
		else {
			fprintf(fp, " +");
		}
	}
	if (flag == 1) {
		fprintf(fp, "-+");
	}
	else {
		fprintf(fp, " +");
	}
	fprintf(fp, "\n");
}

int main(void) {
	int WIDTH, HEIGHT, meet_flag;
	int temp = 0;
	int group = 0;
	FILE* fp = fopen("maze.maz", "wt");

	cout << "WIDTH : ";
	cin >> WIDTH;
	cout << "HEIGHT : ";
	cin >> HEIGHT;

	srand((unsigned int)time(NULL));

	int* maze = new int[WIDTH];
	int* wall = new int[WIDTH - 1];

	write_edge(fp, WIDTH);
	for (int i = 0; i < WIDTH - 1; i++) {
		wall[i] = rand() % 2;
		if (wall[i] == 1) {
			for (int j = temp; j <= i; j++) {
				maze[j] = group;
			}
			temp = i + 1;
			group++;
		}
	}
	for (int i = temp; i < WIDTH; i++) {
		maze[i] = group;
	}
	group++;
	write_room(fp, WIDTH, wall);

	for (int i = 1; i < HEIGHT - 1; i++) {
		meet_flag = 0;
		for (int j = 0; j < WIDTH - 1; j++) {
			wall[j] = rand() % 2;

			if (wall[j] == 0) {
				meet_flag = 1;
			}

			if (maze[j] != maze[j + 1]) {
				if (meet_flag == 0) {
					wall[j] = 0;
				}
				meet_flag = 0;
			}

			if (wall[j] == 1) {
				maze[j] = group++;
			}
		}
		if (meet_flag == 1) {
			maze[WIDTH - 1] = group++;
		}
		write_wall(fp, WIDTH, wall, meet_flag);

		meet_flag = 0;
		for (int j = 0; j < WIDTH - 1; j++) {
			if (maze[j] != maze[j + 1]) {
				meet_flag = rand() % 2;

				if (meet_flag == 1) {
					temp = maze[j + 1];
					maze[j + 1] = maze[j];
					for (int k = 0; k < WIDTH; k++) {
						if (maze[k] == temp) {
							maze[k] = maze[j];
						}
					}

					wall[j] = 0;
				}
				else {
					wall[j] = 1;
				}
			}
			else {
				wall[j] = 1;
			}
		}
		write_room(fp, WIDTH, wall);
	}

	meet_flag = 0;
	for (int i = 0; i < WIDTH - 1; i++) {
		wall[i] = rand() % 2;

		if (wall[i] == 0) {
			meet_flag = 1;
		}

		if (maze[i] != maze[i + 1]) {
			if (meet_flag == 0) {
				wall[i] = 0;
			}
			meet_flag = 0;
		}

		if (wall[i] == 1) {
			maze[i] = group++;
		}
	}
	if (meet_flag == 1) {
		maze[WIDTH - 1] = group++;
	}
	write_wall(fp, WIDTH, wall, meet_flag);

	for (int i = 0; i < WIDTH - 1; i++) {
		if (maze[i] != maze[i + 1]) {
			wall[i] = 0;

			temp = maze[i + 1];
			maze[i + 1] = temp;
			for (int j = i; j < WIDTH; j++) {
				if (maze[j] == temp) {
					maze[j] = maze[i];
				}
			}
		}
		else {
			wall[i] = 1;
		}
	}
	write_room(fp, WIDTH, wall);
	write_edge(fp, WIDTH);

	delete[] maze;
	delete[] wall;
	fclose(fp);

 	return 0;
}