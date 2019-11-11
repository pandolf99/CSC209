#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include "pmake.h"

// Return 1 if the first argument is newer than the second. 0 otherwise.
int compare_time(char *f1,char *f2) {
  struct stat statbf1, statbf2;

  stat(f1, &statbf1);
  stat(f2, &statbf2);

  struct timespec mod_time1 = statbf1.st_mtim;
  time_t secs1 = mod_time1.tv_sec;
  long nano_s1 = mod_time1.tv_nsec;

  struct timespec mod_time2 = statbf2.st_mtim;
  time_t secs2 = mod_time2.tv_sec;
  long nano_s2 = mod_time2.tv_nsec;
  if (secs1 == secs2) {
    return (nano_s1 > nano_s2);
  }
  else {
    return (secs1 > secs2);
  }
}

/*
Return 1 if at least one of the dependencies for rule is newer than the target.
Or the rule has null dependecy. 0 otherwise.
*/
int one_is_newer(Rule* rule) {
  Dependency *cur_d = rule->dependencies;
  if (cur_d == NULL) return 1;
  while (cur_d != NULL) {
    if (compare_time(cur_d->rule->target, rule->target)) return 1;
    cur_d = cur_d->next_dep;
  }
  return 0;
}

void execute_fork(Action *ac) {
  int r = fork();
  if (r == -1) {
    perror("fork");
    exit(1);
  }
  //Child process runs execution.
  else if (r == 0) {
    execvp(ac->args[0], ac->args);
    perror("execvp");
    exit(1);
  }
  //Parent waits.
  else if (r > 0) {
    int status;
    if (wait(&status) == -1) {
      perror("wait");
      exit(1);
    }
    if (WIFEXITED(status) == 1) {
      if (WEXITSTATUS(status) != 0) {
        fprintf(stderr, "Something went wrong when executing action\n");
        exit(1);
      }
    }
    return;
  }
}


void run_make(char *target, Rule *rules, int pflag) {
    //Find correct rule
    Rule *cur_r = rules;
    while (cur_r != NULL) {
      if (target == NULL) {
        break;
      }
      if (!strcmp(cur_r->target, target)) break;
      cur_r = cur_r->next_rule;
      //If target is not found in rules return;
      if (cur_r == NULL)  {
        fprintf(stderr, "Not a valid target\n");
        exit(1);
      }
    }
    //Check if it does not exist.
    struct stat temp;
    int exist = stat(cur_r->target, &temp);
    //Update dependencies
    Dependency *cur_d = cur_r->dependencies;
    int r = -1;
    while(cur_d != NULL) {
      if(!pflag) {
      run_make(cur_d->rule->target, rules, pflag);
      cur_d = cur_d->next_dep;
      }
      //Parallelize
      else {
        r = fork();
        if (r == 0) {
          run_make(cur_d->rule->target, rules, pflag);
          exit(0);
        }
        else {
          cur_d = cur_d->next_dep;
        }
      }
    }
    //Parent waits for all children if running in parallel.
    if (r > 0 && pflag) {
      cur_d = cur_r->dependencies;
      while (cur_d != NULL) {
        int status;
        if (wait(&status) == -1) {
          perror("wait");
          exit(1);
        }
        if (WIFEXITED(status) == 1) {
          if (WEXITSTATUS(status) != 0) {
            fprintf(stderr, "Something went wrong when updating dependencies\n");
            exit(1);
          }
        }
        cur_d = cur_d->next_dep;
      }
    }

    //Execute actions.
    if (exist == -1 || one_is_newer(cur_r)) {
       Action *cur_ac = cur_r->actions;
       while (cur_ac != NULL) {
         execute_fork(cur_ac);
         cur_ac = cur_ac->next_act;
       }
    }
}
