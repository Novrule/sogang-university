#include "userprog/syscall.h"

#include <stdio.h>
#include <syscall-nr.h>

#include "filesys/file.h"
#include "threads/interrupt.h"
#include "threads/synch.h"
#include "threads/thread.h"
#include "threads/vaddr.h"

static void syscall_handler(struct intr_frame *);

struct lock file_lock;

void syscall_init(void) {
  intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall");
  lock_init(&file_lock);
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

  for (int i = 2; i < 128; i++) {
    if (cur_thread->fd[i] != NULL) {
      close(i);
    }
  }

  thread_exit();
}

int exec(const char *file) { 
  if (file == NULL) {
    exit(-1);
  }

  return process_execute(file); 
}

int wait(int pid) { return process_wait(pid); }

int write(int fd, void *buffer, unsigned size) {
  if (fd < 0 || fd > 128 || buffer == NULL) {
    exit(-1);
  }
  
  check_addr(buffer, 0);

  lock_acquire(&file_lock);

  if (fd == 1) {
    putbuf(buffer, size);
    lock_release(&file_lock);

    return size;
  } else if (fd > 1) {
    struct thread *cur_thread = thread_current();

    if (cur_thread->fd[fd] == NULL) {
      lock_release(&file_lock);

      exit(-1);
    } else {
      if (cur_thread->fd[fd]->deny_write != NULL) {
        file_deny_write(cur_thread->fd[fd]);
      }

      int res = file_write(cur_thread->fd[fd], buffer, size);

      lock_release(&file_lock);

      return res;
    }
  } else {
    lock_release(&file_lock);

    return -1;
  }
}

int read(int fd, void *buffer, unsigned size) {
  if (fd < 0 || fd > 128 || buffer == NULL) {
    exit(-1);
  }
	
  check_addr(buffer, 0);

  lock_acquire(&file_lock);

  if (fd == 0) {
    for (int i = 0; i < size; i++) {
      ((char *)buffer)[i] = input_getc();

      if (((char *)buffer)[i] == '\0') {
        break;
      }
    }

    lock_release(&file_lock);

    return size;
  } else if (fd > 1) {
    if (thread_current()->fd[fd] == NULL) {
      lock_release(&file_lock);

      exit(-1);
    } else {
      int res = file_read(thread_current()->fd[fd], buffer, size);

      lock_release(&file_lock);

      return res;
    }
  } else {
    lock_release(&file_lock);

    return -1;
  }
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

bool create(const char *file, unsigned size) {
  if (file == NULL) {
    exit(-1);
  }

  return filesys_create(file, size);
}

bool remove(const char *file) {
  if (file == NULL) {
    exit(-1);
  }

  return filesys_remove(file);
}

int open(const char *file) {
  int ans = -1;
  struct file *fp = NULL;

  if (file == NULL) {
    exit(-1);
  }

  lock_acquire(&file_lock);

  fp = filesys_open(file);

  if (fp == NULL) {
    lock_release(&file_lock);

    return ans;
  } else {
    int i = 2;

    while (i < 128) {
      if (thread_current()->fd[i] == NULL) {
        if (!strcmp(thread_current()->name, file)) {
          file_deny_write(fp);
	}

        thread_current()->fd[i] = fp;
        ans = i;

        break;
      }

      i++;
    }

    lock_release(&file_lock);

    return ans;
  }
}

void close(int fd) {
  if (fd < 0 || fd > 128) {
    exit(-1);
  }

  if (thread_current()->fd[fd] == NULL) {
    exit(-1);
  }

  file_close(thread_current()->fd[fd]);
  thread_current()->fd[fd] = NULL;
}

int filesize(int fd) {
  if (fd < 0 || fd > 128) {
    exit(-1);
  }

  if (thread_current()->fd[fd] == NULL) {
    exit(-1);
  }

  return file_length(thread_current()->fd[fd]);
}

void seek(int fd, unsigned pos) {
  if (fd < 0 || fd > 128) {
    exit(-1);
  }

  if (thread_current()->fd[fd] == NULL) {
    exit(-1);
  }

  file_seek(thread_current()->fd[fd], pos);
}

unsigned tell(int fd) {
  if (fd < 0 || fd > 128) {
    exit(-1);
  }

  if (thread_current()->fd[fd] == NULL) {
    exit(-1);
  }

  return file_tell(thread_current()->fd[fd]);
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
  } else if (*esp == SYS_CREATE) {
    check_addr(esp, 2);
    f->eax = create((char *)esp[1], (unsigned)esp[2]);
  } else if (*esp == SYS_REMOVE) {
    check_addr(esp, 1);
    f->eax = remove((char *)esp[1]);
  } else if (*esp == SYS_OPEN) {
    check_addr(esp, 1);
    f->eax = open((char *)esp[1]);
  } else if (*esp == SYS_CLOSE) {
    check_addr(esp, 1);
    close((int)esp[1]);
  } else if (*esp == SYS_FILESIZE) {
    check_addr(esp, 1);
    f->eax = filesize((int)esp[1]);
  } else if (*esp == SYS_SEEK) {
    check_addr(esp, 2);
    seek((int)esp[1], (unsigned)esp[2]);
  } else if (*esp == SYS_TELL) {
    check_addr(esp, 1);
    f->eax = tell((int)esp[1]);
  }
}
