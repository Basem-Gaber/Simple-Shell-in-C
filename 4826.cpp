#include <iostream>
#include<bits/stdc++.h>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <fstream>

using namespace std;

void run();
int executeCommand(char * str);

ofstream outfile ("log.txt"); //logfile
int main()
{
    while(true){ //infintie program loop
        run();
    }
    return 0;
}

//main program function that reads input from the user and runs forever
void run(){
    char * userInput = (char*)malloc(100*sizeof(char));
    char cwd[100];
    if(getcwd(cwd,sizeof(cwd)) == NULL){
        perror("Couldn't get current active directory.");
    }
    cout << cwd;
    cout << ":$";
    cin.getline(userInput,100);
    if(strcmp(userInput, "exit") == 0 ){
        exit(1);
        outfile.close();
    }else{
        executeCommand(userInput);
    }
}
void signalHandler(int foo){ //SIGCHLD handler
    outfile << "Child process has terminated.\n";
}

int executeCommand(char * str){
    bool asyncFlag = false;//flag that tells i cae of no wait
    char * cmndCopy = (char *)malloc(100*sizeof(char));
    strcpy(cmndCopy,str);//string copy of the command for later use
    char ** words= (char **)malloc(5*sizeof(char));
    for(int i=0; i< 5 ; i++){
        words[i]= (char *)malloc(100*sizeof(char));
    }
    int i =-1;
    do{//splitting the user input edged by whitespaces to formulate the execvp argument
        i++;
        words[i] = strsep(&str , " ");
    }while(words[i] !=NULL);
    if(strcmp(words[i-1] , "&") == 0){
        asyncFlag =true;//in cae of ampersand we work async
        words[i-1] = NULL;
    }
    *(words+i+1) == NULL;
    // Forking a child
    pid_t pid = fork();
    int status;//used in the wait function
    if (pid == -1) {
        printf("\nFailed forking child..");
        return -1;
    } else if (pid == 0) {//inside the child process
        if (execvp(words[0], words) < 0) {
        //execute the Linux command the user inputed or declare an error at wrong command and kill the process
            printf("\nCommand %s not found\n", cmndCopy);
            exit(1);
        }
    } else {//inside the parent process we wait in case of sync
        signal(SIGCHLD, signalHandler);//acting upon interrupt of child termination

        // waiting for child to terminate in case of synchronous processes
        if(asyncFlag != true){
            asyncFlag = false;
            while(wait(&status) != pid){//waiting until the child terminates; blocks any new input from the user
                printf(".");
            };
        }

        return 1;
    }
}
