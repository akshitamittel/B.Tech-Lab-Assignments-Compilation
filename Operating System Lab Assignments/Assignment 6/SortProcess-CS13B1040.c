//A program to run multiple processes and execute different parts of programs within them.
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

int main()
{
  const char* buffer = "buffer";

  //The current clock cycle is stored in the begin variable
  clock_t begin, end;
    double timeTaken;
    begin = clock();

  //The three child processes
  pid_t child1, child2, child3;
  int num, shm_fd, itr, inp;
  int* mem;

  //Inputting the array size from the user
  printf("Enter the number of entries (100 - 1000): ");
  scanf("%d", &num);

  //Creating shared memeory space and allocating memory space.
  shm_fd = shm_open(buffer, O_RDWR | O_CREAT, 0666);
  ftruncate(shm_fd, (1 + 3*num)*sizeof(int));
  //Obtaining a pointer to that memory space.
  mem = (int*)mmap(0, (1 + 3*num)*sizeof(int),PROT_WRITE, MAP_SHARED,shm_fd, 0);

  //Store the number of value's in shared memory.
  mem[0] = num;

  /* Start the random number generator using srand (give the seed);
   * Generate a random number between 0 - 100.
   * Store that random number in the shared memory, corresponding to the sorted and unsorted array.
   */
  srand(2468);
  for(itr = 1; itr <= num; itr++)
  {
    inp = rand()%100;
    //sorted array
    mem[itr] = inp;
    //unsorted array
    mem[itr + num] = inp;
  }

  /* Insertion sort is used in the first child.
   * The first half of the array is sorted.
   * Each element is taken in the memory iteratively and placed in the nearest acsending order.
   * No return value required.
   */
  if(!(child1 = fork()))
  { 
    int i, j, temp, end;
    end = mem[0] / 2;

   //Insertion Sort:
    for(i = 2; i < end + 1; i++)
    {
      temp = mem[i];
      j = i-1;
      while(temp < mem[j] &&  j >= 1 )
      {
        mem[j+1] = mem[j];
        j -= 1;
      }
      mem[j+1] = temp;
    }
    
    exit(0);
  }
  /* Bubble sort is used in the second child.
   * The second half of the array is sorted.
   * Each element is taken in the memory iteratively swapped with its neighbour til ascending order is reached.
   * No return value required.
   */
  else if(!(child2 = fork()))
  {
    int i, j, temp, beg, itr;
    beg = num / 2;
  
    //Bubble sort:
    for(i = beg + 1; i < num; i++)
    {
      for(j = beg + 1; j < beg + num - i + 1; j++)
      {
        if(mem[j] > mem[j+1])
        {
          temp = mem[j];
          mem[j] = mem[j+1];
          mem[j+1] = temp;
        }
      }
    }
    exit(0);
  }
  else
  {
    /* Since the processes run simultaneously, we have to wait for the sorting to get over before we start merge.
     * The merge takes place as follows:
     * The first element is pointed at in both halves of the array.
     * The smallest of the two gets inserted in the temp array (itr + 2* num)
     * The pointers are increased acordingly and the process repeats.
     * The temp array is shifted back into the sorted array.
     */
    wait(&child2);
    wait(&child1);
    if(!(child3 = fork()))
    {
      int  beg, mid, itr1;
      beg = 1;
      mid = (mem[0] / 2) + 1;
      num = mem[0];

      for(itr = 1 + 2*num ; itr <= 3*num;)
      {
        if(beg <= (num/2) && mid <= num)
        {
          if(mem[beg] < mem[mid])
          {
            mem[itr] = mem[beg];
            beg++;
          }
          else
          {
            mem[itr] = mem[mid];
            mid++;
          }
          itr++;
        }
        else if(beg == (num / 2) + 1)
        {
          int itr2;
          for(itr2 = itr; itr2 <= 3*num; itr2++)
          {
            mem[itr] = mem[mid];
            mid++;
            itr++;
          }
        }
        else 
        {
          int itr2;
          for(itr2 = itr; itr2 <= 3*num; itr2++)
          {
            mem[itr] = mem[beg];
            beg++;
            itr++;
          }
        }
      }
     
      for(itr = 1; itr <= num; itr++)
      {
        mem[itr] = mem[itr + 2*num];
      }
      exit(0);
    }
    wait(&child3);
    shm_unlink(buffer); 
    //After the merge is finished, the output is printed and the memory is no longer shared with the child processes.
    printf("The unsorted array is:\n");
    for(itr = 1; itr <= num; itr++)
    {
        printf("%d ", mem[itr + num]);
    }
    printf("\n");
    printf("The sorted array is:\n");
    for(itr = 1; itr <= num; itr++)
    {
        printf("%d ", mem[itr]);
    }
    printf("\n");
  }

  //The current clock cycle is stored in the end. 
  //The difference is calculated and divided by the clock frquency to calculate the time in seconds.
  end = clock();
    printf("Time: %f\n", (double)(end-begin)/CLOCKS_PER_SEC);
  return 0;
}
