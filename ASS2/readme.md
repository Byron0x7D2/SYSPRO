# **SYSPRO assignment 2**

## **Vyron-Georgios Anemogiannis 11520200008**

### Introduction

For this assignment we are tasked with creating a simple server that one can remote to it and cast a vote and a simple client that takes a file with votes and casts each vote to the server.


### Compile and Run

In order to compile and run from directory that contains the server and client folders:

For server:
> cd server

> make

> make run 

or

> make valgrind

For client:
> cd client

> make

> make run 

or

> make valgrind

To create the ``inputFile``

> chmod 777 create_imput.sh

> ./create_input.sh politicalParties.txt 1000

To create the ``pollerResaultsFile``

> chmod 777 tallyVotes.sh

> ./tallyVotes.sh tallyResaultsFile 

To create the ``pollerResultsFile``

> chmod 777 processLogFile.sh

> ./processLogFile.sh pollLog.txt 

To compare the 2 files 

> diff pollerResaultsFile tallyResaultsFile 

If you want to change the host or the port numbers, or the command line arguments in general simply go to the corresponding makefile and change the arguments section. Default is the localhost and port 6969 for its meme potential and the rest are the same as the instructions.

### Problem 1

The solution for this problem can be found in the ``server`` directory. 
 - ``defines.h`` -> This file contains some parameters for the execution and 3 defines the user can change such as the size of the hash table, the maximum active connections and the maximum length of a name/vote. Names as understood by the instructions are a random string of 3 to 12 characters.
 - ``main.c`` -> This file contains the main function for the server. We also initialize the mutexes and condition variables. We have one mutex for access to the hash functions and another paired with the 2 condition variables to recreate the bounded buffer producers consumers problem as shown in the slides for the connections buffer. We also initialize a signal handler for CNTRL C (SIGINT) signal. This allows us to catch it and commence the end state of the server where all workers are unblocked from their condition variables and the master thread is unblocked from the ``accept`` syscall. In order for this unblocking to take place we also mask the signals in all other threads so we know that the master thread takes the SIGING signal. (Allowing it to exit the syscall). Finally, we call the corresponding function for the data stucture to go fill the ``pollStats.txt`` file.
 - ``master_thread.c`` -> This file contains the master thread function. This function starts by creating a socket, (overwrites existing ones with the options function), binds and listens to that socket. After, it creates as many worker threads as specified and goes on a loop until the SIGINT signal comes. Inside the loop, it accepts the connection and sends the fd in the producer side of the buffer. After the interrupt, it waits for the workers to finish and exits.
 - ``worker_thread.c`` -> This file contains the code for the worker threads. We start by defining 2 functions that send or receive a message to that socket. The main worker thread function is a simple loop until the interrupt that as the consumer takes the socket fd from the buffer, exhanges messages and updates, the guarded by mutexes, log data structure (that is the hash ADT basically)
 - ``buffer.c`` -> This file contains the code for the buffer implementation. It is a simple array we loop arround. Nothing noteworthy. The synchronisation for both data structures is managed before calling the functions so no thread safety is taken into account in the implementation.
 - ``log.c`` -> This file is a reuse of the hash ADT from the first assignment. It is enhanced with a seperate list that counts for each party its votes. Whenever we get an entry, we also print in the log file using the assignment format. There is also a function that prints in the given file the statisticks for each party, followed by the total number of votes.
 - Small note, due to valgrind complaining with pthread exit, we exit threads by returning NULL.

### Problem 2
The solution for this problem is in the ``client`` directory
 - As before, ``defines.h`` contains some parameters for the client like the maximum number of concurrent active threads and the size of the names/parties
 - ``main.c`` -> This file contains the main function for the client. We start by finding the number of lines inside the given file. Then we create structs that will be passed as arguments to the threads. Each struct contains the line the struct will be given to send to the server. Last, we create the threads and send them do their work. We can have up to ``MAX_ACTIVE_THREADS`` at a time. When we reach that number, we stop and wait for all the active ones to finish before moving on to create even more threads.
 - ``thread_worker.c`` -> This file contains The function each thread runs. We start by getting the name and the vote from the line in the argument. Then we connect to the server and exhange messages in order to cast the vote. 

 ### Problem 3

  - ``create_input.sh`` -> This script takes the ``politicalParties.txt`` input file and the number of lines and creates the ``inputFile`` file. This is done by taking a random number of number characters contained in the ``characters`` string. For the vote we choose a random line from the input file using ``shif``.
  - ``tallyVotes.sh`` ->  This script takes the ``inputFile`` and in the ``tallyResaultsFile`` that is passed as argument it saves for each party, its votes. This is done using the ``awk``, ``sort`` and ``uniq`` functions. ``awk`` gives the specific collumn, ``sort`` sorts the input and ``uniq`` using the -c flag, counts each entry. Last we reverse the output using ``awk`` again.
  - ``processLogFile.sh`` -> This script takes as argument the ``pollLog.txt`` file and using the same techniques creates the ``pollerResaultsFile``
  - Using ``diff`` we can see the 2 files are the same