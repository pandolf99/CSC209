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
    args[i] = malloc(sizeof(char)*strlen(tok) + 1);
    strncpy(args[i], tok, strlen(tok) + 1);
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

int count_targ_l(char* line) {
  int count = 0;
  char *tok = strtok(line, " ");
  while(tok != NULL) {
    char *s = strchr(tok, ':');
    if (s == NULL) {
      count += 1;
    }
    tok = strtok(NULL, " ");
  }
  return count;
}

/*
Return an array of strings where the first element is a target and the rest of
the array are the name of the dependencies. Last element is null.
*/
char **parse_targ(char *line) {
  //Parse dependencies by token
  char *line_2 = malloc(sizeof(char)*strlen(line) +1);
  strcpy(line_2, line);
  int n_deps = count_targ_l(line_2);
  free(line_2);
  char **arr = malloc(sizeof(char*)*(n_deps + 1));
  char *tok = strtok(line, " ");
  int  i = 0;
  while(tok != NULL) {
    char *s = strchr(tok, ':');
    if (s == NULL) {
      arr[i] = malloc(sizeof(char)*strlen(tok) + 1);
      strncpy(arr[i], tok, strlen(tok) + 1);
      //Remove new_line character at end
      char *c;
      if ((c = strchr(arr[i], '\n')) != NULL) {
        *c = '\0';
      }
      i++;
    }
    tok = strtok(NULL, " ");
  }
  arr[n_deps] = NULL;
  return arr;
}
