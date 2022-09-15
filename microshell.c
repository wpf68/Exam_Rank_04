#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

/*not needed in exam, but necessary if you want to use this tester:
https://github.com/Glagan/42-exam-rank-04/blob/master/microshell/test.sh*/
#ifdef TEST_SH
# define TEST		1
#else
# define TEST		0
#endif

int	ft_print(char *a, char *b)
{
	int	i = -1;
	
	while (a[++i])
		write(2, &a[i], 1);
	i = -1;
	if (b)
		while (b[++i])
			write(2, &b[i], 1);
	write(2, "\n", 1);
	return (1);
}

int	ft_exec(char **ag, int i, int temp, char **env)
{
	ag[i] = NULL;
	dup2(temp, STDIN_FILENO);
	close(temp);
	execve(ag[0], ag, env);
	ft_print("error: cannot execute ", ag[0]);
	return (1);
}

int	main(int ar, char **ag, char **env)
{
	int	i = 0;
	int	pid = 0;
	int	fd[2];
	int	temp;
	(void)	ar;

	temp = dup(STDIN_FILENO);
	while (ag[i] && ag[i + 1])
	{
		ag = &ag[i + 1];
		i = 0;
		while (ag[i] && strcmp(ag[i], ";") && strcmp(ag[i], "|"))
			i++;
		if (strcmp(ag[0], "cd") == 0)
		{
			if (i != 2)
				ft_print("error: cd: bad arguments", NULL);
			else if (chdir(ag[1]))
				ft_print("error: cd: cannot change directory to ", ag[1]);
		}
		else if (i != 0 && (ag[i] == NULL || strcmp(ag[i], ";") == 0))
		{
			pid = fork();
			if (pid == -1)
				ft_print("error: fatal", NULL);
			if (pid == 0)
			{
				ft_exec(ag, i, temp, env);
				return (1);
			}
			else
			{
				close(temp);
			//	while (waitpid(-1, NULL, WUNTRACED) != -1);
			//	waitpid(pid, NULL, 0);
				waitpid(-1, NULL, WUNTRACED);
				temp = dup(STDIN_FILENO);
			}
		} 
		else if (i != 0 && strcmp(ag[i], "|") == 0)
		{
			if (pipe(fd) == -1)
				ft_print("error: fatal", NULL);
			pid = fork();
			if (pid == -1)
				ft_print("error: fatal", NULL);
			if (pid == 0)
			{
				dup2(fd[1], STDOUT_FILENO);
				close(fd[0]);
				close(fd[1]);
				ft_exec(ag, i, temp, env);
				return (1);
			}
			else
			{
				close(temp);
				close(fd[1]);
				temp = fd[0];
			}
		}
	}
	close(temp);

	if (TEST)		// not needed in exam, but necessary if you want to use this tester:
		while (1);	// https://github.com/Glagan/42-exam-rank-04/blob/master/microshell/test.sh

	return (0);
}
