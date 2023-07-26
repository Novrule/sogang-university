#define _CRT_SECURE_NO_WARNINGS
#include <queue>
#include <list>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdio>

typedef unsigned char BYTE;
using namespace std;
const bool ERROR = false;

class tree {
public:
	tree() {}
	tree(unsigned char name1, int freq) { name[0] = name1;  freq = freq;  left_child = list<tree>(); right_child = list<tree>(); }

	unsigned char name[1];
	int freq;
	list<tree> left_child;
	list<tree> right_child;
};

class code {
public:
	unsigned char name[1];
	string huff;
};

struct comp_tree {
	bool operator()(const tree& left_child, const tree& right_child) {
		return left_child.freq > right_child.freq;
	}
};

struct comp_code {
	bool operator()(const code& left_child, const code& right_child) {
		return left_child.huff > right_child.huff;
	}
};

string find_code(vector<code>& v, BYTE buffer[1]) {
	for (unsigned char i = 0; i < v.size(); i++) {
		if (v[i].name[0] == buffer[0]) {
			return v[i].huff;
		}
	}
	return string("NULL");
}

bool frequency(string filename, int* freq) {
	FILE* file = fopen(filename.c_str(), "rt");
	if (file == NULL) {
		return ERROR;
	}

	unsigned char index[1] = { 0 };

	while (fscanf(file, "%c", &index[0]) != EOF) {
		++freq[index[0]];
	}

	fclose(file);
	return true;
}

void make_node(int* freq, priority_queue<tree, vector<tree>, comp_tree>& pq) {
	for (unsigned char i = 0; i < 128; i++)
		if (freq[i] != 0) {
			tree item(i, freq[i]);
			pq.push(item);
		}

	return;
}

void make_tree(priority_queue<tree, vector<tree>, comp_tree>& pq) {
	while (pq.size() >= 2) {
		tree left = pq.top();
		pq.pop();
		tree right = pq.top();
		pq.pop();

		tree parent;
		parent.left_child.push_back(left);
		parent.right_child.push_back(right);
		parent.freq = left.freq + right.freq;

		pq.push(parent);
	}
	return;
}

void inorder(const tree& root, priority_queue<code, vector<code>, comp_code>& huff, string cur_code) {
	if (!root.left_child.empty()) {
		inorder(root.left_child.front(), huff, cur_code + "0");
	}

	if (root.left_child.empty() && root.right_child.empty()) {
		code item;

		item.name[0] = root.name[0];
		item.huff = cur_code;
		huff.push(item);
	}

	if (!root.right_child.empty()) {
		inorder(root.right_child.front(), huff, cur_code + "1");
	}

	return;
}

bool make_code(const priority_queue<tree, vector<tree>, comp_tree>& pq, priority_queue<code, vector<code>, comp_code>& huff) {
	inorder(pq.top(), huff, "");

	return true;
}

void convert_binary(string filename, priority_queue<code, vector<code>, comp_code>& huff) {
	FILE* fp1 = fopen(filename.c_str(), "rt");
	filename.erase(filename.length() - 4, 4);

	string savefilename = filename + ".txt.zz";
	FILE* fp2 = fopen(savefilename.c_str(), "wb");

	int dummy = 0;
	fprintf(fp2, "%c", dummy);
	fprintf(fp2, "%d", (int)huff.size());

	int idx = 0;
	vector<code> v(huff.size());

	while (!huff.empty()) {
		code item = huff.top();
		huff.pop();
		v[idx++] = item;

		fprintf(fp2, "%c", item.name[0]);
		fprintf(fp2, "%c", (char)item.huff.length());

		BYTE buffer = 0;
		int a = -1;

		for (int i = 0; i < item.huff.length(); ++i)
		{
			if (a == 7) {
				fprintf(fp2, "%c", buffer);
				buffer = 0;
				a = -1;
			}

			buffer = buffer << 1;
			buffer = buffer | item.huff[i] - '0';
			++a;
		}

		if (a != -1) {
			while (a != 7) {
				buffer = buffer << 1;
				a++;
			}
			fprintf(fp2, "%c", buffer);
		}
	}

	BYTE word[1];
	BYTE buffer = 0;
	int a = -1;

	while (fscanf(fp1, "%c", &word[0]) != EOF) {
		string write_code = find_code(v, word);

		for (int i = 0; i < write_code.length(); ++i) {
			if (a == 7) {
				fprintf(fp2, "%c", buffer);
				buffer = 0;
				a = -1;
			}
			buffer = buffer << 1;
			buffer = buffer | write_code[i] - '0';
			++a;

		}
	}

	int lastbit = a;
	while (lastbit != 7) {
		buffer = buffer << 1;
		lastbit++;
	}

	fprintf(fp2, "%c", buffer);
	fseek(fp2, 0, SEEK_SET);
	fprintf(fp2, "%c", (char)a);

	fclose(fp1);
	fclose(fp2);

	return;
}

