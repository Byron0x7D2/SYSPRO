# **SYSPRO assignment 1**

## **Vyron-Georgios Anemogiannis 11520200008**

### Introduction

For this assignment we are tasked with creating a simple shell in order for us to exercise with process creation and the management of files and signals. This shell supports:
 - Redirections
 - Pipes 
 - Background execution
 - Cntrl-C and Cntrl-Z signal management
 - Wildcards support
 - Alias support
 - History


### Compile and Run

In order to compile, just go to the directory and type `make`, to run type `./mysh`. There is a `make run` option but then signals won't be working correctly. In addition, there is a `make valgrind` command that allows you to run and checks for leaks a while creating a report file. Last there is a `make clean` command that removes executables and object files and `make cleanall` that also removes the files created duriong execution. Pls update the paths to the files created in your system in the HOME path.

### Basic Program

This whole programm is a simple infinite loop. Each loop in main executes a single command, where a command is anything that ends with a newline or ampersand or semicolumn. This repeasts itself until the user types `rip` as input where the programm ends. To set the parameters of the program like maximum line size etc, go to the `defines.h` file.

Every time the `command` function is called it also starts a loop where it gets tokens from the `get_input_token` function. The latter reads characters continiusly and does the following actions:
 - In the Neutral state:
	- If it finds whitespace, it ignores it and moves on
	- If it finds newline, semicolumn, bar, less than, ambercand it returns the corresponding code to the caller function
	- If it finds greater than, it checks for another one for the append case, either way it sends the corresponding code again.
	- If it finds quotes or the dollar sign it changes the state to the corresponding one
	- If it finds something else, it is part of a word so it stores it in an array and switches the state to inside the word.
 - In the Inword state:
	- If it finds a character that does not belong to a word, it returns it to the input stream and tells the caller function it read a word.
	- Otherwise it stores the character and moves on.
 - In the Indollar state:
	- It also reads until a character that does not belong in the word. When it finishes, it calls the `getenv` function to get the value of the environment variable and returns that.
 - In the Inquote state:
	- Same as before but reads until it finds the other end of the parentheses.


The command function works the following way:
 - It takes the following arguments:
	- force_read, force_write, other_end: pipe ends for the next command to use in the recursion
	- wait_pid: pid of the last executed process, so we canhave access to it for signals and to wait for it if necessary
	- h: aliases ADT
	- ca: history ADT
	- fp: file we get input from, can either be stdin or a file that contains the next command to be executed
 - If it gets a Word:
	- If it is the first word it received in this command, it checks if it is an alias, then it checks for wildcards inside the word. Last, it allocates memory and puts it in the argv array.
 - If it gets a Semicolumn:
	- It executes the arguments so far and returns accordingly to main, so it can call the next command without printing the shell prompt.
 - If it gets a Ampersand:
	- It executes the arguments so far and returns accordingly to main, so it can call the next command without waiting for the process to finish.
 - If it gets a Bar:
	- It starts with creating a pipe for communication.
	- Then it executes the command with the write destination to be the one end of the pipe.
	- Then it recalls the `command` function with updated the read from argument so the next execution takes input from the pipe.
 - If it gets a redirect token:
	- It gets the next word of the input using the `get_input_token` function and this is the name of the file we are replacing the standard input or output with and updates the parameter for the next execution. This means that only one file is supported for redirections per direction. We can only have commands of type (of course whitespace is ignored):
	> exe < file1 > file2

	> exe < file1

	> exe > file2
 - If we get a newline:
	- We execute the command and return to main that the line finished and to print the input prompt again.
	- If the myhistory command was called with a number it informs main to take appropriate actions.
 - If we get an error:
	- We inform main and do nothing.

During the execution phase the following happen:
 - The `execute` function takes the following arguments:
	- argv: in argv[0] is the name of the executable and in the latter places are the command line arguments
	- srcfile: file to replace stdin
	- destfile: file to replace stdout
	- append: boolean if we are to append to the destination file
	- force_read, force_write: pipe ends that we force the input or output to be redirected
	- other_end: other end of the pipe in the previus arguments
	- h: hash table ADT for aliases
	- ca: list ADT for history keeping
 - If we get `cd`, we use `chdir` to update the path.
 - Special treatement for alias and myHistory that will be analyzed later.
 - If we get a `fg` command we resume the stopped process, up to one each time since it is not in the instructions.
 - Updates the input or output streams according to its command line arguments using the `dup2` function.
 - Forks for the child process.
 - Returns the pid of the created process for main to wait for it to end if it has to or -1 of there is nothing to wait. -2 is a special case for myhistory so main changes the input stream.

### **a) Redirection support**

When we find a redirection token, we continue reading in order to get the input or output file and we update the `srcfile` or `destfile` string. If we get the append token, we also make true the `append` variable. Once time comes for execution, If there is a string written in the above mentioned strings, we open the file given by the path to and get their file descriptors. If the `fdsrc` or `fddest` file descriptors have been altered by their original stdin or stdout file descriptors, we use `dup2` to redirect the standard input or output there. We also close any file descrtiptors that are not necessary. 

### **b)Pipes support**

When we get the pipe token (bar) we start by immediately creating a pipe. After, we send to execute the arguments we have gathered so far with the output file descriptor being the write end of the pipe. Last, using recursion, we call the `command` function again with updated the read file descriptor to be the read end of the pipe. This allows us to have multiple piped commands back to back without problem. 

> command | command | command |...

If the file descriptors for the pipe have been altered, during the call of the `execution` function we update the `fdsrc` and `fddest` variables to the value of the pipe. After the fork call we also close the other end of the pipe in the parent, in order to make sure the program running in the right end of the pipe gets EOF after the input given to the pipe. Without it, it would continue to expect input to be given.


### **c) Background execution support**

When getting a token for ampersand, I execute the command as per usual and then with the status returned to main, main knows not to wait for the process to finish so it can run while main fetches the next command. When the next command is executed, the program also forgets the pid of the process sent to the background, since it is not its problem anymore.

### **d) Wild character support**

In the `command` function, when I get a word, I first check if it contains the special characters *,[,],?. If yes, using the glob function we get a list for the matching. We put all the elements in the arguments for the next executable.

### **e) Alias support**

Let's sturt with the ADT to keep the aliases. We have created a simple hash table that stores each alias. This allows us a quick search for each word that comes to see if it is an alias or not. In addition, we have created a memory using a file in the `HOME` path to store the aliases in between executions. We load them at the start and save them at the end. During parsing, we always check the first argument if it is an alias. If yes we start the black magic. Basicall we change the input stream and now we read characters from a file that contains the string that has the command. If during execution, we get `createalias` or `destroyalias` we call the correct functions to store or delete from the hashtable.

### **f) Signal handling**

 - If the father has no child process, we do nothing. This means, that if you were about to type a command, you can still continue to do it.
 - If the father has a child process active, we catch the signal and pass it to the child whose pid we have kept.
 - Up to one stopped process at a time can be resumed with fg. 
 - After the interrupt the shell throws a new prompt or continues execution.

### **g) History support**

Like the aliases, we also have created an ADT to keep the history. It started as an array but ended up being a list, tho we decided not to change the name since it's a black box, who cares how it is impomplemented. For the history we keep the 20 most recent commands excluding the myhistory commands. Each time we read a character we save it to the ADT. If during reading we enclunter a new line, we move to the next slot in history. Calling history without args, prints the last 20 commands during this or previus exicutions. If it is followed by a number it immediately executes the command.


## More details in the comments of the code.