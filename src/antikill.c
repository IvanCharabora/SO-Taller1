#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char* argv[]) {
	int status;
	pid_t child;

	if (argc <= 1) {
		fprintf(stderr, "Uso: %s comando [argumentos ...]\n", argv[0]);
		exit(1);
	}

	/* Fork en dos procesos */
	child = fork();
	if (child == -1) { perror("ERROR fork"); return 1; }
	if (child == 0) {
		/* S'olo se ejecuta en el Hijo */
		ptrace(PTRACE_TRACEME, 0, NULL, NULL);
		execvp(argv[1], argv+1);
		/* Si vuelve de exec() hubo un error */
		perror("ERROR child exec(...)"); exit(1);
	} else {
		/* S'olo se ejecuta en el Padre */
		while(1) {
			if (wait(&status) < 0) { perror("waitpid"); break; }
			if (WIFEXITED(status)) break; /* Proceso terminado */

			int num = ptrace(PTRACE_PEEKUSER, child, 8*ORIG_RAX, NULL);
			printf(num);
			int err = ptrace(PTRACE_PEEKUSER, child, 8*RAX, NULL);
			printf(err);
			int killParam = ptrace(PTRACE_PEEKUSER, child, 8 * RSI, NULL);
			printf(killParam);
			if(num == 62 && killParam == 9 && err == ENOSYS){
				ptrace(PTRACE_KILL);
				printf("muerto");
				break;
			}
		}
	}
	return 0;
}
