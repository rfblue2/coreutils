/**
 * wc.c
 * Emulates the wc utility.
 *
 * Suggestions for improvement:
 * - Smarter number printing (variable size depending on number of digits)
 * - Long options (e.g. --chars)
 */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BFLAG 1 << 0
#define WFLAG 1 << 1
#define LFLAG 1 << 2

enum state { IN, OUT }; // defines if in or outside word
void parseopts(int argc, char *argv[], int *flags);
void count(int fd, int *bcnt, int *wcnt, int *lcnt);
void printcnts(int bcnt, int wcnt, int lcnt, int flags, char *filename);

int main(int argc, char *argv[]) {
  int flags = 0;
  int nfiles = argc - optind;
  int fd, i, bcnt, wcnt, lcnt;
  int btot = 0, wtot = 0, ltot = 0;
  char *filename;

  // note that this changes global optind variable
  parseopts(argc, argv, &flags);

  // no file specified - read from stdin
  if (nfiles == 0) {
    fd = STDIN_FILENO;
    count(fd, &bcnt, &wcnt, &lcnt);
    printcnts(bcnt, wcnt, lcnt, flags, NULL);
  }

  // at least one file
  for (i = optind; i < argc; i++) {
    bcnt = wcnt = lcnt= 0;
    filename = argv[i];
    fd = open(filename, O_RDONLY);
    if (fd == -1) {
      perror(filename);
      exit(EXIT_FAILURE);
    }

    count(fd, &bcnt, &wcnt, &lcnt);
    printcnts(bcnt, wcnt, lcnt, flags, filename);

    btot += bcnt;
    wtot += wcnt;
    ltot += lcnt;
    
    close(fd);
  }

  if (nfiles > 1) 
    printcnts(btot, wtot, ltot, flags, "total");

  return 0;
}

void parseopts(int argc, char *argv[], int *flags) {
  int opt;
  while ((opt = getopt(argc, argv, "mcwl")) != -1) {
    switch (opt) {
    case 'm':
    case 'c': *flags |= BFLAG; break;
    case 'w': *flags |= WFLAG ; break;
    case 'l': *flags |= LFLAG; break;
    case '?': exit(EXIT_FAILURE);
    default:
      fprintf(stderr, "An error occurred parsing options");
      exit(EXIT_FAILURE);
    }
  }

  // turn on all flags if none specified 
  if (!*flags) *flags = BFLAG | WFLAG | LFLAG;
}

void count(int fd, int *bcnt, int *wcnt, int *lcnt) {
  char c;
  enum state state = OUT;
  while (read(fd, &c, 1) != 0) {
    (*bcnt)++;
    if (c == '\n') (*lcnt)++;
    if (c == '\n' || c == ' ' || c == '\t') state = OUT;
    else if (state == OUT) {
      state = IN;
      (*wcnt)++;
    }
  }
}

void printcnts(int bcnt, int wcnt, int lcnt, int flags, char *filename) {
  if (flags & LFLAG) printf("%5d", lcnt);
  if (flags & WFLAG) printf("%5d", wcnt);
  if (flags & BFLAG) printf("%5d", bcnt);

  if (filename != NULL) printf(" %s", filename);

  printf("\n");
}

