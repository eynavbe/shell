#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

/**
 * Support recursive pipe - that is, several times
 */
static void pipe_recursive(char** cmds[], size_t pos, int in_fd) {
  //  last command
  if (cmds[pos + 1] == NULL) { 
    if(in_fd != 0){
      //dup2 takes an in_fd file descriptor to be cloned  and the 0 is the integer for a new file descriptor
      if (dup2(in_fd, 0) != -1){
        // close the in_fd
        if (close(in_fd) == -1) {   
          //Will display the error obtained if close does not work                    
          perror("close"); 
        }
      }else{
        // Will display the error obtained if dup2 does not work
        perror("dup2");
      }
    }
    // replaces the current process image with a new process image.
      // The first argument, should point to the filename associated with the file being executed. 
      // The array of pointers must be terminated by a NULL pointer.
    execvp(cmds[pos][0], cmds[pos]);
    //Will display the error obtained if execvp last does not work                    
    perror("execvp last");
  }
  else { 
    //  output pipe
    int fd[2]; 
    // pipe is a connection between two processes, 
    //such that the standard output from one process becomes the standard input of the other process.
    if (pipe(fd) == -1){
      // Will display the error obtained if pipe does not work
      perror("pipe");
    }
    // get the child id
    pid_t child = fork();
    // error fork
    if (child == -1)
    {
      // Will display the error obtained if fork does not work
      perror("fork");
    //If you are in the process of the child
    }else if (child == 0)
    {
      // close the fd[0]
       if (close(fd[0]) == -1) {   
        // Will display the error obtained if close does not work                    
        perror("close"); 
      }
      if(in_fd != 0){ /* read from in_fd */
      //dup2 takes an in_fd file descriptor to be cloned  and the 0 is the integer for a new file descriptor
        if (dup2(in_fd, 0) != -1){ 
          // close the in_fd
          if (close(in_fd) == -1) {   /* successfully redirected */  
            // Will display the error obtained if close does not work                                      
            perror("close"); 
          }
        }
        else{
          // Will display the error obtained if dup2 does not work                                      
          perror("dup2");
        }
      }
      if(fd[1] != 1){/* write to fd[1] */
      //dup2 takes an fd[1] file descriptor to be cloned  and the 1 is the integer for a new file descriptor
        if (dup2(fd[1], 1) != -1){
          // close the fd[1]
          if (close(fd[1]) == -1) {/* successfully redirected */   
            // Will display the error obtained if close does not work                                         
            perror("close"); 
          }
        }
        else{
          // Will display the error obtained if dup2 does not work                                      
          perror("dup2");
        }
      }
      // replaces the current process image with a new process image.
      // The first argument, should point to the filename associated with the file being executed. 
      // The array of pointers must be terminated by a NULL pointer.
      execvp(cmds[pos][0], cmds[pos]);
      // Will display the error obtained if execvp does not work
      perror("execvp");
    }else{
      // If you are in the parent process
      // close the fd[1]
      if (close(fd[1]) == -1) { /* unused */     
      // Will display the error obtained if close does not work                 
        perror("close"); 
      }
      // close the in_fd
      if (close(in_fd) == -1) {  /* unused */   
      // Will display the error obtained if close does not work                  
        perror("close"); 
      }
      // Perform the recursion
      pipe_recursive(cmds, pos + 1, fd[0]); 
    }
  }
}


int main(int argc, char* argv[]) {
  char promp[1024];
  int f = 1;
  int i = 0;
  printf("Write down the command:\n");
  /*
  Input examples:
  1) ps -ef | grep motzileon | awk {print $2} |
  2)   (Do not write down anything then it will be the default)
  3) ps -ef
  4) ps -ef | grep motzileon
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
    char*cmds1[10][20];
    char** cmds[10];
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
      cmds1[i1][0]= token2;
      cmds1[i1][1]= token3;
      cmds1[i1][2]= NULL;   
      cmds[i1] = cmds1[i1];
      i1++;
    }
    cmds[i1] = NULL;
    // Send to the recursive function that will execute the pipe that will support several processes
    pipe_recursive((char***)cmds, 0, 0);
  }else{
    str2 = strdup(promp);
    token2 = strsep(&str2, " ");
    if (strlen(token2) == 0){
      while (strlen(token2) == 0){
        token2 = strsep(&str2, " ");
      }
    }
    // If there is only one command (without |)
    if (!strchr(token2,'\n')){
      token3 = strsep(&str2, "\n");
      char* cmd1[] = { token2 , token3, NULL };
      char** cmds[] = { cmd1, NULL };
      // Send to the recursive function that will execute the pipe that will support several processes
      pipe_recursive((char***)cmds, 0, 0);
    }
  }
  // If the user typed a space and there is no command
  char* cmd1[] = { "ps" , "-ef", NULL };
  char* cmd2[] = { "grep", "motzileon", NULL };
  char* cmd3[] = { "awk", "{print $2}", NULL};
  char** cmds[] = { cmd1,cmd2,cmd3, NULL };
  // Send to the recursive function that will execute the pipe that will support several processes
  pipe_recursive((char***)cmds, 0, 0);
}