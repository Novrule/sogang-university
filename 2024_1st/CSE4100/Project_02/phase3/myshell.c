/* $begin shellmain */
#include <errno.h>

#include "csapp.h"
#define MAXARGS 128

typedef struct job_t {  // job 정보를 저장하는 구조체
  int jid;
  char state;  //  background(B), foreground(F), stopped(S)
  char cmdline[100];
  pid_t pid;
} job_t;

/* Global varialbes */
int pipe_flag;       /* 파이프 사용 여부 */
int job_id = 1;      /* job에 부여될 id */
job_t job_list[100]; /* job의 목록 */

/* Function prototypes */

void eval(char *cmdline);
int builtin_command(char **argv, int argc);
int parseline(char *buf, char **argv, int *argc);

void split_pipe(char *argv[], char *cmd1[], char *cmd2[]);
void excute_pipe(char *cmd1[], char *cmd2[]);

void init();
void set_signal();

void child_handler(int sig);
void int_handler(int sig);
void stp_handler(int sig);

void create_job(pid_t pid, char state, char *cmdline);
void delete_job(pid_t pid);
job_t *jid_to_job(int jid);
job_t *pid_to_job(pid_t pid);
pid_t find_foreward_pid();
void excute_jobs();
void change_state(char **argv, char flag);

int main(void) {
  char cmdline[MAXLINE]; /* Command line */

  init();

  while (1) {
    pipe_flag = 0;  // 파이프 사용 여부 초기화

    /* Read */
    printf("CSE4100-SP-P2> ");

    char *temp = fgets(cmdline, MAXLINE, stdin);

    if (feof(stdin)) {
      exit(0);
    }

    /* Evaluate */
    eval(cmdline);
  }

  return 0;
}
/* $end shellmain */

