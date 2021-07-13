/*
* Name: Aqeel Mozumder
* VNumber: V00884880
* CSC 360 Assignment 1
* Decription of the Solution: 
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
		only executes the STDIN files. The execute_command function is basically an idea of appendix c as it starts the child process by fork, morover the gettimeofday is taken from appendix F.
		Fifth is the Path Accumalator: This is where the commands from rc file is strcat with / and the actual command
*/






// ******************** LIBRARIES ***********************************************
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

// ********************** PERMITTED SIZE OF CHAR ARRAYS *************************
#define MAX_LINE_LENGTH 80
#define MAX_CMD_ARGUMENTS 9
#define MAX_PROMPT_SIZE 10
#define MAX_DIRS 10
#define MAX_NUM_TOKENS 30

// *********************** GLOBAL VARIABLES & ARRAYS *****************************
char directories[MAX_DIRS][MAX_LINE_LENGTH];
char *arguments[MAX_CMD_ARGUMENTS];
int dir_num;
int fd;
int rd;
int doubleColon =0;
int doubleColon_Index =0;
int hashtags =0;
char *outfilename;
char *infilename;


// ********************** HELPER FUNCTIONS ***************************************

// https://stackoverflow.com/questions/7898215/how-to-clear-input-buffer-in-c
void flush_stdin_buffer() {
	int c;
	while((c = getchar()) !=  '\n' && c != EOF) {}
}

// From appendix_a
void Remove_newline(char *string) {
	
	if (string[strlen(string) - 1] == '\n') {
		string[strlen(string) - 1]='\0';	
	}
		
}

void Starter(char* prompt){
	FILE* rc_file;
	rc_file = fopen(".vshrc", "r");
	if (rc_file == NULL) {
		fprintf(stderr, "ERROR opening .vshrc file!");
		exit(1);
	}
	
	// fgets(prompt, MAX_PROMPT_SIZE, rc_file);
	// Remove_newline(prompt);
	
	// idea taken from https://stackoverflow.com/questions/35225981/how-to-use-fgets-in-2d-arrays-multiple-dimension-arrays
	dir_num = 0;
	while( fgets(directories[dir_num], sizeof directories , rc_file) != NULL && dir_num < MAX_DIRS ) {
		
		Remove_newline(directories[dir_num]);
		fprintf(stdout, "%s", directories[dir_num]);
		dir_num++;
	}
	
	if (dir_num == 0) {
		fprintf(stdout, "No Directory paths!\n");
		exit(1);
	}

}

// To Check when there is doublecolon ::
void CheckColon(char *string){

	char *g;
	char *firsttoken;
	int fileret;

	g = strtok(string, "::");
	if(g == NULL){
	fprintf(stderr, "ERROR: Missing filename, Please type a filename before or after the double colon\n");
	exit(1);
	doubleColon =0;

	}
	else{	
		fileret = strncmp(string, "::", 2 );
		if(fileret == 0){
			outfilename = g;
		}
		else if(fileret>0){	
			infilename = g;
		}
	}

}

// ********************************** PATH ACCUMALATOR ****************************
int FindPath(char *command, char *path) {
	char test_path[MAX_LINE_LENGTH];
	struct stat file_info;

	for (int i=0; i<dir_num; i++) {
		test_path[0]='\0';
		strcat(test_path, directories[i]);
		strcat(test_path, "/");
		strcat(test_path, command);
		
		if(stat(test_path, &file_info) == 0) {
			if(file_info.st_mode & S_IXOTH) {
				memcpy(path, test_path, strlen(test_path)+1);
				return 1; 
			}
		}
	}

	memcpy(path, "\0", 1);	
	return 0;
}

