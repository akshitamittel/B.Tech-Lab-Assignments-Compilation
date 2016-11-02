#include<sys/types.h>
#include<dirent.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

/*
 * A program to recursively go into directories and delete them.
 * The path stores the current path of the fie/subDirectory.
 * It recursively deletes all files and subfolders before removing the entire directory.
 * Ignore directories which are names "." and "..".
 */

void clean(DIR *directory, char* dirName)
{
  struct dirent *next_entry;
  DIR* subDirectory;
  char* path;
  char* seperate;
  seperate = "/";

  while( (next_entry = readdir(directory)) != NULL)
  {
    char* name;
    name = next_entry->d_name;
    //Append the name of the entry at the end of the path.
    path = (char *)malloc(2 + strlen(dirName) + strlen(name));
    strcpy(path, dirName);
    strcat(path, seperate);
    strcat(path, name);
    //Determine whether the entry is a file or Directory.
    //Else throw an error message.
    switch(next_entry->d_type)
    {
      case DT_REG:
        /* Simply remove the file */
        unlink(path);
        break;
      case DT_DIR:
        /* Check if the directory is not "." or ".." */
        if(name[0] != '.')
        {
          /* Open the subDirectory and recurse into it */
          subDirectory = opendir(path);
          clean(subDirectory, path);
        }
        break;
      default:
        printf("Invalid directory type!\n");
        break;
    }
  }  
  //After all the files and SubDirectories are removed. Delete parent file.
  rmdir(dirName);
}

int main(int argc, char *argv[])
{
  if(argc != 2)
  {
    printf("Incorrect number of arguments!\n");
  }
  else
  {
    DIR* directory;
    char* dirName = argv[1];
    directory = opendir(dirName);
    clean(directory, dirName);
    printf("successfully deleted\n");
  }
  return 1;
}


