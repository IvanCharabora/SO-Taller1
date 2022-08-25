#include <signal.h> /* constantes como SIGINT*/
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int signals = 0;

int sigintHandler(){
	wait(NULL);
	exit(0);
}

int sigurgHandler(){
	signals++;
	write(1, "ya va!\n", 7);
}

int main(int argc, char* argv[]) {
  	pid_t pid = fork();
	if(pid == 0){
		signal(SIGURG, sigurgHandler);
		while(signals < 5);
		kill(getppid(), SIGINT);
		execvp(argv[1], argv +1);
		
	} else {
		signal(SIGINT, sigintHandler);
		while(1){
			sleep(1);
			write(1, "sup!\n", 5);
			kill(pid, SIGURG);
		}
		
	}
	return 0;
}
























//Para ver que hace cada signal me fijo en el strace cuando se active "---""