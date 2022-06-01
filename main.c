#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "helpFunc.h"
#include <dirent.h> 
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include<sys/wait.h>
#include<unistd.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#define MAX 1024
#define PORT 8080
#define SA struct sockaddr
#define SERVER_IP_ADDRESS "127.0.0.1"
int sockfd;
int fd;
int fd1;

void start_client()  
{
      //These are the basic structures for all syscalls and functions that deal with internet addresses.
    struct sockaddr_in servaddr;
    // socket create and verification
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
    }
    else
        printf("Socket successfully created..\n");
    /*
    the bzero() function erases the data in the n bytes of the memory
    starting at the location pointed to by s, by writing zeros (bytes containing '\0')
    to that area
    */
    bzero(&servaddr, sizeof(servaddr));
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
   if(inet_pton(AF_INET, (const char*)SERVER_IP_ADDRESS, &servaddr.sin_addr) <=0){
      printf("inet_pton was failed");
    }
    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed...\n");
    }
    else
        printf("connected to the server..\n");
     /*
    It duplicates one file descriptor, making them aliases, and then
    deleting the old file descriptor - stdout.
    */
    fd = dup(1);
    // close the socket
    close(1);
    fd1 = dup(sockfd);
    //close the socket
    close(sockfd);
    //dup - creates an alias for the provided file descriptor - stdout.
    
}