/* $begin eval */
/* eval - Evaluate a command line */
void eval(char *cmdline) {
  int bg;                  /* Should the job run in bg or fg? */
  int len;                 /* 마지막 명령어의 길이 */
  int argc = 0;            /* args의 개수 */
  int single_cnt = 0;      /* '의 개수 */
  int double_cnt = 0;      /* "의 개수 */
  int back_cnt = 0;        /* `의 개수 */
  int single_idx[MAXARGS]; /* '의 위치 */
  int double_idx[MAXARGS]; /* "의 위치 */
  int back_idx[MAXARGS];   /* `의 위치 */
  char buf[MAXLINE];       /* Holds modified command line */
  char *argv[MAXARGS];     /* Argument list execve() */
  pid_t pid;               /* Process id */
  sigset_t mask;           /* 시그널 집합 */

  // 명령어에 ', ", `이 있는지 확인하고 위치를 저장
  for (int i = 0; i < strlen(cmdline); i++) {
    if (cmdline[i] == '|') {
      pipe_flag = 1;
    } else if (cmdline[i] == '\'') {
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
    pipe_flag = 1;  // 파이프를 사용하는 것이므로 flag를 1(=true)로 설정

    for (int i = 0; i < back_cnt; i += 2) {
      cmdline[back_idx[i]] = '|';  // 첫 번째 `를 |로 변경

      for (int j = back_idx[i + 1] - 1; j > back_idx[i];
           j--) {  // ` 사이 명령어를 한 칸씩 뒤로 이동
        cmdline[j + 1] = cmdline[j];
      }

      cmdline[back_idx[i] + 1] = ' ';  // | 뒤에 공백 추가
    }
  }

  strcpy(buf, cmdline);
  bg = parseline(buf, argv, &argc);
  len = strlen(argv[argc - 1]);

  if (argv[argc - 1][len - 1] == '&') {  // 마지막 명령어의 끝이 &인 경우
    bg = 1;                              // 백그라운드로 실행
    argv[argc - 1][len - 1] = '\0';
  }

  if (argv[0] == NULL) { /* Ignore empty lines */
    return;
  }

  if (!builtin_command(argv,
                       argc)) {  // quit -> exit(0), & -> ignore, other -> run
    char path[100] = "/bin/";    // 기본 디렉토리 경로

    strcat(path, argv[0]);  // 기본 디렉토리 경로에 명령어를 붙임

    set_signal(mask);  // 시그널 설정

    if (pid = Fork() == 0) {  // 자식 프로세스인 경우
      if (sigprocmask(SIG_UNBLOCK, &mask, NULL) == -1) {
        unix_error("sigprocmask error");
      }

      if (bg) {  // 백그라운드로 실행하는 경우
        if (setpgid(0, 0) < 0) {
          unix_error("setpgid error");
        }
      }

      if (pipe_flag == 1) {  // 파이프를 사용하는 경우
        char *cmd1[50] = {
            0,
        };
        char *cmd2[50] = {
            0,
        };

        split_pipe(argv, cmd1, cmd2);  // 파이프를 기준으로 명령어를 나눔
        excute_pipe(cmd1, cmd2);  // 나뉘어진 명령어 실행
      } else if (execve(path, argv, environ) < 0) {  // ex) /bin/ls ls -al &
        printf("%s: Command not found.\n", argv[0]);
        exit(0);
      }
    } else {  // 부모 프로세스인 경우
      if (!bg) {
        create_job(pid, 'F', cmdline);  // job 생성
      } else {
        create_job(pid, 'B', cmdline);  // job 생성
      }

      if (sigprocmask(SIG_UNBLOCK, &mask, NULL) == -1) {
        unix_error("sigprocmask error");
      }

      /* Parent waits for foreground job to terminate */
      if (!bg) {
        int status;

        if (waitpid(pid, &status, WUNTRACED) <
            0) {  // 자식 프로세스의 상태 변화를 기다림
          unix_error("Waitpid error");
        }

        if (WIFEXITED(status)) {  // 정상적으로 종료된 경우
          delete_job(pid);        // job 목록에서 삭제
        }
      } else {  // when there is backgrount process!
        if (pid > 0) {
          int jid;

          for (int i = 0; i < 100; i++) {
            if (job_list[i].pid == pid) {
              jid = job_list[i].jid;
            }
          }

          printf("[%d] %d %s", jid, pid, cmdline);
        }
      }
    }
  }

  return;
}

/* If first arg is a builtin command, run it and return true */
int builtin_command(char **argv, int argc) {
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

  if (!strcmp(argv[0], "jobs")) { /* jobs command */
    excute_jobs();                // job 목록 출력

    return 1;
  }

  if (!strcmp(argv[0], "bg")) { /* bg command */
    change_state(argv, 'B');    // job의 상태를 background로 변경

    return 1;
  }

  if (!strcmp(argv[0], "fg")) { /* fg command */
    change_state(argv, 'F');    // job의 상태를 foreground로 변경

    return 1;
  }

  if (!strcmp(argv[0], "kill")) { /* kill command */
    job_t *cur;

    if (argv[1][0] == '%') {  // %로 시작하는 경우 (job id를 받은 경우)
      int jid = atoi(&argv[1][1]);

      if ((cur = jid_to_job(jid)) != NULL) {  // job 목록에 있는 경우
        kill(cur->pid, SIGKILL);              // 해당 pid에 시그널 전송
      }
    } else if (isdigit(argv[1][0])) {  // 숫자로 시작하는 경우 (pid를 받은 경우)
      int pid = atoi(argv[1]);

      if ((cur = pid_to_job(pid)) != NULL) {  // job 목록에 있는 경우
        kill(cur->pid, SIGKILL);              // 해당 pid에 시그널 전송
      }
    }

    return 1;
  }

  return 0; /* Not a builtin command */
}
/* $end eval */

/* $begin parseline */
/* parseline - Parse the command line and build the argv array */
int parseline(char *buf, char **argv, int *argc) {
  char *delim; /* Points to first space delimiter */
  int bg;      /* Background job? */

  buf[strlen(buf) - 1] = ' '; /* Replace trailing '\n' with space */

  while (*buf && (*buf == ' ')) { /* Ignore leading spaces */
    buf++;
  }

  /* Build the argv list */
  *argc = 0;
  while ((delim = strchr(buf, ' '))) {
    argv[(*argc)++] = buf;
    *delim = '\0';
    buf = delim + 1;

    while (*buf && (*buf == ' ')) { /* Ignore spaces */
      buf++;
    }
  }

  argv[*argc] = NULL;

  if (*argc == 0) { /* Ignore blank line */
    return 1;
  }

  /* Should the job run in the background? */
  if ((bg = (*argv[*argc - 1] == '&')) != 0) {
    argv[--(*argc)] = NULL;
  }

  return bg;
}
/* $end parseline */

void split_pipe(char *argv[], char *cmd1[],
                char *cmd2[]) {  // 파이프를 기준으로 명령어를 나눔

  int idx;

  for (int i = 0; argv[i] != NULL; i++) {  // 파이프의 앞 부분을 cmd1에 저장
    if (!strcmp(argv[i], "|")) {           // 파이프를 만난 경우
      idx = i + 1;                         // 파이프의 위치를 저장

      break;
    }

    cmd1[i] = argv[i];
    cmd1[i + 1] = NULL;
  }

  for (int i = 0; argv[i] != NULL; i++) {  // 파이프의 뒷 부분을 cmd2에 저장
    cmd2[i] = argv[idx + i];
    cmd2[i + 1] = NULL;
  }

  return;

  /*
  int i = 0;
  int j = 0;

  while (strcmp(argv[i], "|") &&
         argv[i] != NULL) {  // 파이프를 기준으로 앞부분을 cmd1에 저장
    cmd1[i] = argv[i];
    i++;
  }

  cmd1[i++] = NULL;

  while (argv[i] != NULL) {  // 파이프를 기준으로 뒷부분을 cmd2에 저장
    cmd2[j] = argv[i];
    i++;
    j++;
  }

  cmd2[j] = NULL;

  return;
  */
}

void excute_pipe(char *cmd1[],
                 char *cmd2[]) {  // 파이프를 기준으로 나뉘어진 명령어 실행
  int fd[2];
  sigset_t mask;

  if (pipe(fd) == -1) {  // 파이프 생성에 실패한 경우
    printf("Create pipe error\n");
    exit(1);
  }

  set_signal(mask);  // 시그널 설정

  if (Fork() == 0) {  // 자식 프로세스인 경우
    if (sigprocmask(SIG_UNBLOCK, &mask, NULL) == -1) {
      unix_error("sigprocmask error");
    }

    if (setpgid(0, 0) < 0) {
      unix_error("setpgid error");
    }

    dup2(fd[1], STDOUT_FILENO);  // 표준 출력 변경
    close(fd[0]);                // 파이프의 쓰기 닫기

    if (execvp(cmd1[0], cmd1) == -1) {  // 첫 번째 명령어 실행에 실패한 경우
      printf("First execvp error\n");
      exit(1);
    }
  } else {  // 부모 프로세스인 경우
    char *temp1[50] = {
        0,
    };
    char *temp2[50] = {
        0,
    };

    if (sigprocmask(SIG_UNBLOCK, &mask, NULL) == -1) {
      unix_error("sigprocmask error");
    }

    dup2(fd[0], STDIN_FILENO);  // 표준 입력 변경
    close(fd[1]);               // 파이프의 읽기 닫기

    split_pipe(cmd2, temp1, temp2);  // 두 번째 명령어를 다시 나눔

    if (temp2[0] == NULL) {  // 두 번째 명령어가 없는 경우
      if (execvp(cmd2[0], cmd2) == -1) {  // 두 번째 명령어 실행에 실패한 경우
        printf("Second execvp error\n");
        exit(1);
      }
    } else {                      // 두 번째 명령어가 있는 경우
      excute_pipe(temp1, temp2);  // 재귀적으로 파이프 실행
    }
  }

  return;
}

void init() {                      // 실행할 때 필요한 초기화 작업
  Signal(SIGCHLD, child_handler);  // 자식 프로세스가 종료되면 핸들러로 처리
  Signal(SIGINT, int_handler);   // Ctrl + C가 입력되면 핸들러로 처리
  Signal(SIGTSTP, stp_handler);  // Ctrl + Z가 입력되면 핸들러로 처리

  for (int i = 0; i < 100; i++) {  // job 목록 초기화
    job_list[i].jid = 0;
    job_list[i].state = 0;
    job_list[i].cmdline[0] = '\n';
    job_list[i].pid = 0;
  }
}

void set_signal(sigset_t mask) {  // 시그널 설정
  if (sigemptyset(&mask) == -1) {
    unix_error("sigemptyset error");
  }

  if (sigaddset(&mask, SIGCHLD) == -1) {
    unix_error("sigaddset error");
  }

  if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
    unix_error("sigprocmask error");
  }
}

