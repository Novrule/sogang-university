#include <iostream>
#include <string.h>
#include "Str.h"

using namespace std;

Str::Str(int length) { //string의 길이를 입력받아 그 길이의 문자열을 생성해 줌
    len = length; //전달받은 문자열의 길이 저장

    str = new char[len + 1]; //전달받은 문자열의 길이보다 하나 더 긴 문자열 생성 (문자열의 마지막에 \0을 넣어줘야 하므로)

    str[0] = '\0'; //내용을 입력받지는 않았으니 우선 문자열에 NULL을 저장함
}

Str::Str(const char* contents) { //string에 내용을 넣어줌
    len = strlen(contents); //전달받은 문자열의 길이 저장

    str = new char[len + 1]; //전달받은 문자열의 길이보다 하나 더 긴 새로운 문자열 생성 (문자열의 마지막에 \0을 넣어줘야 하므로)

    strcpy(str, contents); //생성한 문자열에 전달받은 문자열을 복사해서 넣어줌

    str[len] = '\0'; //문자열의 마지막에 NULL을 저장함
}

Str::~Str() { //소멸자
    delete[]str; //str에 있는 문자열을 소멸시켜 줌
}

int Str::length() { //string에 들어간 문자열의 길이 반환
    return len; //앞서 저장한 string의 길이를 반환해 줌
}

char* Str::contents() { //string에 들어간 문자열의 내용 반환
    return str; //앞서 저장한 string을 반환해 줌
}

int Str::compare(const char* a) { //strcmp 함수를 이용해 문자열 비교
    return strcmp(str, a); //str에 있는 문자열과 주어진 문자열을 strcmp로 비교한 후 결과를 반환
}

int Str::compare(Str& a) { //strcmp 함수를 이용해 문자열 비교
    return strcmp(str, a.contents()); //str에 있는 문자열과 주어진 Str안에 있는 문자열을 strcmp로 비교한 후 결과를 반환
}

void Str::operator=(const char* a) { //주어진 문자열의 내용을 저장
    delete[] str; //str에 있는 문자열을 소멸시켜줌

    len = strlen(a); //전달받은 문자열의 길이 저장

    if (len > 0) { //전달받은 문자열의 길이가 0보다 길다면
        str = new char[len + 1]; //전달받은 문자열의 길이보다 하나 더 긴 새로운 문자열 생성 (문자열의 마지막에 \0을 넣어줘야 하므로)

        strcpy(str, a); //생성한 문자열에 전달받은 문자열을 복사해서 넣어줌

        str[len] = '\0'; //문자열의 마지막에 NULL을 저장함
    }
    else { //전달받은 문자열의 길이가 0 이하라면
        cout << "Error" << endl; //무언가 문제가 있으므로 오류 메시지를 출력
    }
}

void Str::operator=(Str& a) { //주어진 Str에 있는 문자열의 내용을 저장
    delete[] str; //str에 있는 문자열을 소멸시켜줌

    len = a.length(); //전달받은 Str에 있는 문자열의 길이 저장

    if (len > 0) { //전달받은 Str에 있는 문자열의 길이가 0보다 길다면
        str = new char[len + 1]; //전달받은 Str에 있는 문자열의 길이보다 하나 더 긴 새로운 문자열 생성 (문자열의 마지막에 \0을 넣어줘야 하므로)

        strcpy(str, a.contents()); //생성한 문자열에 전달받은 Str에 있는 문자열을 복사해서 넣어줌

        str[len] = '\0'; //문자열의 마지막에 NULL을 저장함
    }
    else { //전달받은 Str에 있는 문자열의 길이가 0 이하라면
        cout << "Error" << endl; //무언가 문제가 있으므로 오류 메시지를 출력
    }
}
