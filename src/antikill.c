#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h> /* constantes como SIGINT*/

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
		ptrace(PTRACE_TRACEME);
		execvp(argv[1], argv+1);
		/* Si vuelve de exec() hubo un error */
		perror("ERROR child exec(...)"); exit(1);
	} else {
		ptrace(PTRACE_ATTACH, child);
		/* S'olo se ejecuta en el Padre */
		while(1) {
			if (wait(&status) < 0) { perror("waitpid"); break; }
			if (WIFEXITED(status)) break; /* Proceso terminado */


			int num = ptrace(PTRACE_PEEKUSER, child, 8*ORIG_RAX, NULL); //QUE SYSCALL SE EJECUTA. https://chromium.googlesource.com/chromiumos/docs/+/master/constants/syscalls.md
			int err = ptrace(PTRACE_PEEKUSER, child, 8*RAX, NULL); //Devolverá un error (-ENOSYS) cuando el proceso se encuentre entrando en la syscall.
			int killParam = ptrace(PTRACE_PEEKUSER, child, 8 * RSI, NULL); //parámetro de la syscall (9)


			if(num == 62 && killParam == SIGKILL && err == -ENOSYS){//
				//kill(child,SIGKILL);//
				ptrace(PTRACE_KILL, child, NULL, NULL);
				printf("muertoooooooo %d \n", child);//
				break;//
			}

			ptrace(PTRACE_SYSCALL, child);//
		}
	}
	return 0;
}
