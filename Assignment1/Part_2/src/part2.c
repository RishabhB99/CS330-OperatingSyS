//Name: Rishabh Bhutra
//Roll No.:170569

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc,char *argv[]){
	if(argv[1][0]=='@'){ //if first arg is '@'
		int pid,fd[2];
		
		if(pipe(fd) < 0){ //piping the file descripters
	        perror("pipe");
	        exit(-1);
	    }

	    char str[100];//string to be searched
	    char path[10000];//path
		strcpy(str,argv[2]);//copying string
		strcpy(path,argv[3]);//copying path
		
		pid=fork();//calling fork for the two processes to call two exec

		if(!pid)
		{
			dup2(fd[1],1);//setting stdout to fd[1]
			close(fd[0]);//closing fd[0]
			close(fd[1]);//closing fd[1]
			execl("/bin/grep","grep","-rF",str,path,NULL);//exec grep command
		}
		else
		{
			dup2(fd[0],0);//setting stdin to fd[0]
			close(fd[1]);//closing fd[1]
			close(fd[0]);//closing fd[0]
			execlp("wc","wc","-l",NULL);//exec wc cmd
		}
		
	}
	else if(argv[1][0]=='$')// if first arg is '$'
	{
		int pid,fd[2],fd2[2];
	
		if(pipe(fd) < 0){//piping file descripter for cmd 1 and 2
	        perror("pipe");
	        exit(-1);
	    }

	    if(pipe(fd2)<0){//piping file descripter for cmd 2 and 3
			perror("pipe");
			exit(-1);
		}

	    char str[100];
	    char path[10000];
		strcpy(str,argv[2]);//copying string
		strcpy(path,argv[3]);//copying path
		
		pid=fork();//fork for first two cmd
		fd[1]=open(argv[4],O_RDWR | O_TRUNC | O_CREAT,00777);//opening output.txt and if file doesn't exist create one with required permissions
		if(!pid)
		{
			dup2(fd[1],1);//setting stdout to fd[1] to set output to output.txt
			close(fd[0]);
			close(fd[1]);
			execl("/bin/grep","grep","-rF",str,path,NULL);//exec grep cmd	
		}
		else{
			wait(NULL);//waiting for child process to end
			fd2[0]=open(argv[4],O_RDONLY);//opening output.txt
			pid=fork();//forking for cmd 2,3
			if(!pid)
			{
				dup2(fd2[0],0);//setting stdin to fd[0]
				close(fd2[1]);
				close(fd2[0]);
				execlp(argv[5],argv[5],argv[6],argv[7],argv[8],NULL);//calling cmd 3 with at max 4 arguments
			}
			else{
				wait(NULL);//waiting for child process to end
				exit(0);//exit parent process
			}
		}	
	}
	return 0;
}