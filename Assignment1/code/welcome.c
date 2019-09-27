#include <stdio.h>
#include <unistd.h>

void printDragon() {
  FILE *f;
  char c;
  f = fopen("dragon_welcome", "r");

  if (f == NULL) {
    printf("Welcome to Dragon Shell");
    return;
  }

  while ((c = fgetc(f)) != EOF) {
    printf("%c", c);
  }

  fclose(f);
}