#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

/* Write random integers (in binary) to a file with the name given by the command-line
 * argument.  This program creates a data file for use by the time_reads program.
 */

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: write_test_file filename\n");
        exit(1);
    }

    FILE *fp;
    if ((fp = fopen(argv[1], "w")) == NULL) {
        perror("fopen");
        exit(1);
    }

    srand(time(NULL));
    for(int i = 0; i < 100; i++) {
      int r = rand() % 100;
      fwrite(&r, sizeof(int), 1, fp);
    }

    fclose(fp);
    return 0;
}
