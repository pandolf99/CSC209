#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


#define MAX_ARGS 32
#define MAX_NAME 128

/* Create an array of arguments suitable for passing into execvp
   from "line". line is expected to start with a tab and contain a
   action from a Makefile. Remember to ensure that the last element
   of the array is NULL.

   It is fine to use MAX_ARGS to allocate enough space for the arguments
   rather than iterating through line twice. You may want to use strtok to
   split the line into separate tokens.

   Return NULL if there are only spaces and/or tabs in the line. No memory
   should be allocated and the return value will be NULL.
 */

char **build_args(char *line) {
  char **args = malloc(sizeof(char*)*MAX_ARGS);
  //Advance to the first string by skipping the tab
  line++;
  char *tok = strtok(line, " ");
  int i = 0;
  while(tok != NULL) {
    args[i] = malloc(sizeof(char)*sizeof(tok));
    strcpy(args[i], tok);
    //Remove new_line character at end
    if (strchr(args[i], '\n') != NULL) {
      args[i][strlen(tok) - 1] = '\0';
    }
    tok = strtok(NULL, " ");
    i++;
  }
  args[i] = (char*) NULL;
  return args;
}

/* Return 1 if the line contains only spaces or a comment (#)
   Return 0 if the line contains any other character before a #

   We want to ignore empty lines or lines that contain only a comment.  This
   helper function identifies such lines.
 */
int is_comment_or_empty(char *line) {
    if (line[0] == '\n') {
      return 1;
    }
    for(int i = 0; i < strlen(line); i++){
        if(line[i] == '#') {
            return 1;
        }
        if(line[i] != '\t' && line[i] != ' ') {
            return 0;
        }
    }
    return 1;
}

/* Convert an array of args to a single space-separated string in buffer.
   Returns buffer.  Note that memory for args and buffer should be allocted
   by the caller.
 */
char *args_to_string(char **args, char *buffer, int size) {
    buffer[0] = '\0';
    int i = 0;
    while(args[i] != NULL) {
        strncat(buffer, args[i], size - strlen(buffer));
        strncat(buffer, " ", size - strlen(buffer));
        i++;
    }
    return buffer;
}

/*
Returns how many dependencies are in a target line.
*/
int count_deps(char *line) {
  char *deps = strchr(line, ':');
  int count = 0;
  for (int i = 0; i < strlen(deps); i++) {
    if (deps[i] == ' ' && isalpha(deps[i+1])) {
      count++;
    }
  }
  return count;
}

/*
Return an array of strings where the first element is a target and the rest of
the array are the name of the dependencies. Last element is null.
*/
char **parse_targ(char *line) {
  //Make sure it has dependendencies.
  int n_deps = count_deps(line);
  //Create array of right size;
  char **arr = malloc(sizeof(char*)*(n_deps + 2));
  //Parse the target.
  char *dep_l = strchr(line, ' ');
  int targ_s = strlen(line) - strlen(dep_l);
  arr[0] = malloc(sizeof(char)*targ_s + 1);
  strncpy(arr[0], line, targ_s);
  arr[0][targ_s] = '\0';
  //Parse dependencies by token
  dep_l += 3;
  char *tok = strtok(dep_l, " ");
  int i = 1;
  while(tok != NULL) {
    arr[i] = malloc(sizeof(char)*sizeof(tok));
    strcpy(arr[i], tok);
    //Remove new_line character at end
    if (strchr(arr[i], '\n') != NULL) {
      arr[i][strlen(tok) - 1] = '\0';
    }
    tok = strtok(NULL, " ");
    i++;
  }
  arr[n_deps+1] = NULL;
  return arr;
}
