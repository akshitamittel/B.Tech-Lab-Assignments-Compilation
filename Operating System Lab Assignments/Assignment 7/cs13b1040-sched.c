#define _GNU_SOURCE

#include <sched.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

//A function to set the scheduler and call the process.
void scheduler(char* x, int sched, int prior)
{
	printf("%s %d %d\n", x, sched, prior);
	//Parameter for set affinity
	cpu_set_t set;
	struct sched_param params;

	//The priority ranges for different policies
	int priority;
	if(prior == 1)
	{
		priority = sched_get_priority_max(SCHED_FIFO);
	}
	else if(prior == 2)
	{
		priority = sched_get_priority_min(SCHED_FIFO);
	}
	else
	{
		priority = sched_get_priority_min(SCHED_OTHER);
	}

	//printing the scheduler state before modification
	printf("STATS BEFORE SETTING SCHEDULER:\n");
	int get =  sched_getscheduler(getpid());
	if(get == 0)
		printf("Policy: SCHED_OTHER\n");
	else if(get == 1)
		printf("Policy: SCHED_FIFO\n");
	else
		printf("Policy: SCHED_RR\n");

	//Setting Scheduler Affinity
	CPU_ZERO(&set);
	CPU_SET(0, &set);

	if (sched_setaffinity(getpid(), sizeof(set), &set) == -1)
		printf("ERROR: sched_setaffinity");
	else
		printf("Scheduler affinity set!\n");

		
	//Setting the schelduler according to the policy
	if(sched == 1)
	{
		if(sched_setscheduler(0, SCHED_FIFO, (struct sched_param*)&priority)== -1)
			printf("Scheduling error!\n");
		else
			printf("Scheduled!\n");
	}
	else if(sched == 2)
	{
		if(sched_setscheduler(0, SCHED_RR, (struct sched_param*)&priority)== -1)
			printf("Scheduling error!\n");
		else
			printf("Scheduled!\n");
	}
	else 
	{
		if(sched_setscheduler(0, SCHED_FIFO, (struct sched_param*)&priority)== -1)
			printf("Scheduling error!\n");
		else
			printf("Scheduled!\n");
	}


	//Printing the scheduler state after modification
	printf("STATS AFTER SETTING SCHEDULER:\n");
	get =  sched_getscheduler(getpid());
	if(get == 0)
		printf("Policy: SCHED_OTHER\n");
	else if(get == 1)
		printf("Policy: SCHED_FIFO\n");
	else
		printf("Policy: SCHED_RR\n");
	printf("Priority: %d\n", priority); 

	//Calling exec to function
	execlp("./longrun", "", x, "10000", "10000", NULL);

}

int main(int argc, char *argv[]) 
{
	int i, processes, sched, prior;
	freopen(argv[1],"r",stdin);
	//Get the input from user
	scanf("%d", &processes);
	for(i = 0; i < processes; i++)
	{	
		scanf("%d %d", &sched,&prior);
		char x[4]; 
		sprintf(x, "%d", i);
		pid_t child = fork();

		if(child < 0){
			perror("Could not fork child!\n");
			exit(-1);
		}else if(child == 0){ 
			scheduler(x, sched, prior);
		}
	}
	return 0;
}

