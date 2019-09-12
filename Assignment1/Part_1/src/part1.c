//Name: Rishabh Bhutra
//Roll No.:170569

#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

void grep(char *file,char* pattern)//implementing grep to search a pattern from given file
{

    int fd,j=0; 
    char temp,str[100000]; 
    fd=open(file,O_RDONLY);//opening file in read only mode

    while(read(fd,&temp,1)!= 0)//reading one char and storing it in temp 
    { 
        if(temp!='\n') //append temp in str till we get a newline '\n'
        { 
            str[j]=temp; 
            j++; 
        } 
        else 
        { 
            if(strstr(str,pattern)) //if str has the pattern print str and filename
                printf("%s:%s\n",file,str); 
            memset(str,0,100000);//reset str 
            j=0; 
        } 

    }
}

void findfiles(char *basePath,char *pattern)//recursive function to search files 
{
    char path[1000];//string path to be searched recursively
    struct dirent *dp;
    DIR *dir = opendir(basePath);//opening the directory of basepath

    if (!dir)//if dir returns 0 then return
        return;

    while ((dp = readdir(dir)) != NULL)//read contents of directory
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)//if file is not . or ..
        {
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);//append to basepath the directory/file name
            DIR *temp = opendir(path);
            if(!temp){ //if opening new path returns 0 call grep on the file
                grep(path,pattern);                
            }
            findfiles(path,pattern);//findfiles recursively from the new path
        }
    }

    closedir(dir);//close the directory
}


int main(int argc,char *argv[])
{
    char path[100],patt[100];//path and pattern
    strcpy(patt,argv[1]);//copying pattern
    strcpy(path,argv[2]);//copying path
    findfiles(path,patt);//calling function

    return 0;
}

