#include<pipe.h>
#include<context.h>
#include<memory.h>
#include<lib.h>
#include<entry.h>
#include<file.h>
/***********************************************************************
 * Use this function to allocate pipe info && Don't Modify below function
 ***********************************************************************/
struct pipe_info* alloc_pipe_info()
{
    struct pipe_info *pipe = (struct pipe_info*)os_page_alloc(OS_DS_REG);
    char* buffer = (char*) os_page_alloc(OS_DS_REG);
    pipe ->pipe_buff = buffer;
    return pipe;
}


void free_pipe_info(struct pipe_info *p_info)
{
    if(p_info)
    {
        os_page_free(OS_DS_REG ,p_info->pipe_buff);
        os_page_free(OS_DS_REG ,p_info);
    }
}
/*************************************************************************/
/*************************************************************************/


long pipe_close(struct file *filep)
{
    free_pipe_info(filep->pipe);
    filep=NULL;
    return 1;
    /**
    * TODO:: Implementation of Close for pipe 
    * Free the pipe_info and file object
    * Incase of Error return valid Error code 
    */
    int ret_fd = -EINVAL; 
    return ret_fd;
}



int pipe_read(struct file *filep, char *buff, u32 count)
{
    if(filep==NULL||filep->pipe->read_pos+count>filep->pipe->buffer_offset) //return error on invalid input
    {
        int ret_fd = -EINVAL; 
        return ret_fd;  
    }
    for(int i=filep->pipe->read_pos;i<filep->pipe->read_pos+count;i++) //copying to buff from pipe_buff
    {
        *(buff+i)= *(filep->pipe->pipe_buff + i);
    }
    
    *(buff+count) = '\0';
    filep->pipe->read_pos+=count; //increasing read_pos
    return count;
    /**
    *  TODO:: Implementation of Pipe Read
    *  Read the contect from buff (pipe_info -> pipe_buff) and write to the buff(argument 2);
    *  Validate size of buff, the mode of pipe (pipe_info->mode),etc
    *  Incase of Error return valid Error code 
    */
    
}


int pipe_write(struct file *filep, char *buff, u32 count)
{
    if(filep==NULL) //return error on invalid input
    {
        int ret_fd = -EINVAL; 
        return ret_fd;  
    }
    if(filep->pipe->buffer_offset+count>PIPE_MAX_SIZE) //if pipe_buffer exceeds limit return error
        return -EOTHERS;
    for(int i=filep->pipe->write_pos;i<filep->pipe->write_pos+count;i++) //copying to pipe_buff from buff
    {
        *(filep->pipe->pipe_buff + i) = *(buff+i);
    }
    
    *(filep->pipe->pipe_buff+count) = '\0';
    filep->pipe->write_pos+=count; //increasing pipe_buff size
    filep->pipe->buffer_offset += count; //increasing write_pos
    
    return count;
    /**
    *  TODO:: Implementation of Pipe Read
    *  Write the contect from   the buff(argument 2);  and write to buff(pipe_info -> pipe_buff)
    *  Validate size of buff, the mode of pipe (pipe_info->mode),etc
    *  Incase of Error return valid Error code 
    */
    
}

int create_pipe(struct exec_context *current, int *fd)
{
    struct pipe_info* q = alloc_pipe_info();
    q->read_pos = 0;
    q->write_pos = 0;
    int a,b;
    a= -1;
    b= -1;
    for(int i=3;i<32;i++) //finding free descripters for pipe
    {
        if(current->files[i]==NULL)
        {
            if(a<0)
                a=i;
            else{
                b=i;
                break;
            }   
        }
    }

    if(a<0||b<0) //return error otherwise
    {
        return -EOTHERS;
    }
    fd[0] = a;
    fd[1] = b;
    current->files[a] = alloc_file(); //filling files and pipe of read and write fd
    current->files[b] = alloc_file();
    current->files[a]->type = PIPE;
    current->files[b]->type = PIPE;
    current->files[a]->ref_count = 1;
    current->files[b]->ref_count = 1;
    current->files[a]->pipe = alloc_pipe_info();
    current->files[b]->pipe = alloc_pipe_info();

    current->files[a]->pipe = q;
    current->files[b]->pipe = q;

    int (*read)(struct file*,char*,u32)= &pipe_read;
    int (*write)(struct file*,char*,u32)= &pipe_write;
    long (*close)(struct file*)= &generic_close;
    current->files[a]->fops->read = read;
    current->files[a]->fops->write = write;
    current->files[a]->fops->close = close;

    current->files[b]->fops->read = read;
    current->files[b]->fops->write = write;
    current->files[b]->fops->close = close; 
    return 1;
    /**
    *  TODO:: Implementation of Pipe Create
    *  Create file struct by invoking the alloc_file() function, 
    *  Create pipe_info struct by invoking the alloc_pipe_info() function
    *  fill the valid file descriptor in *fd param
    *  Incase of Error return valid Error code 
    */
    
}