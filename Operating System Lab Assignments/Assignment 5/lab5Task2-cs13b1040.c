#include<signal.h>
#include<stdio.h>
#include<stdlib.h>

int main()
{
  char command[50];
  int id;
  printf("Enter the pid: ");
  scanf("%d", &id);
 
  //If the pid is valid, this command gives the correct group pid.
  //If the pid is not valid, it returns -1.
  if(getpgid(id) == -1)
    printf("Invalid pid!\n");
  else
  /* The pstree command uses the -sp flag
   * The -s provides details of the ancestor in the tree
   * The -p returns the pid of the elements
   * The system function takes the string in the parameter and executes it through the processor.
   * After execution, it returns back to the program.
   */
  {
    sprintf (command, "pstree -s -p %d", id);
    system(command);
  }
  return 0;
}
