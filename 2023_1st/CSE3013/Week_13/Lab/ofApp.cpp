/*

	ofxWinMenu basic example - ofApp.cpp

	Example of using ofxWinMenu addon to create a menu for a Microsoft Windows application.
	
	Copyright (C) 2016-2017 Lynn Jarvis.

	https://github.com/leadedge

	http://www.spout.zeal.co

    =========================================================================
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    =========================================================================

	03.11.16 - minor comment cleanup
	21.02.17 - rebuild for OF 0.9.8

*/

#include "ofApp.h"
#include <iostream>
#include <stack>
#include <algorithm>

using namespace std;

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetWindowTitle("Maze Example"); //Set the app name on the title bar
	ofSetFrameRate(15);
	ofBackground(255, 255, 255);
	//Get the window size for image loading
	windowWidth = ofGetWidth();
	windowHeight = ofGetHeight();
	isdfs = false;
	isOpen = 0;
	//Centre on the screen
	ofSetWindowPosition((ofGetScreenWidth()-windowWidth)/2, (ofGetScreenHeight()-windowHeight)/2);

	//Load a font rather than the default
	myFont.loadFont("verdana.ttf", 12, true, true);

	//Load an image for the example
	//myImage.loadImage("lighthouse.jpg");

	//Window handle used for topmost function
	hWnd = WindowFromDC(wglGetCurrentDC());

	//Disable escape key exit so we can exit fullscreen with Escape (see keyPressed)
	ofSetEscapeQuitsApp(false);

	//
	//Create a menu using ofxWinMenu
	//

	//A new menu object with a pointer to this class
	menu = new ofxWinMenu(this, hWnd);

	//Register an ofApp function that is called when a menu item is selected.
	//The function can be called anything but must exist. 
	//See the example "appMenuFunction".
	menu->CreateMenuFunction(&ofApp::appMenuFunction);

	//Create a window menu
	HMENU hMenu = menu->CreateWindowMenu();

	//
	//Create a "File" popup menu
	//
	HMENU hPopup = menu->AddPopupMenu(hMenu, "File");

	//
	//Add popup items to the File menu
	//

	//Open an maze file
	menu->AddPopupItem(hPopup, "Open", false, false); //Not checked and not auto-checked
	
	//Final File popup menu item is "Exit" - add a separator before it
	menu->AddPopupSeparator(hPopup);
	menu->AddPopupItem(hPopup, "Exit", false, false);

	//
	//View popup menu
	//
	hPopup = menu->AddPopupMenu(hMenu, "View");

	bShowInfo = true; //screen info display on
	menu->AddPopupItem(hPopup, "Show DFS", false, false); //Checked
	bTopmost = false; //app is topmost
	menu->AddPopupItem(hPopup, "Show BFS"); //Not checked (default)
	bFullscreen = false; //not fullscreen yet
	menu->AddPopupItem(hPopup, "Full screen", false, false); //Not checked and not auto-check

	//
	//Help popup menu
	//
	hPopup = menu->AddPopupMenu(hMenu, "Help");
	menu->AddPopupItem(hPopup, "About", false, false); //No auto check

	//Set the menu to the window
	menu->SetWindowMenu();
} //end Setup

//
//Menu function
//
//This function is called by ofxWinMenu when an item is selected.
//The the title and state can be checked for required action.
// 
void ofApp::appMenuFunction(string title, bool bChecked) {
	ofFileDialogResult result;
	string filePath;
	size_t pos;

	//
	//File menu
	//
	if (title == "Open") {
		readFile();
	}
	if (title == "Exit") {
		ofExit(); //Quit the application
	}

	//
	//Window menu
	//
	if(title == "Show DFS") {
		//bShowInfo = bChecked; //Flag is used elsewhere in Draw()
		if (isOpen) {
			DFS();
			bShowInfo = bChecked;
		}
		else {
			cout << "you must open file first" << endl;
		}
	}

	if (title == "Show BFS") {
		doTopmost(bChecked); //Use the checked value directly
	}

	if (title == "Full screen") {
		bFullscreen = !bFullscreen; //Not auto-checked and also used in the keyPressed function
		doFullScreen(bFullscreen); //But als take action immediately
	}

	//
	//Help menu
	//
	if (title == "About") {
		ofSystemAlertDialog("ofxWinMenu\nbasic example\n\nhttp://spout.zeal.co");
	}
} //end appMenuFunction

//--------------------------------------------------------------
void ofApp::update() {

}

