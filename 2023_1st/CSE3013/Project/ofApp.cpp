#include "ofApp.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <stack>
#include <queue>

//--------------------------------------------------------------
void ofApp::setup() { // 시작 설정을 해주는 함수
	ofSetFrameRate(30); // 초당 프레임을 설정해준다.
	ofBackground(255, 255, 255); // 배경의 색을 흰색으로 설정해준다.

	game_flag = 0; // 플래그들을 초기화해준다.
	key_flag = 0;
}

//--------------------------------------------------------------
void ofApp::update() {

}

//--------------------------------------------------------------
void ofApp::draw() { // 미로와 플레이어를 그려주는 함수
	int area = 10; // 방의 크기를 10으로 설정해준다.

	if (game_flag == 1) { // 게임이 시작되었다면 그려준다.
		drawShadow(); // 미로에 그림자를 먼저 그려준다.

		ofSetColor(0); // 선의 색을 검정색으로 설정해준다.
		ofSetLineWidth(10); // 선의 굵기를 10으로 설정해준다.
		for (int i = 0; i < HEIGHT; i += 2) { // 미로를 저장한 배열을 돌면서 선을 그어야 하는 곳을 찾아준다.
			for (int j = 0; j < WIDTH; j += 2) {
				if (i != HEIGHT - 1) {
					if (maze[i][j] == -1 && maze[i + 1][j] == -1) { // 현재 위치가 벽이고 바로 오른쪽도 벽이라면
						ofDrawLine(j * area + 5, i * area, j * area + 5, (i + 2) * area + 10); // 벽을 그려준다.
					}
				}
				if (j != WIDTH - 1) {
					if (maze[i][j] == -1 && maze[i][j + 1] == -1) { //현재 위치가 벽이고 바로 밑도 벽이라면
						ofDrawLine(j * area, i * area + 5, (j + 2) * area + 10, i * area + 5); // 벽을 그려준다
					}
				}
			}
		}

		ofSetColor(0); // 플레이어의 위치와 저장한 위치를 검정색으로 설정한다.
		if (save_flag == 1) { // 만약 게임 중 위치를 저장했다면
			ofDrawCircle(sx * area + 5, sy * area + 5, 5); // 저장한 위치를 그려준다.
		}
		ofDrawCircle(px * area + 5, py * area + 5, 5); // 플레이어를 그려준다.

		if (px == WIDTH - 2 && py == HEIGHT - 2) { // 만약 도착지점에 도착했다면 게임을 깬 것이므로
			gameClear(); // 게임을 깼을 때 필요한 것들을 해준다.
		}
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) { // 키가 클릭이 되었을 때 작동하는 함수
	if (key == 's') { // s를 눌렀고
		if (game_flag == 0 && key_flag == 0) { // 게임 중이 아니면서 다른 작업이 진행 중이 아니라면
			key_flag = 1; // 작업 중으로 표시를 해준다.
			gameStart(); // 게임을 시작할 때 필요한 것들을 해준다.
			key_flag = 0; // 작업이 끝났다고 표시를 해준다.
		}
	}

	if (key == 'r') { // r을 눌렀고
		if (key_flag == 0) { // 다른 작업이 진행 중이 아니라면
			game_flag = 0; // 게임 중이 아니라고 표시를 해준다.
			key_flag = 1; // 작업 중으로 표시를 해준다.
			freeMemory(); // 동적 할당한 메모리를 해제해준다.
			gameStart(); // 게임을 시작할 때 필요한 것들을 해준다.
			key_flag = 0; // 작업이 끝났다고 표시를 해준다.
		}
	}

	if (key == 'q') { // q를 눌렀고
		if (key_flag == 0) { // 다른 작업이 진행 중이 아니라면
			game_flag = 0; // 게임 중이 아니라고 표시를 해준다.
			key_flag = 1; // 작업 중으로 표시를 해준다.
			printf("Good Bye!\n");
			freeMemory(); // 동적 할당한 메모리를 해제해준다.
			_Exit(0); // 프로그램을 종료해준다.
		}
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) { // 키를 클릭했다 떼었을 때 작동하는 함수
	if (key == OF_KEY_RIGHT) { // 오른쪽 방향키를 눌렀다 뗐고
		if (game_flag == 1 && px + 2 < WIDTH) { // 게임 중이면서 플레이어가 맵의 오른쪽 끝에 있지 않고
			if (maze[py][px + 1] != -1) { // 플레이어의 오른쪽에 벽이 없다면
				px += 2; // 플레이어를 오른쪽으로 이동시켜 준다.
				cnt++; // 플레이어가 이동한 횟수에 1을 더해준다.
			}
		}
	}

	if (key == OF_KEY_LEFT) { // 왼쪽 방향키를 눌렀다 뗐고
		if (game_flag == 1 && px - 2 >= 0) { // 게임 중이면서 플레이어가 맵의 왼쪽 끝에 있지 않고
			if (maze[py][px - 1] != -1) { // 플레이어의 왼쪽에 벽이 없다면
				px -= 2; // 플레이어를 왼쪽으로 이동시켜 준다.
				cnt++; // 플레이어가 이동한 횟수에 1을 더해준다.
			}
		}
	}

	if (key == OF_KEY_DOWN) { // 아래쪽 방향키를 눌렀다 뗐고
		if (game_flag == 1 && py + 2 < HEIGHT) { // 게임 중이면서 플레이어가 맵의 아래쪽 끝에 있지 않고
			if (maze[py + 1][px] != -1) { // 플레이어의 아래쪽에 벽이 없다면
				py += 2; // 플레이어를 아래쪽으로 이동시켜 준다.
				cnt++; // 플레이어가 이동한 횟수에 1을 더해준다.
			}
		}
	}

	if (key == OF_KEY_UP) { // 위쪽 방향키를 눌렀다 뗐고
		if (game_flag == 1 && py - 2 >= 0) { // 게임 중이면서 플레이어가 맵의 위쪽 끝에 있지 않고
			if (maze[py - 1][px] != -1) { // 플레이어의 위쪽에 벽이 없다면
				py -= 2; // 플레이어를 위쪽으로 이동시켜 준다.
				cnt++; // 플레이어가 이동한 횟수에 1을 더해준다.
			}
		}
	}

	if (key == 't') { // t를 눌렀다 뗐고
		if (game_flag == 1 && save_flag == 1) { // 게임 중이면서 이전에 플레이어의 위치를 저장했었다면
			px = sx; // 저장한 위치로 플레이어를 이동시켜 준다.
			py = sy;
			cnt++; // 플레이어가 이동한 횟수에 1을 더해준다.
			save_flag = 0; // 플레이어의 위치를 저장하지 않았다고 표시를 해준다.
		}
	}

	if (key == ' ') { // 스페이스바를 눌렀다 뗐고
		if (game_flag == 1) { //게임 중이라면
			sx = px; // 플레이어의 위치를 저장해준다.
			sy = py;
			cnt++; // 플레이어가 이동한 횟수에 1을 더해준다.
			save_flag = 1; // 플레이어의 위치를 저장했다고 표시를 해준다.
		}
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) { 

}

void ofApp::input() { // 미로의 크기와 난이도를 입력받아주는 함수
	printf("Width? : "); // 미로의 가로, 세로의 방 갯수를 받아준다.
	scanf("%d", &WIDTH);
	printf("Height? : ");
	scanf("%d", &HEIGHT);
	HEIGHT = 2 * HEIGHT + 1; // 벽을 포함한 미로의 실제 크기를 계산해준다.
	WIDTH = 2 * WIDTH + 1;

	printf("Mode?\n"); // 플레이할 모드를 받아준다.
	printf("1. Easy\n");
	printf("2. Normal\n");
	printf("3. Hard\n");
	printf("4. Extreme Hard\n");
	while (1) {
		scanf("%d", &mode);
		if (mode == 1 || mode == 2 || mode == 3 || mode == 4) { // 올바른 모드를 받으면
			break; // 그 모드로 플레이한다.
		}
		else { // 올바르지 않은 모드를 받으면
			printf("Please enter correct value.\n"); // 오류 메시지를 출력 후 다시 모드를 입력받는다.
		}
	}
}

void ofApp::init() { // 미로를 저장할 배열을 동적 할당해주고 변수들을 초기화해주는 함수
	px = 1; // 플레이어의 위치를 초기화해준다.
	py = 1;
	cnt = 0; // 플레이어가 움직인 횟수를 초기화해준다.
	save_flag = 0; // 플레이어의 위치를 저장하지 않았다고 표시를 해준다.

	maze = new int* [HEIGHT]; // 미로를 저장할 배열을 동적 할당해준다.
	for (int i = 0; i < HEIGHT; i++) {
		maze[i] = new int[WIDTH];
	}

	for (int i = 0; i < HEIGHT; i++) { // 미로를 저장할 배열을 초기화해준다.
		for (int j = 0; j < WIDTH; j++) {
			maze[i][j] = -1;
		}
	}
}

void ofApp::randDir() { // 미로를 생성할 때 사용할 dir배열을 랜덤으로 섞어주는 함수
	int a, b, cur;

	random_device rd; // 랜덤으로 숫자를 뽑기 위한 seed를 저장하는 변수다.
	mt19937 gen(rd()); // 랜덤으로 생성된 seed를 저장해준다.
	uniform_int_distribution<int> dis(0, 3); // 생성 범위를 0부터 3까지로 지정해준다.

	for (int i = 0; i < 4; i++) { // 0부터 3까지 dir배열을 돌면서 랜덤으로 선택된 index에 있는 방향과 현재의 index에 있는 방향을 바꿔준다.
		cur = dis(gen); // seed를 이용해 0부터 3까지 랜덤으로 뽑은 숫자를 저장해준다.

		a = dir[i][0];
		b = dir[i][1];

		dir[i][0] = dir[cur][0];
		dir[i][1] = dir[cur][1];

		dir[cur][0] = a;
		dir[cur][1] = b;
	}
}

void ofApp::makeMaze() { // 입력받은 미로의 크기를 바탕으로 랜덤한 미로를 생성해주는 함수
	int row, col, nrow, ncol, push_flag;
	stack<pair<int, int>> s; // DFS로 미로를 생성하기 위해서 stack을 선언해준다.

	s.push(make_pair(1, 1)); // 현재 위치를 stack에 넣어주고 방문했다고 표시를 해준다.
	maze[1][1] = 0;
	while (s.empty() == 0) { // stack이 빌 때까지 반복해준다.
		push_flag = 0; // stack에 push했다는 것을 표시를 해주는 플래그다.

		row = s.top().first; // stack의 맨 위에 있는 위치를 가져와준다. (pop은 하지 않는다.)
		col = s.top().second;

		randDir();
		for (int i = 0; i < 4; i++) { // 이동할 방향이 위, 아래, 왼쪽, 오른쪽 총 4개 있으므로 4번 반복한다.
			nrow = row + 2 * dir[i][0]; // 현재 위치와 이동할 방향에 따라서 이동할 위치를 저장해준다.
			ncol = col + 2 * dir[i][1];

			if (nrow >= 1 && nrow <= HEIGHT - 2 && ncol >= 1 && ncol <= WIDTH - 2 && maze[nrow][ncol] == -1) { // 이동할 위치가 맵을 벗어나지 않고 아직 방문하지 않는 곳이라면
				s.push(make_pair(nrow, ncol)); // 이동할 위치를 stack에 넣어준다.
				push_flag = 1; // stack에 push했다고 표시를 해준다.
				maze[nrow][ncol] = 0; // 이동할 위치를 방문한 곳이라고 표시를 해준다.
				maze[row + dir[i][0]][col + dir[i][1]] = 0; // 방끼리 이동하기 위해선 사이에 있는 벽도 통과해야 하므로 벽도 방문한 곳이라고 표시를 해준다.
			}
		}

		if (push_flag == 0) { // for문을 다 돌았음에도 stack에 push를 하지 않았다면 (현재 위치에서 갈 수 있는 곳이 없는 것과 같다.)
			s.pop(); // 현재 위치를 stack에서 pop해준다.
		}
	}
}

void ofApp::mazeSolve() { // 랜덤으로 만든 미로를 통과하기 위해 필요한 움직임의 최소값을 구해주는 함수
	int row, col, nrow, ncol, mrow, mcol;
	int num = 1; // 반복문이 반복된 횟수를 저장해주는 변수
	ans = 0; // 미로를 통과하기 위해 필요한 움직임의 최소값을 초기화해준다.
	queue<pair<int, int>> q; // 항상 최선의 정답을 도출하는 BFS를 사용할 것이므로 queue를 선언한다.

	q.push(make_pair(1, 1)); // 현재 위치를 queue에 넣어주고 방문했다고 표시를 해준다.
	maze[1][1] = num++;

	while (q.empty() == 0) { // queue가 빌 때까지 반복해준다.
		row = q.front().first; // queue의 맨 앞에 있는 위치를 가져와주고 pop해준다.
		col = q.front().second;
		q.pop();

		if (row == HEIGHT - 2 && col == WIDTH - 2) { // 도착지에 도착했다면
			break; // 반복문을 종료해준다.
		}

		for (int i = 0; i < 4; i++) { // 이동할 방향이 위, 아래, 왼쪽, 오른쪽 총 4개 있으므로 4번 반복한다.
			nrow = row + dx[i]; // 현재 위치와 이동할 방향에 따라서 이동할 위치를 저장해준다.
			ncol = col + dy[i];

			if (maze[nrow][ncol] == 0) { // 이동할 위치가 이동할 수 있는 곳이라면
				q.push(make_pair(nrow, ncol)); //이동할 위치를 queue에 넣어준다.
				maze[nrow][ncol] = num; // 이동할 위치를 방문한 곳이라고 표시하는데 반복문을 반복한 횟수로 표시를 해준다.
			}
		}
		num++; // 반복문이 반복되었으므로 반복문이 반복된 횟수를 저장해주는 변수에 1을 더해준다. 
	}

	row = HEIGHT - 2; // 최적의 정답을 찾기 위해 도착지에서부터 이동할 것이므로 현재 위치를 도착지로 바꿔준다.
	col = WIDTH - 2;
	while (1) {
		for (int i = 0; i < 4; i++) { // 이동할 방향이 위, 아래, 왼쪽, 오른쪽 총 4개 있으므로 4번 반복한다.
			mrow = row + dx[i]; // 현재 위치와 이동할 방향에 따라서 이동할 위치와 그 사이에 있는 위치를 저장해준다.
			nrow = row + dx[i] * 2;
			mcol = col + dy[i];
			ncol = col + dy[i] * 2;

			if (nrow >= 1 && nrow <= HEIGHT - 2 && ncol >= 1 && ncol <= WIDTH - 2) { // 이동할 위치가 맵을 벗어나지 않고
				if (maze[row][col] > maze[nrow][ncol] && maze[mrow][mcol] > 0 && maze[nrow][ncol] > 0) { // 이동할 위치가 현재 위치보다 반복문이 반복된 횟수가 적고 (많다면 출발지로 가까워 지는 것이 아니므로 최적의 정답이 아니게 된다.) 이동할 위치와 중간에 있는 위치가 모두 이동할 수 있는 곳이라면 (이동할 위치와 현재 위치의 중간에 벽이 있으면 이동할 수 없다.)
					ans++; // 미로를 통과하기 위해 필요한 움직임의 최소값에 1을 더해준다.
					row = nrow; // 이동할 위치를 현재 위치로 저장해준다.
					col = ncol;
					break; // 이동할 수 있는 위치는 하나밖에 없으므로 이동할 위치를 찾으면 종료해준다. (BFS의 특성 상 이동할 위치 중 반복문이 반복된 횟수가 현재 위치보다 적은 곳은 하나밖에 없다.)
				}
			}
		}

		if (row == 1 && col == 1) { // 출발지에 도착했다면
			break; // 반복문을 종료해준다.
		}
	}
}

void ofApp::freeMemory() { // 동적 할당한 메모리를 해제해주는 함수
	for (int i = 0; i < HEIGHT; i++) { // 동적 할당한 배열을 초기화해준다.
		delete[] maze[i];
	}
	delete[] maze;
}

void ofApp::gameStart() { // 게임을 시작하기 위해 필요한 것들을 해주는 함수
	input(); // 입력을 받아준다.
	init(); // 미로를 저장할 배열을 동적 할당해주고 변수들을 초기화 해준다.
	makeMaze(); // 미로를 만들어준다.
	mazeSolve(); // 움직임의 최소값을 구해준다.
	printf("Game Start!\n"); // 게임이 시작되었다는 메시지를 출력해준다.
	game_flag = 1; // 게임이 시작되었다고 표시를 해준다.
}

void ofApp::drawShadow() { // 모드에 따라 미로에 그림자를 그려주는 함수
	int area = 10; // 방들 크기를 10으로 설정해준다.

	ofSetColor(0); // 그림자의 색을 검정색으로 설정해준다.
	if (mode == 2) { // 모드가 Normal이라면
		ofDrawRectangle(5, 5, WIDTH * area - 5, HEIGHT * area - 5); // 미로를 전부 그림자로 덮어준다.
		ofSetColor(255); // 배경의 색을 흰색으로 설정해준다.
		ofDrawRectangle((px - 10) * area + 5, (py - 10) * area + 5, 20 * area, 20 * area); // 플레이어의 위치로부터 위, 아래, 왼쪽, 오른쪽으로 10칸씩 밝혀준다.
	}
	else if (mode == 3) { // 모드가 Hard라면
		ofDrawRectangle(5, 5, WIDTH * area - 5, HEIGHT * area - 5); // 미로를 전부 그림자로 덮어준다.
		ofSetColor(255); // 배경의 색을 흰색으로 설정해준다.
		ofDrawRectangle((px - 5) * area + 5, (py - 5) * area + 5, 10 * area, 10 * area); // 플레이어의 위치로부터 위, 아래, 왼쪽, 오른쪽으로 5칸씩 밝혀준다.
	}
	else if (mode == 4) { // 모드가 Extreme Hard라면
		ofDrawRectangle(5, 5, WIDTH * area - 5, HEIGHT * area - 5); // 미로를 전부 그림자로 덮어준다.
		ofSetColor(255); // 배경의 색을 흰색으로 설정해준다.
		ofDrawRectangle((px - 3) * area + 5, (py - 3) * area + 5, 6 * area, 6 * area); // 플레이어의 위치로부터 위, 아래, 왼쪽, 오른쪽으로 3칸씩 밝혀준다.
	}
}

void ofApp::gameClear() { // 게임을 깼을 때 필요한 것들을 해주는 함수
	game_flag = 0; // 게임이 종료되었다고 표시를 해준다.

	printf("Game Clear!\n"); // 게임이 종료되었다는 메시지를 출력해준다.
	printf("Your score is %d.\n", cnt); // 플레이어가 이동한 횟수를 출력해준다.
	if (cnt <= ans * 1.5) { // 플레이어가 이동한 횟수와 최적의 정답의 비율을 바탕으로 랭크를 출력해준다.
		printf("Your rank is A.\n");
	}
	else if (cnt <= ans * 2) {
		printf("Your rank is B.\n");
	}
	else if (cnt <= ans * 2.5) {
		printf("Your rank is C.\n");
	}
	else if (cnt <= ans * 3) {
		printf("Your rank is D.\n");
	}
	else {
		printf("Your rank is F.\n");
	}
	printf("If you want to play again, please press R.\n"); // 다시 하고 싶으면 r, 종료하고 싶으면 q를 누르라는 메시지를 출력해준다.
	printf("Or if you want to exit, please press Q.\n");
}