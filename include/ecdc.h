#include <math.h>
#include <stdlib.h>
#include <string.h>

#define clm(x) (memset(x, 0, sizeof(x)))
#define clmi(x) (memset(x, 0, strlen(x)))

#define mask(a) 1 << (a)

typedef struct {
  unsigned char *output;
  int numByte;
} result;
result encoder(unsigned char *, int);

result decoder(unsigned char *out);

