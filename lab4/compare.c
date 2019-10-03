#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
  Write the main() function of a program that takes exactly two arguments,
  and prints one of the following to standard output:
    - "Same\n" if the arguments are the same.
    - "Different\n" if the arguments are different.
    - "Invalid\n" if the program is called with an incorrect number of
      arguments.
	  NOTE: Cut and paste these strings into your printf statements to
	  ensure that they will be exactly the strings above.

  Your main function should return 0, regardless of what is printed.
*/

int main(int argc, char **argv) {
  if (argc != 3) {
    printf("Invalid\n");
    return 0;
  }
  char *s1 = argv[1];
  char *s2 = argv[2];
  int length1 = strlen(s1);
  int length2 = strlen(s2);
  int valid_length = (length1 == length2);
  if (!valid_length) {
    printf("Different\n");
    return 0;
  }
  for (int i = 0; i < length1; i++) {
    int char_m = (s1[i] == s2[i]);
    if (!char_m) {
      printf("Different\n");
      return 0;
    }
  }
  printf("Same\n");
  return 0;
}
