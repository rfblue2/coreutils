#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 101

int main(int argc, char *argv[]) {
  uid_t id = geteuid();
  FILE *fptr = fopen("/etc/passwd", "r");

  if (fptr == NULL) {
    printf("passwd file not found");
    exit(1);
  }

  char buffer[BUF_SIZE];
  char *nameptr;
  int iid;

  while (fgets(buffer, BUF_SIZE, fptr) != NULL) {
    if (buffer[0] == '#') continue; // lines starting with comment
    nameptr = strtok(buffer, ":");
    strtok(NULL, ":");
    iid = atoi(strtok(NULL, ":"));
    if (iid == id) {
      printf("%s\n", nameptr);
      exit(0);
    }
  }

  if (feof(fptr)) {
    printf("User with id %u not found\n", id);
    fclose(fptr);
    exit(1);
  } else {
    printf("Error reading /etc/passwd\n");
    fclose(fptr);
    exit(1);
  }
}