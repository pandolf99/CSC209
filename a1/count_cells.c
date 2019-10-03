#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void read_image(int num_rows, int num_cols,
                int arr[num_rows][num_cols], FILE *fp);

void print_image(int num_rows, int num_cols, int arr[num_rows][num_cols]);

int count_cells(int num_rows, int num_cols, int arr[num_rows][num_cols]);

// Remember to include the function prototype for count_cells


int main(int argc, char **argv) {
    // Print a message to stderr and exit with an argument of 1 if there are
    // not the right number of parameters, or the second argument is not -p

    if (((argc - 1) < 1) || ((argc - 1) > 2) ||
    (argc == 3 && strcmp(argv[2], "-p") != 0)) {
      fprintf(stderr, "Usage: count_cells <imagefile.txt> [-p] \n");
      exit(1);
    }

    // Declare important variables and get size
    FILE *f = fopen(argv[1], "r");
    int height;
    int width;
    fscanf(f, "%d %d\n", &height, &width);
    int arr[height][width];

    read_image(height, width, arr, f);
    int count;
    count = count_cells(height, width, arr);
    // Check for second parameter
    if (argc == 3 && strcmp(argv[2], "-p") == 0) {
       print_image(height, width, arr);
     }
    printf("Number of Cells is %d\n", count);
    return 0;
}
