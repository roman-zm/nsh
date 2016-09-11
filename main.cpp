#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <unistd.h>
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

void execCommand(vector<char*> params, vector<string> commands)
{
    if(params[0][0]=='c' && params[0][1]=='d'){
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
                execStatus = execve(commands[pathNumber].c_str() , &params[0], 0);
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
        execCommand(params, commands);

    }
}

