#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    ofSetFrameRate(15);
    ofBackground(255, 255, 255);
    ofSetLineWidth(4);

    draw_flag = 0;
    load_flag = 0;
    diameter = 20.0f;
}

//--------------------------------------------------------------
void ofApp::update() {

}

//--------------------------------------------------------------
void ofApp::draw() {
    ofSetColor(127, 23, 31);
    ofDrawRectangle(0, 0, 1024, 40);
    ofDrawRectangle(0, 728, 1024, 40);
    ofSetLineWidth(5);

    int x, y;

    if (draw_flag != 0) {
        for (int i = 0; i < line_num; i++) {
            ofDrawLine(line_arr[i].from_x, line_arr[i].from_y, line_arr[i].to_x, line_arr[i].to_y);
        }

        ofSetColor(0, 0, 0);
        for (int i = 0; i < dot_num; i++) {
            ofDrawCircle(dot_arr[i].x, dot_arr[i].y, 10);
        }

        ofSetColor(255, 0, 0);
        if (draw_flag == 2) {
            x = dot_arr[index].x;
            y = dot_arr[index].y;
            ofDrawCircle(x, y, 10);
        }
        else {
            ofDrawCircle(dot_arr[0].x, dot_arr[0].y, 10);
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    if (key == 'v') {
        glReadBuffer(GL_FRONT);
        ofSaveScreen("savedScreenshot_" + ofGetTimestampString() + ".png");
    }

    if (key == 'q') {
        draw_flag = 0;

        delete[] line_arr;
        cout << "Memory for line segement has been freed." << endl;

        delete[] dot_arr;
        cout << "Memory for dot has been freed." << endl;

        _Exit(0);
    }

    if (key == 'd') {
        if (!load_flag) {
            return;
        }

        draw_flag = 1;
        draw();
    }

    if (key == 's') {
    }

    if (key == 'e') {
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
    if (key == 'l') {
        ofFileDialogResult openFileResult = ofSystemLoadDialog("Select a only txt for Waterfall");

        if (openFileResult.bSuccess) {
            ofLogVerbose("User selected a file");

            processOpenFileSelection(openFileResult);
            load_flag = 1;
        }
    }

    if (key == OF_KEY_RIGHT) {
        draw_flag = 2;

        if (index == dot_num - 1) {
            index = 0;
        }
        else {
            index++;
        }

        cout << "Selcted Dot Coordinate is (" << dot_arr[index].x << ", " << dot_arr[index].y << ")" << endl;
    }
    if (key == OF_KEY_LEFT) {
        draw_flag = 2;

        if (index == 0) {
            index = dot_num - 1;
        }
        else {
            index--;
        }

        cout << "Selcted Dot Coordinate is (" << dot_arr[index].x << ", " << dot_arr[index].y << ")" << endl;
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

void ofApp::processOpenFileSelection(ofFileDialogResult openFileResult) {
    int x1, y1, x2, y2, min, t1, t2;
    int index = 0;
    int input = 0;
    int cnt = 0;
    string fileName = openFileResult.getName();
    ofFile file(fileName);
    ofBuffer buffer(file);

    if (!file.exists()) {
        cout << "Target file does not exists." << endl;
    }
    else {
        cout << "We found the target file." << endl;
    }

    for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; ++it) {
        string line = *it;

        vector<string> words = ofSplitString(line, " ");

        if (words.size() == 1) {
            if (input == 0) {
                line_num = atoi(words[0].c_str());
                cout << "The number of line is : " << line_num << endl;

                line_arr = new water_line[line_num];
            }
            else {
                dot_num = atoi(words[0].c_str());
                cout << "The number of dot is : " << dot_num << endl;

                dot_arr = new water_dot[dot_num];
                cnt = 0;
            }
        }
        else if (words.size() >= 2) {
            if (input == 0) {
                x1 = atoi(words[0].c_str());
                y1 = atoi(words[1].c_str());
                x2 = atoi(words[2].c_str());
                y2 = atoi(words[3].c_str());

                line_arr[cnt].from_x = x1;
                line_arr[cnt].to_x = x2;
                line_arr[cnt].from_y = y1;
                line_arr[cnt].to_y = y2;

                cnt++;
            }
            else {
                x1 = atoi(words[0].c_str());
                y1 = atoi(words[1].c_str());

                dot_arr[cnt].x = x1;
                dot_arr[cnt].y = y1;

                cnt++;
            }
        }

        if (cnt == line_num) input = 1;
    } 

    for (int i = 0; i < dot_num; i++) {
        min = 100000;

        for (int j = i; j < dot_num; j++) {
            if (min > dot_arr[j].x) {
                min = dot_arr[j].x;
                index = j;
            }
        }

        t1 = dot_arr[i].x;
        dot_arr[i].x = dot_arr[index].x;
        dot_arr[index].x = t1;

        t2 = dot_arr[i].y;
        dot_arr[i].y = dot_arr[index].y;
        dot_arr[index].y = t2;
    }
}

void ofApp::initializeWaterLines() {
    ;
}