bool huffman_encode(string file, priority_queue<code, vector<code>, comp_code>& huff) {
	int freq[128] = { 0 };
	priority_queue<tree, vector<tree>, comp_tree>  pq;

	if (frequency(file, freq) == ERROR)
		return ERROR;

	make_node(freq, pq);
	make_tree(pq);
	make_code(pq, huff);
	convert_binary(file, huff);

	return true;
}

bool my_sort(const code& left_child, const code& right_child) {
	return left_child.huff < right_child.huff;
}

bool search_code(vector<code>& v, string& s, BYTE word[1]) {
	int left = 0;
	int right = v.size() - 1;

	while (left <= right) {
		int mid = (left + right) / 2;

		if (v[mid].huff < s)
			left = mid + 1;
		else if (s < v[mid].huff)
			right = mid - 1;
		else {
			word[0] = v[mid].name[0];
			return true;
		}
	}
	return false;
}

bool huffman_decode(string filename) {
	vector<code> v;

	FILE* fp1 = fopen(filename.c_str(), "rb");
	if (fp1 == NULL) {
		return false;
	}
	string outfilename = filename;
	outfilename += ".yy";
	FILE* fp2 = fopen(outfilename.c_str(), "wt");
	char a;
	int b;

	fscanf(fp1, "%c", &a);
	fscanf(fp1, "%d", &b);

	for (int i = 0; i < b; i++) {
		code item;
		char a;

		fscanf(fp1, "%c", &item.name[0]); 0;
		fscanf(fp1, "%c", &a);

		BYTE buffer = 0;
		while (a > 0) {
			fscanf(fp1, "%c", &buffer);

			for (int j = 7; j >= 0; --j) {
				if (a <= 0)
					break;
				char data = (buffer >> j) & 1;

				item.huff.push_back(data + '0');
				--a;
			}
		}
		v.push_back(item);
	}
	sort(v.begin(), v.end(), my_sort);

	BYTE buffer, check;
	int cnt = 0;
	string huff;
	while (fscanf(fp1, "%c", &buffer) != EOF) {
		if (fscanf(fp1, "%c", &check) == EOF) {
			for (int i = 7; i >= 7 - a; --i) {
				char bitdata = (buffer >> i) & 1;
				huff.push_back(bitdata + '0');

				BYTE write[1] = { 0 };
				bool found = false;

				found = search_code(v, huff, write);

				if (found) {
					fprintf(fp2, "%c", write[0]);
					huff.clear();
					break;
				}
			}
		}
		else {
			fseek(fp1, -1, SEEK_CUR);
			for (int i = 7; i >= 0; --i) {
				char bitdata = (buffer >> i) & 1;
				huff.push_back(bitdata + '0');

				BYTE write[1] = { 0 };
				bool found = false;

				found = search_code(v, huff, write);

				if (found) {
					fprintf(fp2, "%c", write[0]);
					huff.clear();
				}
			}
		}
	}
	fclose(fp1);
	fclose(fp2);

	return true;
}

int main(int argc, char* argv[]) {
	string index = argv[1];
	string name = argv[2];

	if (index == "-c") {
		priority_queue<code, vector<code>, comp_code> huff;
		if (huffman_encode(name, huff) == false) {
			printf("Error!\n");
			return 0;
		}
	}
	else if (index == "-d") {
		if (huffman_decode(name) == false) {
			printf("Error!\n");
			return 0;
		}
	}
	else {
		printf("Error!\n");
		return 0;
	}
	return 0;
}
