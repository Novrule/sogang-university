class Str {
private:
    char* str; //string에 들어갈 내용
    int len; //string에 들어간 내용의 길이
public:
    Str(int leng); //string의 크기를 leng으로 지정함
    Str(const char* neyong); //string의 내용을 neyong으로 지정함
    ~Str(); //소멸자
    int length(void); //string에 들어간 내용의 길이를 반환해주는 함수
    char* contents(void); //string에 들어간 내용을 반환해주는 함수
    int compare(class Str& a); //a에 있는 내용과 strcmp해주는 함수
    int compare(const char* a); //a에 있는 내용과 strcmp해주는 함수
    void operator=(const char* a); //string의 내용을 저장해주는 함수
    void operator=(class Str& a); //Str의 내용을 저장해주는 함수
};