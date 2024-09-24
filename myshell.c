#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<string.h>
#include<fcntl.h>

#define TRUE 1

void parseCommand(char command[], char **args, char *isRedirect, char **out, int *isPipe, char **pipeArgs)
{
  *out = NULL;
  *isRedirect=0;
  *isPipe=0;
  char *token = strtok(command, " ");
  int i = 0;
  while(token != NULL)
  {
    if(strcmp(token, ">") == 0)
    {
      token = strtok(NULL, " ");
      *isRedirect = 1;
      *out = token;
      if(strtok(NULL, " ")==NULL)
        break;
    }
    else if(strcmp(token, "|") == 0)
    {
      *isPipe = 1;
      args[i] = NULL;
      int j = 0;
      token = strtok(NULL, " ");
      while(token != NULL)
      {
        if(strcmp(token, ">")==0)
        {
          token = strtok(NULL, " ");
          *isRedirect=1;
          *out=token;
          break;
        }
        pipeArgs[j] = token;
        j++;
        token = strtok(NULL, " ");
      }
      pipeArgs[j] = NULL;
      return;
    }
    else
    {
      args[i] = token;
      i++;
      token = strtok(NULL, " ");
    }
  }
  args[i] = NULL;
}

void handleRedirection(char *out)
{
      int fd = creat(out, 0644);
      if(fd < 0 )
      {
        printf("Error opening file...\n");
        exit(1);
      }
      printf("Output redirected to %s\n", out);
      dup2(fd, 1);
      close(fd);
}

void execute(char **tokens, int isRedirect, char *out)
{
  char *pathEnv = getenv("PATH");
  char path[1024];
  char copy[1024];
  strcpy(copy, pathEnv);
  char *token = strtok(copy, ":");
  struct stat stats;
  
  if(pathEnv == NULL)
  {
    printf("PATH environment variable not found\n");
  }
  pid_t pid = fork();
  if(pid == 0)
  { 
    if (isRedirect == 1)
        handleRedirection(out);
    char file[1024];
    int found;
    while(token != NULL)
    {
        found=0;
        strncpy(path, token, sizeof(path) - 1);
        path[sizeof(path) - 1] = '\0';
        strcat(path, "/");
        strcat(path, tokens[0]);
        if(stat(path, &stats)==0){
          strncpy(file, path, 1024);
          execv(file, tokens);
          found=1;
        }
      token = strtok(NULL, ":");
    }
    if(found == 0)
    {
      printf("Command not found\n");
      exit(1);
    }
  }
  else
  {
    wait(NULL);
  }
}

void handlePiping(char **tokens, char**pipeTokens, int isRedirect, char *out)
{
  int fd[2];
  
  pipe(fd);
  
  pid_t pid1, pid2; 
  pid1 = fork();
  if(pid1 == 0)
  {
    close(fd[0]);
    dup2(fd[1], 1);
    close(fd[1]);
    execute(tokens, 0, NULL);
    exit(1);
  }
  else if(pid1 > 0)
  {
    pid2=fork();
    if(pid2 == 0)
    {
      close(fd[1]);
      dup2(fd[0], 0);
      close(fd[0]);
      if(isRedirect)
      {
        handleRedirection(out);
      }
      execute(pipeTokens, 0, NULL); 
      exit(1);
    }
    else if(pid2>0)
    {
      close(fd[0]);
      close(fd[1]);
      wait(NULL);
      wait(NULL);
    }
  }
}

int main()
{

  char *tokens[100];
  int isPipe = 0;
  char isRedirect = 0;
  char *out;
  char *pipeTokens[100];

  while(TRUE)
  {
    char command[1024];
    printf("[Enter Command]> ");
    fgets(command, sizeof(command), stdin); 
    command[strcspn(command, "\n")] = 0;
    if(strcmp(command, "exit")==0)
      exit(0);
    parseCommand(command, tokens, &isRedirect, &out, &isPipe, pipeTokens);
    if(isPipe == 1)
      handlePiping(tokens, pipeTokens, isRedirect, out);
    else
      execute(tokens, isRedirect, out);
    }

  return 0;
}