void child_handler(int sig) {  // 자식 프로세스의 종료를 처리
  int status;
  pid_t pid;

  while (1) {  // 자식 프로세스의 종료를 기다림
    if ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) >
        0) {  // 자식 프로세스의 상태 변화가 있는 경우
      job_t *cur = pid_to_job(pid);

      if (WIFSTOPPED(status)) {  // 자식 프로세스가 중지된 경우
        cur->state = 'S';        // 상태를 S로 변경
      } else if (WIFEXITED(status) ||
                 WIFSIGNALED(status)) {  // 자식 프로세스가 종료된 경우
        delete_job(pid);                 // job 목록에서 삭제
      }
    } else {  // 자식 프로세스의 상태 변화가 없는 경우
      break;
    }
  }

  return;
}

void int_handler(int sig) {  // Ctrl + C 입력을 처리
  pid_t pid = find_foreward_pid();

  if (pid > 0) {  // 현재 실행 중인 프로세스가 있는 경우
    kill(-pid, SIGINT);  // 해당 프로세스가 속한 그룹에 SIGINT 시그널 전송
  }

  return;
}

void stp_handler(int sig) {  // Ctrl + Z 입력을 처리
  pid_t pid = find_foreward_pid();

  if (pid > 0) {  // 현재 실행 중인 프로세스가 있는 경우
    job_t *cur = pid_to_job(pid);

    if (cur != NULL) {   // job 목록에 있는 경우
      cur->state = 'S';  // 상태를 S로 변경
    }

    kill(-pid, SIGTSTP);  //  해당 프로세스가 속한 그룹에 SIGTSTP 시그널 전송
  }

  return;
}

