#include <stdio.h>
#include <stdlib.h>


int main(void) {
  char phone[11];
  int d;
  int error = 0;
  scanf("%s",phone);
  while(scanf("%d", &d) != EOF) {
    if (d <= 9 && d >= 0) {
      char cd;
      cd = phone[d];
      printf("%c\n", cd);
    }
    else if (d == -1) printf("%s\n", phone);

    else {
      printf("Error\n");
      error = 1;
    }
  }
  return error;
}
