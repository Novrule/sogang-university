#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	int draw_flag;
	int load_flag;

	int line_num;
	int dot_num;
	float diameter;

	typedef struct water_line {
		int from_x, to_x;
		int from_y, to_y;
	} water_line;

	typedef struct water_dot {
		int x;
		int y;
	} water_dot;

	water_line* line_arr;
	water_dot* dot_arr;
	water_dot* water_arr;

	int dot_index;
	int fall_flag;
	int fall_index;

	void processOpenFileSelection(ofFileDialogResult openFileResult);
	void initializeWaterLines();
};