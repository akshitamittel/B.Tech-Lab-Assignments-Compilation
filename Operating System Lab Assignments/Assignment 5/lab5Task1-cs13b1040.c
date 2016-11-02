//A program to run two child processes simultaneously and use their resuts in the parent.
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

//Find a random number from 1 - max. 
//Keep iterating till you get a non-zero number.
int setRand(int n)
{
  int k = 0;
  while( k == 0)
  {
     k = rand()%n;
  }
  return k;
}

int main()
{
  const char* buffer = "buffer";
  //The two child processes
  pid_t child1, child2;
  int diff, max, num, shm_fd;
  int* mem;
  printf("Enter num: ");
  scanf("%d", &num);
  printf("Enter max: ");
  scanf("%d", &max);
  //Creating shared memeory space and allocating memory space.
  shm_fd = shm_open(buffer, O_RDWR | O_CREAT, 0666);
  ftruncate(shm_fd, (4 + 2*num)*sizeof(int));
  //Obtaining a pointer to that memory space.
  mem = (int*)mmap(0, (4 + 2*num)*sizeof(int),PROT_WRITE, MAP_SHARED,shm_fd, 0);
  //Store the maximum and number of value's in shared memory.
  mem[0] = max;
  mem[1] = num;
  /* The child1 process accesses the two variables from shared memory
   * It then stores all the random numbers in the shared memory along with the result. 
   */
  if(!(child1 = fork()))
  { 
    int prod = 1, i, term, x, shm_fd;
    srand(2468);
    x = 4 + mem[1];
    for(i = 0; i < mem[1]; i++)
    {
       term = setRand(mem[0]);
       prod *= term;
       mem[x] = term;
       x++;
    }
    mem[3] = prod;
    exit(0);
  }
  /* The child2 does a similar process
   * However the random number generator seed is different to get a different set of numbers.
   */
  else if(!(child2 = fork()))
  {
    int sum = 0, i, term, x = 4, shm_fd;
    srand(1357);
    for(i = 0; i < mem[1]; i++)
    {
       term = setRand(mem[0]);
       sum += term;
       mem[x] = term;
       x++;
    }
    mem[2] = sum;
    exit(0);
  }
  else
  /* The parent process waits for both children to terminate.
   * It then acceses the data stored in the shared memory to compute the difference.
   * Then it prints out all the information according to the specified format.
   */
  {
    int i,x;
    wait(&child2);
    wait(&child1);
    diff = mem[3] - mem[2];
    printf("Output of c1: ");
    x = 4 + mem[1];
    for(i = 0; i < mem[1]; i++)
    {
       printf("%d ", mem[x]);
       x++;
    }
    printf("prod: %d\n", mem[3]);
    printf("Output of c2: ");
    x = 4;
    for(i = 0; i < mem[1]; i++)
    {
       printf("%d ", mem[x]);
       x++;
    }
    printf("sum: %d\n", mem[2]);
  } 
  printf("Output of p: %d\n", diff);
  shm_unlink(buffer); 
  return 0;
}
