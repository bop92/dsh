// (╯°□°）╯ "HE'S GOING FOR IT! THE LONG DRIVE DOWN MID FIELD!"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/fcntl.h>

#define MAX_BUF 1024

struct cmd{
	int redirect_in;
	int redirect_out;
	int redirect_append;
	int background;
	int piping;
	char * infile;
	char * outfile;
	char * argv1[10];
	char * argv2[10];
};

int cmdscan(char * cmdbuf, struct cmd * com);

int main(){
	char buffer[MAX_BUF];
	int file, status, test;
	struct cmd COMMAND;
	int pid;
	int child[100];
	int count = 0;
	int i, fd[2], PD[2];
	int stdout = STDOUT_FILENO;
	int stdin = STDIN_FILENO;
	printf("$ :");
	while((gets(buffer) != NULL)){

	  if(cmdscan(buffer, &COMMAND)){
		printf("$ :");
		continue;
	  }
	  if(COMMAND.redirect_in){
		if((stdin = open(COMMAND.infile, O_RDONLY)) < 0){
			perror("Infile open error");
		}
	  }
	  if(COMMAND.redirect_out && COMMAND.redirect_append){
		if((stdout = open(COMMAND.outfile, 
O_WRONLY|O_APPEND|O_CREAT,0666)) < 0){
			perror("Outfile open error");
		}
	  }
	  if((COMMAND.redirect_out) && !(COMMAND.redirect_append)){
		if((stdout = open(COMMAND.outfile,
				O_WRONLY|O_CREAT|O_TRUNC,0666)) <0){
			perror("Outfile open error");
		}
	  }
	  if(strcmp(COMMAND.argv1[0],"exit") == 0){
		printf("exiting shell...\n");
		exit(0);
	  }
	  else
	  {
	     pipe(PD);
	     switch(child[count] = fork())
	     {
		   case -1:
			perror("fork error");
		   case 0:
			if(COMMAND.redirect_out)
			{dup2(PD[1], STDOUT_FILENO);}
			if(COMMAND.redirect_in)
			{dup2(stdin, STDIN_FILENO);}
			close(PD[1]);
			close(PD[0]);

		      if(COMMAND.piping)
		      {
			pipe(fd);
			switch(fork())
			{
		   	   case -1:
		     		perror("fork error");
		     	   	break;
		   	   case 0:
		     		dup2(fd[1], STDOUT_FILENO);
		     		close(fd[0]);
		     		close(fd[1]);
		     		execvp(COMMAND.argv1[0], COMMAND.argv1);
		     		perror("exec error");
		     		exit(-1);

		   	   default:
				dup2(fd[0], STDIN_FILENO);
				close(fd[1]);
				close(fd[0]);
				if(!(COMMAND.background))
				wait(&status);
				execvp(COMMAND.argv2[0], COMMAND.argv2);
				perror("exec error");
				exit(-1);
			}
		      }
		      else{
		    	     execvp(COMMAND.argv1[0], COMMAND.argv1);
			     perror("exec error");
			     exit(-1);
		      }
		   default:
			if(COMMAND.redirect_out){
				close(PD[1]);
		    		while((file = read(PD[0], buffer, sizeof(buffer))) != 0)
				   write(stdout, buffer, file);
			}
		}
	}

	if(COMMAND.redirect_out){
		close(stdout);
	}

	if(!(COMMAND.background)){
		waitpid(child[count], &status, 0);
	}
	if(waitpid(-1, &test, 1) < 0){
		for(i = 0; i <= count; i++){
			waitpid(child[count], &test, 0);
		}
		count = 0;
	}

	if(count == 99){
		count = 0;
	}
	count++;
	printf("$ :");

	}
	exit(0);
}