//--------------------------------------------------------------
void ofApp::draw() {
	char str[256];
	int area = 20;
	//ofBackground(0, 0, 0, 0);
	ofSetColor(0);
	ofSetLineWidth(10);

	if (isOpen) {
		ofDrawLine(0, 5, (WIDTH - 1) * area + 5, 5);
		ofDrawLine(5, 0, 5, (HEIGHT - 1) * area + 5);
		ofDrawLine(0, (HEIGHT - 1) * area + 5, (WIDTH - 1) * area + 10, (HEIGHT - 1) * area + 5);
		ofDrawLine((WIDTH - 1) * area + 5, 0, (WIDTH - 1) * area + 5, (HEIGHT - 1) * area + 10);

		for (int i = 1; i < HEIGHT - 1; i++) {
			for (int j = 1; j < WIDTH - 1; j++) {
				if (input[i][j] == '|') {
					ofDrawLine(j * area + 5, (i - 1) * area, j * area + 5, (i + 1) * area + 10);
				}
				else if (input[i][j] == '-') {
					ofDrawLine((j - 1) * area, i * area + 5, (j + 1) * area + 10, i * area + 5);
				}
			}
		}
	}

	if (isdfs) {
		if (isOpen) {
			dfsdraw();
		}
		else {
			cout << "You must open file first" << endl;
		}
	}

	if (bShowInfo) {
		//Show keyboard duplicates of menu functions
		sprintf(str, "comsil project");
		myFont.drawString(str, 15, ofGetHeight() - 20);
	}
} //end Draw

void ofApp::doFullScreen(bool bFull) {
	//Enter full screen
	if (bFull) {
		//Remove the menu but don't destroy it
		menu->RemoveWindowMenu();
		//hide the cursor
		ofHideCursor();
		//Set full screen
		ofSetFullscreen(true);
	}
	else { 
		//return from full screen
		ofSetFullscreen(false);
		//Restore the menu
		menu->SetWindowMenu();
		//Restore the window size allowing for the menu
		ofSetWindowShape(windowWidth, windowHeight + GetSystemMetrics(SM_CYMENU));
		//Centre on the screen
		ofSetWindowPosition((ofGetScreenWidth() - ofGetWidth()) / 2, (ofGetScreenHeight() - ofGetHeight()) / 2);
		//Show the cursor again
		ofShowCursor();
		//Restore topmost state
		if (bTopmost) {
			doTopmost(true);
		}
	}
} //end doFullScreen