int main(){

/*
//Section A - Run in the loop Show prompt (for example yes master?) And receive input from the user.
  char promp_a[100];
  printf("yes master?\n");
  int f_a = 1;
  int i_a = 0;

  while(f_a){
      scanf("%c", &promp_a[i_a]);
      if(promp_a[i_a] == '\n'){
          f_a= 0;
      }
      i_a++;
  }
*/

//Section B - Read the current directory (using getcwd) and view it promptly.
  char cwd[256];
    //getcwd - Determines the path name of the working directory and stores it in buffer(cwd).
  if (getcwd(cwd, sizeof(cwd)) == NULL)
    perror("getcwd() error");
  else
    printf("%s\n", cwd);      

  char promp[1024];
  int f = 1;
  int i = 0;
  printf("Write down the command, type 'EXIT' to exit:\n");
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

//Section A - If the EXIT command is received - exit what shell you are writing.
  char exit[4] = "EXIT";
  while (!compareStr(promp, exit, 4))
  {

//Section C - Command starting with ECHO and print to standard output the output that appears after ECHO
    char echo[4] = "ECHO";
    /*
    Checks if at the beginning of the sentence the word "echo" appears, 
    if so, will print what comes next
    */
    if(compareStr(promp, echo, 4)){
      for(int i = 5; i<strlen(promp); i++)
        printf("%c", promp[i]);
    }

//Section D - TCP PORT command - will open a TCP connection to Localhost (in the client role) and the output standard will be copied there. (I.e. all output went to socket instead of normal stdout)
// Run a server file before running the command "TCP PORT" ie open another register to register:
// 1) gcc server.c -o s
// 2) ./s
    char tcp_port[8] = "TCP PORT";
        //Checks if at the beginning of the sentence the word "TCP PORT" appears, 
    if (compareStr(promp, tcp_port, 8)){
    //A TCP connection will be opened for Localhost 
      start_client();
    }

//Section E - LOCAL command - the standard output will go back to the terminal. Any socket if any (section D) will disconnect.
    char local[5] = "LOCAL";
        //Checks if at the beginning of the sentence the word "LOCAL" appears
    if (compareStr(promp, local, 5)){
      //Any socket if one exists will disconnect.
       // close socket server
      close(3);
      // close socket client
      close(1);
      //The standard output will be passed back to the terminal.
      fd1 = dup(fd);
    }


//Section F - DIR command - View the list of files in the current directory for standard output.
    char dir[3] = "DIR";
        //Checks if at the beginning of the sentence the word "DIR" appears.
    if(compareStr(promp, dir, 3)){
      struct dirent *dir1;
      //Goes to the file list in the current directory.
      DIR * d = opendir(".");
      if (d != NULL) {
         /*
        The readdir() function shall return a pointer to a structure representing the directory entry
        at the current position in the directory stream specified by the argument dirp,
        and position the directory stream at the next entry. 
        */
        while ((dir1 = readdir(d)) != NULL) {
          if(dir1-> d_type != DT_DIR){//If it is a file and not a directory 
            //print file name
            printf("%s\n", dir1->d_name);
          }
        }
        /*
        The closedir() function shall close the
        directory stream referred to by the argument dir1.
        */
        closedir(d);
      }
    }

//Section G - CD command - change directory to partition that comes as a parameter after CD
/*
Answer to the question- Is chdir a library function or a system call:
System Call- An interface between a user application and a service
provided by the operating system (or kernel) chdir is system call because of The
chdir function requests service from an operating system and seeks to change the
 current directory to another using an operating system cd
*/   
    char CD[2] = "CD";
    //Checks if at the beginning of the sentence the word "CD" appears
    if(compareStr(promp, CD, 2)){
      int n = 0;
      int j = 0;
      //Checking the length of the new library name
      for(int i = 3; i<strlen(promp); i++){
        if (promp[i] != ' ' && promp[i] != '\n')
        {
          j++;
        }
      }
      n=0;
      char * wordS = (char*)malloc(j*sizeof(char));
      //Put the name of the new directory where we assigned it
      for(int i = 0; i<j; i++){
        wordS[n++] = promp[i+3];
      }
      /*
      chdir() changes the current working directory of the calling
      process to the directory specified in path.
      */
      if (chdir(wordS) != 0) {
        fprintf(stderr, " chdir() to %s failed: %s\n",wordS,strerror(errno));
      }else{
        /*
        getcwd - These functions return a null-terminated string containing an absolute
        pathname that is the current working directory of the calling process
        and print the new directory.
        */
        getcwd(cwd, sizeof(cwd));
        printf("%s\n", cwd);
      }
      free(wordS);
    }

//Section H - every single command you get - run it using a system
/*
Answer to the question- Is the system a library function or a system call:
system is system call because of the system function requests service from an 
operating system and is used to execute commands through an operating system
*/

// system("cp server.c check.c ");  // To check section H, you must enable this line before removing section H from a comment
/*
    printf("\n___Section H - system___\n");
//system of Section B
    printf("\n___Section H - system of Section B___\n");
    system("pwd");
//system of section C
    printf("\n___Section H - system of Section C___\n");
    system("echo HAIDE MACCABI YAFO");
//system of Section F
    printf("\n___Section H - system of Section F___\n");
    system("dir");
//system of Section G
    printf("\n___Section H - system of Section G___\n");
    getcwd(cwd, sizeof(cwd));
    char command[1024];
    char command_error[1024];
    char * q = "cd ";
    char * w = cwd;
    char * t = "/check_file ; pwd";
    char * e = "/check ; pwd";
    char y;
    char u;
    for (int i = 0; i < strlen(q)+strlen(w)+strlen(t); i++)
    {
      if (i <= strlen(q)-1)
      {
        y = q[i];
        u = q[i];
      }else if (i-strlen(q) <=strlen(w)-1)
      {
          y = w[i-strlen(q)];
          u = w[i-strlen(q)];
      }else 
      {
        y = t[i-strlen(q)-strlen(w)];
        u = e[i-strlen(q)-strlen(w)];
      }
      command[i] = y;
      command_error[i] = u;
    }
    system(command);
    system(command_error);
//system of Section J
    printf("\n___Section H - system of Section J - COPY___\n");
    system("cp server.c b.c ");
//system of Section K
    printf("\n___Section H - system of Section K - DELETE___\n");
    system("unlink check.c");
*/


//Section I - every single command you get - run it using fork, exec, wait (without system)
// system("cp server.c b.c ");  // To check section I, you must enable this line before removing section I from a comment
/*    
    printf("\n___Section I - wait, exec, fork\n");
//get the process parent id
    pid_t parent = getpid();
//get the child id
    pid_t pid = fork();
    if (pid == -1)
    {
      perror("error");
    } 
//If you are in the parent process
    else if (pid > 0)
    {
      int status;
      waitpid(pid, &status, 0);
    }
//If you are in the process of the child
    else 
    {
      char *argv[] = {"/bin/sh", "-c", "pwd && echo HAIDE MACCABI YAFO && dir && cp server.c check.c && unlink b.c && cd check_file && pwd", 0};

// This causes the program that is currently being run by the 
// calling process to be replaced with a new program, with newly initialized
// stack, heap, and (initialized and uninitialized) data segments.

      execve(argv[0], &argv[0], NULL);
    }
*/

//Section J - COPY command - Copy the file that appears in the first word after COPY in the name of the second word after COPY.
//example: COPY server.c b.c 
/*
Answer to the question- Does your implementation use system calls or a library function:
System Call- An interface between a user application and
a service provided by the operating system (or kernel).
File management is system call because knowing the outline of the
file was used in reading the system.
*/
   char copy[4] = "COPY";
//Checks if at the beginning of the sentence the word "COPY" appears
    if(compareStr(promp, copy, 4)){
//Allocates space for source and destination names(
      char * src = (char*)malloc(30*sizeof(char));
      char * dst = (char*)malloc(30*sizeof(char));
      int r = 0;
      int nWord = 0;
      int i = 5;
//Write down the source name where we assigned it.
      while(promp[i] != ' ' || r < 1)
      {
        if (promp[i] == '.')
        {
          r = 1;
        }
        src[nWord++] = promp[i];
        i++;
      }
      nWord = 0;
      i++;
//Write down the destination name where we assigned it
      while((promp[i] != ' ' || r < 2) && promp[i] != '\n')
      {
        if (promp[i] == '.')
        {
          r = 2;
        }
        dst[nWord++] = promp[i];
        i++;
      }
      char charWord;
//open source file
      FILE *sourceFile = fopen(src, "rb");
//Opens a destination file so that we can write the contents of the source file into it.
      FILE *dstFile = fopen(dst, "wb");
//As long as there is something to read from the source file.
      while(fread(&charWord, 1, 1, sourceFile) == 1)
      {
//Write the contents of the original file in the destination file.
        fwrite(&charWord, 1, 1, dstFile);
      }
      // Close The Files
      fclose(sourceFile);
      fclose(dstFile);
      free(src);
      free(dst); 
    }

//Section K - DELETE command - delete the file after DELETE (using unlink)
/*
Answer to the question- Does your implementation use system calls or a library function:
System Call- An interface between a user application and a service
provided by the operating system (or kernel).
unlink is system call because knowing the outline of the file was used in reading the system.
*/

    char delete[6] = "DELETE";
//Checks if at the beginning of the sentence the word "DELETE" appears.
    if(compareStr(promp, delete, 6)){
      char * src_delete = (char*)malloc (15*sizeof(char));
      int r_delete = 0;
      int nWord_delete = 0;
      int i_delete = 7;
//Checks the file name to delete
      while((promp[i_delete] != ' ' || r_delete < 1) && promp[i_delete] != '\n')
        {
          if (promp[i_delete] == '.')
          {
            r_delete = 1;
          }
          src_delete[nWord_delete++] = promp[i_delete];
          i_delete++;
        }
/*
  unlink - The filename of the file which has to be deleted is sent as a 
  parameter and the function returns True on success and False on failure.
*/
        if (unlink(src_delete) != 0){
            perror("unlink() error");
        }
        free(src_delete);
    }  

//Receives new input from the user.
    f = 1;
    i = 0;
    bzero(promp, sizeof(promp));
    printf("Write down the command, type 'EXIT' to exit:\n");
    while(f){
      scanf("%c", &promp[i]);
      if(promp[i] == '\n'){
        f= 0;
      }
      else{
        i++;
      }  
    }
  }
  // If client did "EXIT" before doing "LOCAL" then it will close the server socket
  printf("%s","LOCAL");
}