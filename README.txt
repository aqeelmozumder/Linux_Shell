 Decription of the Solution: 
		An Example on how to use a simple command:
		 vsh% <type here>
		Example 1: vsh% ls
		Example 2: vsh% ls -la /usr/bin ##
		Example 3: vsh% ls -l ::output.txt
		Example 4: vsh% wc output.txt::

		To Exit: type "exit"
		Example: vsh% exit

		There are 5 sections which are important for this solution to be understood. 
		First is the Main: The Main initiallzes a starter function which will be explained in the helper function section. After that from Appendix A, the for loop ensures that the
			shell prompts the user continously till the user types "exit". All other commands will be directed to a function called other_commands which will be explained under Executors section
		
		Second is the Helper Functions: The first function flush_stdin_buffer helps to clear the buffer as it was creating some issues for me. Remove_neline is a finction that removes the trailing 
			newline (in the rc file), THERE ARE CAN BE SOME ISSUES LIKE LINES fROM RC FILE NOT BEING READ PROPERLY WHICH CAN BE A PROBLEM FOR PATH. 
			
******	For my vshrc file,  I had to write the directories as 
																vsh %
																/usr/bin
																/bin
			THIS ARRANGEMENT MIGHT ONLY WORK FOR "/bin" BECAUSE THE FIRST TWO LINES MIGHT GET CORRUPTED
			SO THEREFORE ONLY THE LAST LINE DIRECTORY WORKS IF THE ISSUES PERSISTS.
*******			
			Check_Colon is a function that checks if the :: was at the start of the string or end of the string.
		Third is the Tokenizer: The Tokenizer function idea is taken from Appendix E, where it divides the input to smaller tokens, and other the tokens :: and ## all other  tokens are arguments.
		Fourth are the Executors: The other_command is a function that checks if the path exists or not, if it does then execute. The Exec_Output only executues the STDOUT files and the Exec_Input
		only executes the STDIN files. The execute_command function is basically an idea of appendix b,c, d as it starts the child process by fork, morover the gettimeofday is taken from appendix F.
		Fifth is the Path Accumalator: This is where the commands from rc file is strcat with / and the actual command

