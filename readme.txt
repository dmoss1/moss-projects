Welcome to Daniel Moss' shell!
This guide will walk you through the basics of how to use the shell.
Enter a command to be executed into the command line, the shell will
continue to accept commands until"quit" is entered.
--------------------------------------------------------------------
1. Basic built in commands include:
	quit - Close the shell.
	clr - Clears the screen.
	dir [directory] - Display all of the files in the current directory.
	echo [comment] - Prints the [comment] to the screen.
	help - Displays this .txt file.
	cd [directory] - Changes the current directory to the path specified with [directory].
	environ - Displays the environment variables, including: 
		USER    The name of the logged-in user
      		HOME    A user's login directory.
	       	PATH    The sequence of directory prefixes that sh(1) and many other
          		programs apply in searching for a file known by an incomplete
			pathname.  The prefixes are separated by ':'.  (Similarly one
            	 	has CDPATH used by some shells to find the target of a change
            	  	directory command, MANPATH used by man(1) to find manual
             		pages, and so on)
      		PWD     The current working directory.
       		SHELL   The pathname of the user's login shell.

2. Redirection:
	< [inputfile] - Redirects any input from keyboard into [inputfile], usable on any executable file.
	> [outputfile] - Redrects input from the screen to [outputfile]. If the file exists, it will overwrite the 
			 current content of [outputfile]. If not, [outputfile] will be created and written to.
			 Usable on built-in commands dir, environ, echo, help, and any executable files.
	>> [outputfile] -Redrects input from the screen to [outputfile]. If the file exists, it will append to 
			 the current content of [outputfile]. If not, [outputfile] will be created and written to.
			 Usable on built-in commands dir, environ, echo, help, and any executable files.
3. Piping:
	Operations/commands can be executed together using the pipe operator. (|)
		process1 | process2
		This executes process1 and passes the output to process2 as input.
		When piping commands, the first command must give some output, and the second
			must accept some type of input.
4. Executables and Background Execution:
	To run an executable, enter the path to the executable.
	Using the & argument after the path, this executable will be ran in the background
	