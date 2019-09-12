//Name: Rishabh Bhutra
//Roll No.:170569
/*Explanation: In the program for each sub directory of the given root we create a new process by fork. In the child process compute the size of sub directory
  and use pipe to pass size to parent process where we will print the sub directory and it's size. We wait for child process to end before parent process in order
  to print root at the top and sub directories at the bottom.
*/
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>

int sizef(char *basePath)//function to output size o directory
{
    char path[1000];
    struct dirent *dp;
    DIR *dir = opendir(basePath);//opening given path

    if (!dir)//if dir returns 0 return 0
        return 0;
    int size = 0;
    while ((dp = readdir(dir)) != NULL)//reading files/sub-dir of directory
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)//if file is not . and ..
        {
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);//append file name to given path
            DIR *temp = opendir(path);
            if(!temp){// if path returns 0
                struct stat st;
                stat(path,&st);
                size+=st.st_size;//add size of file to overall size        
            }
            else
                size+=sizef(path);//add size of sub directory to overall by recursively calling sizef
        }
    }

    closedir(dir);//close dir
    return size;//return/home/rishabh/Desktop/Assignment1/Part_3/Test_Cases/Testcase2/ size of directory
}

void directory(char *basePath)//function to print size of dir and sub dir
{
    char path[1000];
    int pid,t;
    struct dirent *dp;
    DIR *dir = opendir(basePath);//opening path
    int fd;
    if (!dir)//if dir returns 0 return
        return;
    int size = 0;
    while ((dp = readdir(dir)) != NULL)//reading files/sub-dir of directory
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)//if file is not . and ..
        {
            strcpy(path, basePath);
            strcat(path,"/");
            strcat(path, dp->d_name);//append file name to given path
            DIR *temp = opendir(path);
            if(!temp){// if path returns 0
                struct stat st;
                stat(path,&st);
                size+=st.st_size;//add size of file to overall size              
            }
            else{//if path is a sub dir
            	int fd[2];
            	pipe(fd);//forming pipe to pass size from child to parent
                pid = fork();//forking for each dir 
                if(!pid)//in child process
                { 
                	close(fd[0]);
                    t = sizef(path);//recursively computng size of sub dir
                    size+=t;//adding size of sub dir
                    write(fd[1], &t, sizeof(t));//writing size at write end
                    close(fd[1]);
                }  
                else
                {
                	close(fd[1]);
                    read(fd[0], &t, sizeof(t));//reading size at read end
                    wait(NULL);//wait for child process to end
                    printf("%s %d\n",dp->d_name,t);//printing sub dir and size
                    close(fd[0]); 
                    exit(0);//exit process
                }  
            }    
        }
    }
    printf("%s %d\n",basePath,size); //printing path and size 
    closedir(dir);//close dir
}


int main(int argc,char *argv[])
{
    directory(argv[1]);//calling function
    return 0;
}