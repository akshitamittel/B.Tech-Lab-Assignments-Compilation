#include<fcntl.h>
#include<sys/stat.h>
#include<unistd.h>
#include<stdio.h>

/*
 * Use lseek to find the offset till the end of file.
 * Use stat to retrieve a structure that stores the information of the file.
 * Get the size of the file from both and compare. 
 */

int main()
{
  int file;
  char* fileName = "t2-inp.txt";
  //The structure that holds all file information.
  struct stat buf;
  file = open("in.txt", O_RDONLY, 0644);
 
  //lseek:
  int size_lseek = lseek(file, 0, SEEK_END);
  if(size_lseek == -1)
  {
    printf("File not found!\n");
  }
  else
  {
    printf("lseek size: %d \n", size_lseek);
  }
  
  //stat
  stat(fileName, &buf);
  int size_stat = buf.st_size;
  printf("stat size: %d \n", size_stat);
  
  return 1;
}
