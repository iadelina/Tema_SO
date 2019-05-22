#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>


int parentChild[2]; //(0 e capat de citire, 1 e capat de scriere)
int childParent[2];
pid_t child;
unsigned long poz = 0, seconds = 0, totalChar = 0, charNow = 0, charPerSeconds[5]; 

void ToPrint(int sig) 
{
	int index = 0;
	if(seconds == 5)
	{
		kill(child, SIGUSR2);
		read(childParent[0], &totalChar, sizeof(unsigned long));
		printf("totalChar: %ld\n", totalChar);
		while (read(childParent[0], &charPerSeconds[index], sizeof(unsigned long)) > 0)  
        	{  
			printf("charPerSeconds: %d: %ld\n", index+1 , charPerSeconds[index]);
			index++;
		}
		printf("\n");
		close(childParent[0]);
		exit(0);
	} 
	else 
	{
		kill(child, SIGUSR1); 
		seconds++;
		alarm(1); 
	}
}

void StatisticToPrint(int sig)
{
	int index;
	write(childParent[1], &totalChar, sizeof(unsigned long));
	for (index = 0; index < 5; index++)
	{
		write(childParent[1], &charPerSeconds[index], sizeof(unsigned long));
	}
	close(childParent[1]);
	exit(0);
}

void ChildStatistic(int sig)
{	
	printf(" a, SIGUSR1\n");
	printf("********************\n");
	charPerSeconds[poz] = charNow;        
	totalChar += charNow;
	charNow = 0;
	poz += 1;
}

int main(void)
{
	char buf;
	int i;

	pipe(childParent);
	pipe(parentChild);
	child = fork();

	if (!child)  //ne aflam in fiu
   	{
		close(parentChild[1]); 
		close(childParent[0]);
		signal(SIGUSR1, ChildStatistic);
		signal(SIGUSR2, StatisticToPrint);
		
		while(read(parentChild[0], &buf, 1) > 0) //citeste din parinte_fiu in buffer cate un caracter
                {
			charNow++;
		}
		exit(0);
	} 
	else //ne aflam in parinte
        {
		close(parentChild[0]); 
		close(childParent[1]);
		signal(SIGALRM, ToPrint);		
		alarm(1); //in cat timp sa se dea semnalul SIGALRM (1 secunda), prima setare
		while(1)
		{
			write(parentChild[1], "a", 1);  
		}
	}
	return 0;
}