void ofApp::doTopmost(bool bTop) {
	if (bTop) {
		//get the current top window for return
		hWndForeground = GetForegroundWindow();
		//Set this window topmost
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		ShowWindow(hWnd, SW_SHOW);
	}
	else {
		SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		ShowWindow(hWnd, SW_SHOW);
		//Reset the window that was topmost before
		if (GetWindowLong(hWndForeground, GWL_EXSTYLE) & WS_EX_TOPMOST) {
			SetWindowPos(hWndForeground, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		else {
			SetWindowPos(hWndForeground, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
	}
} //end doTopmost

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	//Escape key exit has been disabled but it can be checked here
	if (key == VK_ESCAPE) {
		//Disable fullscreen set, otherwise quit the application as usual
		if (bFullscreen) {
			bFullscreen = false;
			doFullScreen(false);
		}
		else {
			ofExit();
		}
	}

	//Remove or show screen info
	if (key == ' ') {
		bShowInfo = !bShowInfo;
		//Update the menu check mark because the item state has been changed here
		menu->SetPopupItem("Show DFS", bShowInfo);
	}

	if (key == 'f') {
		bFullscreen = !bFullscreen;
		doFullScreen(bFullscreen);
		//Do not check this menu item
		//If there is no menu when you call the SetPopupItem function it will crash
	}
} //end keyPressed

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

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
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}

bool ofApp::readFile() {
	ofFileDialogResult openFileResult = ofSystemLoadDialog("Select .maz file");
	string filePath;
	size_t pos;
	//Check whether the user opened a file
	if (openFileResult.bSuccess) {
		ofLogVerbose("User selected a file");

		//We have a file, check it and process it
		string fileName = openFileResult.getName();
		//string fileName = "maze0.maz";
		printf("File name is %s\n", fileName);
		filePath = openFileResult.getPath();
		printf("Open\n");
		pos = filePath.find_last_of(".");

		if (pos != string::npos && pos != 0 && filePath.substr(pos + 1) == "maz") {
			ofFile file(fileName);

			if (!file.exists()) {
				cout << "Target file does not exists." << endl;
				return false;
			}
			else {
				cout << "We found the target file." << endl;
				isOpen = 1;
			}

			ofBuffer buffer(file);

			freeMemory();

			ofBuffer::Line start = buffer.getLines().begin();
			ofBuffer::Line end = buffer.getLines().end();
			string line = *start;

			HEIGHT = 0;
			for (; start != end; ++start) {
				HEIGHT++;
			}
			WIDTH = line.size();

			input = new char* [HEIGHT];
			visited = new int* [HEIGHT];
			for (int i = 0; i < HEIGHT; i++) {
				input[i] = new char[WIDTH];
				visited[i] = new int[WIDTH];
			}

			start = buffer.getLines().begin();
			end = buffer.getLines().end();
			for (int i = 0; start != end; ++start, i++) {
				line = *start;
				for (int j = 0; j < WIDTH; j++) {
					input[i][j] = line[j];
					if (line[j] == ' ') {
						visited[i][j] = 0;
					}
					else {
						visited[i][j] = -1;
					}
				}
			}
		}
		else {
			printf("Needs a '.maz' extension\n");
			return false;
		}
	}
}

void ofApp::freeMemory() {
	isdfs = false;

	for (int i = 0; i < HEIGHT; i++) {
		delete[] input[i];
		delete[] visited[i];
	}
	delete[] input;
	delete[] visited;
}

void ofApp::DFS() { //DFS탐색을 하는 함수
	int next_col, next_row;
	int num = 1;
	isdfs = true;
	stack<pair<int, int>> s;

	s.push(make_pair(1, 1));
	visited[1][1] = num++;

	while (s.empty() == 0) {
		maze_col = s.top().first;
		maze_row = s.top().second;
		s.pop();

		if (maze_col == HEIGHT - 2 && maze_row == WIDTH - 2) {
			break;
		}

		for (int i = 0; i < 4; i++) {
			next_col = maze_col + dy[i];
			next_row = maze_row + dx[i];

			if (visited[next_col][next_row] == 0) {
				s.push(make_pair(next_col, next_row));
				visited[next_col][next_row] = num;
			}
		}
		num++;
	}
}

void ofApp::dfsdraw() {
	int mid_col, mid_row, next_col, next_row;
	int cur_col = HEIGHT - 2;
	int cur_row = WIDTH - 2;
	int area = 20;

	ofSetColor(200);
	for (int i = 1; i < HEIGHT - 1; i += 2) {
		for (int j = 1; j < WIDTH - 1; j += 2) {
			if (visited[i][j] > 0) {
				for (int k = 0; k < 4; k++) {
					mid_col = i + dy[k];
					next_col = i + dy[k] * 2;
					mid_row = j + dx[k];
					next_row = j + dx[k] * 2;

					if (next_col >= 1 && next_col <= HEIGHT - 2 && next_row >= 1 && next_row <= WIDTH - 2) {
						if (visited[mid_col][mid_row] > 0 && visited[next_col][next_row] > 0) {
							if (dx[k] != 0) {
								ofDrawLine(j * area, i * area + 5, next_row * area + 10, next_col * area + 5);
							}
							else {
								ofDrawLine(j * area + 5, i * area, next_row * area + 5, next_col * area + 10);
							}
						}
					}
				}
			}
		}
	}

	ofSetColor(100);
	while (cur_col != 1 || cur_row != 1) {
		for (int i = 0; i < 4; i++) {
			mid_col = cur_col + dy[i];
			next_col = cur_col + dy[i] * 2;
			mid_row = cur_row + dx[i];
			next_row = cur_row + dx[i] * 2;

			if (next_col >= 1 && next_col <= HEIGHT - 2 && next_row >= 1 && next_row <= WIDTH - 2) {
				if (visited[cur_col][cur_row] > visited[next_col][next_row] && visited[mid_col][mid_row] > 0 && visited[next_col][next_row] > 0) {
					if (dx[i] != 0) {
						ofDrawLine(next_row * area, next_col * area + 5, cur_row * area + 10, cur_col * area + 5);
					}
					else {
						ofDrawLine(next_row * area + 5, next_col * area, cur_row * area + 5, cur_col * area + 10);
					}
					cur_col = next_col;
					cur_row = next_row;
					break;
				}
			}
		}
	}
}

