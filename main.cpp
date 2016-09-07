#include <iostream>
#include <cstring>
#include <unistd.h>
#include <vector>
#include <sys/wait.h>
#include <cstdio>
#include <cstdlib>

using namespace std;

vector<char*> getPaths(char *paths){
    vector<char*> path;
    char* buff = strtok(paths, ":");
    path.push_back(buff);
    while(buff){
        buff = strtok(NULL, ":");
        path.push_back(buff);
    }

    return path;
}

void printPromt()
{
    char* USER;
    char* PWD;
    USER=getenv("USER"); PWD=get_current_dir_name();
    if(PWD[3]!='/') strcat(PWD,"/");
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
    params.push_back(NULL);
    return params;
}

void execCommand(vector<char*> params, char* PATH_ENV)
{
    if(params[0][0]=='c' && params[0][1]=='d'){
        if(params.size() != 3){
            if(params[1][0]=='/'){
                chdir(params[1]);
            }else{
                char* currPath = get_current_dir_name()+'/';
                strcat(currPath, params[1]);
                chdir(currPath);
            }
        }
    } else {
        int status, execStatus=-1, pathNumber=0;
        vector<char*> path;
        char *buff  = strtok(PATH_ENV, ":");
        path.push_back(buff);
        while(buff){
            buff = strtok(PATH_ENV, ":");
            path.push_back(buff);
        }
        char *command;
        switch(pid_t pid=fork()){
        case -1:
            cout<<"Error -1"<<endl;
        case 0:
            while(execStatus==-1 && pathNumber<path.size()){
                cout<<path[pathNumber]<<endl;
                command = path[pathNumber];
                strcat(command, "/");
                strcat(command, params[0]);
                cout<<command<<endl;
                execStatus = execve(command, &params[0], 0);
                cout<<execStatus<<endl;
                pathNumber++;
            }
        default:
            waitpid(-1, &status, 0);
        }
    }
}

int main(int argc, char *argv[], char *envp[])
{
    char* PATH_ENVIOREMENT = getenv("PATH");
    cout<<"Welcome to nsh!"<<endl<<PATH_ENVIOREMENT<<endl;
    while(true)
    {
        vector<char*> params;
        printPromt();


        string inputString;
        getline(cin, inputString);
        char cInputString[inputString.length()+1];
        strcpy(cInputString, inputString.c_str());

        params = parseCommand(cInputString);

        execCommand(params, PATH_ENVIOREMENT);
        inputString.clear();
        params.clear();
    }
}