void create_job(pid_t pid, char state, char *cmdline) {  // job 생성
  if (pid <= 0) {  // pid가 0 이하인 경우 (오류)
    return;
  }

  for (int i = 0; i < 100; i++) {
    if (job_list[i].jid == 0) {  // 비어있는 자리기 있는 경우
      job_list[i].jid = job_id++;
      job_list[i].state = state;
      strcpy(job_list[i].cmdline, cmdline);
      job_list[i].pid = pid;

      break;
    }
  }

  return;
}

void delete_job(pid_t pid) {  // job 삭제
  if (pid <= 0) {             // pid가 0 이하인 경우 (오류)
    return;
  }

  for (int i = 0; i < 100; i++) {
    if (job_list[i].pid == pid) {  // 해당 pid를 가진 job을 찾은 경우
      job_list[i].jid = 0;
      job_list[i].state = 0;
      job_list[i].cmdline[0] = '\n';
      job_list[i].pid = 0;

      break;
    }
  }

  return;
}

job_t *jid_to_job(int jid) {  // jid를 가진 job을 찾음
  if (jid <= 0) {             // jid가 0 이하인 경우 (오류)
    return NULL;
  }

  for (int i = 0; i < 100; i++) {
    if (job_list[i].jid == jid) {  // jid를 가진 job을 찾은 경우
      return &job_list[i];
    }
  }

  return NULL;
}

job_t *pid_to_job(pid_t pid) {  // pid를 가진 job을 찾음
  if (pid <= 0) {               // pid가 0 이하인 경우 (오류)
    return NULL;
  }

  for (int i = 0; i < 100; i++) {
    if (job_list[i].pid == pid) {  // pid를 가진 job을 찾은 경우
      return &job_list[i];
    }
  }

  return NULL;
}

pid_t find_foreward_pid() {  // 현재 실행 중인 프로세스의 pid를 찾음
  for (int i = 0; i < 100; i++) {
    if (job_list[i].state == 'F') {  // 실행 중인 프로세스를 찾은 경우
      return job_list[i].pid;
    }
  }

  return 0;
}

void excute_jobs() {  // job 목록을 출력
  for (int i = 0; i < 100; i++) {
    if (job_list[i].jid > 0) {         // job 목록에 있는 경우
      if (job_list[i].state == 'B') {  // background인 경우
        printf("[%d] %d background %s", job_list[i].jid, job_list[i].pid,
               job_list[i].cmdline);
      } else if (job_list[i].state == 'F') {  // foreground인 경우
        printf("[%d] %d foreground %s", job_list[i].jid, job_list[i].pid,
               job_list[i].cmdline);
      } else if (job_list[i].state == 'S') {  // stopped인 경우
        printf("[%d] %d stopped %s", job_list[i].jid, job_list[i].pid,
               job_list[i].cmdline);
      } else {  // 그 외의 경우 (오류)
        printf("[%d] %d's state error\n", job_list[i].jid, job_list[i].pid);
      }
    }
  }

  return;
}

void change_state(char **argv, char flag) {  // job의 상태를 변경
  char *cmdline = argv[1];
  job_t *cur;

  if (cmdline != NULL) {      // 명령어가 있는 경우
    if (cmdline[0] == '%') {  // %로 시작하는 경우 (job id를 받은 경우)
      int jid = atoi(&cmdline[1]);

      if ((cur = jid_to_job(jid)) == NULL) {  // job 목록에 없는 경우
        return;
      }
    } else if (isdigit(cmdline[0])) {  // 숫자로 시작하는 경우 (pid를 받은 경우)
      int pid = atoi(cmdline);

      if ((cur = pid_to_job(pid)) == NULL) {  // job 목록에 없는 경우
        return;
      }
    } else {  // 그 외의 경우 (오류)
      return;
    }
  }

  if (flag == 'B') {   // background로 변경하는 경우
    cur->state = 'B';  // 상태를 B로 변경
    Kill(-(cur->pid),
         SIGCONT);  // 해당 프로세스가 속한 그룹에 SIGCONT 시그널 전송
  } else if (flag == 'F') {  // foreground로 변경하는 경우
    cur->state = 'F';        // 상태를 F로 변경
    Kill(-(cur->pid),
         SIGCONT);  // 해당 프로세스가 속한 그룹에 SIGCONT 시그널 전송
  }

  return;
}
