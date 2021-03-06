#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <ctype.h>

#include "pmake.h"


/* Print the list of actions */
void print_actions(Action *act) {
    while(act != NULL) {
        if(act->args == NULL) {
            fprintf(stderr, "ERROR: action with NULL args\n");
            act = act->next_act;
            continue;
        }
        printf("\t");

        int i = 0;
        while(act->args[i] != NULL) {
            printf("%s ", act->args[i]) ;
            i++;
        }
        printf("\n");
        act = act->next_act;
    }
}

/* Print the list of rules to stdout in makefile format. If the output
   of print_rules is saved to a file, it should be possible to use it to
   run make correctly.
 */
void print_rules(Rule *rules){
    Rule *cur = rules;

    while(cur != NULL) {
        if(cur->dependencies || cur->actions) {
            // Print target
            printf("%s : ", cur->target);

            // Print dependencies
            Dependency *dep = cur->dependencies;
            while(dep != NULL){
                if(dep->rule->target == NULL) {
                    fprintf(stderr, "ERROR: dependency with NULL rule\n");
                }
                printf("%s ", dep->rule->target);
                dep = dep->next_dep;
            }
            printf("\n");

            // Print actions
            print_actions(cur->actions);
        }
        cur = cur->next_rule;
    }
}
/*
Appends rul1 to the end of rul2 .
*/
void append_rule(Rule* rul1, Rule* rul2) {
  Rule *cur_r = rul2;
  while(cur_r->next_rule != NULL) {
    cur_r = cur_r->next_rule;
  }
  cur_r->next_rule = rul1;
}

/*
Return the node with target t if it is in rules, NULL otherwise.
*/
Rule *t_in_r(char *t, Rule *rules) {
  Rule *cur_r = rules;
  while(cur_r != NULL) {
    if (strcmp(cur_r->target, t) == 0) break;
    cur_r = cur_r->next_rule;
  }
  return cur_r;
}

/*
Initializes a rule to Null values.
Mainly to avoid memory errors. Should be called after memory is allocated for
a rule.
*/
void init_null(Rule *r) {
  r->dependencies = NULL;
  r->target = NULL;
  r->actions = NULL;
  r->next_rule = NULL;
}

/*
Sets the target for rule.
*/
void set_targ(char *targ, Rule *rule) {
  rule->target = malloc(sizeof(char)*strlen(targ)+1);
  strncpy(rule->target, targ, strlen(targ)+1);
}
/*
Set the dependencies for rule.
Create rules for each dependency and links them to the end of rule struct if
there is not a rule for that dependecy already.
New rule has target initialized.
*/
void set_deps(char **targ, Rule *rule, Rule *r_h) {
  if (targ[1] == NULL) {
    return;
  }
  int i = 1;
  Rule *cur_r = rule;
  cur_r->dependencies = malloc(sizeof(Dependency));
  Dependency *cur_r_d = cur_r->dependencies;
  while (targ[i] !=  NULL) {
    //Check if it already exists.
    Rule *pot_r = t_in_r(targ[i], r_h);
    if (pot_r != NULL) {
      cur_r_d->rule = pot_r;
    }
    //Else create a new rule
    else {
      cur_r_d->rule = malloc(sizeof(Rule));
      init_null(cur_r_d->rule);
      set_targ(targ[i], cur_r_d->rule);
      append_rule(cur_r_d->rule, rule);
    }
    if (targ[i+1] != NULL) {
      cur_r_d->next_dep = malloc(sizeof(Dependency));
      cur_r_d = cur_r_d->next_dep;
    }
    else {
      cur_r_d->next_dep = NULL;
    }
    free(targ[i]);
    i++;
  }
  free(targ[i]);
}

/*
Set an action for rule.
*/
void set_act(char *line, Rule* rule) {
  Action *ac = malloc(sizeof(Action));
  ac->next_act = NULL;
  ac->args = build_args(line);
  //If it is first rule
  if (rule->actions == NULL) {
    rule->actions = ac;
    return;
  }
  else {
    //Go to last rule
    Action *cur_ac = rule->actions;
    while (cur_ac->next_act != NULL) {
      cur_ac = cur_ac->next_act;
    }
    cur_ac->next_act = ac;
  }
}

/* Create the rules data structure and return it.
   Figure out what to do with each line from the open file fp
     - If a line starts with a tab it is an action line for the current rule
     - If a line starts with a word character it is a target line, and we will
       create a new rule
     - If a line starts with a '#' or '\n' it is a comment or a blank line
       and should be ignored.
     - If a line only has space characters ('', '\t', '\n') in it, it should be
       ignored.
 */
Rule *parse_file(FILE *fp) {
  char line[MAXLINE];
  Rule *r_h = malloc(sizeof(Rule));
  init_null(r_h);
  Rule *cur_r = r_h;
  char** targ_arr;
  while (fgets(line, MAXLINE, fp) != NULL) {
    if (is_comment_or_empty(line)) {
      continue;
    }
    //If it is a target.
    else if (isalpha(line[0])) {
      targ_arr = parse_targ(line);
      if (cur_r == r_h) {
        set_targ(targ_arr[0], cur_r);
        free(targ_arr[0]);
        set_deps(targ_arr, cur_r, r_h);
        free(targ_arr);
        cur_r = cur_r->next_rule;
        continue;
      }
      //Find rule if it exists.
      cur_r = t_in_r(targ_arr[0], r_h);
      //If it is a new rule
      if (cur_r == NULL) {
        cur_r = malloc(sizeof(Rule));
        init_null(cur_r);
        set_targ(targ_arr[0], cur_r);
        set_deps(targ_arr, cur_r, r_h);
        append_rule(cur_r, r_h);
      }
      //If it is an existing rule, just set deps. 
      else {
        set_deps(targ_arr, cur_r, r_h);
      }
      free(targ_arr[0]);
      free(targ_arr);
    }
    else {
      set_act(line, cur_r);
      continue;
    }
  }
  return r_h;
}
