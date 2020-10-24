/**
 * wc.c
 * Emulates the wc utility.
 *
 * Suggestions for improvement:
 * - Smarter number printing (variable size depending on number of digits)
 * - Long options (e.g. --chars)
 * - Read from stdin if no input
 */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef BUF_SIZE // override with cc -D 
#define BUF_SIZE 1024
#endif

#define BFLAG 1 << 0
#define WFLAG 1 << 1
#define LFLAG 1 << 2

enum state { IN, OUT }; // defines if in or outside word
void parseopts(int argc, char *argv[], int *flags);
void printcnts(int bcnt, int wcnt, int lcnt, int flags, char *filename);

int main(int argc, char *argv[]) {
  int flags = 0;

  // note that this changes global optind variable
  parseopts(argc, argv, &flags);

  int argind = optind;

  if (argind == argc) {
    printf("No file(s) specified\n");
    exit(1);
  }

  int multifile = argc - argind > 1;
  int bcnt, wcnt, lcnt;
  int btot = 0, wtot = 0, ltot = 0;
  int fd;
  char c;
  char *filename;
  enum state state = OUT;
  for (; argind < argc; argind++) {
    bcnt = wcnt = lcnt= 0;
    filename = argv[argind];
    fd = open(filename, O_RDONLY);

    while (read(fd, &c, 1) != 0) {
      bcnt++;
      if (c == '\n') lcnt++;
      if (c == '\n' || c == ' ' || c == '\t') state = OUT;
      else if (state == OUT) {
        state = IN;
        wcnt++;
      }
    }

    printcnts(bcnt, wcnt, lcnt, flags, filename);

    btot += bcnt;
    wtot += wcnt;
    ltot += lcnt;
    
    close(fd);
  }

  if (multifile) 
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
    case '?': exit(1);
    default:
      printf("An error occurred parsing options");
      exit(1);
    }
  }

  // turn on all flags if none specified 
  if (!*flags) *flags = BFLAG | WFLAG | LFLAG;
}

void printcnts(int bcnt, int wcnt, int lcnt, int flags, char *filename) {
  if (flags & LFLAG) printf("%5d", lcnt);
  if (flags & WFLAG) printf("%5d", wcnt);
  if (flags & BFLAG) printf("%5d", bcnt);

  if (filename != NULL) printf(" %s", filename);

  printf("\n");
}

