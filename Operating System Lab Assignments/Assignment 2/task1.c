#include<fcntl.h>
#include<sys/types.h>
#include<unistd.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>

/*
 * Open the input file, and create and output file using syscalls.
 * Find the size of the file and reset the pointer to the beginning.
 * Read the entire file into a buffer string.
 * Parse the string in reverse, and break whenever you get a newline.
 * Write the reversed string into the output file, by reversing it again.
 */

int main( )
{
  char *buffer, *line;
  int inputFile, outputFile;
 
  inputFile = open("t1-inp.txt", O_RDONLY);
  outputFile = open("t1-out.txt", O_WRONLY | O_CREAT, 0666);

  size_t size = lseek(inputFile, 0, SEEK_END);
  lseek(inputFile, 0, SEEK_SET);

  buffer = (char *)malloc(size);
  line = (char *)malloc(50*(sizeof(char*)));

  int n = read(inputFile, buffer, size);

  if(n == -1)
  {
    printf("The file does not exist!\n");
  }
  else
  {
    int i,j;
    for(i = ((int)size)-2; i >= 0; i--)
    {
       //Sepearating at a newline.
       if(buffer[i] == '\n' || i == 0)
       {   
           if(i == 0)
           {
              line[j] = buffer[i];
              j++;
           }
           int k;
           //Re-reversing the reversed input string while writing.
           for(k = j-1; k >=0; k--)
           {
             write(outputFile, &line[k], 1);
           }
           write(outputFile, "\n", 1);
           j = 0;
       }
       //Reading the input into a string.
       else
       {
          line[j] = buffer[i];
	  j++;
       }
    }
  }

  close(inputFile);
  close(outputFile);

  printf("File Transaction complete!\n");

  return 1;
}
