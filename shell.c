#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define DLMTARGS " \n"

char **processaLinha(char *linha, char *dlmtARGS)
{
	int aux = 0;
	int count = 100;
	char **resultado = malloc(count * sizeof(char *));

	char *str = strtok(linha, dlmtARGS);
	while (str != NULL)
	{
		resultado[aux] = str;
		aux++;

		if (aux >= count)
		{
			count += 100;
			resultado = realloc(resultado, count * sizeof(char *));
		}

		str = strtok(NULL, dlmtARGS);
	}

	resultado[aux] = NULL;
	return resultado;
}

char *shellComandos[] = {
	"cd",
	"exit"
};

struct comando
{
	char **args;
	int index;
};

struct listaComandos
{
	struct comando *cmd;
	int count;
};

struct listaComandos *processaLinhaLida(char *linha){
	int aux = 0; 
	int count = 1;
	struct listaComandos *resultado = malloc(sizeof(struct listaComandos));
	resultado->cmd = malloc(sizeof(struct listaComandos));

	char **linhaCmd = processaLinha(linha, "&");

	while(linhaCmd[aux] != NULL){
		resultado->cmd[aux].index = aux;
		resultado->cmd[aux].args = processaLinha(linhaCmd[aux], DLMTARGS);
		aux++;
		resultado->cmd = realloc(resultado->cmd, (aux+1)*sizeof(struct listaComandos));
		resultado->count = aux;
	}

	return resultado;
}

int shellCd(char **args)
{
	if (args[1] == NULL || chdir(args[1]) != 0)
	{
		printf("CD falhou. Verifique o comando e tente novamente...\n");
	}
	return 0;
}

int shellExit(char **args)
{
	return -1;
}

int verificaShellComandos(char **args)
{
	int index = -1;
	int count = sizeof(shellComandos) / sizeof(char *);
	for (int i = 0; i < count; i++)
	{
		if (strcasecmp(args[0], shellComandos[i]) == 0)
		{
			index = i;
		}
	}

	switch (index)
	{
	case 0:
		return shellCd(args);
		break;
	case 1:
		return shellExit(args);
		break;
	default:
		return 1;
		break;
	}
}


int main()
{
	char buffer[1024];
	int pid = 0;
  int aux = 0;
	int exec = 1;
  int status = 0;
	char **args;
	char prompt[7] = "Shell-$\0";

	struct listaComandos *listaComandosLidos = malloc(sizeof(struct listaComandos));
	struct comando cmd;

	listaComandosLidos->count = 0;

	while (exec >= 0)
	{
		if(aux >= listaComandosLidos->count){
			printf("%s", prompt);
			fgets(buffer, 1024, stdin);
			listaComandosLidos = processaLinhaLida(buffer);
			aux = 0;
		}

		cmd = (struct comando)listaComandosLidos->cmd[aux];

		if(cmd.args[0] != NULL){
			exec = verificaShellComandos(cmd.args);
		}else{
			exec = 0;
		}
		
		if (exec == 1)
		{
			pid = fork();
			if(pid == -1){
				printf("O processo de forking falhou...\n");
        return 0;
			}else{
				if (pid == 0)
				{
					execvp(cmd.args[0], cmd.args);
					return 0;
				}
				else
				{
					if(cmd.index >= (listaComandosLidos->count -1)){
						while (wait(&status) != pid);
					}									
				}
			}
		}
		aux++;
	}
	return 0;
}
