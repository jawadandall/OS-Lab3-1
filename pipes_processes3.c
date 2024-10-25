#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

/**
 * Executes the command "cat scores | grep Lakers".  In this quick-and-dirty
 * implementation the parent doesn't wait for the child to finish and
 * so the command prompt may reappear before the child terminates.
 *
 */

int main(int argc, char **argv)
{
  int pipefd1[2]; // Pipe from parent to child 1
  int pipefd2[2]; // Pipe from parent to child 2
  int pid;
  int pid2;


  char *cat_args[] = {"cat", "scores", NULL};
  char *grep_args[] = {"grep", argv[1], NULL};
  char *sort_args[] = {"sort", NULL};


  // printf("%s\n", argv[1]);
  // make a pipe (fds go in pipefd1[0] and pipefd1[1])

  pipe(pipefd1);
  pipe(pipefd2);


  pid = fork();

  if (pid == 0) // child gets here and handles grep "argv"
    {
       pid2 =fork(); 

      if (pid2 ==0){ // child gets here and forks to handle sort
        
        //sort output from pipe 2
        dup2(pipefd2[0], 0);  // replace standard input with input part of pipe 2
        
        close(pipefd2[1]);
        close(pipefd2[1]);  //Close writing end of pipe 2
        close(pipefd1[0]);  //Close pipes not in use
        close(pipefd1[1]);
        execvp("sort",sort_args); //Executes sort command based on arguments passed

        

      }

      
      dup2(pipefd1[0], 0);  // replace standard input with input part of pipe 1

       
       // close writing end of pipe 1

      close(pipefd1[1]);
      close(pipefd1[0]);
      dup2(pipefd2[1], 1);  // replace standard output with output part of pipe 2

      close(pipefd2[0]);  // close reading end of pipe 2
      close(pipefd2[1]);
      execvp("grep", grep_args);  // execute grep

    }
  else
    {
      // parent gets here and handles "cat scores"

      // replace standard output with output part of pipe

      dup2(pipefd1[1], 1);

      // close unused input half of pipe

      close(pipefd1[0]);
      close(pipefd1[1]);
      close(pipefd2[1]);
      close(pipefd2[0]);
      // execute cat

      execvp("cat", cat_args);
    }
}