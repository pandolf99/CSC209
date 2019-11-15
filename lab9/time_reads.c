/* The purpose of this program is to practice writing signal handling
 * functions and observing the behaviour of signals.
 */

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

/* Message to print in the signal handling function. */
#define MESSAGE "%ld reads were done in %ld seconds.\n"

/* Global variables to store number of read operations and seconds elapsed.
 */
long num_reads, seconds;

/*
Signal handler for SIFPROF
*/
void sigprof_hand(int sig) {
  printf(MESSAGE, num_reads, seconds);
  exit(0);
}

/* The first command-line argument is the number of seconds to set a timer to run.
 * The second argument is the name of a binary file containing 100 ints.
 * Assume both of these arguments are correct.
 */

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: time_reads s filename\n");
        exit(1);
    }
    seconds = strtol(argv[1], NULL, 10);

    FILE *fp;
    if ((fp = fopen(argv[2], "r")) == NULL) {
      perror("fopen");
      exit(1);
    }
    //Create sig_action
    struct sigaction sa;
    sa.sa_handler = sigprof_hand;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGPROF, &sa, NULL);

    //Set the timer
    struct itimerval t;
    t.it_interval.tv_sec = 0;
    t.it_interval.tv_usec = 0;
    t.it_value.tv_sec = seconds;
    t.it_value.tv_usec = 0;

    /* In an infinite loop, read an int from a random location in the file,
     * and print it to stderr.
     */
    srand(time(NULL));
    num_reads = 0;
    setitimer(ITIMER_PROF, &t, NULL);
    for (;;) {
      int r = rand() % 100;
      fseek(fp, sizeof(int)*r, SEEK_SET);
      int i;
      fread(&i, sizeof(int), 1, fp);
      num_reads += 1;
      //printf("%d\n", i);
    }
    return 1; // something is wrong if we ever get here!
}
