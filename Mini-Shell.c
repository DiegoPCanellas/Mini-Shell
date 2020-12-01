#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

void get_cmd() {

	/* ------------- LE COMANDOS NA SHELL ----------------*/
	char line[1024], aux[1024];
	char *cmd1, *cmd2;
	int count = 0;
	int fd[2], pid, status;

	for (;; ) { //le input
		int c = fgetc(stdin);
		line[count++] = (char)c;
		if (c == '\n') break;
	}

	cmd1 = strtok(line, "|"); // primeiro comando
	cmd2 = strtok(NULL, "\n"); // segundo comando

	if (cmd2 == NULL) { //caso seja passado apenas 1 comando
		cmd1 = strtok(line, "\n"); //retira \n
		pid = fork();
		if (pid == 0) {
			execlp(cmd1, cmd1, (char *)NULL);//executa unico comando
		}
		waitpid(pid, &status, 0);
	}

	/*------- EXECUTA COMANDOS --------*/
	pipe(fd);
	pid = fork();
	if (pid == 0) {
		dup2(fd[1], 1); //faz output redirecionar para entrada da pipe
		close(fd[1]);
		close(fd[0]);
		execlp(cmd1, cmd1, (char *)NULL); //executa primeiro comando
		exit(1);
	}
	else {
		pid = fork();
		if (pid == 0) {
			dup2(fd[0], 0); //faz input redirecionar para saida da pipe
			close(fd[1]);
			close(fd[0]);
			execlp(cmd2, cmd2, (char *)NULL); //executa segundo comando
			exit(1);
		}
		else {
			close(fd[1]);
			close(fd[0]);
			waitpid(pid, &status, 0);
		}
	}

	return;
}

int main(void) {
	while (1) {
		printf("$");
		get_cmd();
	}
	return 0;
}