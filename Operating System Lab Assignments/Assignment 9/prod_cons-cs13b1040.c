#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <math.h>

sem_t mutex, full, empty;

//The input
int size, p, c, countP, countC, delP, delC;

//The buffer location counter
int count = 0;
int buffer[100];

//The avergae time delays.
double averageC, averageP;

//This function gets and returns the local time as a string.
void printTime()
{
	time_t now = time(0);
	struct tm timeS;
	char buffer[50];
	timeS = *localtime(&now);
	strftime(buffer, sizeof(buffer), "%X", &timeS);
	printf("%s", buffer);
}

//The time is distributed exponentially with respect to the time given in the input file.
int getTime(int x, int i)
{
	double ret, e;
	e = (-1.0) * i / 5;
	ret = x * exp(e);
	return (int) ret * 1000;
}

//The producer thread
void *producer(void *arg) 
{
	int *id=(int*)arg;
    	int i;
	char *c;

    	for (i = 0; i < countP; i++) 
	{
	
		if(i == 0)
			c = "st ";
		else if (i == 1)
			c = "nd ";
		else if (i == 2)
			c = "rd ";
		else
			c = "th ";

		struct timespec tstart={0,0}, tend={0,0};

		//indicating that the producer is waiting to produce
		clock_gettime(CLOCK_MONOTONIC, &tstart);

		//semaphore
		sem_wait(&empty);
		sem_wait(&mutex);

		//Incrementing memory location.
        	count++;
		int ind = (count - 1) % size;
		buffer[ind] = i;

		printf("%d %s item produced by thread %d at ", i+1, c, (*id));
		printTime();
		printf(" into buffer location %d.\n", count);	

		sem_post(&mutex);
		sem_post(&full);	

		//Get the time laspe in the semaphore.
		clock_gettime(CLOCK_MONOTONIC, &tend);
		double diff = ((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - 
          		((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec);
		averageP += diff;

		//Make the program wait for exponential time.
		int t = getTime(i+1, delP);
		sleep(t/1000);
    	}
 }


//The consumer thread.
void *consumer(void *arg) 
{
	int *id=(int*)arg;
	int i;
	char *c;

	for (i = 0; i < countC; i++) 
	{
		
		if(i == 0)
			c = "st ";
		else if (i == 1)
			c = "nd ";
		else if (i == 2)
			c = "rd ";
		else
			c = "th ";

		struct timespec tstart={0,0}, tend={0,0};

		//indicating that the consumer is waiting to consume.
		clock_gettime(CLOCK_MONOTONIC, &tstart);

		//Semaphore
        	sem_wait(&full);
		sem_wait(&mutex);

		//Decrementing the memory location.
		int get = buffer[count];
	        count--;

		printf("%d %s item consumed by thread %d at ", i+1, c, (*id));
		printTime();
		printf(" into buffer location %d.\n", count + 1);

		sem_post(&mutex);
		sem_post(&empty);
		
		//Get the time laspe in the semaphore.
		clock_gettime(CLOCK_MONOTONIC, &tend);
		double diff = ((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - 
          		((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec);
		averageC += diff;
		
		//Make the program wait for exponential time.
		int t = getTime(i+1, delC);
		sleep(t/1000);

    }
}

int main() 
{
	int i;

	//Taking input from a user text file.
	freopen("in.txt", "r", stdin);

	scanf("%d %d %d %d %d %d %d", &size, &p, &c, &countP, &countC, &delP, &delC); 

	//Creating the semaphores with the initial value's.
	sem_init(&mutex, 0, 1);
	sem_init(&full, 0, 0);
	sem_init(&empty, 0, size);
	
	pthread_t tidp[p], tidc[c];
	int argP[p], argC[c];

	//Creation of producer and consumer threads.
	for(i = 0; i < p; i++)
	{
		argP[i] = i +1;
        	pthread_create(&tidp[i], NULL, producer, &argP[i]);
	}
	for(i = 0; i < c; i++)
	{
		argC[i] = i +1;
        	pthread_create(&tidc[i], NULL, consumer, &argC[i]);
	}

	//Waiting for the consumer and producer threads.
	for(i = 0; i < p; i++)
	{
        	pthread_join(tidp[i], NULL);
	}
	for(i = 0; i < c; i++)
	{
        	pthread_join(tidc[i], NULL);
	}

	//Destroying the semaphores
	sem_destroy(&mutex);
	sem_destroy(&full);
	sem_destroy(&empty);

	printf("Average Producer time: %f\n", averageC/countC*1000);
	printf("Average Consumer time: %f\n", averageP/countP*1000);
	
	return 0;
}
