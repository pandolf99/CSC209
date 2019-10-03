#include <stdio.h>
#include <stdlib.h>


//Return the value of the number that starts at index i from str.
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

/* Reads the image from the open file fp into the two-dimensional array arr
 * num_rows and num_cols specify the dimensions of arr
 */
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

/*
Returns a binary array to indicate if for a cell (i,j) the neighboring cells are
in bounds. First val is left, second is up, third is right, fourth is down.
*/
int *checkbounds(int num_rows, int num_cols, int i, int j) {
  int l;
  int r;
  int u;
  int d;
  if (i - 1 >= 0) u = 1;
  else u = 0;

  if (i + 1 < num_rows) d = 1;
  else d = 0;

  if (j + 1 < num_cols) r = 1;
  else r = 0;

  if (j - 1 >= 0) l = 1;
  else l = 0;

  int *a = malloc(sizeof(int)*4);
  a[0] = l;
  a[1] = u;
  a[2] = r;
  a[3] = d;
  return a;
}

 /*
 For a given pixel in i, j delete the entire cell from arr i.e turn that pixel
 to 0.
 */
void delete_cell(int i, int j, int row_n, int col_n, int arr[row_n][col_n])
{
  if (arr[i][j] == 255) {
    arr[i][j] = 0;
    int *bounds = checkbounds(row_n, col_n, i, j);
    if (bounds[0]) delete_cell(i, j - 1, row_n, col_n, arr);
    if (bounds[1]) delete_cell(i - 1, j, row_n, col_n, arr);
    if (bounds[2]) delete_cell(i, j + 1, row_n, col_n, arr);
    if (bounds[3]) delete_cell(i+1, j, row_n, col_n, arr);
    free(bounds);
    return;
  }
  else return;

}

int count_cells(int num_rows, int num_cols, int arr[num_rows][num_cols]) {
  int count = 0;
  for (int i = 0; i < num_rows; i++) {
    for (int j = 0; j < num_cols; j++){
      if (arr[i][j] == 255) {
        count += 1;
        delete_cell(i, j, num_rows, num_cols, arr);
      }
    }
  }
  return count;
}
