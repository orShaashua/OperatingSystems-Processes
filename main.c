/**
 * NAME: Or Shaashua
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>

#define PROMPT "prompt> "
#define INPUT_SIZE 1000
#define QUOTATION_MARKS '"'
#define FAIL (-1)
#define HOME "HOME"
#define JOBS_NUM 500
#define SIGN_BACKGROUND "&"
#define TILDA "~"
#define DOUBLE_POINT ".."
#define HYPHEN "-"

//jobs struct
typedef struct jobs_struct{
    pid_t pid;        // PID of the job
    char Command[INPUT_SIZE]; //The command
}jobs_struct;

//boolean
typedef enum bool {false=0, true=1} bool;

//function declaration
void run();
bool execute(char input[], char* tokens[],jobs_struct jobs[], int* counterOfJobs);
bool printListOfActivateJobs(jobs_struct jobs[], int* counterOfJobs);
bool cdCommand(char* tokens[]);
void checkIfBackground(char input[],char* tokens[], bool* isBackground);
bool systemCall(char input[],char *tokens[],jobs_struct jobs[] ,bool isBackground, int* counterOfJobs);
void getArgs(char input[],char* tokens[]);



int main() {
    run();
    return 0;
}

void run(){

    bool status = true;
    char *tokens[INPUT_SIZE];
    char input[INPUT_SIZE], inputCopy[INPUT_SIZE];
    jobs_struct jobs[JOBS_NUM];
    int counterOfJobs = 0;
    int* counterOfJobs_P = & counterOfJobs;
    while(status){

        printf(PROMPT);
        fgets(input, INPUT_SIZE, stdin);
        input[strlen(input) - 1] = '\0';
        strcpy(inputCopy, input);
        getArgs(inputCopy,tokens);
        status = execute(input,tokens,jobs,counterOfJobs_P);
    }
}

/**
 * This function divides the user input into an array
 * of arguments space-separated (tokens) unless it close in quotation marks.
 * @param input - The input of the user.
 * @param tokens - Array of all the args in the user input separated by a space.
 */
void getArgs(char input[], char* tokens[]){
    char *p, *start_of_word;
    int c;
    enum states { DULL, IN_WORD, IN_STRING } state = DULL;
    size_t argc = 0;

    for (p = input; argc < strlen(input) && *p != '\0'; p++) {
        c = (unsigned char) *p;
        switch (state) {
            case DULL:
                if (isspace(c)) {
                    continue;
                }

                if (c == QUOTATION_MARKS) {
                    state = IN_STRING;
                    start_of_word = p + 1;
                    continue;
                }
                state = IN_WORD;
                start_of_word = p;
                continue;

            case IN_STRING:
                if (c == QUOTATION_MARKS) {
                    *p = 0;
                    tokens[argc++] = start_of_word;
                    state = DULL;
                }
                continue;

            case IN_WORD:
                if (isspace(c)) {
                    *p = 0;
                    tokens[argc++] = start_of_word;
                    state = DULL;
                }
                continue;
        }
    }

    if (state != DULL && argc < strlen(input)) {
        tokens[argc++] = start_of_word;
    }
    tokens[argc]=NULL;
}

/**
 * This function execute the command depend of command type.
 * @param input - The input of the user.
 * @param tokens - Array of all the args in the user input separated by a space.
 * @param jobs - Array of jobs (structure that contains the command and the pid).
 * @param counterOfJobs - The numbers of jobs.
 * @return the status (true or false).
 */
bool execute(char input[], char* tokens[],jobs_struct jobs[], int* counterOfJobs){

    bool isBackground = false;
    checkIfBackground(input, tokens, &isBackground);
    if (tokens[0]==NULL){ // in case of empty input
        return true;
    }else if (strcmp(tokens[0] ,"jobs") == 0){
        return printListOfActivateJobs(jobs, counterOfJobs);
    }else if (strcmp(tokens[0] ,"cd") == 0){
        return cdCommand(tokens);
    }else if (strcmp(tokens[0] ,"exit")==0){
		//print id of calling process
		printf("%d \n", (int)getpid());
        exit(0);
    }else {
        return systemCall(input, tokens, jobs, isBackground, counterOfJobs);
    }
}

/**
 * This function check if the command need to execute in background if
 * so isBackground will get the value true.
 * @param input - The input of the user.
 * @param tokens - Array of all the args in the user input separated by a space.
 * @param isBackground -if the input contain '&' so isBackground will point to true
 * else will point to false.
 */
void checkIfBackground(char input [], char* tokens[], bool* isBackground){
    int position = 0;
    while(tokens[position] != NULL){
        position++;
    }
    if (position!=0 && strcmp(tokens[position-1], SIGN_BACKGROUND) == 0){
        tokens[position-1]=NULL;
        int size=strlen(input);
        input[size-1]='\0';
        *isBackground = true;
    }

}

/**
 * This function print all the activate jobs.
 * @param jobs - Array of jobs (structure that contains the command and the pid).
 * @param counterOfJobs  - The numbers of jobs.
 * @return  the status (true or false).
 */
bool printListOfActivateJobs(jobs_struct jobs[], int * counterOfJobs){
    int i;
    for (i = 0;i < *counterOfJobs;i++){
        pid_t returnPid = waitpid(jobs[i].pid, NULL, WNOHANG);
        if (returnPid == 0) {
            printf("%d %s\n", (int) jobs[i].pid, jobs[i].Command);
        }
    }
    return true;
};


/**
 * This function execute the cd command.
 * @param tokens -Array of all the args in the user input separated by a space.
 * @return the status (true or false).
 */
bool cdCommand(char* tokens[]){
    //print id of calling process
    printf("%d \n", (int)getpid());
    //return to home directory
    if (tokens[1] == NULL ||strcmp(tokens[1],TILDA) == 0){
        chdir(getenv(HOME));

    }//return to previous directory
    else if(strcmp(tokens[1],HYPHEN) == 0){
        chdir(DOUBLE_POINT);
    }else {
        if (chdir(tokens[1]) == FAIL) {
            fprintf(stderr, "Error in system call\n");
        }
    }
    return true;
}



/**
 * This function will execute the system call.
 * @param input - The input of the user.
 * @param tokens - Array of all the args in the user input separated by a space.
 * @param jobs - Array of jobs (structure that contains the command and the pid).
 * @param counterOfJobs - The numbers of jobs.
 * @param isBackground -if the input contain '&' so isBackground will point to true
 * else will point to false.
 * @return the status (true or false).
 */
bool systemCall(char input[], char *tokens[], jobs_struct jobs[],
                bool isBackground, int* counterOfJobs) {
    int status;
    pid_t pid = fork();

    if (pid == 0) {
        // Son
        execvp(tokens[0], tokens);
        // can only get here on error
        fprintf(stderr, "Error in system call\n");
        return false;
    } else if (pid > 0) {
        // Father
        printf("%d \n", pid);
        if (isBackground) {
            strcpy(jobs[*counterOfJobs].Command,input);
            jobs[*counterOfJobs].pid = pid;
            (*counterOfJobs)++;
        }else {
            //Wait for a specific child process to end
            waitpid(pid, &status, 0);
        }
    }else {
        //error
        fprintf(stderr, "Error in system call\n");
        return false;
    }
    return true;
}
