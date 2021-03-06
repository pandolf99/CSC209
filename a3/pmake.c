#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include "pmake.h"



/*
Function to free all the memory used by the rule struct.
*/

void free_args(char **args) {
  int i = 0;
  while(args[i] != NULL) {
    free(args[i]);
    i++;
  }
  free(args);
}

void free_acs(Action *act) {
  if (act == NULL) {
    free(act);
    return;
  }
  if (act->next_act == NULL) {
    free_args(act->args);
    free(act);
  }
  else {
    free_acs(act->next_act);
    free_args(act->args);
    free(act);
  }
}

void free_ds(Dependency *d) {
  if (d == NULL) {
    free(d);
    return;
  }
  if (d->next_dep == NULL) {
    free(d);
  }
  else {
    free_ds(d->next_dep);
    free(d);
  }
}

void free_r(Rule *r) {
  free(r->target);
  free_ds(r->dependencies);
  free_acs(r->actions);
  free(r);
}

void free_l(Rule *r) {
  if (r == NULL) {
    free(r);
    return;
  }
  if (r->next_rule == NULL) {
    free_r(r);
  }
  else {
    free_l(r->next_rule);
    free_r(r);
  }
}


/* This program reads and evaluates a Makefile very much like the program make
   but with some simplifications, and the option to evaluate dependencies in
   parallel.

   Options:
     "-o"  - print the rules data structure as if it were a Makefile
     "-p"  - create a new process to evaluate each dependency of a rule
     "-f <filename>" - Use "filename" as the input Makefile.  If this option is
             not provided then pmake will use a file called "Makefile"
     "target" - specify the rule in the file to evaluate. If no target is
             provided, pmake will evaluate the first rule.
 */

int main(int argc, char **argv) {
    FILE *fp;
    char *filename = "Makefile";
    char *target = NULL;
    int output = 0;
    int parallel = 0;
    char opt;

    while ((opt = getopt(argc, argv, "f:op")) != -1) {
        switch (opt) {
        case 'o':
            output = 1;
            break;
        case 'p':
            parallel = 1;
            break;
        case 'f':
            filename = optarg;
            break;
        default: /* '?' */
            fprintf(stderr, "Usage: %s [-op][-f <filename>] [target]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if(optind < argc) {
        target = argv[optind];
    }
    if((fp = fopen(filename, "r")) == NULL) {
        perror("fopen");
    }

    Rule *rules = parse_file(fp);
    if(output) {
        print_rules(rules);
    }

    run_make(target, rules, parallel);

    free_l(rules);

    fclose(fp);

    return 0;
}
