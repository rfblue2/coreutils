#include <stdio.h>

int main(int argc, char *argv[]) {
  char *expletive = "y";
  if (argc > 1) {
    expletive = argv[1];
  }

  while (1) {
    printf("%s\n", expletive);
  }
  return 0;
}