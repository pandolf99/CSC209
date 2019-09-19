#include <stdio.h>
#include <stdlib.h>

int main(void) {
  char phone[11];
  int d;
  scanf("%s %d",phone, &d);
  if (d <= 9 && d >= 0) {
    char cd;
    cd = phone[d];
    printf("%c\n", cd);
  }
  else if (d == -1) printf("%s\n", phone);

  else {
    printf("Error\n");
    return 1;
  }

  return 0;
}
