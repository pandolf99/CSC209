#include <stdio.h>
#include <stdlib.h>

/* Reads the image from the open file fp into the two-dimensional array arr
 * num_rows and num_cols specify the dimensions of arr
 */

int get_val_at_i(int i, char *str) {
  if (i == 0) {
    return strtol(str, NULL, 10);
  }
  else {
    char *other = NULL;
    strtol(str, &other, 10);
    return get_val_at_i(i-1, other);
  }
}

void read_image(int num_rows, int num_cols,
                int arr[num_rows][num_cols], FILE *fp) {

                  for (int i = 0; i < num_rows; i++) {
                    // Max size of line is if all numbers are of 3 digits
                    char *line = malloc(sizeof(char)*(2*num_cols*3));
                    fscanf(fp, "%[^\n]\n", line);
                    for (int i2 = 0; i2 < num_cols; i2++) {
                      arr[i][i2] = get_val_at_i(i2, line);
                    }
                    free(line);
                  }
}

/* Print to standard output the contents of the array arr */
void print_image(int num_rows, int num_cols, int arr[num_rows][num_cols]) {
  for (int i = 0; i < num_rows; i++) {
    for (int i2 = 0; i2 < num_cols; i2++) {
      printf(" %d ", arr[i][i2]);
    }
    printf("\n");
  }
}

/* TODO: Write the count_cells function */
