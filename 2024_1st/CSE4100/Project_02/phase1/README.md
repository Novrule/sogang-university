## Phase 1 : Building and Testing Your Shell

### 구현 내용

#### 명령어 구현

- cd : 특정 디렉터리로 이동
- ls : 디렉터리의 내용물 출력
- mkdir : 디렉터리 생성
- rmdir : 디렉터리 삭제
- touch : 파일 생성
- cat : 파일 내용 출력
- echo : 파일 내용 출력
- exit : 쉘 종료
- quit : 쉘 종료

#### 기타 구현

- ', ", \`가 짝수 번 사용되었다면 (', ", \`로 둘러 쌓였다면) 그 사이에 있는 내용물의 의미를 제거
- echo \`pwd\`와 같은 명령어는 pwd의 결과를 출력하도록 구현
