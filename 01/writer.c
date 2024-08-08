#include <stdio.h>
int main() {
  char buf[1024];
  for (int i = 0; i < 1024; i++)
    buf[i] = 0;
  while (1) {
    FILE *f = fopen("3000", "ab");
    printf("TERM : ");
    *((short int *)buf) = 3000;
    scanf("%s", buf + 2);
    printf("%d", *((short int *)buf));
    printf("%s\n", buf);
    fwrite(buf, 1024, 1, f);
    fclose(f);
  }
  return 0;
}
