#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int main (void)
{
	int status;
	pid_t pid, childPid;

	/*Tạo tiến trình con bằng fork()*/

	childPid = fork();
	switch(childPid)
	{
		/*fork() trả về lỗi*/
		case -1:
			printf("Error: fork\n");
			return -1;
			
		/*Trong tiến trình con, return để kết thúc tiến trình con*/
		case 0:
			abort();
			
		/*Trong tiến trình cha, dùng wait() để theo dõi con*/
		default:
		pid = wait(&status);

		if (-1 == pid)
		{
			printf("Error, wait" );
		}
		/*In PID của tiến trình con khi wait() return*/
		printf ("pid=%d\n" , pid);

		if (WIFEXITED (status))
			printf ("Normal termination with exit status=%d\n", WEXITSTATUS (status));

		if (WIFSIGNALED (status))
			printf ("Killed by signal=%d%s\n" ,WTERMSIG (status), WCOREDUMP (status) ? " (dumped core)" : "" );

		if (WIFSTOPPED (status))
			printf ("Stopped by signal=%d\n", WSTOPSIG (status));

		if (WIFCONTINUED (status))
			printf ("Continued\n" );

	}
	return 0;
}