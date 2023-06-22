#pragma onceㅁ

#include "ofMain.h"

class ofApp : public ofBaseApp{
public:
	void setup(); // 시작 설정을 해주는 함수
	void update();
	void draw(); // 미로와 플레이어를 그려주는 함수
	void keyPressed(int key); // 키가 클릭이 되었을 때 작동하는 함수
	void keyReleased(int key); // 키를 클릭했다 떼었을 때 작동하는 함수
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	void input(); // 미로의 크기와 난이도를 입력받아주는 함수
	void init(); // 미로를 저장할 배열을 동적 할당해주고 초기화해주는 함수
	void randDir(); // 미로를 생성할 때 사용할 dir배열을 랜덤으로 섞어주는 함수
	void makeMaze(); // 입력받은 미로의 크기를 바탕으로 랜덤한 미로를 생성해주는 함수
	void mazeSolve(); // 랜덤으로 만든 미로를 통과하기 위해 필요한 움직임의 최소값을 구해주는 함수
	void freeMemory(); // 동적 할당한 메모리를 해제해주는 함수
	void gameStart(); // 게임을 시작하기 위해 필요한 것들을 해주는 함수
	void drawShadow(); // 모드에 따라 미로에 그림자를 그려주는 함수
	void gameClear(); // 게임을 깼을 때 필요한 것들을 해주는 함수

	int HEIGHT, WIDTH; // 미로의 크기를 저장하는 변수
	int mode; // 난이도를 저장하는 변수
	int ans; // 미로를 통과하기 위해 필요한 움직임의 최소값을 저장하는 변수
	int px, py; // 플레이어의 위치를 나타내는 변수
	int cnt; // 플레이어가 움직인 횟수를 저장하는 변수
	int sx, sy; // 플레이어가 저장한 위치를 나타내는 변수
	int** maze; // 미로를 저장하는 배열
	int dir[4][2] = { {1, 0}, {-1, 0}, {0, 1}, {0,-1} }; // 미로를 생성할 때 DFS를 사용하는데, 이 때 이동할 방향을 모아놓은 배열
	int dx[4] = { 1, -1, 0, 0 }; // 미로에서 길을 찾을 때 BFS를 사용하는데 이 때 x축으로 이동할 방향을 모아놓은 배열
	int dy[4] = { 0, 0, 1, -1 }; // 미로에서 길을 찾을 때 BFS를 사용하는데 이 때 y축으로 이동할 방향을 모아놓은 배열
	int game_flag; // 게임이 시작되었는지 않았는지를 나타내는 플래그
	int key_flag; // 이미 키가 눌려서 작업 중이므로 다른 작업이 불가능한지 키가 눌리지 않아 다른 작업이 가능한지 나타내는 플래그
	int save_flag; // 위치를 저장했는지 하지 않았는지를 나타내는 플래그
};