// ****************************** TOKENIZER ********************************
// Ideas taken from Appendix E
int Tokenizer(char* input, char *tokens[]){
    char* ret;
	char* hash;
    int Tokens_num = 0;
	int args = 0;
	char *t;
	
	t = strtok(input, " ");
	while (t != NULL && Tokens_num < MAX_NUM_TOKENS) {
		tokens[Tokens_num] = t;
		
		ret = strstr(t,"::");
		hash = strstr(t,"##");
		if(ret){
			doubleColon = 1;
			doubleColon_Index = Tokens_num;
			
		}
		else{
			if(hash){
				hashtags =1;
			}
			else{
				arguments[args] = t;
				
				args++;
			}

		}
		
		if(hash){
			hashtags =1;

		}
		
		Tokens_num++;
		t = strtok(NULL, " ");
	}

	if(doubleColon == 1){
		
		CheckColon(tokens[doubleColon_Index]);
		
	}
	return Tokens_num;

}

// *************************** EXECUTORS ****************************************

//Executes STDOUT File
void Exec_Output(int doubleColon, char *outfilename){
	fd = open(outfilename, O_CREAT|O_WRONLY , S_IRUSR|S_IWUSR);
		if (fd == -1) {
			fprintf(stderr, "cannot open the file for writing\n");
			doubleColon = 0;
			exit(1);
		}
		dup2(fd, 1);
		dup2(fd, STDOUT_FILENO); 
}

//Executes STDIN File
void Exec_Input(int doubleColon, char *infilename){
	rd = open(infilename, O_RDONLY);
		if(dup2(rd, STDIN_FILENO) < 0) {
			fprintf(stderr, "cannot open the file, maybe does not exist!\n");
			doubleColon = 0;
			exit(1);
		}
	dup2(rd, 0);

}

// Ideas staken from Appendix C and Appendix F
void execute_command(char *path, char **args, int total_tokens) {
	char *envp[] = { 0 };
	int pid;
	int status;
	struct timeval before, after;
	if(hashtags == 1){
	gettimeofday(&before, NULL);
	}
	
	args[total_tokens] = 0;
	
	if ((pid = fork()) == 0) {
		
		if(doubleColon == 1){
			if(outfilename){
				Exec_Output(doubleColon, outfilename);
				
			}
			if(infilename){
				Exec_Input(doubleColon, infilename);
			
			}
			doubleColon = 0;
		}
		if(hashtags == 1){
			
			gettimeofday(&after, NULL);
			fprintf(stdout, "The command executed for %lu microseconds\n",
			(after.tv_sec - before.tv_sec) * 1000000 +
				after.tv_usec - before.tv_usec);
		}
		execve(path, args, envp);
		
	}

	while (wait(&status) > 0) {}
}

void other_commands(char* input){

    char *tokens[MAX_NUM_TOKENS];
	char Path[MAX_LINE_LENGTH];
	int Tokens_num;

	Tokens_num = Tokenizer(input, tokens);
	
    int found = FindPath(arguments[0], Path);
	if (found != 1) {
		fprintf(stderr, "Can't find executable for above command!\n");
		return;
	}
	
	execute_command(Path, arguments, Tokens_num);

}





// ****************************** MAIN *****************************************
int main(int argc,char *argv[]) {
    char input[MAX_LINE_LENGTH];
    char *tokens[MAX_NUM_TOKENS];
	char prompt[MAX_PROMPT_SIZE];
	char input_copy[MAX_LINE_LENGTH];
    int  line_len;


	Starter(prompt);

	// Ideas taken from Appendix A
    for(;;) {
        fprintf(stdout, "vsh%%  ");
        fflush(stdout);
        fgets(input, MAX_LINE_LENGTH, stdin);
        
		if (strchr(input, '\n')) {
			Remove_newline(input);
		} else {
			fprintf(stderr, "ERROR: A maximum of %d characters is allowed\n", MAX_LINE_LENGTH);
			flush_stdin_buffer();
			continue;
		}
        

        line_len = strlen(input); 
		strcpy(input_copy, input);

        Tokenizer(input_copy, tokens);
        

        if (strcmp(tokens[0], "exit") == 0) {
            exit(0);
        }
        else {
			
			other_commands(input);
		}
        
    }

}

