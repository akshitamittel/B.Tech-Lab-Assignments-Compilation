#include <atomic>
#include <iostream>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>
#include <mutex>

using namespace std;

//The atomic class library to implement swap and compare
atomic<bool> lock_cas;

//The mutex is required 
mutex display_mutex;

//The value of n, k, t1, and t2 are global variables.
int n,k,t1,t2;
double average;

//An array is used for bounded waiting. 
//It is used to denote which process is ready to be executed.
bool waiting[500];

//This function gets and returns the local time as a string.
string printTime()
{
	time_t now = time(0);
	struct tm timeS;
	char buffer[50];
	timeS = *localtime(&now);
	strftime(buffer, sizeof(buffer), "%X", &timeS);

	return buffer;
}

//We use mutexes to stop one thread from printing simultaneously with another when they are not in the critical section.
void atomic_display(int cs, string one, string two, string three, int id)
{
	display_mutex.lock();
	cout<< cs << one << two << printTime() << three << id << endl;
	display_mutex.unlock();
}

//The time is distributed exponentially with respect to the time given in the input file.
int getTime(int x, int i)
{
	double ret, e;
	e = (-1) * i / 5;
	ret = x * exp(e);
	return (int) ret * 1000;
}

void* testCS(void* vargp)
{
	//Convert the void* argument from the thread function to the appropriate data type.
	long itr = (long)vargp;
	int id = (int)itr;
	int i;
	bool key;
	string c;
	//Making k requests to the critical section per thread
	for(i = 0; i < k; i++)
	{
		if(i == 0)
			c = "st ";
		else if (i == 1)
			c = "nd ";
		else if (i == 2)
			c = "rd ";
		else
			c = "th ";

		//indicating that the request is sent
		time_t wait = time(0);
		atomic_display(i+1, c , "CS Request at ", " by thread ", id+1);

		//Bounded waiting to indicate that the thread is waiting to enter the CS.
		waiting[id] = true;
		key = true;
		while(waiting[id] && key)
		{
			//Waiting to obtain the key. Through the compare and swap mechanism.
			while(key)
			{
				key = false;
				lock_cas.compare_exchange_strong(key, true);
			}
			/* do nothing */
		}

		//Setting the condition after exit to indicate that it needs to wait no longer.
		waiting[id] = false;

		time_t start = time(0);
		double diff = start - wait;
		average += diff;
		atomic_display(i+1, c , "CS entered at ", " by thread ", id+1);
		//Stall the thread for an exponential time based on the average time provided by the user.
		usleep(getTime(t1, i));

		/* critical section */

		atomic_display(i+1, c , "CS exited at ", " by thread ", id+1);
		
		int j;
		//To check for the next waiting thread. It implements bounded waiting by ensuring that every thread gets checked for waiting once every round trip.
		j = (id + 1)%n;
		while((j != id) && !waiting[j])
			j = (j + 1)%n;

		//Lock = 0
		lock_cas.store(false, memory_order_relaxed);
		if(j != id)
			waiting[j] = false;

		/* Remainder section */

		//Time synonymous to the remainder section
		usleep(getTime(t2, i));
	}
	pthread_exit(NULL);
}

int main(){

	//Initializing the lock.
	lock_cas.store(false, memory_order_relaxed);

	//Taking input from a user text file.
	freopen("in.txt", "r", stdin);
	cin>>n>>k>>t1>>t2;
	
	pthread_t thread[n];
	int i, tc;

	//initializing the waiting conditions for all the threads.
	cout<<"Bounded Waiting Time lock output:"<<endl;
	for(i = 0; i < n; i++)
	{
		waiting[i] = false;
	}
	
	//Creating n threads and calling the critical section fuction for each.
	for(i=0; i<n; i++){
		tc = pthread_create(&thread[i],NULL,testCS,(void*)i);

		if(tc)
		{
			cout<<"Unable to create thread "<< i+1 << ". Exiting...";
			exit(-1);
		}
	}
	
	//Waiting for the n threads to complete and exit gracefully.
	for(i=0;i<n;i++){
		pthread_join(thread[i],NULL);
	}

	float wait = average / (n*k);
	cout<<"Performance: "<<wait<<endl;
	return 0;

}
