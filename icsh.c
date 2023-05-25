/* ICCS227: Project 1: icsh
 * Name: Jotsarup Singh Narula
 * StudentID: 6380841
 */

#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <signal.h>

#define MAX_CMD_BUFFER 255

// MILESTONE 1

// echo command repeats the input

void echoo(char buffer[50]){
    /*
    The buffer holds the string currently typed
    we wanna get the first word of that, see if its echo
    then printf the rest of the string
    this is just string manipulation
    */

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

    printf("%s ", secondWord);
    printf("%s", remainder);
    //system(); performs a system call
}

// !! -- double-bang command - repeats the last command given
// double bang will print out the last command then run it too
// when there is no last command itll just promt you

char cmdhist[2][50];           // creating an array cmdhist of size 2 that can contain 50 characters to keep track of the command history
void cmdhistory(){
    strcpy(cmdhist[0], "null");    // current buffer
    strcpy(cmdhist[1], "null");    // previous buffer
}

void history(char *string){        // This functions sole purpose is to update the history of the last and current command
    strcpy(cmdhist[0], cmdhist[1]);
    strcpy(cmdhist[1], string);
}

void bang(){
    /*
    So i think this one is a bit tricky, we can store the history in a array though that seems pretty simple, 1 array is 1 command and we only need an array of size 1
    */
    printf("%s\n", cmdhist[0]);
    system(cmdhist[0]);                // changed this to the system() so it can run commands that aren't in my shell for milestone 4
}

// exit <num> - exit code to exit back to shell or whatever
// use system call exit()
// exit code should be between 0-255, if a larger number is given
// we have to truncate them to fit in 8 bits
// then write a goodbye message

int exitt(char exitcode[50]) {
    char delimiter[] = " ";
    char *firstWord, *exitnum, *remainder, *context;

    int inputLength = strlen(exitcode);
    char *inputCopy = (char*) calloc(inputLength + 1, sizeof(char));
    strncpy(inputCopy, exitcode, inputLength);

    firstWord = strtok_r(inputCopy, delimiter, &context);
    exitnum = strtok_r(NULL, delimiter, &context);

    if (exitnum != NULL) {
        int exitnumint = atoi(exitnum);  // converting the number from char to int
        printf("bye\n");
        exit(exitnumint);
    } else {
        printf("bye\n");
        exit(0);
    }
}


// MILESTONE 2

/*
run commands from a file one by one
*/

void fileread(char *filename){
    FILE* ptr;
    char str[50];
    ptr = fopen(filename, "a+");
    if (NULL == ptr) {
        printf("file can't be opened \n");
    }
    printf("content of this file are \n");

    while (fgets(str, 50, ptr) != NULL) {
        printf("%s", str);

        int length = strlen(str); // getting the length of the buffer text inputted
        char strcopy[length];        // creating a place to store a copy that can be altered

        strcpy(strcopy, str);    // storing the buffer into strcopy
        
        history(strcopy);            // calling the history function to update the history of commands
        
        char *token = strtok(strcopy, " "); 
        
        if (strcmp(token, "echo") == 0){           // comparing echo and the first word
            echoo(str);
        }
        else if (strcasecmp(token, "!!\n") == 0){
            bang();
        }
        else if (strcmp(token, "\n") == 0){                    // ignore empty input
            continue;
        }
        else if(strcmp(token, "exit") == 0){
            int output = exitt(str);
            printf("%d", output);
            //return output;
        }
        else{
            printf("bad command\n");
        }

        
    }
    fclose(ptr);
}

// MILESTONE 4
/*
1. Control-Z generates a SIGTSTP. This should not cause your shell to be suspended. Instead, it should cause your shell to suspend the processes in the current foreground job. If there is no foreground job, it should have no effect.

2. Control-C generates a SIGINT. This should not kill your shell. Instead it should cause your shell to kill the processes in the current foreground job. If there is no foreground job, it should have no effect.
*/

// void sigintHandler(){
//     received_sigint = true;
// }

// void enableSignalHandlers() {
//     struct sigaction sa;
//     sa.sa_handler = sigintHandler;
//     sa.sa_flags= 0;
//     sigemptyset(&sa.sa_mask);
//     sigaction(SIGINT, &sa, NULL);
// }

//void execute(char *buffer){}


int main() {
    // signal(SIGINT, SIG_IGN);        // ignore ctrl c
    // signal(SIGTSTP, SIG_IGN);       // ignore ctrl z
    char buffer[MAX_CMD_BUFFER];
    cmdhistory();
    
    while (1) {
        printf("icsh $ ");
        fgets(buffer, 255, stdin);
        //printf("you said: %s\n", buffer);
        int length = strlen(buffer); // getting the length of the buffer text inputted
        char strcopy[length];        // creating a place to store a copy that can be altered

        strcpy(strcopy, buffer);    // storing the buffer into strcopy
        
        history(strcopy);            // calling the history function to update the history of commands
        
        char *token = strtok(strcopy, " ");        // getting the first word in the string by using a space as the seperator
        //printf("%s", token);

        int found = 0;
        char *p = strrchr(token, '.');
        
        if (strcmp(token, "echo") == 0){           // comparing echo and the first word
            echoo(buffer);
        }
        else if (strcmp(token, "!!\n") == 0) {
          bang();
        }
        else if (strcmp(token, "\n") == 0){                    // ignore empty input
            continue;
        }
        else if(strcmp(token, "exit") == 0){
            int output = exitt(buffer);
            printf("%d", output);
            return output;
        }
        else if(p){
            found = strcmp(p, ".sh") == 0;
            if(found){
                fileread(token);
            }
        }
        else{ // as far as i understand forking isnt really necessary since this is in the else 
            int id = fork();
            if (id == 0){
                int ret = (system(buffer));
                if (ret == -1){
                    printf("bad command\n");
                }
            }
            if (id != 0){
                wait();
            }
        }
    }
}
