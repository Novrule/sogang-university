/* $begin shellmain */
#include <errno.h>

#include "csapp.h"
#define MAXARGS 128

/* Global varialbes */
int argc; /* args의 개수 */

/* Function prototypes */
void eval(char *cmdline);
int parseline(char *buf, char **argv);
int builtin_command(char **argv);

int main() {
  char cmdline[MAXLINE]; /* Command line */

  while (1) {
    /* Read */
    printf("CSE4100-SP-P2> ");

    char *temp = fgets(cmdline, MAXLINE, stdin);

    if (feof(stdin)) {
      exit(0);
    }

    /* Evaluate */
    eval(cmdline);
  }
}
/* $end shellmain */

/* $begin eval */
/* eval - Evaluate a command line */
void eval(char *cmdline) {
  int bg;                  /* Should the job run in bg or fg? */
  int single_cnt = 0;      /* '의 개수 */
  int double_cnt = 0;      /* "의 개수 */
  int back_cnt = 0;        /* `의 개수 */
  int single_idx[MAXARGS]; /* '의 위치 */
  int double_idx[MAXARGS]; /* "의 위치 */
  int back_idx[MAXARGS];   /* `의 위치 */
  char buf[MAXLINE];       /* Holds modified command line */
  char *argv[MAXARGS];     /* Argument list execve() */
  pid_t pid;               /* Process id */

  // 명령어에 ', ", `이 있는지 확인하고 위치를 저장
  for (int i = 0; i < strlen(cmdline); i++) {
    if (cmdline[i] == '\'') {
      single_idx[single_cnt++] = i;
    } else if (cmdline[i] == '\"') {
      double_idx[double_cnt++] = i;
    } else if (cmdline[i] == '`') {
      back_idx[back_cnt++] = i;
    }
  }

  // 명령어에 ', ", `이 있다면 제거
  if ((single_cnt % 2) == 0 && single_cnt != 0) {
    for (int i = 0; i < single_cnt; i++) {
      cmdline[single_idx[i]] = ' ';
    }
  }

  if ((double_cnt % 2) == 0 && double_cnt != 0) {
    for (int i = 0; i < double_cnt; i++) {
      cmdline[double_idx[i]] = ' ';
    }
  }

  if ((back_cnt % 2) == 0 && back_cnt != 0) {
    for (int i = 0; i < back_cnt; i++) {
      cmdline[back_idx[i]] = ' ';
    }
  }

  strcpy(buf, cmdline);
  bg = parseline(buf, argv);

  // echo면서 `을 사용한 경우에 `안에 있는 명령어를 실행
  if (!strcmp(argv[0], "echo") && (back_cnt % 2) == 0) {
    strcpy(argv[0], argv[1]);

    for (int i = 1; i < argc; i++) {
      argv[i] = NULL;
    }
  }

  if (argv[0] == NULL) { /* Ignore empty lines */
    return;
  }

  if (!builtin_command(argv)) {  // quit -> exit(0), & -> ignore, other -> run
    char path[100] = "/bin/";    // 기본 디렉토리 경로

    strcat(path, argv[0]);  // 기본 디렉토리 경로에 명령어를 붙임

    if (pid = Fork() == 0) {                  // 자식 프로세스인 경우
      if (execve(path, argv, environ) < 0) {  // ex) /bin/ls ls -al &
        printf("%s: Command not found.\n", argv[0]);
        exit(0);
      }
    } else {  // 부모 프로세스인 경우
      /* Parent waits for foreground job to terminate */
      if (!bg) {
        int status;

        Wait(&status);
      } else {  // when there is backgrount process!
        printf("%d %s", pid, cmdline);
      }
    }
  }

  return;
}

/* If first arg is a builtin command, run it and return true */
int builtin_command(char **argv) {
  if (!strcmp(argv[0], "quit") || !strcmp(argv[0], "exit")) { /* quit command */
    exit(0);
  }

  if (!strcmp(argv[0], "&")) { /* Ignore singleton & */
    return 1;
  }

  if (!strcmp(argv[0], "cd")) { /* cd command */
    char *path;                 // 디렉토리의 경로

    if (argc == 1) {          // cd만 입력한 경우
      path = getenv("HOME");  // 디렉토리의 경로를 환경변수에서 가져옴

      if (path == NULL) {  // 환경변수에 HOME이 없는 경우
        path = ".";        // 현재 디렉토리로 설정
      }
    } else {           // cd 뒤에 디렉토리 경로가 있는 경우
      path = argv[1];  // 디렉토리 경로로 설정
    }

    if (chdir(path) != 0) {  // 디렉토리 변경에 실패한 경우
      printf("Invalid path : %s\n", path);  // 에러 메시지 출력
    }

    return 1;
  }

  return 0; /* Not a builtin command */
}
/* $end eval */

/* $begin parseline */
/* parseline - Parse the command line and build the argv array */
int parseline(char *buf, char **argv) {
  char *delim; /* Points to first space delimiter */
  int bg;      /* Background job? */

  buf[strlen(buf) - 1] = ' '; /* Replace trailing '\n' with space */

  while (*buf && (*buf == ' ')) { /* Ignore leading spaces */
    buf++;
  }

  /* Build the argv list */
  argc = 0;
  while ((delim = strchr(buf, ' '))) {
    argv[argc++] = buf;
    *delim = '\0';
    buf = delim + 1;

    while (*buf && (*buf == ' ')) { /* Ignore spaces */
      buf++;
    }
  }

  argv[argc] = NULL;

  if (argc == 0) { /* Ignore blank line */
    return 1;
  }

  /* Should the job run in the background? */
  if ((bg = (*argv[argc - 1] == '&')) != 0) {
    argv[--argc] = NULL;
  }

  return bg;
}
/* $end parseline */
