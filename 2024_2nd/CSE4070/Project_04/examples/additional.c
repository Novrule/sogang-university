#include <stdio.h>
#include <stdlib.h>
#include <syscall.h>

int main(int argc, char *argv[]) {
  if (argc != 5) {
    printf("Usage: ./additional [num 1] [num 2] [num 3] [num 4]\n");

    return EXIT_FAILURE;
  } else {
    int atoi_argv[4];

    for (int i = 0; i < 4; i++) {
      atoi_argv[i] = atoi(argv[i + 1]);
    }

    printf("%d %d\n", fibonnaci(atoi_argv[0]),
           max_of_four_int(atoi_argv[0], atoi_argv[1], atoi_argv[2],
                           atoi_argv[3]));

    return EXIT_SUCCESS;
  }
}