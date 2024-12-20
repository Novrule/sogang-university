#include "userprog/syscall.h"

#include <stdio.h>
#include <syscall-nr.h>

#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"

static void syscall_handler(struct intr_frame *);

void syscall_init(void) {
  intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall");
}

void check_addr(void *addr, int n) {
  for (int i = 0; i <= n; i++) {
    if (!is_user_vaddr(addr + i * 4)) {
      exit(-1);
    }
  }
}

void halt(void) { shutdown_power_off(); }

void exit(int status) {
  struct thread *cur_thread = thread_current();
  char name[128];

  for (int i = 0; i < strlen(thread_name()) + 1; i++) {
    if (thread_name()[i] == ' ') {
      name[i] = '\0';

      break;
    }

    name[i] = thread_name()[i];
  }

  printf("%s: exit(%d)\n", name, status);
  cur_thread->exit_status = status;
  thread_exit();
}

int exec(const char *file) { return process_execute(file); }

int wait(int pid) { return process_wait(pid); }

int write(int fd, void *buffer, unsigned size) {
  if (fd == 1) {
    putbuf(buffer, size);

    return size;
  }

  return -1;
}

int read(int fd, void *buffer, unsigned size) {
  if (fd == 0) {
    for (int i = 0; i < size; i++) {
      ((char *)buffer)[i] = input_getc();
    }

    return size;
  }

  return -1;
}

int fibonacci(int n) {
  if (n == 0) {
    return 0;
  } else if (n == 1) {
    return 1;
  }

  return fibonacci(n - 1) + fibonacci(n - 2);
}

int max_of_four_int(int a, int b, int c, int d) {
  int max = a;

  if (b > max) {
    max = b;
  }

  if (c > max) {
    max = c;
  }

  if (d > max) {
    max = d;
  }

  return max;
}

static void syscall_handler(struct intr_frame *f UNUSED) {
  uint32_t *esp = (uint32_t *)f->esp;

  if (*esp == SYS_HALT) {
    check_addr(esp, 0);
    halt();
  } else if (*esp == SYS_EXIT) {
    check_addr(esp, 1);
    if (esp != -1) {
      exit((int)esp[1]);
    } else {
      exit(-1);
    }
  } else if (*esp == SYS_EXEC) {
    check_addr(esp, 1);
    f->eax = exec((char *)esp[1]);
  } else if (*esp == SYS_WAIT) {
    check_addr(esp, 1);
    f->eax = wait((int)esp[1]);
  } else if (*esp == SYS_WRITE) {
    check_addr(esp, 3);
    f->eax = write((int)esp[1], (void *)esp[2], (unsigned)esp[3]);
  } else if (*esp == SYS_READ) {
    check_addr(esp, 3);
    f->eax = read((int)esp[1], (void *)esp[2], (unsigned)esp[3]);
  } else if (*esp == SYS_FIBO) {
    check_addr(esp, 1);
    f->eax = fibonacci((int)esp[1]);
  } else if (*esp == SYS_MAX) {
    check_addr(esp, 4);
    f->eax =
        max_of_four_int((int)esp[1], (int)esp[2], (int)esp[3], (int)esp[4]);
  }
}
