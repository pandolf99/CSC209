#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAXLINE 256

#define SUCCESS "Password verified\n"
#define INVALID "Invalid password\n"
#define NO_USER "No such user\n"

int main(void) {
  char user_id[MAXLINE];
  char password[MAXLINE];

  /* The user will type in a user name on one line followed by a password
     on the next.
     DO NOT add any prompts.  The only output of this program will be one
	 of the messages defined above.
   */

  if(fgets(user_id, MAXLINE, stdin) == NULL) {
      perror("fgets");
      exit(1);
  }
  if(fgets(password, MAXLINE, stdin) == NULL) {
      perror("fgets");
      exit(1);
  }

  int fd[2];
  if ((pipe(fd)) == -1) {
      perror("pipe");
      exit(1);
  }

  int r = fork();

  //If its a child, run validate
  if (r ==  0) {
    //Change stdin so that it reads from pipe.
    if ((dup2(fd[0], fileno(stdin))) == -1) {
        perror("dup2");
        exit(1);
    }
    //Run validate
    execl("./validate", "validate", NULL);
    printf("Something bad happened");
    return 1;
  }
  //If its the parent print status
  else if (r > 0) {
    //close reading
    close(fd[0]);
    //write into the pipe the values
    write(fd[1], user_id, MAXLINE);
    write(fd[1], password, MAXLINE);

    //Wait for status
    int status;
    if (wait(&status) == -1) {
      perror("wait");
      exit(1);
    }
    int print_code = -1;
    if (WIFEXITED(status) == 1) {
      print_code = WEXITSTATUS(status);
    }
    else {
      exit(1);
    }
    if (print_code == 0) {
      printf("%s\n", SUCCESS);
    }
    else if (print_code == 1) {
      printf("Could not open pass.txt\n");
    }
    else if (print_code == 2) {
      printf("%s\n", INVALID);
    }
    else if (print_code == 3) {
      printf("%s\n", NO_USER);
    }
    return 0;
  }
  //else its an error
  else {
    perror("fork");
    exit(1);
  }
}
