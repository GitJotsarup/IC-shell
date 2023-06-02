/* ICCS227: Project 1: icsh
 * Name: Jotsarup Singh Narula
 * StudentID: 6380841
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

#define MAX_COMMAND_LENGTH 100

// Global variables
char cmdhist[2][50]; // creating an array cmdhist of size 2 that can contain 50 characters to keep track of the command history
void cmdhistory()
{
    strcpy(cmdhist[0], "null"); // current buffer
    strcpy(cmdhist[1], "null"); // previous buffer
}

void history(char *string)
{
    strcpy(cmdhist[0], cmdhist[1]);
    strcpy(cmdhist[1], string);
}

void bang()
{
    printf("%s\n", cmdhist[0]);
    system(cmdhist[0]); // changed this to the system() so it can run commands that aren't in my shell for milestone 4
}

// backjob
typedef struct
{
    pid_t pid;
    char command[MAX_COMMAND_LENGTH];
    int completed; // 0 = running, 1 = completed
} Job;

Job jobs[100]; // arrya to keep track of the  jobs
int num_jobs = 0; // num of jobs

void execute_command(const char *command, int background)
{
    // Check if the command is executable
    // if (access(command, X_OK) != 0)
    // {
    //     printf("bad command\n");
    //     return;
    // }

    pid_t pid = fork();

    if (pid == -1)
    {
        perror("fork dieded");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        // Child process

        // Check if input redirection is specified
        char *input_file = NULL;
        char *output_file = NULL;

        char *symbol = strstr(command, "<");
        if (symbol != NULL)
        {
            *symbol = '\0';
            input_file = strtok(symbol + 1, " \t"); //input file name
        }

        // checking if output redirection is given
        symbol = strstr(command, ">");
        if (symbol != NULL)
        {
            *symbol = '\0';
            output_file = strtok(symbol + 1, " \t"); // output file name
        }

        // redirect input
        if (input_file != NULL)
        {
            int fd_input = open(input_file, O_RDONLY);
            if (fd_input == -1)
            {
                perror("Failed to open input file");
                exit(EXIT_FAILURE);
            }
            dup2(fd_input, STDIN_FILENO);
            close(fd_input);
        }

        // redirect output
        if (output_file != NULL) {
            int fd_output = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd_output == -1) {
                perror("failed to write file");
                exit(EXIT_FAILURE);
            }
            dup2(fd_output, STDOUT_FILENO);
            close(fd_output);
        }

        char *argv[] = {"/bin/sh", "-c", (char *)command, NULL};
        execv("/bin/sh", argv);
        perror("execv failed");
        exit(EXIT_FAILURE);
    }
    else {
        // Parent
        if (!background) {
            int status;
            waitpid(pid, &status, 0);
        }
        else {
            // background bg
            Job job;
            job.pid = pid;
            strcpy(job.command, command);
            job.completed = 0;
            jobs[num_jobs++] = job;
        }
    }
}

void signal_handler(int signum) {
    if (signum == SIGINT) {
        // printf("\ngot it\n");
        fflush(stdout);
    }
    else if (signum == SIGCHLD) {
        // Child process terminated
        pid_t pid;
        int status;

        while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
            for (int i = 0; i < num_jobs; i++) {
                if (jobs[i].pid == pid) {
                    printf("bnackground job terminated: %s\n", jobs[i].command);
                    fflush(stdout);

                    for (int j = i; j < num_jobs - 1; j++) {
                        jobs[j] = jobs[j + 1];
                    }
                    num_jobs--;
                    break;
                }
            }
        }
    }
}

void list_jobs() {
    for (int i = 0; i < num_jobs; i++) {
        printf("[%d] %s %s\n", i + 1, jobs[i].completed ? "Completed" : "Running", jobs[i].command);
    }
}

void echoo(char buffer[50]) {
    int length = strlen(buffer);
    char strcopy1[length];
    strcpy(strcopy1, buffer);

    char delimiter[] = " ";
    char *firstWord, *secondWord, *remainder, *context;

    int inputLength = strlen(strcopy1);
    char *inputCopy = (char*) calloc(inputLength + 1, sizeof(char));
    strncpy(inputCopy, strcopy1, inputLength);

    firstWord = strtok_r (inputCopy, delimiter, &context);
    secondWord = strtok_r (NULL, delimiter, &context);
    remainder = context;

    if (secondWord != NULL) {
        printf("%s ", secondWord);
    }

    if (remainder != NULL) {
        printf("%s", remainder);
    }
}

// void extra(){
// 		if(!waitt)
// 			wait(NULL);
// 		int dont_wait = checkWait();
// 		char * argvs[MAX_ARGS];

// 		if(args[0]){
// 			if(!strcmp(args[0], "echo")){	//"echo" command
// 				arg = args;
// 				*arg++;
// 				while(*arg){
// 					fprintf(stdout, "%s ",*arg++);
// 				}
// 				fprintf(stdout, "\n");
// 				return;
// 			}
            
		
// 			if(!strcmp(args[0], "help")){	//"help" command
// 				printf("a simple list of built in commands: \n");
//                 printf("bang - !! - prints & executes the previous commant\n");
//                 printf("echo - echo - prints the string from the buffer\n");
//                 printf("exit - exit <num> - exits from Jotsarups shell\n");
//                 printf("I/O redirection with arrows <<, <, > and >>\n");
//                 printf("background process - & - forces a process to move to the background\n");
//                 printf("File execution");
//                 printf("File navigation - cd - to move between directories");
//                 printf("File directory showoff - ls - shows the contents of the current directory");
                

// 			}			
// 	        // milestone 1
// 			if(!strcmp(args[0], "exit")){
//                 printf("%s", args[1]);
//                 printf("buh-bye");
// 				exit(0);
//             }

//             if(p){             // this identifies the .sh       
//                 found = strcmp(p, ".sh") == 0;
//                 if(found){
//                     fileread(args[0]);
//                 }
//             }
            

// 			if(!strcmp(args[0], "cd")){	//"cd" command

// 				arg = args;	
// 				char *current_dir = (char *)malloc(MAX_ARGS);
// 				char *dir = (char *)malloc(MAX_ARGS);
                
// 				if(!getcwd(current_dir, MAX_ARGS)){
// 					printf("error \n");
// 					return;
// 					}
// 				fprintf(stdout, "Previous Directory: %s\n",current_dir);
// 				*arg++;
// 				dir = strcpy(dir, *arg);
                
// 				fprintf(stdout, "the current directory is -  %s\n",getcwd(dir, MAX_ARGS));		
	
// 				strcpy(prompt, "icsh $");
// 				strcat(prompt, dir);
	
// 				free(current_dir);
//                 free(dir);
// 				return;
// 			}

//             // if (strcasecmp(args[0], "!!") == 0){
//             //     printf("%s\n", cmdhist[0]);
//             //     char h = system(cmdhist[0]);
//             //     printf("%s", h);
//             // }
            
// 			if(!strcmp(args[0], "dir")){	//"dir" command
// 				arg = args;
// 				int flagr = 0;
// 				int flagw = 0;
// 				int flagdir = 0;
// 				char *rfile = (char *)malloc(MAX_ARGS);
// 				char *wfile = (char *)malloc(MAX_ARGS); 
// 				argvs[0] = "ls";
// 				argvs[1] = "-al";			
				
// 				*arg++;
// 				while(*arg){
// 					if(!strcmp(*arg, "<")){
// 						*arg++;
// 						flagr = 1;
// 						strcpy(rfile, *arg);
// 						*arg++;
// 						continue;
// 						}
// 					if(!strcmp(*arg, ">")){
// 						*arg++;
// 						flagw = 1;
// 						strcpy(wfile, *arg);
// 						*arg++;
// 						continue;
// 						}
// 					if(!strcmp(*arg, ">>")){
// 						*arg++;
// 						flagw = 2;
// 						strcpy(wfile, *arg);
// 						*arg++;
// 						continue;
// 						}
					
// 					argvs[2] = *arg;
// 					argvs[3] = NULL;
// 					flagdir = 1;
// 					*arg++;
// 					}				


// 				if(!flagdir)				
// 					argvs[2] = NULL;

// 				pid_t pid = 0;	
// 				switch (pid = fork ()) {
// 					case -1:
// 						fprintf(stderr, "fork error\n");
// 					case 0:			// child
// 						if(flagr)
// 							if(!freopen(rfile, "r", stdin))
// 								fprintf(stdout, "%s can't be open for input redirection.\n",rfile);
// 						if(flagw == 1)
// 							if(!freopen(wfile, "w", stdout))
// 								fprintf(stdout, "%s can't be open for output redirection.\n",wfile);
// 						if(flagw == 2) 
// 							if(!freopen(wfile, "a", stdout))
// 								fprintf(stdout, "%s can't be open for output redirection.\n",wfile);

// 						execvp("ls", argvs);
// 						fprintf(stderr, "exec error\n");
// 					default:		// parent
// 						if (!dont_wait){
// 							waitt = 1;
// 							wait(NULL);
// 						}
// 						else
// 							waitt = 0;

// 				}
// 				free(rfile);free(wfile);
// 				return;
// 			}

		
// 			arg = args;
// 			int flagr = 0;
// 			int flagw = 0;
// 			char *rfile = (char *)malloc(MAX_ARGS);
// 			char *wfile = (char *)malloc(MAX_ARGS); 
// 			arg++;
// 			while(*arg){
// 				if(!strcmp(*arg, "<")){
// 					arg++;
// 					flagr = 1;
// 					strcpy(rfile, *arg);
// 					}
// 				if(!strcmp(*arg, ">")){
// 					arg++;
// 					flagw = 1;
// 					strcpy(wfile, *arg);
// 					}
// 				if(!strcmp(*arg, ">>")){
// 					arg++;
// 					flagw = 2;
// 					strcpy(wfile, *arg);
// 					}
// 				arg++;
// 				}		
// 			arg = args;
            
// 			int n = 0;
// 			while(*arg++) n++;
// 			if(flagr && flagw) args[n - 4] = NULL;
// 			if(flagr || flagw) args[n - 2] = NULL;
// 			pid_t pid = 0;	
// 			switch (pid = fork ()) {
// 				case -1:
// 					fprintf(stderr, "fork error\n");
// 				case 0:			// child
// 						if(flagr)
// 							if(!freopen(rfile, "r", stdin))
// 								printf("error \n");
// 						if(flagw == 1)
// 							if(!freopen(wfile, "w", stdout))
// 								printf("error cant open \n");
// 						if(flagw == 2) 
// 							if(!freopen(wfile, "a", stdout))
// 								printf("error \n");
					
// 					execvp(args[0], args);
// 					fprintf(stderr, "error\n");
// 					arg = args;
// 					while(*arg) fprintf(stdout, "%s ", *arg++);
// 					fputs("\n", stdout);
// 					exit(0);        // child?		
// 				default:
// 					if (!dont_wait){        // going ahead
// 						waitt = 1;
// 						wait(NULL);
// 					}
// 					else
// 						waitt = 0;
// 				}
//             free(wfile);
// 			free(rfile);
// 	}
// }


int main(int argc, char *argv[]) {
    char command[MAX_COMMAND_LENGTH];
    signal(SIGINT, SIG_IGN);        // ignore ctrl c
    // signal(SIGTSTP, SIG_IGN);       // ignore ctrl z
    signal(SIGINT, signal_handler);
    signal(SIGCHLD, signal_handler);

    if (argc == 2) {
        // Script mode
        FILE *script_file = fopen(argv[1], "r");
        if (script_file == NULL)
        {
            perror("Failed to open script file");
            exit(EXIT_FAILURE);
        }

        while (fgets(command, sizeof(command), script_file) != NULL) {
            command[strcspn(command, "\n")] = '\0';

            history(command);

            int length = strlen(command); // getting the length of the buffer text inputted
            char strcopy[length];        // creating a place to store a copy that can be altered
            strcpy(strcopy, command);    // storing the buffer into strcopy
            char *token = strtok(strcopy, " ");        // getting the first word in the string by using a space as the seperator

            if (strcmp(command, "exit") == 0) {
                printf("Goodbye!\n");
                break;
            }
            else if (strcmp(command, "!!") == 0) {
                bang();
            } else if (strcmp(command, "echo") == 0) {
                echoo(command);
            }
            else if (strcmp(command, "jobs") == 0) {
                list_jobs();
            }
            else {
                int background = 0;
                char *symbol = strchr(command, '&');
                if (symbol != NULL) {
                    *symbol = '\0';
                    background = 1;
                }
              // execute order 66
                execute_command(command, background);
            }
        }

        fclose(script_file);
    }
    else {
        while (1) {
            printf("jotShell $ ");
            fflush(stdout);

            fgets(command, sizeof(command), stdin);
            command[strcspn(command, "\n")] = '\0'; // remove newline

            history(command);

            int length = strlen(command); // getting the length of the buffer text inputted
            char strcopy[length];        // creating a place to store a copy that can be altered
            strcpy(strcopy, command);    // storing the buffer into strcopy
            char *token = strtok(strcopy, " ");        // getting the first word in the string by using a space as the seperator
        //printf("%s", token);

            if (strcmp(command, "exit") == 0) {
                printf("Goodbye!\n");
                break;
            } else if (strcmp(token, "echo") == 0) {
                echoo(command);
            } else if (strcmp(command, "!!") == 0) {
                bang();
            } else if (strcmp(command, "jobs") == 0) {
                list_jobs();
            }
            else {
                int background = 0;
                char *symbol = strchr(command, '&');
                if (symbol != NULL){
                    *symbol = '\0';
                    background = 1;
                }
                execute_command(command, background);
            }
        }
    }

    return 0;
}
