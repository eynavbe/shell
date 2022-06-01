# shell
shell that receive commands and run them
 - Run in the loop Show prompt (for example yes master?) And receive input from the user. In a note.
 - EXIT command - Exit what shell you are writing.
 - Read the current directory (using getcwd) and view it promptly.
 - Command starting with ECHO - print to standard output the output that appears after ECHO.
 - TCP PORT command - will open a TCP connection to Localhost (in the client role) and the standard output will be copied there. (I.e. all output went to socket instead of normal stdout).
 - Build a server that displays the output obtained.
Run a server file before running the command "TCP PORT" ie open another register to register:
 1) gcc server.c -o s
 2) ./s
 - LOCAL command - the standard output will go back to the terminal. Any socket if any (section D) will disconnect.
 - DIR Command - Displays the list of files in the current directory to the standard output.
 - CD command - change directory to partition that comes as a parameter after CD.
 - every single command you get - run it using a system. In a note.
 - every single command you get - run it using fork, exec, wait (without system). In a note.
 - COPY command - Copy the file that appears in the first word after COPY in the name of the second word after COPY.
 - DELETE command - delete the file after DELETE (using unlink).
 - bonus_ex1.c - Supports sign | (pipe) In order to generate processes when the output of one is directed to the input of the other. (
 - bonus_ex2.c - Supports recursive pipe - i.e. several times.


## Through the run:
### Option 1: Without using makeFile:
1. Open a terminal in Linux routed to the folder where the files are located.
2. gcc main.c helpFunc.c -o run
3. ./run

### Option 2: Using makeFile:
1. Open a terminal in Linux routed to the folder where the files are located.
2. make all
3. ./run

### If you are running the TCP PORT command you need to run it first:
1. Open another terminal in Linux routed to the folder where the files are located.
2. gcc server.c -o s
3. ./s

### Run bonus_ex1.c:
1. Open a terminal in Linux routed to the folder where the files are located.
2. gcc bonus_ex1.c -o run_ex1
3. ./run_ex1

### Run bonus_ex2.c:
1. Open a terminal in Linux routed to the folder where the files are located.
2. gcc bonus_ex1.c -o run_ex2
3. ./run_ex2

