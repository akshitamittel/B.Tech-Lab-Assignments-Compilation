#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//The num of entries and the three arrays are stored globally
int num;
int sorted[1000], unsorted[1000], temporary[1000];

/* Insertion sort takes place in the forst thread.
 * The first half of the array is sorted.
 * Each element is taken in the global array iteratively and placed in the nearest acsending order.
 * No return value required.
 */
void *sortOne(void *vargp)
{
  int i, j, temp, end, itr;
  end = num / 2;

  //Insertion Sort:
  for(i = 1; i < end; i++)
  {
    temp = sorted[i];
    j = i-1;
    while(temp < sorted[j] &&  j >= 0 )
    {
      sorted[j+1] = sorted[j];
      j -= 1;
    }
    sorted[j+1] = temp;
  }

  return NULL;
}

/* Bubble sort is used in the second child.
 * The second half of the array is sorted.
 * Each element is taken in the memory iteratively swapped with its neighbour til ascending order is reached.
 * No return value required.
*/
void *sortTwo(void *vargp)
{
  int i, j, temp, beg, itr;
  beg = num / 2;

  //Bubble sort:
  for(i = beg; i < num - 1; i++)
  {
    for(j = beg ; j < beg + num - i - 1; j++)
    {
      if(sorted[j] > sorted[j+1])
      {
        temp = sorted[j];
        sorted[j] = sorted[j+1];
        sorted[j+1] = temp;
      }
    }
  }
  
  return NULL;
}

/* The merge takes place as follows:
 * The first element is pointed at in both halves of the array.
 * The smallest of the two gets inserted in the temp array (itr + 2* num)
 * The pointers are increased acordingly and the process repeats.
 * The temp array is shifted back into the sorted array.
 */
void *merge(void *vargp)
{
  int  beg, mid, itr;
  beg = 0;
  mid = num / 2;

  for(itr = 0; itr < num;)
  {
    if(beg < (num/2) && mid < num)
    {
        if(sorted[beg] < sorted[mid])
        {
            temporary[itr] = sorted[beg];
            beg++;
        }
        else
        {
            temporary[itr] = sorted[mid];
            mid++;
        }
        itr++;
    }
    else if(beg == num / 2)
    {
        int itr2;
        for(itr2 = itr; itr2 < num; itr2++)
        {
            temporary[itr] = sorted[mid];
            mid++;
            itr++;
        }
    }
    else 
    {
        int itr2;
        for(itr2 = itr; itr2 < num; itr2++)
        {
            temporary[itr] = sorted[beg];
            beg++;
            itr++;
        }
    }
  }
  
  for(itr = 0; itr < num; itr++)
  {
    sorted[itr] = temporary[itr];
  }
  return NULL;
}

int main()
{
    //The current clock cycle is stored in the begin variable.
    clock_t begin, end;
    double timeTaken;
    begin = clock();

    int itr, inp;
    pthread_t tid;

    //Inputting the array size from the user.
    printf("Enter the number of entries (100 - 1000): ");
    scanf("%d", &num);

    /* Start the random number generator using srand (give the seed);
     * Generate a random number between 0 - 100.
     * Store that random number in the global array, corresponding to the sorted and unsorted array.
     */
    srand(2468);
    for(itr = 0; itr < num; itr++)
    {
        inp = rand()%100;
        sorted[itr] = inp;
        unsorted[itr] = inp;
    }

    /* pthread_create:
     * The third argument is the function name. 
     * The fourth argument is the argument sent.
     
     * pthread_join:
     * Waits for the thread that is being executed.
     */
    //The two sorting threads are called and executed.
    pthread_create(&tid, NULL, sortOne, NULL);
    pthread_join(tid, NULL);
    pthread_create(&tid, NULL, sortTwo, NULL);
    pthread_join(tid, NULL);

    //The merge is called after the sorting threads have completed.
    pthread_create(&tid, NULL, merge, NULL);
    pthread_join(tid, NULL);

    //After the merge is finished, the output is printed.
    printf("The unsorted array is:\n");
    for(itr = 0; itr < num; itr++)
    {
        printf("%d ", unsorted[itr]);
    }
    printf("\n");
    printf("The sorted array is:\n");
    for(itr = 0; itr < num; itr++)
    {
        printf("%d ", sorted[itr]);
    }
    printf("\n");

    //The current clock cycle is stored in the end. 
    //The difference is calculated and divided by the clock frquency to calculate the time in seconds.
    end = clock();
    printf("Time: %f\n", (double)(end-begin)/CLOCKS_PER_SEC);

    return 0;
}