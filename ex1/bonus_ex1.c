#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

/**
 * Supports sign | (pipe) In order to generate processes 
 * when the output of one is directed to the input of the other.
 */
void pipe_exec(char *args1[], char *args2[]) {
  //  output pipe
  int fd[2];
  // pipe is a connection between two processes, 
  //such that the standard output from one process becomes the standard input of the other process.
  if (pipe(fd) == -1){
    // Will display the error obtained if pipe does not work
    perror("pipe");
  }
  // get the child id
  pid_t pid = fork();
  // error fork
  if (pid == -1)
  {
    // Will display the error obtained if fork does not work
    perror("fork");
  //If you are in the process of the child
  }else if (pid == 0)
  {
    // get the second input
    // dup2 takes an fd[0](input part of pipe) file descriptor to be cloned and the 0 is the integer for a new file descriptor
    dup2(fd[0], 0);
    // close the fd[1] - unused of pipe
    close(fd[1]);
    // replaces the current process image with a new process image.
    // The first argument, should point to the filename associated with the file being executed. 
    // The array of pointers must be terminated by a NULL pointer.
    execvp(args2[0], args2);
  }else{
    // get the first input
    // dup2 takes an fd[1](output part of pipe) file descriptor to be cloned and the 1 is the integer for a new file descriptor
    dup2(fd[1], 1);
    // close the fd[0] - unused of pipe
    close(fd[0]);
    // replaces the current process image with a new process image.
    // The first argument, should point to the filename associated with the file being executed. 
    // The array of pointers must be terminated by a NULL pointer.
    execvp(args1[0], args1);
  }
}
int main(int argc, char **argv)
{
  char promp[1024];
  int f = 1;
  int i = 0;
  printf("Write down the command:\n");
  /*
  Input examples:
  1) cat bonus_ex1.c | grep dup2 
  2)   (Do not write down anything then it will be the default)
  3) ps -ef | grep motzileon
  4) cat etc/passwd | grep motzileon
  */
    //Receives input from the user
  while(f){
      scanf("%c", &promp[i]);
      if(promp[i] == '\n'){
          f= 0;
      }
      else{
          i++;
      }  
  }
  char *token, *str,*str2,*token2 ,*token3;
  // duplicate a string
  str = strdup(promp);  
  // When there is more than one command
  if (strchr(promp, '|')){
    char*cmds1[20];
    char*cmds2[20];
    // char** cmds[10];
    int i1 = 0;
    // split string - to receive each command individually
    while ((token = strsep(&str, "|"))) {
      // duplicate a string
      str2 = strdup(token);
      // split string 
      token2 = strsep(&str2, " ");
      // there is a space in the 0 position
      if (strlen(token2) == 0){
        while (strlen(token2) == 0){
          token2 = strsep(&str2, " ");
        }
      }
      token3 = strsep(&str2, "\n");
      int  ind[10],loop,j;
      char ch= ' ';
      char str[30];
      strcpy(str,token3);
      j=0;
      // to check if there is a profit in the last daughter of the sentence
      for(loop=0; str[loop]!='\0'; loop++){
        if(str[loop]==ch){
          ind[j++]=loop;
        }   
      }
      // if there is a space in the last daughter of the sentence then delete it
      if (ind[j-1] == strlen(token3)-1){
        token3[strlen(token3)-1] = '\0';
      }
      if (i1 == 0)
      {
        cmds1[0]= token2;
        cmds1[1]= token3;
        cmds1[2]= NULL; 
      }
      if (i1 == 1)
      {
        cmds2[0]= token2;
        cmds2[1]= token3;
        cmds2[2]= NULL; 
         // Send to the function that will execute the pipe that will support 2 processes
        pipe_exec(cmds1,cmds2);
      }
      i1++;
    }
  }else{
    // If the user typed a space and there is no command
    char *args1[] = {"cat", "bonus_ex1.c", NULL};
    char *args2[] = {"grep", "dup2", NULL};
    //  Send to the function that will execute the pipe that will support 2 processes
    pipe_exec(args1,args2);
  }
}