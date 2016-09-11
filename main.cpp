#define STD_INPUT 0
#define STD_OUTPUT 1

#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <sys/wait.h>
#include <cstdio>
#include <cstdlib>

using namespace std;

void printPromt()
{
    char* USER;
    char* PWD;
    USER=getenv("USER"); PWD=get_current_dir_name();
    printf("%s@%s:$ ", USER, PWD);
    setenv("PWD", PWD, 1);
    setenv("USER", USER, 1);
}

vector<char*> parseCommand(char* input)
{
    vector<char*> params;
    char* buff =  strtok(input," ");
    params.push_back(buff);
    while(buff){
        buff = strtok(NULL, " ");
        params.push_back(buff);
    }
    return params;
}

void execCommand(vector<char*> params, vector<string> commands, char* envp[])
{
    if(!(strncmp("cd", params[0], strlen(params[0])))){
        if(params.size() != 2){
            if(params[1][0]=='/'){
                chdir(params[1]);
            }else{
                char currPath[4096];
                strcpy(currPath, get_current_dir_name());
                strcat(currPath, "/");
                strcat(currPath, params[1]);
                chdir(currPath);
            }
        }

    } else if(!(strncmp("setenv", params[0], strlen(params[0])))){
        char* envName = strtok(params[1], "=");
        char* envValue = strtok(NULL, "=");
        setenv(envName, envValue, 1);

    } else if(!(strncmp("getenv", params[0], strlen(params[0])))) {
        printf("%s\n", getenv(params[1]));
    } else if(!(strncmp("exit", params[0], strlen(params[0])))){
        exit(0);
    } else {
        int status, execStatus=-1;
        size_t pathNumber=0;
        pid_t pid;

        switch(pid=fork()){
        case -1:
            printf("Error -1\n");
            break;
        case 0:
            printf("%i", execStatus);
            do{
                execStatus = execve(commands[pathNumber].c_str() , &params[0], envp);
                pathNumber++;
            }while(execStatus==-1 && pathNumber<commands.size());
            break;
        default:
            waitpid(-1, &status, 0);
            break;
        }
    }
}

vector<string> getCommand(vector<char*> params){
    char* CPATH=getenv("PATH");
    string PATH(CPATH);
    istringstream iss(PATH);
    vector<string> commands;
    string buffer;
    while(getline(iss, buffer, ':')){
        buffer+="/";
        buffer+=params[0];
        commands.push_back(buffer);
    }
    return commands;
}

bool includes(vector<char*> params, char* delim){
    for(size_t i=0; i<params.size(); i++){
        if(params[i] ==NULL) return false;
        if(!strncmp(params[i], delim, sizeof(params[i]))) return true;
    }
    return false;
}

void execPipe(vector<char*> params)
{
    int execStatus =-1;
    size_t pathNumber=0;
    vector<char*> params1; vector<char*> params2;

    for(int i=0; params[i][0] != '|'; i++){
        if(params[i][0]!='|')params1.push_back(params[i]);
    }
    params1.push_back(NULL);
    for(size_t i=params1.size(); i<params.size(); i++){
        params2.push_back(params[i]);
    }

    vector<string> commands1 = getCommand(params1);
    vector<string> commands2 = getCommand(params2);

    int fd[2];
    pipe((&fd[0]));
    if(fork()){
        execStatus=-1;
        pathNumber=0;
        close(fd[0]);
        close(STD_OUTPUT);
        dup(fd[1]);
        close(fd[1]);
        do{
            execStatus = execve(commands1[pathNumber].c_str() , &params1[0], 0);
            pathNumber++;
        }while(execStatus==-1 && pathNumber<commands1.size());
    } else {
        execStatus=-1;
        pathNumber=0;
        close(fd[1]);
        close(STD_INPUT);
        dup(fd[0]);
        close(fd[0]);
        do{
            execStatus = execve(commands2[pathNumber].c_str() , &params2[0], 0);
            pathNumber++;
        }while(execStatus==-1 && pathNumber<commands2.size());
    }


}

int main(int argc, char *argv[], char *envp[])
{
    cout<<"Welcome to nsh!"<<endl;
    while(true)
    {
        vector<char*> params;
        printPromt();

        string input;
        getline(cin, input);
        char* inputString = (char*)input.c_str();

        params = parseCommand(inputString);
        vector<string> commands = getCommand(params);
        if(params[0]!=NULL){
            if(includes(params, "|")) execPipe(params);
            else execCommand(params, commands, envp);
        }
    }
}

